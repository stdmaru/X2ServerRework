USE [Statistics];
GO

/*
    Additive database support for the GlobalServer timed-enchant and limited-exchange features.

    Behavior inferred directly from the server:
    - A timed-enchant event is a single-use, server-wide offer. The first successful claim consumes it.
    - LimitQuantity is a shared quota for one LimitUID across all destination items configured under its TargetID.
    - Exchange claims reduce the quota on load; a rollback deletes its log record and restores the in-memory quota.
    Schema assumptions because the original table definitions are unavailable:
    - The new table and column names/types below are an additive model for the server's query contracts.
    - Event/configuration rows are intentionally not seeded here. Operators supply schedules, levels, limits,
      and target items that match their game configuration.

    This script only adds new tables and procedures. It does not alter existing tables or data.
    Target: SQL Server 2008 or newer (compatible with database compatibility level 100).
*/

IF OBJECT_ID(N'dbo.LEvent_RandomEnchant', N'U') IS NULL
BEGIN
    CREATE TABLE dbo.LEvent_RandomEnchant
    (
        EventUID       int           NOT NULL,
        BeginDate      datetime      NOT NULL,
        EndDate        datetime      NOT NULL,
        EnchantLevel   tinyint       NOT NULL,
        UsedUnitUID    bigint        NULL,
        UsedDate       datetime      NULL,
        CONSTRAINT PK_LEvent_RandomEnchant PRIMARY KEY CLUSTERED (EventUID),
        CONSTRAINT CK_LEvent_RandomEnchant_DateRange CHECK (BeginDate < EndDate),
        CONSTRAINT CK_LEvent_RandomEnchant_Claim CHECK
            ((UsedUnitUID IS NULL AND UsedDate IS NULL) OR (UsedUnitUID IS NOT NULL AND UsedDate IS NOT NULL))
    );
END;
GO

IF OBJECT_ID(N'dbo.LExchangeLimitInfo', N'U') IS NULL
BEGIN
    CREATE TABLE dbo.LExchangeLimitInfo
    (
        LimitUID       int           NOT NULL,
        TargetID       int           NOT NULL,
        LimitQuantity  int           NOT NULL,
        BeginDate      datetime      NOT NULL,
        EndDate        datetime      NOT NULL,
        CONSTRAINT PK_LExchangeLimitInfo PRIMARY KEY CLUSTERED (LimitUID),
        CONSTRAINT CK_LExchangeLimitInfo_Quantity CHECK (LimitQuantity >= 0),
        CONSTRAINT CK_LExchangeLimitInfo_DateRange CHECK (BeginDate < EndDate)
    );
END;
GO

IF OBJECT_ID(N'dbo.LExchangeLimitItem', N'U') IS NULL
BEGIN
    CREATE TABLE dbo.LExchangeLimitItem
    (
        TargetID       int NOT NULL,
        DestItemID     int NOT NULL,
        CONSTRAINT PK_LExchangeLimitItem PRIMARY KEY CLUSTERED (TargetID, DestItemID)
    );
END;
GO

IF OBJECT_ID(N'dbo.LExchangeLimitLog', N'U') IS NULL
BEGIN
    CREATE TABLE dbo.LExchangeLimitLog
    (
        RollBackUID    int           IDENTITY(1,1) NOT NULL,
        LimitUID       int           NOT NULL,
        TargetID       int           NOT NULL,
        UnitUID        bigint        NOT NULL,
        DestItemID     int           NOT NULL,
        RegDate        datetime      NOT NULL,
        CONSTRAINT PK_LExchangeLimitLog PRIMARY KEY CLUSTERED (RollBackUID),
        CONSTRAINT FK_LExchangeLimitLog_Info FOREIGN KEY (LimitUID)
            REFERENCES dbo.LExchangeLimitInfo (LimitUID),
        CONSTRAINT FK_LExchangeLimitLog_Item FOREIGN KEY (TargetID, DestItemID)
            REFERENCES dbo.LExchangeLimitItem (TargetID, DestItemID)
    );

    CREATE INDEX IX_LExchangeLimitLog_LimitUID
        ON dbo.LExchangeLimitLog (LimitUID);
END;
GO

