-- The character list shows units with no GUnitPVP row (server/add-character, 2026-11-15).
--
-- gup_get_unit_info_by_useruid joined GUnitPVP, so a unit without a PvP row (10 in the development backup,
-- 2026-11-15) was missing from its account's character list. As in the V5 source's Game01, the join is a LEFT JOIN
-- and such a unit's Rating is 0.
--
-- Can be run again. Apply: sqlcmd -C -S localhost -U sa -P <password> -d Game01 -i <this file>

CREATE OR ALTER PROCEDURE [dbo].[gup_get_unit_info_by_useruid]
	@iUserUID_ [bigint]
AS
SET NOCOUNT ON;

SELECT  a.UnitUID
,       a.UnitClass
,       a.[Exp]
,       a.[Level]
,       a.GamePoint
,		ISNULL(c.Rating, 0) AS Rating
,       a.BaseHP
,       a.AtkPhysic
,       a.AtkMagic
,       a.DefPhysic
,       a.DefMagic
,       a.SPoint
,		CASE WHEN b.NickName LIKE '%[_]%' THEN '__DELETED__' ELSE b.NickName END
,       a.LastPosition
    FROM dbo.GUnit as a WITH(NOLOCK)
		LEFT JOIN dbo.GUnitPVP AS c WITH (NOLOCK) ON a.UnitUID = c.UnitUID
        JOIN dbo.GUnitNickName as b WITH(NOLOCK)
            ON a.UserUID = @iUserUID_ AND a.Deleted = 0 AND a.UnitUID = b.UnitUID	-- not the deleted units
				ORDER BY a.LastDate DESC
GO
