-- Event, personal-shop and pet procedures the GameServer calls and our Game01 lacked or could not run
-- (tools/db-missing-procedures, 2026-11-16).
--
-- All were missing (V5's are stubs) or referenced objects that do not exist, so their features failed with an ODBC
-- error. Written from the server's calls (GSGameDBThread.cpp): parameters in the order the server passes them and
-- the result it fetches.
--   P_GEvent_GlobalMission_INS  (@Day 'yyyy-mm-dd', @UnitUID) -> 0: a unit that counted for the day's global mission
--                               (Statistics' LEvent_GlobalMission). Once per unit and day.
--   Virtual-marriage event (DBE_EVENT_PROPOSE_USER_FIND_REQ, _MAKING_SUCCESS_REQ, _DIVORCE_REQ and the unit
--   select): table GEvent_Couple, one row per couple.
--     P_GEvent_Couple_CHK  (@NickName) -> (Result, UnitUID, UserUID, UnitClass, IsCouple); Result -1 when no live unit
--                          has the name (the server answers ERR_RELATIONSHIP_04).
--     P_GEvent_Couple_INS  (@RequestUnitUID, @AcceptUnitUID) -> 0; -1 when either is already in a couple.
--     P_GEvent_Couple_DEL  (@UnitUID, @PartnerUnitUID) -> 0; -1 when they are not a couple.
--     P_GEvent_Couple_SEL  (@UnitUID) -> (PartnerUnitUID, PartnerNickName); was a stub answering (0, NULL), which it
--                          still answers for a unit with no couple.
--   gup_insert_xmas_event (@UnitUID, @EventUID) -> the number of times the unit got the cumulative-time event's
--                          reward, this one included (DBE_CHECK_TIME_EVENT_COMPLETE_REQ: below 5 the event restarts).
--   P_GPshopInfo_INT      (@UserUID, @UnitUID, @Days, @ShopType) -> (Result, ExpirationDate text): a personal-shop
--                          agency (ED or premium) bought or extended by a guarantee item. As gup_insert_PShopInfo
--                          (which it replaces since the shop types), plus the shop type; an agency that has not
--                          expired is extended, otherwise it starts now.
--   P_GPet_Info_DEL       released pets were copied into GameDataMgmt.dbo.DeletePetMgmt, a database we do not
--                          have, so releasing a pet always failed; they go to Game01's GPet_Info_Deleted.
--
-- Can be run again. Apply: sqlcmd -C -S localhost -U sa -P <password> -d Game01 -i <this file>

SET NOCOUNT ON;
GO

-- Tables ---------------------------------------------------------------------------------------------------------

IF OBJECT_ID('dbo.GEvent_GlobalMission') IS NULL
    CREATE TABLE dbo.GEvent_GlobalMission (
        MissionDay date NOT NULL, UnitUID bigint NOT NULL, RegDate datetime NOT NULL DEFAULT GETDATE(),
        PRIMARY KEY (MissionDay, UnitUID));
IF OBJECT_ID('dbo.GEvent_Couple') IS NULL
    CREATE TABLE dbo.GEvent_Couple (
        CoupleUID int IDENTITY(1,1) NOT NULL PRIMARY KEY, RequestUnitUID bigint NOT NULL UNIQUE,
        AcceptUnitUID bigint NOT NULL UNIQUE, RegDate datetime NOT NULL DEFAULT GETDATE());
IF OBJECT_ID('dbo.GEvent_Xmas') IS NULL
    CREATE TABLE dbo.GEvent_Xmas (
        UnitUID bigint NOT NULL, EventUID int NOT NULL, RewardCount int NOT NULL,
        LastRewardDate datetime NOT NULL DEFAULT GETDATE(), PRIMARY KEY (UnitUID, EventUID));
IF OBJECT_ID('dbo.GPet_Info_Deleted') IS NULL
    CREATE TABLE dbo.GPet_Info_Deleted (
        PetUID bigint NOT NULL PRIMARY KEY, UnitUID bigint NOT NULL, PetID int NOT NULL, PetName nvarchar(16) NOT NULL,
        EvolutionStep tinyint NOT NULL, Satiety smallint NOT NULL, Intimacy int NOT NULL, Extroversion smallint NOT NULL,
        Emotion smallint NOT NULL, RegDate smalldatetime NOT NULL, DelDate smalldatetime NOT NULL, IsCall bit NOT NULL,
        AutoFeed bit NOT NULL, LastFeedDate smalldatetime NOT NULL, LastcallDate smalldatetime NULL, PickUP bit NULL,
        ReleaseDate datetime NOT NULL DEFAULT GETDATE());
GO

-- Global mission -------------------------------------------------------------------------------------------------

CREATE OR ALTER PROCEDURE dbo.P_GEvent_GlobalMission_INS
    @strDay   nvarchar(32)
,   @iUnitUID bigint
AS
SET NOCOUNT ON;
-- The databases run at compatibility level 100 (SQL Server 2008): no TRY_CONVERT.
DECLARE @dtDay date = CASE WHEN ISDATE(@strDay) = 1 THEN CONVERT(date, @strDay, 120) END;
IF @dtDay IS NULL
BEGIN
    SELECT -1; RETURN;
END
IF NOT EXISTS (SELECT 1 FROM dbo.GEvent_GlobalMission WHERE MissionDay = @dtDay AND UnitUID = @iUnitUID)
    INSERT INTO dbo.GEvent_GlobalMission (MissionDay, UnitUID) VALUES (@dtDay, @iUnitUID);
SELECT 0;
GO

-- Virtual-marriage event -----------------------------------------------------------------------------------------

CREATE OR ALTER PROCEDURE dbo.P_GEvent_Couple_CHK
    @strNickName nvarchar(16)
AS
SET NOCOUNT ON;
DECLARE @iUnitUID bigint, @iUserUID bigint, @iUnitClass tinyint;
SELECT  @iUnitUID = U.UnitUID, @iUserUID = U.UserUID, @iUnitClass = U.UnitClass
    FROM dbo.GUnitNickName AS N WITH (NOLOCK)
    JOIN dbo.GUnit AS U WITH (NOLOCK) ON U.UnitUID = N.UnitUID
    WHERE N.NickName = @strNickName AND U.Deleted = 0;
IF @iUnitUID IS NULL
BEGIN
    SELECT -1, CAST(0 AS bigint), CAST(0 AS bigint), CAST(0 AS tinyint), CAST(0 AS bit); RETURN;
END
SELECT  0, @iUnitUID, @iUserUID, @iUnitClass,
        CAST(CASE WHEN EXISTS (SELECT 1 FROM dbo.GEvent_Couple WITH (NOLOCK)
                               WHERE RequestUnitUID = @iUnitUID OR AcceptUnitUID = @iUnitUID) THEN 1 ELSE 0 END AS bit);
GO

CREATE OR ALTER PROCEDURE dbo.P_GEvent_Couple_INS
    @iRequestUnitUID bigint
,   @iAcceptUnitUID  bigint
AS
SET NOCOUNT ON;
IF @iRequestUnitUID = @iAcceptUnitUID
   OR EXISTS (SELECT 1 FROM dbo.GEvent_Couple
              WHERE RequestUnitUID IN (@iRequestUnitUID, @iAcceptUnitUID)
                 OR AcceptUnitUID IN (@iRequestUnitUID, @iAcceptUnitUID))
BEGIN
    SELECT -1; RETURN;
END
INSERT INTO dbo.GEvent_Couple (RequestUnitUID, AcceptUnitUID) VALUES (@iRequestUnitUID, @iAcceptUnitUID);
SELECT 0;
GO

CREATE OR ALTER PROCEDURE dbo.P_GEvent_Couple_DEL
    @iUnitUID        bigint
,   @iPartnerUnitUID bigint
AS
SET NOCOUNT ON;
DELETE dbo.GEvent_Couple
    WHERE (RequestUnitUID = @iUnitUID AND AcceptUnitUID = @iPartnerUnitUID)
       OR (RequestUnitUID = @iPartnerUnitUID AND AcceptUnitUID = @iUnitUID);
SELECT CASE WHEN @@ROWCOUNT = 1 THEN 0 ELSE -1 END;
GO

CREATE OR ALTER PROCEDURE dbo.P_GEvent_Couple_SEL
    @UnitUID bigint
AS
SET NOCOUNT ON;
DECLARE @iPartner bigint;
SELECT  @iPartner = CASE WHEN RequestUnitUID = @UnitUID THEN AcceptUnitUID ELSE RequestUnitUID END
    FROM dbo.GEvent_Couple WITH (NOLOCK)
    WHERE RequestUnitUID = @UnitUID OR AcceptUnitUID = @UnitUID;
IF @iPartner IS NULL
BEGIN
    SELECT CAST(0 AS bigint), CAST(NULL AS nvarchar(16)); RETURN;
END
SELECT @iPartner, (SELECT NickName FROM dbo.GUnitNickName WITH (NOLOCK) WHERE UnitUID = @iPartner);
GO

-- Cumulative-time event ------------------------------------------------------------------------------------------

CREATE OR ALTER PROCEDURE dbo.gup_insert_xmas_event
    @iUnitUID  bigint
,   @iEventUID int
AS
SET NOCOUNT ON;
DECLARE @iCount int;
UPDATE dbo.GEvent_Xmas
    SET @iCount = RewardCount = RewardCount + 1, LastRewardDate = GETDATE()
    WHERE UnitUID = @iUnitUID AND EventUID = @iEventUID;
IF @@ROWCOUNT = 0
BEGIN
    INSERT INTO dbo.GEvent_Xmas (UnitUID, EventUID, RewardCount) VALUES (@iUnitUID, @iEventUID, 1);
    SET @iCount = 1;
END
SELECT @iCount;
GO

-- Personal-shop agency -------------------------------------------------------------------------------------------

CREATE OR ALTER PROCEDURE dbo.P_GPshopInfo_INT
    @iUserUID  bigint
,   @iUnitUID  bigint
,   @iDays     smallint
,   @iShopType tinyint
AS
SET NOCOUNT ON;
SET XACT_ABORT ON;
DECLARE @sdtNow smalldatetime = GETDATE(), @sdtExpiration smalldatetime, @iOK int = 0;
IF NOT EXISTS (SELECT 1 FROM dbo.GUnit WITH (NOLOCK) WHERE UnitUID = @iUnitUID AND Deleted = 0)
BEGIN
    SELECT -1, CAST(NULL AS nvarchar(19)); RETURN;
END
BEGIN TRAN;
    SELECT @sdtExpiration = ExpirationDate FROM dbo.GPShopInfo WITH (UPDLOCK) WHERE UnitUID = @iUnitUID;
    IF @@ROWCOUNT = 0
    BEGIN
        SET @sdtExpiration = DATEADD(DAY, @iDays, @sdtNow);
        INSERT INTO dbo.GPShopInfo (UserUID, UnitUID, IsPShopOpen, ShopType, PShopName, BeginDate, ExpirationDate, On_Off)
        VALUES (@iUserUID, @iUnitUID, 0, @iShopType, '', @sdtNow, @sdtExpiration, 0);
        IF @@ROWCOUNT <> 1 SET @iOK = -11;
    END
    ELSE
    BEGIN
        SET @sdtExpiration = DATEADD(DAY, @iDays, CASE WHEN @sdtExpiration > @sdtNow THEN @sdtExpiration ELSE @sdtNow END);
        UPDATE dbo.GPShopInfo SET ExpirationDate = @sdtExpiration, ShopType = @iShopType WHERE UnitUID = @iUnitUID;
        IF @@ROWCOUNT <> 1 SET @iOK = -12;
    END
IF @iOK = 0 COMMIT TRAN; ELSE ROLLBACK TRAN;
SELECT @iOK, CONVERT(nvarchar(19), @sdtExpiration, 120);
GO

-- Pet release ----------------------------------------------------------------------------------------------------

CREATE OR ALTER PROCEDURE dbo.P_GPet_Info_DEL
    @PetUID bigint
AS
SET NOCOUNT ON;
DECLARE @Result int;
BEGIN TRY
    BEGIN TRAN;
    SET @Result = -1;
    DELETE dbo.GPet_Info
        OUTPUT DELETED.PetUID, DELETED.UnitUID, DELETED.PetID, DELETED.PetName, DELETED.EvolutionStep, DELETED.Satiety,
               DELETED.Intimacy, DELETED.Extroversion, DELETED.Emotion, DELETED.RegDate, DELETED.DelDate, DELETED.IsCall,
               DELETED.AutoFeed, DELETED.LastFeedDate, DELETED.LastcallDate, DELETED.PickUP
        INTO dbo.GPet_Info_Deleted (PetUID, UnitUID, PetID, PetName, EvolutionStep, Satiety, Intimacy, Extroversion,
                                    Emotion, RegDate, DelDate, IsCall, AutoFeed, LastFeedDate, LastcallDate, PickUP)
        WHERE PetUID = @PetUID;
    IF @@ROWCOUNT = 0
        RAISERROR(N'[Result : %d]P_GPet_Info_DEL %I64d', 15, 1, @Result, @PetUID) WITH SETERROR;
    COMMIT TRAN;
    SET @Result = 0;
END TRY
BEGIN CATCH
    DECLARE @NO_XACT_STATE smallint = XACT_STATE();
    IF @NO_XACT_STATE <> 0
        ROLLBACK TRAN;
    EXEC dbo.SYS_SP_ErrorLog_INT @NO_XACT_STATE;
    IF @Result = 0 OR @Result IS NULL
        SET @Result = -1;
END CATCH
SELECT ISNULL(@Result, 0) AS Result;
GO