DECLARE @ProcedureDDL nvarchar(max);
SET @ProcedureDDL =
    CASE WHEN OBJECT_ID(N'dbo.P_LEvent_RandomEnchant_SEL', N'P') IS NULL
        THEN N'CREATE PROCEDURE dbo.P_LEvent_RandomEnchant_SEL'
        ELSE N'ALTER PROCEDURE dbo.P_LEvent_RandomEnchant_SEL'
    END + N'
AS
BEGIN
    SET NOCOUNT ON;

    SELECT  EventUID,
            CONVERT(nvarchar(32), BeginDate, 120) AS BeginDate,
            CONVERT(nvarchar(32), EndDate, 120) AS EndDate,
            EnchantLevel
    FROM dbo.LEvent_RandomEnchant
    WHERE UsedUnitUID IS NULL
    ORDER BY BeginDate, EventUID;
END;';
EXEC sys.sp_executesql @ProcedureDDL;
GO

DECLARE @ProcedureDDL nvarchar(max);
SET @ProcedureDDL =
    CASE WHEN OBJECT_ID(N'dbo.P_LEvent_RandomEnchant_UPD', N'P') IS NULL
        THEN N'CREATE PROCEDURE dbo.P_LEvent_RandomEnchant_UPD'
        ELSE N'ALTER PROCEDURE dbo.P_LEvent_RandomEnchant_UPD'
    END + N'
    @EventUID int,
    @UnitUID bigint,
    @RegDate nvarchar(30)
AS
BEGIN
    SET NOCOUNT ON;

    DECLARE @ClaimDate datetime;
    DECLARE @Result int;
    SET @ClaimDate = NULL;
    SET @Result = -1;

    BEGIN TRY
        SET @ClaimDate = CONVERT(datetime, @RegDate, 120);
    END TRY
    BEGIN CATCH
        SET @ClaimDate = NULL;
    END CATCH;

    IF @ClaimDate IS NOT NULL
    BEGIN
        UPDATE dbo.LEvent_RandomEnchant WITH (ROWLOCK)
        SET UsedUnitUID = @UnitUID,
            UsedDate = @ClaimDate
        WHERE EventUID = @EventUID
          AND UsedUnitUID IS NULL
          AND @ClaimDate >= BeginDate
          AND @ClaimDate < EndDate;

        IF @@ROWCOUNT = 1
            SET @Result = 0;
    END;

    SELECT @Result AS Result;
END;';
EXEC sys.sp_executesql @ProcedureDDL;
GO

DECLARE @ProcedureDDL nvarchar(max);
SET @ProcedureDDL =
    CASE WHEN OBJECT_ID(N'dbo.P_LExchangeLimitInfo_GET', N'P') IS NULL
        THEN N'CREATE PROCEDURE dbo.P_LExchangeLimitInfo_GET'
        ELSE N'ALTER PROCEDURE dbo.P_LExchangeLimitInfo_GET'
    END + N'
AS
BEGIN
    SET NOCOUNT ON;

    SELECT  Info.LimitUID,
            Info.TargetID,
            CASE
                WHEN Info.LimitQuantity - COUNT(Log.RollBackUID) < 0 THEN 0
                ELSE Info.LimitQuantity - COUNT(Log.RollBackUID)
            END AS LimitQuantity,
            CONVERT(nvarchar(32), Info.BeginDate, 120) AS BeginDate,
            CONVERT(nvarchar(32), Info.EndDate, 120) AS EndDate
    FROM dbo.LExchangeLimitInfo AS Info
    LEFT JOIN dbo.LExchangeLimitLog AS Log ON Log.LimitUID = Info.LimitUID
    GROUP BY Info.LimitUID, Info.TargetID, Info.LimitQuantity, Info.BeginDate, Info.EndDate
    ORDER BY Info.BeginDate, Info.LimitUID;
END;';
EXEC sys.sp_executesql @ProcedureDDL;
GO

DECLARE @ProcedureDDL nvarchar(max);
SET @ProcedureDDL =
    CASE WHEN OBJECT_ID(N'dbo.P_LExchangeLimitItem_GET', N'P') IS NULL
        THEN N'CREATE PROCEDURE dbo.P_LExchangeLimitItem_GET'
        ELSE N'ALTER PROCEDURE dbo.P_LExchangeLimitItem_GET'
    END + N'
    @TargetID int
