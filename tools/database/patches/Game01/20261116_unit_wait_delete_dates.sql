-- A unit waiting for deletion can be restored at once and deleted permanently after 24 hours (client/unit-wait-delete,
-- 2026-11-16).
--
-- P_GUnitWaitDelete_INT gave a deleted unit DelAbleDate = DelDate + 3 days and RestoreAbleDate = DelDate + 1 day, so for
-- a whole day both buttons of character select (permanent deletion and restore) stayed disabled. The US client's own
-- message after deleting (STR_ID_16103) says permanent deletion is available after one day (24 hours), and nothing in
-- it delays restoring. Now DelAbleDate = DelDate + 1 day and RestoreAbleDate = DelDate (less a minute, below). Units already waiting
-- (GUnitWaitDelete.Deleted = 0) get the same dates.
--
-- Admin accounts (Account.dbo.MUser.AuthLevel >= 2, UAL_GM and above) can delete permanently at once, to test
-- characters and delete them right after; P_GUnitRestore_SET lets them delete a restored unit again at once too
-- (others wait a day, ReDelAbleDate). Account is read across databases: it is on the same SQL Server as Game01.
-- The dates are one minute in the past because smalldatetime rounds GETDATE() to the nearest minute, up to 30 s in
-- the future, and the client checks them only when the unit is selected.
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
	,		@iWaitMinutes	int

	SET @sdtNow	 = GETDATE()

	-- Minutes before permanent deletion opens: a day, none for admin accounts
	IF EXISTS ( SELECT * FROM Account.dbo.MUser WITH(NOLOCK) WHERE UserUID = @UserUID AND AuthLevel >= 2 )
		SET @iWaitMinutes = -1
	ELSE
		SET @iWaitMinutes = 24 * 60

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
				OUTPUT DELETED.UnitUID, INSERTED.DelDate, DATEADD(mi, @iWaitMinutes, INSERTED.DelDate), DATEADD(mi, -1, INSERTED.DelDate), 0
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

CREATE OR ALTER PROC [dbo].[P_GUnitRestore_SET]
	@UnitUID	bigint
,	@UserUID	bigint					-- the owner, against forged restore requests
AS
	SET NOCOUNT ON

	DECLARE @Result		int
	DECLARE @sdtNow 			smalldatetime
	DECLARE @RestoreAbleDate	smalldatetime
	DECLARE @ReDelAbleDate		smalldatetime

	SELECT @RestoreAbleDate = RestoreAbleDate, @sdtNow = GETDATE()
	FROM dbo.GUnitWaitDelete WITH(NOLOCK)
	WHERE UnitUID = @UnitUID
		AND Deleted = 0

	IF @@ROWCOUNT = 0
		SET @Result = -31	-- not waiting for deletion
	ELSE IF @RestoreAbleDate > GETDATE()
		SET @Result = -32	-- cannot be restored yet
	ELSE IF NOT EXISTS ( SELECT * FROM dbo.GUnit WITH(NOLOCK) WHERE UnitUID = @UnitUID AND UserUID = @UserUID AND Deleted = 1 )
		SET @Result = -33	-- not deleted, or not this user's
	ELSE BEGIN
		-- A restored unit can be deleted again after a day, at once on admin accounts
		IF EXISTS ( SELECT * FROM Account.dbo.MUser WITH(NOLOCK) WHERE UserUID = @UserUID AND AuthLevel >= 2 )
			SET @ReDelAbleDate = DATEADD(mi, -1, @sdtNow)
		ELSE
			SET @ReDelAbleDate = DATEADD(dd, 1, @sdtNow)

		BEGIN TRY
			BEGIN TRAN

			-- Back to a normal unit
			SET @Result = -11
			UPDATE dbo.GUnit
			SET DelDate = RegDate
            WHERE UnitUID = @UnitUID
				AND Deleted = 1

			SET @Result = -34
			DELETE dbo.GUnitWaitDelete
				OUTPUT DELETED.UnitUID, DELETED.DelDate, @sdtNow, @ReDelAbleDate
				INTO dbo.GUnitRestore ( UnitUID, DelDate, RestoreDate, ReDelAbleDate )
			WHERE UnitUID = @UnitUID

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

	SELECT ISNULL(@Result, 0) AS Result, ISNULL(@RestoreAbleDate, '2079-06-05') AS RestoreAbleDate
GO

-- Units already waiting get the same dates
UPDATE w
SET DelAbleDate = CASE WHEN m.AuthLevel >= 2 THEN DATEADD(mi, -1, w.DelDate) ELSE DATEADD(dd, 1, w.DelDate) END
,	RestoreAbleDate = DATEADD(mi, -1, w.DelDate)
FROM dbo.GUnitWaitDelete AS w
	JOIN dbo.GUnit AS u ON u.UnitUID = w.UnitUID
	LEFT JOIN Account.dbo.MUser AS m ON m.UserUID = u.UserUID
WHERE w.Deleted = 0
GO
