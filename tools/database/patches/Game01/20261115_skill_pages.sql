-- Skill pages: a unit's skills and skill slots are kept per page (server/add-character, 2026-11-15).
--
-- The servers keep up to three skill pages per unit (P_GSkill_Expand_INS with a maximum of 3) and pass the page
-- number to every skill and slot procedure, but our Game01 could not hold more than one: the primary keys of
-- GSkill_New (UnitUID, SkillID), GSkillSlot_New and GSkillSlot2_New (UnitUID) left PageNumber out, GSkill_Expand did
-- not exist, P_GSkill_Expand_SEL always answered one page, and P_GSkill_Expand_INS/UPD/UPD_Active,
-- P_GSkillSlot2_New_BUY/INS and P_GSkill_New_All_DEL_UnitClassChange_20131212 (skill reset when a class change
-- item is used) were missing. P_GSkill_New_UPD looked for the skill on any page, and P_GSkill_New_All_DEL_20131212
-- (skill reset item) reset every page instead of the one it is called for.
--
-- The design follows the V5 source's Game01 (DB_20260908), which has these keys, the table and the procedures;
-- P_GSkillSlot2_New_BUY is written here: the server calls it with the number of pages, and V5's assumed one row per
-- unit. SkillPageCNT is the number of pages (1 to 3), as the GameServer reads it (GSGameDBThread.cpp, P_GSkill_Expand_SEL).
--
-- Can be run again. Apply: sqlcmd -C -S localhost -U sa -P <password> -d Game01 -i <this file>

SET NOCOUNT ON;
GO

-- Keys with PageNumber -------------------------------------------------------------------------------------------

UPDATE dbo.GSkill_New SET PageNumber = 1 WHERE PageNumber IS NULL;
UPDATE dbo.GSkillSlot_New SET PageNumber = 1 WHERE PageNumber IS NULL;
UPDATE dbo.GSkillSlot2_New SET PageNumber = 1 WHERE PageNumber IS NULL;
GO

IF NOT EXISTS (SELECT 1 FROM sys.index_columns ic JOIN sys.columns c ON c.object_id = ic.object_id AND c.column_id = ic.column_id
               WHERE ic.object_id = OBJECT_ID('dbo.GSkill_New')
                 AND ic.index_id = INDEXPROPERTY(ic.object_id, 'PKC_GSkill_New_UnitUID', 'IndexID') AND c.name = 'PageNumber')
BEGIN
    ALTER TABLE dbo.GSkill_New DROP CONSTRAINT PKC_GSkill_New_UnitUID;
    ALTER TABLE dbo.GSkill_New ALTER COLUMN PageNumber int NOT NULL;
    ALTER TABLE dbo.GSkill_New ADD CONSTRAINT PKC_GSkill_New_UnitUID PRIMARY KEY CLUSTERED (UnitUID, SkillID, PageNumber);
END
GO

IF NOT EXISTS (SELECT 1 FROM sys.index_columns ic JOIN sys.columns c ON c.object_id = ic.object_id AND c.column_id = ic.column_id
               WHERE ic.object_id = OBJECT_ID('dbo.GSkillSlot_New')
                 AND ic.index_id = INDEXPROPERTY(ic.object_id, 'PKC_GSkillSlot_New_UnitUID', 'IndexID') AND c.name = 'PageNumber')
BEGIN
    ALTER TABLE dbo.GSkillSlot_New DROP CONSTRAINT PKC_GSkillSlot_New_UnitUID;
    ALTER TABLE dbo.GSkillSlot_New ALTER COLUMN PageNumber int NOT NULL;
    ALTER TABLE dbo.GSkillSlot_New ADD CONSTRAINT PKC_GSkillSlot_New_UnitUID PRIMARY KEY CLUSTERED (UnitUID, PageNumber);
END
GO

IF NOT EXISTS (SELECT 1 FROM sys.index_columns ic JOIN sys.columns c ON c.object_id = ic.object_id AND c.column_id = ic.column_id
               WHERE ic.object_id = OBJECT_ID('dbo.GSkillSlot2_New')
                 AND ic.index_id = INDEXPROPERTY(ic.object_id, 'PKC_GSkillSlot2_New_UnitUID', 'IndexID') AND c.name = 'PageNumber')
BEGIN
    ALTER TABLE dbo.GSkillSlot2_New DROP CONSTRAINT PKC_GSkillSlot2_New_UnitUID;
    ALTER TABLE dbo.GSkillSlot2_New ALTER COLUMN PageNumber int NOT NULL;
    ALTER TABLE dbo.GSkillSlot2_New ADD CONSTRAINT PKC_GSkillSlot2_New_UnitUID PRIMARY KEY CLUSTERED (UnitUID, PageNumber);
