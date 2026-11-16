-- A unit waiting for deletion can be restored at once and deleted permanently after 24 hours (client/unit-wait-delete,
-- 2026-11-16).
--
-- P_GUnitWaitDelete_INT gave a deleted unit DelAbleDate = DelDate + 3 days and RestoreAbleDate = DelDate + 1 day, so for
-- a whole day both buttons of character select (permanent deletion and restore) stayed disabled. The US client's own
-- message after deleting (STR_ID_16103) says permanent deletion is available after one day (24 hours), and nothing in
-- it delays restoring. Now DelAbleDate = DelDate + 1 day and RestoreAbleDate = DelDate. Units already waiting
-- (GUnitWaitDelete.Deleted = 0) get the same dates.
--
-- Can be run again. Apply: sqlcmd -C -S localhost -U sa -P <password> -d Game01 -i <this file>

CREATE OR ALTER PROC [dbo].[P_GUnitWaitDelete_INT]
	@UnitUID	bigint
,	@UserUID	bigint					-- the owner, against forged deletion requests
AS
	SET NOCOUNT ON

	DECLARE @Result			int
	DECLARE @ReDelAbleDate	smalldatetime
	,		@sdtNow			smalldatetime

	SET @sdtNow	 = GETDATE()

	SELECT TOP 1 @ReDelAbleDate = ReDelAbleDate
	FROM dbo.GUnitRestore WITH(NOLOCK)
	WHERE UnitUID = @UnitUID
	ORDER BY LogUID DESC

	IF @ReDelAbleDate > GETDATE()
		SET @Result = -22	-- restored too recently to be deleted again
	ELSE IF EXISTS ( SELECT * FROM dbo.GUnitWaitDelete WITH(NOLOCK) WHERE UnitUID = @UnitUID )
		SET @Result = -1	-- already waiting for deletion
	ELSE IF NOT EXISTS ( SELECT * FROM dbo.GUnit WITH(NOLOCK) WHERE UnitUID = @UnitUID AND UserUID = @UserUID AND Deleted = 0 )
		SET @Result = -1	-- already deleted, or not this user's
	ELSE BEGIN
		BEGIN TRY
			BEGIN TRAN

			-- Mark the unit deleted and start its wait
			SET @Result = -11
			UPDATE dbo.GUnit
			SET @sdtNow = DelDate = CASE WHEN @sdtNow = RegDate THEN DATEADD(mi, 1, @sdtNow) ELSE @sdtNow END
				OUTPUT DELETED.UnitUID, INSERTED.DelDate, DATEADD(dd, 1, INSERTED.DelDate), INSERTED.DelDate, 0
				INTO dbo.GUnitWaitDelete ( UnitUID, DelDate, DelAbleDate, RestoreAbleDate, Deleted )
            WHERE UnitUID = @UnitUID
				AND Deleted = 0

			-- Remove it from the tutor system
			SET @Result = -12
			UPDATE dbo.GTutor
			SET DelDate = @sdtNow
			WHERE (TeacherUID = @UnitUID OR StudentUID = @UnitUID)
				AND DELETED = 0

			COMMIT TRAN

			SET @Result = 0
		END TRY
		BEGIN CATCH
			DECLARE @NO_XACT_STATE		smallint
			SET @NO_XACT_STATE			= XACT_STATE()

			IF @NO_XACT_STATE <> 0
				ROLLBACK TRAN

			EXEC dbo.SYS_SP_ErrorLog_INT @NO_XACT_STATE

			IF @Result = 0 OR @Result IS NULL
				SET @Result = -11
		END CATCH
	END

	SELECT ISNULL(@Result, 0) AS Result, @ReDelAbleDate AS ReDelAbleDate
GO

UPDATE dbo.GUnitWaitDelete
SET DelAbleDate = DATEADD(dd, 1, DelDate)
,	RestoreAbleDate = DelDate
WHERE Deleted = 0
GO
