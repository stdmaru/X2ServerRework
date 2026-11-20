-- The daily ranking (gup_update_rank) groups units by character for its per-class ranks (UClass), and knew only
-- Ara's first path among the characters after Chung: Ara's 2nd and 3rd paths, Elesis and Add ranked with no
-- character (NULL). Every class the servers have now has its character (server/third-paths, 2026-11-20): Ara 7
-- (28, 31, 34, 118, 119, 124), Elesis 8 (29, 30, 35, 120, 121, 125), Add 9 (32, 33, 36, 122, 123, 126).
--
-- It also failed outright: it left out staff through Account.dbo.MUserAuthority, which our Account database does not
-- have (Invalid object name); the authority level is Account.dbo.MUser.AuthLevel, as in 20261115_add_character.sql.
--
-- Can be run again. Apply: sqlcmd -C -S localhost -U sa -P <password> -d Game01 -i <this file>

SET NOCOUNT ON;
GO

-- The procedure as it was, with the three characters' classes and MUser for the authority level.
CREATE OR ALTER PROCEDURE [dbo].[gup_update_rank]
	@iOK_	[int] = 0
/* 
Hwang
작성일 : 2011년 07월 26일
사용 : GameDB용 랭킹 갱신에 사용될 SP
*/

AS
SET NOCOUNT ON;
SET XACT_ABORT ON;

BEGIN TRAN

	INSERT INTO dbo.GRank ( VsRank, ExpRank, ClassVsRank, ClassExpRank, UnitUID, NickName, Win, [Level], [Exp], UnitClass, Grade, RPoint, OfficialMatchCnt, UClass, RegDate )
	SELECT ROW_NUMBER() OVER(ORDER BY Grade DESC, RPoint DESC, win DESC, UnitUID ASC) AS VsRank
	, RANK() OVER(ORDER BY [Exp] DESC, Start ASC) AS ExpRank
	, ROW_NUMBER() OVER(PARTITION BY UClass ORDER BY Grade DESC, RPoint DESC, win DESC, UnitUID ASC) AS ClassVsRank
	, RANK() OVER(PARTITION BY UClass ORDER BY [Exp] ASC) AS ClassExpRank
	, UnitUID, NickName, Win, [Level], [Exp], UnitClass, Grade, RPoint, OfficialMatchCnt
	, UClass
	, CONVERT(char(10), DATEADD(dd, -1, GETDATE()), 120) AS RegDate
	FROM (
		SELECT A.UnitUID, C.Grade, C.Win, C.RPoint, C.OfficialMatchCnt, A.[Level], A.[Exp], b.NickName, A.UnitClass, LV.Start
		, CASE
			WHEN A.UnitClass IN (1, 10, 11, 22, 100, 101, 112) THEN 1        -- Elsword
			WHEN A.UnitClass IN (3, 12, 13, 24, 102, 103, 114) THEN 3        -- Rena
			WHEN A.UnitClass IN (2, 14, 15, 23, 104, 105, 113) THEN 2        -- Aisha
			WHEN A.UnitClass IN (4, 16, 17, 25, 106, 107, 115) THEN 4        -- Raven
			WHEN A.UnitClass IN (5, 18, 19, 26, 108, 109, 116) THEN 5        -- Eve
			WHEN A.UnitClass IN (6, 20, 21, 27, 110, 111, 117) THEN 6        -- Chung
			WHEN A.UnitClass IN (7, 28, 31, 34, 118, 119, 124) THEN 7        -- Ara
			WHEN A.UnitClass IN (8, 29, 30, 35, 120, 121, 125) THEN 8        -- Elesis
			WHEN A.UnitClass IN (9, 32, 33, 36, 122, 123, 126) THEN 9        -- Add
			ELSE NULL END AS UClass											 -- None
		FROM dbo.GUnit AS A WITH(NOLOCK)
		JOIN dbo.GUnitPVP_Season2 AS C WITH(NOLOCK)
			ON A.UnitUID = C.UnitUID
		JOIN dbo.GUnitNickName AS B WITH(NOLOCK)
			ON A.UnitUID = B.UnitUID
		JOIN [Statistics].dbo.LCharLvUp AS LV WITH(NOLOCK)
			ON A.UnitUID = LV.CharUID AND A.Level = LV.Level
		WHERE A.Deleted = 0
			AND NOT EXISTS (SELECT * FROM dbo.GRank_Except WITH(NOLOCK) WHERE UserUID = A.UserUID)
			AND NOT EXISTS (SELECT * FROM Account.dbo.MUser WITH(NOLOCK) WHERE UserUID = A.UserUID AND AuthLevel > 0)
	) AS TempTable
	ORDER BY VsRank ASC
	
COMMIT TRAN

GOTO END_PROC

FAIL_TRAN:
ROLLBACK TRAN

END_PROC:
SELECT @iOK_
GO
