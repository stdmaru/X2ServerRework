-- P_GSkill_BeforeList_INS: a copy of a unit's learned skills before its cash skill points expire
-- (server/add-character, 2026-11-15).
--
-- The GameServer calls it when a cash skill point item expires (GSGameDBThread.cpp, DBE_EXPIRE_CASH_SKILL_POINT_REQ),
-- before gup_update_cash_skill_point_info zeroes the points, and stops the expiry unless it answers 0. Our Game01 had
-- the table GSkill_BeforeList (UnitUID, SkillID, Level, CSP, RegDate) but not the procedure, and V5's is a stub.
-- Skills are kept per page since 20261115_skill_pages.sql, so the copy keeps the page too (PageNumber, added here).
--
-- Can be run again. Apply: sqlcmd -C -S localhost -U sa -P <password> -d Game01 -i <this file>

SET NOCOUNT ON;
GO

IF COL_LENGTH('dbo.GSkill_BeforeList', 'PageNumber') IS NULL
    ALTER TABLE dbo.GSkill_BeforeList ADD PageNumber int NULL;
GO

-- Result: 0, -1 failed.
CREATE OR ALTER PROCEDURE dbo.P_GSkill_BeforeList_INS
    @iUnitUID bigint
AS
SET NOCOUNT ON;

DECLARE @iOK int = 0

BEGIN TRY
    INSERT INTO dbo.GSkill_BeforeList (UnitUID, SkillID, [Level], CSP, RegDate, PageNumber)
        SELECT UnitUID, SkillID, [Level], CSP, GETDATE(), PageNumber
        FROM dbo.GSkill_New WITH (NOLOCK)
        WHERE UnitUID = @iUnitUID AND Deleted = 0
END TRY
BEGIN CATCH
    SET @iOK = -1
END CATCH

SELECT @iOK
GO
