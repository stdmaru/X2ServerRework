-- GlobalServer's global mission, and server-list and log procedures that answered the servers wrongly
-- (tools/db-missing-procedures, 2026-11-16).
--
-- Global mission (GlobalServer, GlobalLogDBThread.cpp; GlobalMissionManager.cpp): a server-wide mission per day,
-- counted by every GameServer (GSUserRoomCommon.cpp: second-job Elesis players at level 35 or above killing the
-- mission's NPC). Both procedures were missing (V5's are stubs), so GlobalServer logged an ODBC error at start.
--   P_LEvent_GlobalMission_SEL   -> rows (BeginDate, EndDate, MissionType, ClearCondition, ClearCount, NowCount);
--                                   dates as 'yyyy-mm-dd hh:mi:ss' text, BeginDate is the mission's key. The server
--                                   reads every row, so only missions that have not ended are returned.
--   P_LEvent_GlobalMission_UPD   (@BeginDate text, @NowCount) -> 0: saves the count of the active mission.
-- No mission is seeded: the repository has no schedule. To run one, insert a row in LEvent_GlobalMission, e.g.
--   INSERT dbo.LEvent_GlobalMission (BeginDate, EndDate, MissionType, ClearCondition, ClearCount)
--   VALUES ('2026-11-20 00:00', '2026-11-20 23:59', 1, <NPC ID>, 1000);
-- MissionType 1 counts kills of NPC ClearCondition; the GameServer matches the mission keyed 'yyyy-mm-dd 00:00:00'.
--
-- Server list and logs:
--   P_LServerList_SEL            returned IP (one column); every server fetches PublicIP and PrivateIP since the
--                                2013 private-IP change (GSLogDBThread.cpp WriteServerInfoToDB), so the GameServers'
--                                server list read one column too many.
--   P_LAccusationLog_INT         the CenterServer passes (RoomUID, PVPChannelClass, MatchType, UnitUID, RegDate)
--                                without a server group first (CnLogDBThread.cpp DBE_UDP_CHECK_KICK_USER_NOT);
--                                @iServerSetID moves last, with a default.
--   P_StatCCUMinuteGuestUser_INS its table StatCCUMinuteGuestUser did not exist (LoginServer, guest-user CCU).
--   P_LTradeBank_INT             returned nothing, and the GameServer logs an error unless it reads 0.
--
-- Can be run again. Apply: sqlcmd -C -S localhost -U sa -P <password> -d Statistics -i <this file>

SET NOCOUNT ON;
GO

-- Tables ---------------------------------------------------------------------------------------------------------

IF OBJECT_ID('dbo.LEvent_GlobalMission') IS NULL
    CREATE TABLE dbo.LEvent_GlobalMission (
        BeginDate smalldatetime NOT NULL PRIMARY KEY, EndDate smalldatetime NOT NULL,
        MissionType int NOT NULL, ClearCondition int NOT NULL, ClearCount int NOT NULL,
        NowCount int NOT NULL DEFAULT 0, ModDate datetime NULL, RegDate datetime NOT NULL DEFAULT GETDATE());
IF OBJECT_ID('dbo.StatCCUMinuteGuestUser') IS NULL
    CREATE TABLE dbo.StatCCUMinuteGuestUser (
        CCUMinuteUID bigint IDENTITY(1,1) NOT NULL PRIMARY KEY, ServerSetID tinyint NULL,
        RegDate smalldatetime NOT NULL, ServerUID int NOT NULL, CCU int NOT NULL);
GO

-- Global mission -------------------------------------------------------------------------------------------------

CREATE OR ALTER PROCEDURE dbo.P_LEvent_GlobalMission_SEL
    @iServerSetID int = NULL