END
GO

-- Pages per unit: a row only once the unit has expanded (no row = one page, the first active) --------------------

IF OBJECT_ID('dbo.GSkill_Expand', 'U') IS NULL
    CREATE TABLE dbo.GSkill_Expand (
        UnitUID      bigint   NOT NULL CONSTRAINT PKC_GSkill_Expand_UnitUID PRIMARY KEY CLUSTERED,
        SkillPageCNT int      NOT NULL CONSTRAINT DF_GSkill_Expand_SkillPageCNT DEFAULT (1),
        ActivePage   int      NOT NULL CONSTRAINT DF_GSkill_Expand_ActivePage DEFAULT (1),
        RegDate      datetime NOT NULL CONSTRAINT DF_GSkill_Expand_RegDate DEFAULT (GETDATE()),
        ModDate      datetime NOT NULL CONSTRAINT DF_GSkill_Expand_ModDate DEFAULT (GETDATE())
    );
GO

-- Result: number of pages, active page.
CREATE OR ALTER PROCEDURE dbo.P_GSkill_Expand_SEL
    @iUnitUID bigint
AS
    SET NOCOUNT ON

    IF EXISTS (SELECT 1 FROM dbo.GSkill_Expand WITH(NOLOCK) WHERE UnitUID = @iUnitUID)
        SELECT SkillPageCNT, ActivePage FROM dbo.GSkill_Expand WITH(NOLOCK) WHERE UnitUID = @iUnitUID
    ELSE
        SELECT 1, 1
GO

-- Adds a page. Result: 0, -1 no such unit, -3 already at @iMaxPage, -2/-4/-9 failed.
CREATE OR ALTER PROCEDURE dbo.P_GSkill_Expand_INS
    @iUnitUID bigint
,   @iMaxPage int
AS
    SET NOCOUNT ON

    DECLARE @iResult int, @iCnt int

    IF NOT EXISTS (SELECT 1 FROM dbo.GUnit WITH(NOLOCK) WHERE UnitUID = @iUnitUID AND Deleted = 0)
    BEGIN
        SELECT -1
        RETURN
    END

    SELECT @iCnt = SkillPageCNT FROM dbo.GSkill_Expand WITH(NOLOCK) WHERE UnitUID = @iUnitUID

    IF ISNULL(@iCnt, 1) >= @iMaxPage
    BEGIN
        SELECT -3
        RETURN
    END

    BEGIN TRY
        BEGIN TRAN
        IF @iCnt IS NULL
        BEGIN
            SET @iResult = -2
            INSERT INTO dbo.GSkill_Expand (UnitUID, SkillPageCNT, ActivePage) VALUES (@iUnitUID, 2, 1)
        END
        ELSE
        BEGIN
            SET @iResult = -4
            UPDATE dbo.GSkill_Expand WITH(ROWLOCK)
                SET SkillPageCNT = SkillPageCNT + 1, ModDate = GETDATE()
                WHERE UnitUID = @iUnitUID
        END
        COMMIT TRAN
        SET @iResult = 0
    END TRY
    BEGIN CATCH
        IF XACT_STATE() <> 0
            ROLLBACK TRAN
        IF @iResult = 0 OR @iResult IS NULL
            SET @iResult = -9
    END CATCH

    SELECT @iResult
GO

-- Chooses the active page. Result: 0, -1 never expanded (and page > 1), -2 invalid page or failed.
CREATE OR ALTER PROCEDURE dbo.P_GSkill_Expand_UPD
    @iUnitUID    bigint
,   @iActivePage int
AS
    SET NOCOUNT ON

    DECLARE @iCnt int
    SELECT @iCnt = SkillPageCNT FROM dbo.GSkill_Expand WITH(NOLOCK) WHERE UnitUID = @iUnitUID

    IF @iCnt IS NULL
    BEGIN
        SELECT CASE WHEN @iActivePage = 1 THEN 0 ELSE -1 END
        RETURN
    END

    IF @iActivePage < 1 OR @iActivePage > @iCnt
    BEGIN
        SELECT -2
        RETURN
    END

    UPDATE dbo.GSkill_Expand WITH(ROWLOCK)
        SET ActivePage = @iActivePage, ModDate = GETDATE()
        WHERE UnitUID = @iUnitUID

    IF @@ERROR <> 0 OR @@ROWCOUNT <> 1
        SELECT -2
    ELSE
        SELECT 0
GO

CREATE OR ALTER PROCEDURE dbo.P_GSkill_Expand_UPD_Active
    @iUnitUID    bigint
