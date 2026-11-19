-- The squares the CenterServer opens at startup (docs/original/squares.md, 2026-11-18).
--
-- LSquareTable has one row per square: the CenterServer's IP (LServerList.PublicIP of its row), the square's
-- type and its name. Only types 0 (Square) and 4 (Square_Market) have a map the client can use; types 1-3 have no
-- start positions (Besma, Elder) or no world script at all (Ruben: the client crashes). Six markets of type 4, plus
-- the type 0 test square. Run again safely: it rewrites the rows of this IP.
USE [Statistics];
GO

DECLARE @ip NVARCHAR(20) = N'10.226.10.2';  -- the CenterServer's IP in LServerList

DELETE FROM dbo.LSquareTable WHERE IP = @ip;

INSERT INTO dbo.LSquareTable (IP, Type, Name) VALUES
    (@ip, 0, N'Test'),
    (@ip, 4, N'Mercado 1'),
    (@ip, 4, N'Mercado 2'),
    (@ip, 4, N'Mercado 3'),
    (@ip, 4, N'Mercado 4'),
    (@ip, 4, N'Mercado 5'),
    (@ip, 4, N'Mercado 6');
GO
