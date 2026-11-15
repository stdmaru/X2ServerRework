-- Log procedures the servers call that our Statistics lacked (server/add-character, 2026-11-15).
--
-- Each was in none of our databases and only a stub (or nothing) in V5's, so the servers logged an ODBC error at
-- every call. Each gets a table named after it and a procedure written from the server's call: the parameters in the
-- order the server passes them and the result it fetches (0 on success; P_LBossFieldLog_INS also returns the log's
-- UID, which P_LBossFieldLog_Unit_INS receives). Callers:
--   GameServer (GSLogDBThread.cpp): lup_insert_inventory_ED_log, P_LChangePetLog_INS, P_LCoupleSystemLog_INT,
--     P_LItemTradeError_INT, P_LSkillPageExpandLog_INT, P_LUnlimitUnitClassChangeLog_INS
--   CenterServer (CnLogDBThread.cpp): P_LBossFieldLog_INS, P_LBossFieldLog_Unit_INS, P_LCoupleSystemLog_INT,
--     P_LHenirHackUserLog_INS, P_StatsFieldBossKill_INS, P_StatsFieldBossSpawn_INS
-- Dates arrive as text ('2026-11-15 12:00:00'; the Henir log's as '261115 12:00:00') and are kept as text (EventDate),
-- beside the row's own RegDate. Levels and unit classes are numbers since x2::format writes narrow characters given
-- to %d as numbers (docs/porting/platform.md); with Boost the original sent them as characters.
--
-- Can be run again. Apply: sqlcmd -C -S localhost -U sa -P <password> -d Statistics -i <this file>

SET NOCOUNT ON;
GO

-- Tables ---------------------------------------------------------------------------------------------------------

IF OBJECT_ID('dbo.LInventoryEDLog') IS NULL
    CREATE TABLE dbo.LInventoryEDLog (
        LogUID bigint IDENTITY(1,1) NOT NULL PRIMARY KEY, UnitUID bigint NOT NULL, UserUID bigint NOT NULL,
        BeforeED int NOT NULL, UseED int NOT NULL, Category int NOT NULL, NumSlot int NOT NULL, UnitLevel int NOT NULL,
        RegDate datetime NOT NULL DEFAULT GETDATE());
IF OBJECT_ID('dbo.LChangePetLog') IS NULL
    CREATE TABLE dbo.LChangePetLog (
        LogUID bigint IDENTITY(1,1) NOT NULL PRIMARY KEY, UnitUID bigint NOT NULL, PetUID bigint NOT NULL,
        BeforePetID int NOT NULL, AfterPetID int NOT NULL, RegDate datetime NOT NULL DEFAULT GETDATE());
IF OBJECT_ID('dbo.LCoupleSystemLog') IS NULL
    CREATE TABLE dbo.LCoupleSystemLog (
        LogUID bigint IDENTITY(1,1) NOT NULL PRIMARY KEY, UserUID bigint NOT NULL, UnitUID bigint NOT NULL,
        [Level] int NOT NULL, UnitClass int NOT NULL, CoupleType int NOT NULL, EventDate nvarchar(32) NULL,
        RegDate datetime NOT NULL DEFAULT GETDATE());
IF OBJECT_ID('dbo.LItemTradeError') IS NULL
    CREATE TABLE dbo.LItemTradeError (
        LogUID bigint IDENTITY(1,1) NOT NULL PRIMARY KEY, UnitUID bigint NOT NULL, [Type] int NOT NULL,
        EventDate nvarchar(32) NULL, RegDate datetime NOT NULL DEFAULT GETDATE());
IF OBJECT_ID('dbo.LSkillPageExpandLog') IS NULL
    CREATE TABLE dbo.LSkillPageExpandLog (
        LogUID bigint IDENTITY(1,1) NOT NULL PRIMARY KEY, UnitUID bigint NOT NULL, NickName nvarchar(64) NULL,
        [Level] int NOT NULL, UnitClass int NOT NULL, BeforePageCount int NOT NULL, PaymentType int NOT NULL,
        RegDate datetime NOT NULL DEFAULT GETDATE());
IF OBJECT_ID('dbo.LUnlimitUnitClassChangeLog') IS NULL
    CREATE TABLE dbo.LUnlimitUnitClassChangeLog (
        LogUID bigint IDENTITY(1,1) NOT NULL PRIMARY KEY, UnitUID bigint NOT NULL, OldUnitClass int NOT NULL,
        NewUnitClass int NOT NULL, RegDate datetime NOT NULL DEFAULT GETDATE());
IF OBJECT_ID('dbo.LBossFieldLog') IS NULL
    CREATE TABLE dbo.LBossFieldLog (
        LogUID int IDENTITY(1,1) NOT NULL PRIMARY KEY, BossFieldID int NOT NULL, PortalMapID int NOT NULL,
        PortalOpenTime nvarchar(32) NULL, Succeed bit NOT NULL, PlayTime int NOT NULL, RemainUserCount int NOT NULL,
        RegDate datetime NOT NULL DEFAULT GETDATE());
IF OBJECT_ID('dbo.LBossFieldLog_Unit') IS NULL
    CREATE TABLE dbo.LBossFieldLog_Unit (
        LogUID int NOT NULL, UnitUID bigint NOT NULL, NickName nvarchar(64) NULL, [Level] int NOT NULL,
        UnitClass int NOT NULL, CompletionType int NOT NULL, ContributionRank int NOT NULL,
        EXP int NOT NULL, ED int NOT NULL, GivenDamage bigint NOT NULL, AttackDamage bigint NOT NULL, NumResurrectionStone int NOT NULL,
        RegDate datetime NOT NULL DEFAULT GETDATE(), PRIMARY KEY (LogUID, UnitUID));
IF OBJECT_ID('dbo.LHenirHackUserLog') IS NULL
    CREATE TABLE dbo.LHenirHackUserLog (
        LogUID bigint IDENTITY(1,1) NOT NULL PRIMARY KEY, UserUID bigint NOT NULL, UnitUID bigint NOT NULL,
        MemberNum int NOT NULL, PlayTime int NOT NULL, EventDate nvarchar(32) NULL,
        RegDate datetime NOT NULL DEFAULT GETDATE());
IF OBJECT_ID('dbo.StatsFieldBossSpawn') IS NULL
    CREATE TABLE dbo.StatsFieldBossSpawn (
        LogUID bigint IDENTITY(1,1) NOT NULL PRIMARY KEY, FieldID int NOT NULL, SpawnID int NOT NULL,
        RegDate datetime NOT NULL DEFAULT GETDATE());
IF OBJECT_ID('dbo.StatsFieldBossKill') IS NULL
    CREATE TABLE dbo.StatsFieldBossKill (
        LogUID bigint IDENTITY(1,1) NOT NULL PRIMARY KEY, FieldID int NOT NULL, SpawnID int NOT NULL,
        NumUser int NOT NULL, RegDate datetime NOT NULL DEFAULT GETDATE());
GO

-- Procedures (result: 0, -1 failed) ------------------------------------------------------------------------------

CREATE OR ALTER PROCEDURE dbo.lup_insert_inventory_ED_log
    @iUnitUID bigint, @iUserUID bigint, @iBeforeED int, @iUseED int, @iCategory int, @iNumSlot int, @iUnitLevel int
AS
SET NOCOUNT ON;
DECLARE @iOK int = 0
BEGIN TRY
    INSERT INTO dbo.LInventoryEDLog (UnitUID, UserUID, BeforeED, UseED, Category, NumSlot, UnitLevel)
        VALUES (@iUnitUID, @iUserUID, @iBeforeED, @iUseED, @iCategory, @iNumSlot, @iUnitLevel)
END TRY
BEGIN CATCH SET @iOK = -1 END CATCH
SELECT @iOK
GO

CREATE OR ALTER PROCEDURE dbo.P_LChangePetLog_INS
    @iUnitUID bigint, @iPetUID bigint, @iBeforePetID int, @iAfterPetID int
AS
SET NOCOUNT ON;
DECLARE @iOK int = 0
BEGIN TRY
    INSERT INTO dbo.LChangePetLog (UnitUID, PetUID, BeforePetID, AfterPetID)
        VALUES (@iUnitUID, @iPetUID, @iBeforePetID, @iAfterPetID)
END TRY
BEGIN CATCH SET @iOK = -1 END CATCH
SELECT @iOK
GO

CREATE OR ALTER PROCEDURE dbo.P_LCoupleSystemLog_INT
    @iUserUID bigint, @iUnitUID bigint, @iLevel int, @iUnitClass int, @iCoupleType int, @strRegDate nvarchar(32)
AS
SET NOCOUNT ON;
DECLARE @iOK int = 0
BEGIN TRY
    INSERT INTO dbo.LCoupleSystemLog (UserUID, UnitUID, [Level], UnitClass, CoupleType, EventDate)
        VALUES (@iUserUID, @iUnitUID, @iLevel, @iUnitClass, @iCoupleType, @strRegDate)
END TRY
BEGIN CATCH SET @iOK = -1 END CATCH
SELECT @iOK
GO

CREATE OR ALTER PROCEDURE dbo.P_LItemTradeError_INT
    @iUnitUID bigint, @iType int, @strRegDate nvarchar(32)
AS
SET NOCOUNT ON;
DECLARE @iOK int = 0
BEGIN TRY
    INSERT INTO dbo.LItemTradeError (UnitUID, [Type], EventDate) VALUES (@iUnitUID, @iType, @strRegDate)
END TRY
BEGIN CATCH SET @iOK = -1 END CATCH
SELECT @iOK
GO

CREATE OR ALTER PROCEDURE dbo.P_LSkillPageExpandLog_INT
    @iUnitUID bigint, @strNickName nvarchar(64), @iLevel int, @iUnitClass int, @iBeforePageCount int,
    @iPaymentType int
AS
SET NOCOUNT ON;
DECLARE @iOK int = 0
BEGIN TRY
    INSERT INTO dbo.LSkillPageExpandLog (UnitUID, NickName, [Level], UnitClass, BeforePageCount, PaymentType)
        VALUES (@iUnitUID, @strNickName, @iLevel, @iUnitClass, @iBeforePageCount, @iPaymentType)
END TRY
BEGIN CATCH SET @iOK = -1 END CATCH
SELECT @iOK
GO

CREATE OR ALTER PROCEDURE dbo.P_LUnlimitUnitClassChangeLog_INS
    @iUnitUID bigint, @iOldUnitClass int, @iNewUnitClass int
AS
SET NOCOUNT ON;
DECLARE @iOK int = 0
BEGIN TRY
    INSERT INTO dbo.LUnlimitUnitClassChangeLog (UnitUID, OldUnitClass, NewUnitClass)
        VALUES (@iUnitUID, @iOldUnitClass, @iNewUnitClass)
END TRY
BEGIN CATCH SET @iOK = -1 END CATCH
SELECT @iOK
GO

-- Result: 0 and the log's UID, or -1 and 0.
CREATE OR ALTER PROCEDURE dbo.P_LBossFieldLog_INS
    @iBossFieldID int, @iPortalMapID int, @strPortalOpenTime nvarchar(32), @bSucceed bit, @iPlayTime int,
    @iRemainUserCount int
AS
SET NOCOUNT ON;
DECLARE @iOK int = 0, @iLogUID int = 0
BEGIN TRY
    INSERT INTO dbo.LBossFieldLog (BossFieldID, PortalMapID, PortalOpenTime, Succeed, PlayTime, RemainUserCount)
        VALUES (@iBossFieldID, @iPortalMapID, @strPortalOpenTime, @bSucceed, @iPlayTime, @iRemainUserCount)
    SET @iLogUID = SCOPE_IDENTITY()
END TRY
BEGIN CATCH SET @iOK = -1 END CATCH
SELECT @iOK, @iLogUID
GO

CREATE OR ALTER PROCEDURE dbo.P_LBossFieldLog_Unit_INS
    @iLogUID int, @iUnitUID bigint, @strNickName nvarchar(64), @iLevel int, @iUnitClass int, @iCompletionType int,
    @iContributionRank int, @iEXP int, @iED int, @iGivenDamage bigint, @iAttackDamage bigint,
    @iNumResurrectionStone int
AS
SET NOCOUNT ON;
DECLARE @iOK int = 0
BEGIN TRY
    INSERT INTO dbo.LBossFieldLog_Unit (LogUID, UnitUID, NickName, [Level], UnitClass, CompletionType,
                                        ContributionRank, EXP, ED, GivenDamage, AttackDamage, NumResurrectionStone)
        VALUES (@iLogUID, @iUnitUID, @strNickName, @iLevel, @iUnitClass, @iCompletionType, @iContributionRank, @iEXP,
                @iED, @iGivenDamage, @iAttackDamage, @iNumResurrectionStone)
END TRY
BEGIN CATCH SET @iOK = -1 END CATCH
SELECT @iOK
GO

CREATE OR ALTER PROCEDURE dbo.P_LHenirHackUserLog_INS
    @iUserUID bigint, @iUnitUID bigint, @iMemberNum int, @iPlayTime int, @strRegDate nvarchar(32)
AS
SET NOCOUNT ON;
DECLARE @iOK int = 0
BEGIN TRY
    INSERT INTO dbo.LHenirHackUserLog (UserUID, UnitUID, MemberNum, PlayTime, EventDate)
        VALUES (@iUserUID, @iUnitUID, @iMemberNum, @iPlayTime, @strRegDate)
END TRY
BEGIN CATCH SET @iOK = -1 END CATCH
SELECT @iOK
GO

CREATE OR ALTER PROCEDURE dbo.P_StatsFieldBossSpawn_INS
    @iFieldID int, @iSpawnID int
AS
SET NOCOUNT ON;
DECLARE @iOK int = 0
BEGIN TRY
    INSERT INTO dbo.StatsFieldBossSpawn (FieldID, SpawnID) VALUES (@iFieldID, @iSpawnID)
END TRY
BEGIN CATCH SET @iOK = -1 END CATCH
SELECT @iOK
GO

CREATE OR ALTER PROCEDURE dbo.P_StatsFieldBossKill_INS
    @iFieldID int, @iSpawnID int, @iNumUser int
AS
SET NOCOUNT ON;
DECLARE @iOK int = 0
BEGIN TRY
    INSERT INTO dbo.StatsFieldBossKill (FieldID, SpawnID, NumUser) VALUES (@iFieldID, @iSpawnID, @iNumUser)
END TRY
BEGIN CATCH SET @iOK = -1 END CATCH
SELECT @iOK
GO