,   @iActivePage int
AS
    SET NOCOUNT ON
    EXEC dbo.P_GSkill_Expand_UPD @iUnitUID, @iActivePage
GO

-- A skill's level and cash points on one page. Result: 0, -1 no such unit or failed, -10/-2 failed.
CREATE OR ALTER PROCEDURE [dbo].[P_GSkill_New_UPD]
    @iUnitUID         bigint
,   @iSkillID         int
,   @iSkillLevel      int
,   @iSkillPoint      int
,   @iSkillPageNumber int
AS
SET NOCOUNT ON;

DECLARE @Result int

    IF NOT EXISTS (SELECT * FROM dbo.GUnit WITH(NOLOCK) WHERE UnitUID = @iUnitUID AND Deleted = 0)
        SET @Result = -1
    ELSE BEGIN
        BEGIN TRY
            BEGIN TRAN
                IF NOT EXISTS (SELECT SkillID FROM dbo.GSkill_New WITH (NOLOCK)
                               WHERE UnitUID = @iUnitUID AND SkillID = @iSkillID AND PageNumber = @iSkillPageNumber) BEGIN
                    SET @Result = -10
                    INSERT INTO dbo.GSkill_New (UnitUID, SkillID, [Level], CSP, PageNumber)
                        VALUES (@iUnitUID, @iSkillID, @iSkillLevel, @iSkillPoint, @iSkillPageNumber)
                END
                ELSE BEGIN
                    SET @Result = -2
                    UPDATE dbo.GSkill_New
                        SET [Level] = @iSkillLevel, CSP = @iSkillPoint
                        WHERE UnitUID = @iUnitUID AND SkillID = @iSkillID AND PageNumber = @iSkillPageNumber
                END
            COMMIT TRAN
            SET @Result = 0
        END TRY
        BEGIN CATCH
            DECLARE @NO_XACT_STATE smallint
            SET @NO_XACT_STATE = XACT_STATE()
            IF @NO_XACT_STATE <> 0
                ROLLBACK TRAN
            EXEC dbo.SYS_SP_ErrorLog_INT @NO_XACT_STATE
            IF @Result = 0 OR @Result IS NULL
                SET @Result = -1
        END CATCH
    END
    SELECT ISNULL(@Result, 0) AS Result
GO

-- Skill reset item, for one page: every skill but the basic ones is removed, the basic ones go back to level 1
-- (added if missing), cash points are restored and the page's slots emptied. Result: 0, or the failed step.
CREATE OR ALTER PROCEDURE dbo.P_GSkill_New_All_DEL_20131212
    @iUnitUID     bigint
,   @iCSPoint     int
,   @iBasicSkill1 int
,   @iBasicSkill2 int
,   @iBasicSkill3 int
,   @iBasicSkill4 int
,   @iBasicSkill5 int
,   @iBasicSkill6 int
,   @iPageNumber  int
AS
    SET NOCOUNT ON

    DECLARE @Result int
    DECLARE @Basic TABLE (SkillID int PRIMARY KEY)
    INSERT INTO @Basic (SkillID)
        SELECT DISTINCT v.SkillID FROM (VALUES (@iBasicSkill1), (@iBasicSkill2), (@iBasicSkill3),
                                               (@iBasicSkill4), (@iBasicSkill5), (@iBasicSkill6)) AS v (SkillID)
        WHERE v.SkillID > 0

    BEGIN TRY
        BEGIN TRAN

        SET @Result = -1
        DELETE dbo.GSkill_New
            WHERE UnitUID = @iUnitUID AND PageNumber = @iPageNumber AND SkillID NOT IN (SELECT SkillID FROM @Basic)

        SET @Result = -6
        UPDATE dbo.GSkill_New SET [Level] = 1, CSP = 0
            WHERE UnitUID = @iUnitUID AND PageNumber = @iPageNumber
        INSERT INTO dbo.GSkill_New (UnitUID, SkillID, [Level], CSP, PageNumber)
            SELECT @iUnitUID, b.SkillID, 1, 0, @iPageNumber FROM @Basic AS b
            WHERE NOT EXISTS (SELECT 1 FROM dbo.GSkill_New AS s
                              WHERE s.UnitUID = @iUnitUID AND s.SkillID = b.SkillID AND s.PageNumber = @iPageNumber)

        IF @iCSPoint > 0 AND EXISTS (SELECT 1 FROM dbo.GSkill_Cash WITH(NOLOCK) WHERE UnitUID = @iUnitUID)
        BEGIN
            SET @Result = -3
            UPDATE dbo.GSkill_Cash WITH(ROWLOCK) SET CsPoint = @iCSPoint WHERE UnitUID = @iUnitUID
        END

        SET @Result = -4
        UPDATE dbo.GSkillSlot_New WITH(ROWLOCK) SET Slot01 = 0, Slot02 = 0, Slot03 = 0, Slot04 = 0
            WHERE UnitUID = @iUnitUID AND PageNumber = @iPageNumber

        SET @Result = -5
        UPDATE dbo.GSkillSlot2_New WITH(ROWLOCK) SET Slot01 = 0, Slot02 = 0, Slot03 = 0, Slot04 = 0
            WHERE UnitUID = @iUnitUID AND PageNumber = @iPageNumber

        COMMIT TRAN
        SET @Result = 0
    END TRY
    BEGIN CATCH
        DECLARE @NO_XACT_STATE smallint
        SET @NO_XACT_STATE = XACT_STATE()
        IF @NO_XACT_STATE <> 0
            ROLLBACK TRAN
        EXEC dbo.SYS_SP_ErrorLog_INT @NO_XACT_STATE
        IF @Result = 0 OR @Result IS NULL
            SET @Result = -9
    END CATCH

    SELECT ISNULL(@Result, 0) AS Result