AS
BEGIN
    SET NOCOUNT ON;

    SELECT DestItemID
    FROM dbo.LExchangeLimitItem
    WHERE TargetID = @TargetID
    ORDER BY DestItemID;
END;';
EXEC sys.sp_executesql @ProcedureDDL;
GO

DECLARE @ProcedureDDL nvarchar(max);
SET @ProcedureDDL =
    CASE WHEN OBJECT_ID(N'dbo.P_LExchangeLimitLog_INT', N'P') IS NULL
        THEN N'CREATE PROCEDURE dbo.P_LExchangeLimitLog_INT'
        ELSE N'ALTER PROCEDURE dbo.P_LExchangeLimitLog_INT'
    END + N'
    @LimitUID int,
    @UnitUID bigint,
    @DestItemID int,
    @RegDate nvarchar(30)
AS
BEGIN
    SET NOCOUNT ON;

    DECLARE @ClaimDate datetime;
    DECLARE @TargetID int;
    DECLARE @LimitQuantity int;
    DECLARE @UsedQuantity int;
    DECLARE @Result int;
    DECLARE @RollBackUID int;
    SET @ClaimDate = NULL;
    SET @Result = -1;
    SET @RollBackUID = 0;

    BEGIN TRY
        SET @ClaimDate = CONVERT(datetime, @RegDate, 120);
    END TRY
    BEGIN CATCH
        SET @ClaimDate = NULL;
    END CATCH;

    BEGIN TRY
        BEGIN TRANSACTION;

        SELECT  @TargetID = TargetID,
                @LimitQuantity = LimitQuantity
        FROM dbo.LExchangeLimitInfo WITH (UPDLOCK, HOLDLOCK)
        WHERE LimitUID = @LimitUID;

        IF @ClaimDate IS NOT NULL
           AND @TargetID IS NOT NULL
           AND @ClaimDate >= (SELECT BeginDate FROM dbo.LExchangeLimitInfo WHERE LimitUID = @LimitUID)
           AND @ClaimDate <  (SELECT EndDate FROM dbo.LExchangeLimitInfo WHERE LimitUID = @LimitUID)
           AND EXISTS
               (SELECT 1 FROM dbo.LExchangeLimitItem
                WHERE TargetID = @TargetID AND DestItemID = @DestItemID)
        BEGIN
            SELECT @UsedQuantity = COUNT(*)
            FROM dbo.LExchangeLimitLog WITH (UPDLOCK, HOLDLOCK)
            WHERE LimitUID = @LimitUID;

            IF @UsedQuantity < @LimitQuantity
            BEGIN
                INSERT INTO dbo.LExchangeLimitLog (LimitUID, TargetID, UnitUID, DestItemID, RegDate)
                VALUES (@LimitUID, @TargetID, @UnitUID, @DestItemID, @ClaimDate);

                SET @RollBackUID = CONVERT(int, SCOPE_IDENTITY());
                SET @Result = 0;
            END;
        END;

        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF XACT_STATE() <> 0
            ROLLBACK TRANSACTION;

        SET @Result = -1;
        SET @RollBackUID = 0;
    END CATCH;

    SELECT @Result AS Result, @RollBackUID AS RollBackUID;
END;';
EXEC sys.sp_executesql @ProcedureDDL;
GO

DECLARE @ProcedureDDL nvarchar(max);
SET @ProcedureDDL =
    CASE WHEN OBJECT_ID(N'dbo.P_LExchangeLimitLog_DEL', N'P') IS NULL
        THEN N'CREATE PROCEDURE dbo.P_LExchangeLimitLog_DEL'
        ELSE N'ALTER PROCEDURE dbo.P_LExchangeLimitLog_DEL'
    END + N'
    @RollBackUID int
AS
BEGIN
    SET NOCOUNT ON;

    DELETE FROM dbo.LExchangeLimitLog
    WHERE RollBackUID = @RollBackUID;

    SELECT CASE WHEN @@ROWCOUNT = 1 THEN 0 ELSE -1 END AS Result;
END;';
EXEC sys.sp_executesql @ProcedureDDL;
GO