AS
SET NOCOUNT ON;
SELECT  CONVERT(nvarchar(19), BeginDate, 120) AS BeginDate
,       CONVERT(nvarchar(19), EndDate, 120) AS EndDate
,       MissionType, ClearCondition, ClearCount, NowCount
    FROM dbo.LEvent_GlobalMission WITH (NOLOCK)
    WHERE EndDate > GETDATE()
    ORDER BY BeginDate;
GO

CREATE OR ALTER PROCEDURE dbo.P_LEvent_GlobalMission_UPD
    @strBeginDate nvarchar(32)
,   @iNowCount    int
AS
SET NOCOUNT ON;
-- The databases run at compatibility level 100 (SQL Server 2008): no TRY_CONVERT.
DECLARE @sdtBegin smalldatetime = CASE WHEN ISDATE(@strBeginDate) = 1 THEN CONVERT(smalldatetime, @strBeginDate, 120) END;
IF @sdtBegin IS NULL
BEGIN
    SELECT -1; RETURN;
END
UPDATE dbo.LEvent_GlobalMission SET NowCount = @iNowCount, ModDate = GETDATE() WHERE BeginDate = @sdtBegin;
SELECT CASE WHEN @@ROWCOUNT = 1 THEN 0 ELSE -2 END;
GO

-- Server list and logs -------------------------------------------------------------------------------------------

CREATE OR ALTER PROCEDURE dbo.P_LServerList_SEL
    @iServerSetID tinyint
AS
SET NOCOUNT ON;
SELECT  [UID]
,       [Name]
,       ServerSetID
,       ServerType
,       ISNULL(PublicIP, IP) AS PublicIP
,       ISNULL(PrivateIP, IP) AS PrivateIP
,       Port1
,       Port2
,       MaxConcurrentUser
,       ConcurrentUser
,       [Enable]
    FROM dbo.LServerList WITH (NOLOCK);
GO

CREATE OR ALTER PROCEDURE dbo.P_LAccusationLog_INT
    @RoomUID         bigint
,   @PVPChannelClass smallint
,   @MatchType       smallint
,   @UnitUID         bigint
,   @RegDate         smalldatetime
,   @iServerSetID    tinyint = 0
AS
SET NOCOUNT ON;
DECLARE @Result int;
BEGIN TRY
    SET @Result = -1;
    INSERT INTO dbo.LAccusationLog (RoomUID, PVPChannelClass, MatchType, UnitUID, RegDate)
    VALUES (@RoomUID, @PVPChannelClass, @MatchType, @UnitUID, @RegDate);
    SET @Result = 0;
END TRY
BEGIN CATCH
    EXEC dbo.SYS_SP_ErrorLog_INT 0;
    SET @Result = -2;
END CATCH
SELECT @Result;
GO

CREATE OR ALTER PROCEDURE dbo.P_StatCCUMinuteGuestUser_INS
    @iServerSetID     tinyint
,   @sdtRegDate_      smalldatetime
,   @iUID_            int
,   @iConcurrentUser_ int
AS
SET NOCOUNT ON;
INSERT INTO dbo.StatCCUMinuteGuestUser (ServerSetID, RegDate, ServerUID, CCU)
VALUES (@iServerSetID, @sdtRegDate_, @iUID_, @iConcurrentUser_);
GO

CREATE OR ALTER PROCEDURE dbo.P_LTradeBank_INT
    @ServerSetID tinyint
,   @FromUnitUID bigint
,   @ToUnitUID   bigint
,   @FromItemUID bigint
,   @ToItemUID   bigint
,   @ItemID      int
,   @SealCnt     int
AS
SET NOCOUNT ON;
INSERT INTO dbo.LTradeBank (ServerSetID, FromUnitUID, ToUnitUID, FromItemUID, ToItemUID, ItemID, SealCnt, RegDate)
VALUES (@ServerSetID, @FromUnitUID, @ToUnitUID, @FromItemUID, @ToItemUID, @ItemID, @SealCnt, GETDATE());
SELECT CASE WHEN @@ERROR = 0 THEN 0 ELSE -1 END;
GO
