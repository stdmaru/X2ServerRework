-- Account tables that existing procedures use but the database never had, and the authority procedures that read
-- a table the database does not use (tools/db-missing-procedures, 2026-11-16).
--
-- Block lists (LoginServer, LoginAccountDBThread.cpp DBE_CHECK_ACCOUNT_BLOCK_LIST_REQ; AccountBlockManager.h):
-- the LoginServer reads ReleaseTick every few seconds and reloads a list only when its tick grows. ReleaseTick was
-- empty, so no list ever loaded, and the lists' tables did not exist, so registering a user failed.
--   type 0  BT_CONNECT_BLOCK     mup_get_rejected_user  / mup_insert_rejected_user  -> table RejectedUser
--   type 1  BT_TRADE_BLOCK       mup_get_TradeBlock_user / mup_insert_TradeBlock_user -> table TradeBlock
--   type 5  BT_MACHINE_ID_BLOCK  mup_get_add (missing)  -> table MachineIDBlock (new)
--   type 4  GlobalServer's penalty list (GlobalAccountDBThread.cpp DBE_CHECK_BLOCK_LIST_REQ, P_MPenalty_GET)
-- The seeded ticks start at 1, so every list loads once at start. After editing a list by hand, raise its tick:
--   UPDATE dbo.ReleaseTick SET [Count] = [Count] + 1 WHERE [Type] = 5;
-- (mup_insert_rejected_user and mup_insert_TradeBlock_user already raise theirs.)
--
-- Other procedures whose tables were missing or incomplete:
--   mup_update_second_pw          logs to SecondPWChangeLog (missing): changing the second password always failed.
--   mup_create_Comebackuser_info  writes MUserComeback.RewardLevel and RegDate (missing columns); UserUID becomes
--                                 bigint like every other UserUID (the table is empty and has no index on it).
--
-- Authority: this database keeps a user's level in MUser.AuthLevel (read at login). mup_get_authority_list,
-- mup_set_user_authority (GameServer admin commands, GSAccountDBThread.cpp), mup_update_tool_authlevel and
-- mup_get_user_info read the original's MUserAuthority table, which does not exist; they now use MUser.AuthLevel.
--
-- Can be run again. Apply: sqlcmd -C -S localhost -U sa -P <password> -d Account -i <this file>

SET NOCOUNT ON;
GO

-- Tables ---------------------------------------------------------------------------------------------------------

IF OBJECT_ID('dbo.RejectedUser') IS NULL
    CREATE TABLE dbo.RejectedUser (
        RejectedUID int IDENTITY(1,1) NOT NULL PRIMARY KEY, UserUID bigint NOT NULL,
        RegDate datetime NOT NULL DEFAULT GETDATE(), Reason tinyint NOT NULL DEFAULT 0,
        INDEX IX_RejectedUser_UserUID (UserUID));
IF OBJECT_ID('dbo.TradeBlock') IS NULL
    CREATE TABLE dbo.TradeBlock (
        TradeBlockUID int IDENTITY(1,1) NOT NULL PRIMARY KEY, UserUID bigint NOT NULL,
        RegDate datetime NOT NULL DEFAULT GETDATE(), Reason tinyint NOT NULL DEFAULT 0,
        INDEX IX_TradeBlock_UserUID (UserUID));
IF OBJECT_ID('dbo.MachineIDBlock') IS NULL
    CREATE TABLE dbo.MachineIDBlock (
        MachineID varchar(64) NOT NULL PRIMARY KEY, RegDate datetime NOT NULL DEFAULT GETDATE(),
        Reason tinyint NOT NULL DEFAULT 0, Comment nvarchar(100) NULL);
IF OBJECT_ID('dbo.SecondPWChangeLog') IS NULL
    CREATE TABLE dbo.SecondPWChangeLog (
        LogUID int IDENTITY(1,1) NOT NULL PRIMARY KEY, UserUID bigint NOT NULL, RegDate datetime NOT NULL);

IF COL_LENGTH('dbo.MUserComeback', 'RewardLevel') IS NULL
    ALTER TABLE dbo.MUserComeback ADD RewardLevel tinyint NULL;
IF COL_LENGTH('dbo.MUserComeback', 'RegDate') IS NULL
    ALTER TABLE dbo.MUserComeback ADD RegDate smalldatetime NULL;
IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.MUserComeback') AND name = 'UserUID'
           AND system_type_id = TYPE_ID('int'))
    ALTER TABLE dbo.MUserComeback ALTER COLUMN UserUID bigint NULL;

INSERT INTO dbo.ReleaseTick ([Type], [Count])
    SELECT t.[Type], 1 FROM (VALUES (0), (1), (4), (5)) AS t([Type])
    WHERE NOT EXISTS (SELECT * FROM dbo.ReleaseTick r WHERE r.[Type] = t.[Type]);
GO

-- Machine ID block list ------------------------------------------------------------------------------------------

CREATE OR ALTER PROCEDURE dbo.mup_get_add
AS
SET NOCOUNT ON;
SELECT MachineID FROM dbo.MachineIDBlock WITH (NOLOCK);
GO

-- Authority ------------------------------------------------------------------------------------------------------

CREATE OR ALTER PROCEDURE dbo.mup_get_authority_list
    @iAuthLevel tinyint
AS
SET NOCOUNT ON;
SELECT UserID, UserName FROM dbo.MUser WITH (NOLOCK) WHERE AuthLevel = @iAuthLevel AND Deleted = 0;
GO

CREATE OR ALTER PROCEDURE dbo.mup_set_user_authority
    @strUserID_ nvarchar(20)
,   @iAuthLevel tinyint
,   @iOK        int = 0
AS
SET NOCOUNT ON;
-- Invalid authority level.
IF @iAuthLevel > 4
BEGIN
    SELECT -1; RETURN;
END
UPDATE dbo.MUser SET AuthLevel = @iAuthLevel WHERE UserID = @strUserID_;
-- -2: no such user.
SELECT CASE WHEN @@ROWCOUNT = 0 THEN -2 ELSE 0 END;
GO

CREATE OR ALTER PROCEDURE dbo.mup_update_tool_authlevel
    @strUserID  nvarchar(24)
,   @iAuthLevel tinyint
,   @iOK        int = 0
AS
SET NOCOUNT ON;
UPDATE dbo.MUser SET AuthLevel = @iAuthLevel WHERE UserID = @strUserID;
SELECT CASE WHEN @@ROWCOUNT = 0 THEN -1 ELSE 0 END;
GO

CREATE OR ALTER PROCEDURE dbo.mup_get_user_info
    @iUserUID_ bigint
,   @iOK       int = 0
AS
SET NOCOUNT ON;
DECLARE @strName nvarchar(16), @iAuthLevel int;
SELECT @strName = UserName, @iAuthLevel = AuthLevel
    FROM dbo.MUser WITH (NOLOCK) WHERE UserUID = @iUserUID_ AND Deleted = 0;
IF @strName IS NULL
    SELECT -1, N'NULL', 0;
ELSE
    SELECT 0, @strName, ISNULL(@iAuthLevel, 0);
GO