GO

-- The same reset when a class change item is used, which also gives back every skill point (@iSPointMax).
CREATE OR ALTER PROCEDURE dbo.P_GSkill_New_All_DEL_UnitClassChange_20131212
    @iUnitUID     bigint
,   @iSPointMax   int
,   @iCSPoint     int
,   @iBasicSkill1 int
,   @iBasicSkill2 int
,   @iBasicSkill3 int
,   @iBasicSkill4 int
,   @iBasicSkill5 int
,   @iBasicSkill6 int
,   @iPageNumber  int
AS
    SET NOCOUNT ON

    DECLARE @Result int
    DECLARE @Basic TABLE (SkillID int PRIMARY KEY)
    INSERT INTO @Basic (SkillID)
        SELECT DISTINCT v.SkillID FROM (VALUES (@iBasicSkill1), (@iBasicSkill2), (@iBasicSkill3),
                                               (@iBasicSkill4), (@iBasicSkill5), (@iBasicSkill6)) AS v (SkillID)
        WHERE v.SkillID > 0

    BEGIN TRY
        BEGIN TRAN

        SET @Result = -1
        DELETE dbo.GSkill_New
            WHERE UnitUID = @iUnitUID AND PageNumber = @iPageNumber AND SkillID NOT IN (SELECT SkillID FROM @Basic)

        SET @Result = -6
        UPDATE dbo.GSkill_New SET [Level] = 1, CSP = 0
            WHERE UnitUID = @iUnitUID AND PageNumber = @iPageNumber
        INSERT INTO dbo.GSkill_New (UnitUID, SkillID, [Level], CSP, PageNumber)
            SELECT @iUnitUID, b.SkillID, 1, 0, @iPageNumber FROM @Basic AS b
            WHERE NOT EXISTS (SELECT 1 FROM dbo.GSkill_New AS s
                              WHERE s.UnitUID = @iUnitUID AND s.SkillID = b.SkillID AND s.PageNumber = @iPageNumber)

        SET @Result = -2
        UPDATE dbo.GUnit WITH(ROWLOCK) SET SPoint = @iSPointMax WHERE UnitUID = @iUnitUID

        IF @iCSPoint > 0 AND EXISTS (SELECT 1 FROM dbo.GSkill_Cash WITH(NOLOCK) WHERE UnitUID = @iUnitUID)
        BEGIN
            SET @Result = -3
            UPDATE dbo.GSkill_Cash WITH(ROWLOCK) SET CsPoint = @iCSPoint WHERE UnitUID = @iUnitUID
        END

        SET @Result = -4
        UPDATE dbo.GSkillSlot_New WITH(ROWLOCK) SET Slot01 = 0, Slot02 = 0, Slot03 = 0, Slot04 = 0
            WHERE UnitUID = @iUnitUID AND PageNumber = @iPageNumber

        SET @Result = -5
        UPDATE dbo.GSkillSlot2_New WITH(ROWLOCK) SET Slot01 = 0, Slot02 = 0, Slot03 = 0, Slot04 = 0
            WHERE UnitUID = @iUnitUID AND PageNumber = @iPageNumber

        COMMIT TRAN
        SET @Result = 0
    END TRY
    BEGIN CATCH
        DECLARE @NO_XACT_STATE smallint
        SET @NO_XACT_STATE = XACT_STATE()
        IF @NO_XACT_STATE <> 0
            ROLLBACK TRAN
        EXEC dbo.SYS_SP_ErrorLog_INT @NO_XACT_STATE
        IF @Result = 0 OR @Result IS NULL
            SET @Result = -9
    END CATCH

    SELECT ISNULL(@Result, 0) AS Result
GO

-- Skill slot B, bought for @iPeriod days (0 = permanent) on pages 1 to @iNumPages. The server calls it once per
-- page, always with the number of pages, so a second call changes nothing. Result: iOK (0, -11/-12 failed), EndDate.
CREATE OR ALTER PROCEDURE [dbo].[P_GSkillSlot2_New_BUY]
    @iUnitUID  bigint
,   @iPeriod   int
,   @iNumPages int = 1
AS
SET NOCOUNT ON;

DECLARE @iOK int = 0
,       @sdtEnd smalldatetime = CASE WHEN @iPeriod = 0 THEN '2049-12-31 23:59' ELSE DATEADD(DD, @iPeriod, GETDATE()) END
,       @iPage int = 1

BEGIN TRY
    BEGIN TRAN
    WHILE @iPage <= CASE WHEN @iNumPages < 1 THEN 1 ELSE @iNumPages END
    BEGIN
        IF NOT EXISTS (SELECT 1 FROM dbo.GSkillSlot2_New WITH (NOLOCK) WHERE UnitUID = @iUnitUID AND PageNumber = @iPage)
        BEGIN
            SET @iOK = -11
            INSERT INTO dbo.GSkillSlot2_New (UnitUID, Slot01, Slot02, Slot03, Slot04, PageNumber, EndDate)
                VALUES (@iUnitUID, 0, 0, 0, 0, @iPage, @sdtEnd)
        END
        ELSE
        BEGIN
            SET @iOK = -12
            UPDATE dbo.GSkillSlot2_New SET EndDate = @sdtEnd WHERE UnitUID = @iUnitUID AND PageNumber = @iPage
        END
        SET @iPage += 1
    END
    COMMIT TRAN
    SET @iOK = 0
END TRY
BEGIN CATCH
    IF XACT_STATE() <> 0
        ROLLBACK TRAN
    IF @iOK = 0
        SET @iOK = -11
END CATCH

SELECT @iOK, @sdtEnd
GO

-- A page's skill slot B. An empty @wstrEndDate keeps the stored end date. Result: 0, -1 no such unit or failed.
CREATE OR ALTER PROCEDURE [dbo].[P_GSkillSlot2_New_INS]
    @iUnitUID    bigint
,   @iSlot01     int
,   @iSlot02     int
,   @iSlot03     int
,   @iSlot04     int
,   @PageNumber  int
,   @wstrEndDate nvarchar(300) = NULL
AS
SET NOCOUNT ON;

    DECLARE @iOK int = 0

    IF NOT EXISTS (SELECT * FROM dbo.GUnit WITH(NOLOCK) WHERE UnitUID = @iUnitUID AND Deleted = 0)
        SET @iOK = -1
    ELSE BEGIN
        BEGIN TRY
            BEGIN TRAN
                IF NOT EXISTS (SELECT UnitUID FROM dbo.GSkillSlot2_New WITH (NOLOCK) WHERE UnitUID = @iUnitUID AND PageNumber = @PageNumber) BEGIN
                    SET @iOK = -10
                    INSERT INTO dbo.GSkillSlot2_New (UnitUID, Slot01, Slot02, Slot03, Slot04, PageNumber, EndDate)
                        VALUES (@iUnitUID, @iSlot01, @iSlot02, @iSlot03, @iSlot04, @PageNumber,
                                ISNULL(NULLIF(@wstrEndDate, N''), '1900-01-01 00:00:00'))
                END
                ELSE BEGIN
                    SET @iOK = -2
                    UPDATE dbo.GSkillSlot2_New
                        SET Slot01 = @iSlot01, Slot02 = @iSlot02, Slot03 = @iSlot03, Slot04 = @iSlot04,
                            EndDate = ISNULL(NULLIF(@wstrEndDate, N''), EndDate)
                        WHERE UnitUID = @iUnitUID AND PageNumber = @PageNumber
                END
            COMMIT TRAN
            SET @iOK = 0
        END TRY
        BEGIN CATCH
            DECLARE @NO_XACT_STATE smallint
            SET @NO_XACT_STATE = XACT_STATE()
            IF @NO_XACT_STATE <> 0
                ROLLBACK TRAN
            EXEC dbo.SYS_SP_ErrorLog_INT @NO_XACT_STATE
            IF @iOK = 0 OR @iOK IS NULL
                SET @iOK = -1
        END CATCH
    END
    SELECT @iOK
GO
