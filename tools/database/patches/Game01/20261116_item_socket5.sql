-- The fifth socket and expanded sockets in mail, the personal-shop agency and the shared bank
-- (tools/db-missing-procedures, 2026-11-16).
--
-- The 2013 item revamp gave items a fifth socket and an expanded-socket count (GItemSocket.Socket5,
-- ExpandedSocketNum; P_GItemSocket_SET, P_GItem_ItemList_SEL). Our mail, agency and bank procedures predate it:
--   P_GPost_INS              the GameServer sends Socket1-5 and ExpandedSocketNum before the attributes
--                            (GSGameDBThread.cpp DBE_PREPARE_INSERT_LETTER_TO_POST_REQ, 23 values); the procedure
--                            took 21, so every letter failed with "too many arguments".
--   P_Gpost_SEL              the server reads Socket1-5 and ExpandedSocketNum (28 columns); the procedure returned
--                            26, so the mailbox list failed (DB Query Exception in ON_DBE_GET_POST_LETTER_LIST_REQ).
--   P_GItem_GET_BankItem_New the shared bank: the same two columns were missing (21 read, 19 returned).
--   P_GPShopItem_SEL         the agency's items (27 read, 25 returned).
--   P_GPShopItem_INT_DEL_A_New/_B_New/_C_New (through P_GItem_Total_Pshop_New, LoginServer): an item registered in
--                            the agency lost its fifth socket and expanded sockets, and got them back neither when
--                            sold nor when taken back.
-- GPost_Socket and GPShopItem get the two columns (NULL in old rows, read as 0). The procedures are ours with the
-- columns added; nothing else changes.
--
-- Can be run again. Apply: sqlcmd -C -S localhost -U sa -P <password> -d Game01 -i <this file>

SET NOCOUNT ON;
GO

IF COL_LENGTH('dbo.GPost_Socket', 'Socket5') IS NULL
    ALTER TABLE dbo.GPost_Socket ADD Socket5 int NULL;
IF COL_LENGTH('dbo.GPost_Socket', 'ExpandedSocketNum') IS NULL
    ALTER TABLE dbo.GPost_Socket ADD ExpandedSocketNum tinyint NULL;
IF COL_LENGTH('dbo.GPShopItem', 'Socket5') IS NULL
    ALTER TABLE dbo.GPShopItem ADD Socket5 int NULL;
IF COL_LENGTH('dbo.GPShopItem', 'ExpandedSocketNum') IS NULL
    ALTER TABLE dbo.GPShopItem ADD ExpandedSocketNum tinyint NULL;
GO

-- =============================================
-- Author:		Hwang
-- Create date: 2013.05.27
-- Description:	store a letter (random sockets added)
-- =============================================
CREATE OR ALTER PROCEDURE [dbo].[P_GPost_INS]
	@FromUnitUID		bigint
,	@ToUnitNickName		nvarchar(16)
,	@Quantity			int
,	@ScType				tinyint
,	@ScIndex			int
,	@Title				nvarchar(38)
,	@Message			nvarchar(800)
,	@ELevel				smallint
,	@Socket1			int
,	@Socket2			int
,	@Socket3			int
,	@Socket4			int
,	@Socket5			int
,	@ExpandedSocketNum	tinyint
,	@Attribute1			tinyint
,	@Attribute2			tinyint
,	@Attribute3			tinyint
,	@iSealCnt			tinyint
,	@Socket1R			int
,	@Socket2R			int
,	@Socket3R			int
,	@Socket4R			int
,	@Socket5R			int

AS
	SET NOCOUNT ON

	-- was a parameter, now a variable
	DECLARE @Result		int

	DECLARE @sdtNow		smalldatetime
	,	@PostNo			bigint
	,	@ToUnitUID		bigint

	SELECT @ToUnitUID = UnitUID, @sdtNow = GETDATE()
	FROM dbo.GunitNickName WITH(NOLOCK)
    WHERE NickName = @ToUnitNickName


	BEGIN TRY
		BEGIN TRAN

		-- put it in the post office
		SET @Result = -1
		INSERT INTO dbo.GPost (FromUnitUID, ToUnitUID, Quantity, ScType, ScIndex, RegDate, DelDate, Title, Message )
		VALUES ( @FromUnitUID, @ToUnitUID, @Quantity, @ScType, @ScIndex, @sdtNow, @sdtNow, @Title, @Message )

		SET @PostNo = SCOPE_IDENTITY()

		-- done when the item has no options
		IF @ELevel = 0 AND @Socket1 = 0 AND @Socket2 = 0 AND @Socket3 = 0 AND @Socket4 = 0 AND @Socket5 = 0 AND @ExpandedSocketNum = 0 
			AND @Attribute1 = 0 AND @Attribute2 = 0 AND @Attribute3 = 0 AND @iSealCnt = 0 
			AND @Socket1R = 0 AND @Socket2R = 0 AND @Socket3R = 0 AND @Socket4R = 0 AND @Socket5R = 0
		BEGIN SET @Result = 0 END
		
		ELSE BEGIN		
				-- enchanted,
				IF @ELevel > 0 BEGIN
					SET @Result = -2

					INSERT INTO dbo.GPost_Enchant (PostNo, ELevel)
					VALUES ( @PostNo, @ELevel )
				END

				-- sockets,
				IF ISNULL(@Socket1, 0) <> 0 OR ISNULL(@Socket2, 0) <> 0 OR ISNULL(@Socket3, 0) <> 0 OR ISNULL(@Socket4, 0) <> 0 OR ISNULL(@Socket5, 0) <> 0 OR ISNULL(@ExpandedSocketNum, 0) <> 0 BEGIN
					SET @Result = -3
					INSERT INTO dbo.GPost_Socket ( PostNo, Socket1, Socket2, Socket3, Socket4, Socket5, ExpandedSocketNum )
					VALUES ( @PostNo, @Socket1 , @Socket2 , @Socket3 , @Socket4, @Socket5, @ExpandedSocketNum )
				END

				-- attributes,
				IF (@Attribute1 + @Attribute2 + @Attribute3) > 0 BEGIN
					SET @Result = -4
					INSERT INTO GPost_Attribute ( PostNo, Attribute1, Attribute2, Attribute3 )
					VALUES ( @PostNo, @Attribute1, @Attribute2, @Attribute3 )
				END

				-- seal count
				IF (@iSealCnt > 0) BEGIN
					SET @Result = -5
					INSERT INTO dbo.GPost_Seal ( PostNo, SealCnt )
					VALUES ( @PostNo, @iSealCnt )
				END
				
				-- random sockets,
				IF ISNULL(@Socket1R, 0) <> 0 OR ISNULL(@Socket2R, 0) <> 0 OR ISNULL(@Socket3R, 0) <> 0 OR ISNULL(@Socket4R, 0) <> 0 OR ISNULL(@Socket5R, 0) <> 0 BEGIN
					SET @Result = -6
					INSERT INTO dbo.GPost_Socket_Random ( PostNo, Socket1, Socket2, Socket3, Socket4, Socket5 )
					VALUES ( @PostNo, @Socket1R , @Socket2R , @Socket3R , @Socket4R, @Socket5R )
				END
		END

		COMMIT TRAN

		SET @Result = 0
	END TRY
	BEGIN CATCH
		DECLARE @NO_XACT_STATE		smallint
		SET @NO_XACT_STATE			= XACT_STATE()		-- transaction state

		IF @NO_XACT_STATE <> 0
			ROLLBACK TRAN

		EXEC dbo.SYS_SP_ErrorLog_INT @NO_XACT_STATE

		IF @Result = 0 OR @Result IS NULL
			SET @Result = -1
	END CATCH

	SELECT ISNULL(@Result, 0) AS Result, @PostNo AS PostNo, @ToUnitUID AS ToUnitUID, @sdtNow AS RegDate

GO

-- =============================================
-- Author:		Hwang
-- Create date: 2013.05.27
-- Description:	read the mailbox (random sockets added)
-- =============================================
CREATE OR ALTER PROCEDURE [dbo].[P_Gpost_SEL]
	@UnitUID	bigint	
,	@iOK		int = 0
AS
SET NOCOUNT ON;

	SELECT A.PostNo, ISNULL(B.nickname,'__DELETED__'), A.toUnitUID, A.Quantity, A.ScType, A.ScIndex, A.RegDate, A.IsReceive, A.IsRead
		 , A.title, A.[Message], C.ELevel, D.socket1, D.socket2, D.socket3, D.socket4, D.Socket5, ISNULL(D.ExpandedSocketNum, 0), E.Attribute1, E.Attribute2, E.Attribute3
		 , G.Socket1, G.Socket2, G.Socket3, G.Socket4, G.Socket5, F.SealCnt, A.FromUnitUID
		FROM dbo.GPost AS A WITH (NOLOCK)
			JOIN dbo.GUnitNickName AS B WITH (NOLOCK)
				ON a.FromUnitUID = b.UnitUID
			LEFT OUTER JOIN dbo.GPost_Enchant AS C WITH (NOLOCK)		-- enchant
				ON A.PostNo = C.PostNo
			LEFT OUTER JOIN dbo.GPost_Socket AS D WITH (NOLOCK)			-- socket
				ON A.PostNo = D.PostNo
			LEFT OUTER JOIN dbo.GPost_Attribute AS E WITH (NOLOCK)		-- attribute
				ON A.PostNo = E.PostNo
			LEFT OUTER JOIN dbo.GPost_Seal AS F WITH (NOLOCK)			-- seal
				ON A.PostNo = F.PostNo
			LEFT OUTER JOIN dbo.GPost_Socket_Random AS G WITH (NOLOCK)  -- random sockets
				ON A.PostNo = G.PostNo	
		WHERE a.ToUnitUID = @UnitUID AND a.DELETED = 0

GO

CREATE OR ALTER PROCEDURE [dbo].[P_GItem_GET_BankItem_New]
	@UnitUID			bigint
AS
	SET NOCOUNT ON

	SELECT
		ITM.ItemUID
	,	ITM.ItemID
	,   ITM.UsageType
	,	ISNULL(ITM.Quantity, 0) AS Quantity
	,	ISNULL(ITM.Endurance, -1) AS Endurance
	,	ISNULL(PER.Period, 0) AS Period
	,	PER.ExpirationDate
	,	ISNULL(ENC.Elevel, 0) AS Elevel
	,	ISNULL(SOC.Socket1, 0) AS Socket1
	,	ISNULL(SOC.Socket2, 0) AS Socket2
	,	ISNULL(SOC.Socket3, 0) AS Socket3
	,	ISNULL(SOC.Socket4, 0) AS Socket4
	,	ISNULL(SOC.Socket5, 0) AS Socket5
	,	ISNULL(SOC.ExpandedSocketNum, 0) AS ExpandedSocketNum
	,	ISNULL(RSC.Socket1, 0) AS RSocket1
	,	ISNULL(RSC.Socket2, 0) AS RSocket2
	,	ISNULL(RSC.Socket3, 0) AS RSocket3
	,	ISNULL(RSC.Socket4, 0) AS RSocket4
	,	ISNULL(RSC.Socket5, 0) AS RSocket5
	,	ITM.InventoryCategory
	,	ITM.SlotID
	FROM dbo.GItem						AS ITM WITH(NOLOCK)
	LEFT OUTER JOIN dbo.GItemEnchant	AS ENC WITH(NOLOCK) ON ENC.UnitUID = @UnitUID AND ITM.ItemUID = ENC.ItemUID  --//CL
	LEFT OUTER JOIN dbo.GItemPeriod		AS PER WITH(NOLOCK) ON PER.UnitUID = @UnitUID AND ITM.ItemUID = PER.ItemUID  --//NC
	LEFT OUTER JOIN dbo.GItemSocket		AS SOC WITH(NOLOCK) ON SOC.UnitUID = @UnitUID AND ITM.ItemUID = SOC.ItemUID  --//NC
	LEFT OUTER JOIN dbo.GItemSocket_Random	AS RSC WITH(NOLOCK) ON RSC.UnitUID = @UnituID AND ITM.ItemUID = RSC.ItemUID
	WHERE ITM.UnitUID = @UnitUID
		AND ITM.Inventorycategory = 8

GO

CREATE OR ALTER PROCEDURE [dbo].[P_GPShopItem_SEL]
	@iUnitUID	BIGINT
	
AS
SET NOCOUNT ON;

SELECT  ItemUID
,		ItemID
,		UsageType
,		Quantity
,		Endurance
,		SealCnt
,		ELevel
,		Attribute1
,		Attribute2
,		Attribute3
,		Socket1
,		Socket2
,		Socket3
,		Socket4
,		ISNULL(Socket5, 0) AS Socket5
,		ISNULL(ExpandedSocketNum, 0) AS ExpandedSocketNum
,		SocketR1		-- random socket
,		SocketR2		-- random socket
,		SocketR3		-- random socket
,		SocketR4		-- random socket
,		SocketR5		-- random socket
,		PricePerOne
,		FeePerOne
,		TotalSellEDIn
,		TotalSoldItemQuantity
,		TotalSellCommissionED
,		Category
FROM dbo.GPShopItem WITH(NOLOCK)
WHERE UnitUID = @iUnitUID and deleted = 0
GO

CREATE OR ALTER PROCEDURE [dbo].[P_GPShopItem_INT_DEL_A_New]
	@UnitUID_S		bigint				-- seller
,	@Quantity		int					-- quantity registered
,	@ItemUID_Old	bigint 				-- seller's item UID
,	@PricePerOne	int					-- price per item
,	@Category		tinyint				-- item category
,	@Reason			tinyint				-- deletion reason
,	@ItemUID		bigint = 0 OUTPUT
AS
	SET NOCOUNT ON
	
	DECLARE @Result			int
	
	DECLARE @sdtNow			smalldatetime
		,	@sdtEndDate		smalldatetime
		,	@ItemID			int
		,	@UsageType		tinyint
		,	@Quantity_Old	int
		,	@Attribute1		tinyint
		,	@Attribute2		tinyint
		,	@Attribute3		tinyint
		
	SET @sdtNow = CURRENT_TIMESTAMP
		
	SELECT @Attribute1 = Attribute
		FROM dbo.GItemAttribute with(nolock)
			WHERE UnitUID  = @UnitUID_S AND ItemUID = @ItemUID_Old AND SlotNo = 0

	SELECT @Attribute2 = Attribute
		FROM dbo.GItemAttribute with(nolock)
			WHERE UnitUID  = @UnitUID_S AND ItemUID = @ItemUID_Old AND SlotNo = 1

	SELECT @Attribute3 = Attribute
		FROM dbo.GItemAttribute with(nolock)
			WHERE UnitUID  = @UnitUID_S AND ItemUID = @ItemUID_Old AND SlotNo = 2	
	
	SELECT @ItemID = ItemID, @UsageType = UsageType, @Quantity_Old = Quantity
		FROM dbo.GItem WITH(NOLOCK)
			WHERE UnitUID = @UnitUID_S AND ItemUID = @ItemUID_Old
	
		
	-- the item does not exist.
	IF @@ROWCOUNT = 0
		SET @Result = -1
	ELSE IF @UsageType = 2 AND @Quantity_Old - @Quantity < 0
		SET @Result = -2
	ELSE BEGIN
		BEGIN TRY
			BEGIN TRAN
			
			-- register the item
			SET @Result = -11

			-- insert into the agency table
			INSERT INTO dbo.GPShopItem ( UnitUID, ItemID, UsageType, Quantity, Endurance, SealCnt, ELevel, Attribute1, Attribute2, Attribute3,
								Socket1, Socket2, Socket3, Socket4, Socket5, ExpandedSocketNum, SocketR1, SocketR2, SocketR3, SocketR4, SocketR5, PricePerOne, FeePerOne, TotalSellEDIn, TotalSoldItemQuantity, TotalSellCommissionED,
								RegDate, DelDate, OldItemUID, Category )
			SELECT @UnitUID_S
				, @ItemID
				, @UsageType
				, CASE WHEN @UsageType = 2 THEN @Quantity ELSE 1 END
				, CASE WHEN @UsageType = 1 THEN 10000 ELSE -1 END
				, ISNULL(SEC.SealCnt, 0) AS SealCnt
				, ISNULL(ENC.ELevel, 0) AS ELevel
				, ISNULL(@Attribute1, 0) AS Attribute1
				, ISNULL(@Attribute2, 0) AS Attribute2
				, ISNULL(@Attribute3, 0) AS Attribute3
				, ISNULL(SOC.Socket1, 0) AS Socket1
				, ISNULL(SOC.Socket2, 0) AS Socket2
				, ISNULL(SOC.Socket3, 0) AS Socket3
				, ISNULL(SOC.Socket4, 0) AS Socket4
				, ISNULL(SOC.Socket5, 0) AS Socket5
				, ISNULL(SOC.ExpandedSocketNum, 0) AS ExpandedSocketNum
				, ISNULL(SOR.Socket1, 0) AS SocketR1
				, ISNULL(SOR.Socket2, 0) AS SocketR2
				, ISNULL(SOR.Socket3, 0) AS SocketR3
				, ISNULL(SOR.Socket4, 0) AS SocketR4
				, ISNULL(SOR.Socket5, 0) AS SocketR5
				, @PricePerOne
				, 0 AS FeePerOne
				, 0 AS TotalSellEDIn
				, 0 AS TotalSoldItemQuantity
				, 0 AS TotalSellCommissionED
				, @sdtNow
				, @sdtNow
				, @ItemUID_Old
				, @Category
			FROM (
				SELECT UnitUID, ItemUID, ItemID, UsageType, Quantity, InventoryCategory
				FROM dbo.GItem WITH(NOLOCK)
				WHERE UnitUID = @UnitUID_S AND ItemUID = @ItemUID_Old
			) AS ITM
			LEFT OUTER JOIN	( -- seal
				SELECT ItemUID, SealCnt
				FROM dbo.GItemSeal WITH(NOLOCK)
				WHERE ItemUID = @ItemUID_Old
			) AS SEC
			ON ITM.ItemUID = SEC.ItemUID
			LEFT OUTER JOIN ( -- enchant
				SELECT UnitUID, ItemUID, ELevel
				FROM dbo.GItemEnchant WITH(NOLOCK)
				WHERE UnitUID = @UnitUID_S AND ItemUID = @ItemUID_Old
			)AS ENC 
			ON ENC.UnitUID = ITM.UnitUID AND ITM.ItemUID = ENC.ItemUID
			LEFT OUTER JOIN ( -- socket
				SELECT UnitUID, ItemUID, Socket1, Socket2, Socket3, Socket4, Socket5, ExpandedSocketNum
				FROM dbo.GItemSocket WITH(NOLOCK)
				WHERE UnitUID = @UnitUID_S AND ItemUID = @ItemUID_Old
			)AS SOC 
			ON SOC.UnitUID = ITM.UnitUID AND ITM.ItemUID = SOC.ItemUID
			LEFT OUTER JOIN ( -- random socket
				SELECT UnitUID, ItemUID, Socket1, Socket2, Socket3, Socket4, Socket5
				FROM dbo.GItemSocket_Random WITH(NOLOCK)
				WHERE UnitUID = @UnitUID_S AND ItemUID = @ItemUID_Old
			)AS SOR
			ON SOR.UnitUID = ITM.UnitUID AND ITM.ItemUID = SOR.ItemUID

			SET @ItemUID = SCOPE_IDENTITY() -- the agency's new ItemUID
			
			
			--> from here, delete the inventory item
			-- equipment or accessories (no durability, not stackable)
			IF @UsageType IN (0, 1) BEGIN
				SET @Result = -16
				
				DELETE dbo.GItem
					OUTPUT DELETED.ItemUID, DELETED.UnitUID, DELETED.ItemID, DELETED.InventoryCategory, DELETED.SlotID, DELETED.UsageType, DELETED.Quantity, DELETED.Endurance, DELETED.Inserted, DELETED.RegDate, GETDATE(), @Reason
					INTO dbo.GItem_Log ( ItemUID, UnitUID, ItemID, InventoryCategory, SlotID, UsageType, Quantity, Endurance, Inserted, RegDate, DelDate, Deleted )
				WHERE ItemUID = @ItemUID_Old
			END
			
			-- change a stackable item's quantity
			ELSE IF @UsageType = 2 AND (@Quantity_Old - @Quantity) > 0 BEGIN
				SET @Result = -17
				
				UPDATE dbo.GItem
					SET Quantity = Quantity - @Quantity
						WHERE UnitUID = @UnitUID_S AND ItemUID = @ItemUID_Old					
				END
			
			-- delete a stackable item (its whole slot was traded)
			ELSE IF @UsageType = 2 AND (@Quantity_Old - @Quantity) = 0 BEGIN
				SET @Result = -18
				
				DELETE dbo.GItem
					OUTPUT DELETED.ItemUID, DELETED.UnitUID, DELETED.ItemID, DELETED.InventoryCategory, DELETED.SlotID, DELETED.UsageType, DELETED.Quantity, DELETED.Endurance, DELETED.Inserted, DELETED.RegDate, GETDATE(), @Reason
					INTO dbo.GItem_Log ( ItemUID, UnitUID, ItemID, InventoryCategory, SlotID, UsageType, Quantity, Endurance, Inserted, RegDate, DelDate, Deleted )
				WHERE ItemUID = @ItemUID_Old		
			END

			COMMIT TRAN
			SET @Result = 0
		END TRY
		BEGIN CATCH
				DECLARE @NO_XACT_STATE		smallint
				SET @NO_XACT_STATE			= XACT_STATE()		-- transaction state

				IF @NO_XACT_STATE <> 0
					ROLLBACK TRAN

				EXEC dbo.SYS_SP_ErrorLog_INT @NO_XACT_STATE

				SET @Result = -1
		END CATCH
	END

	RETURN @Result

GO

-- =============================================
-- Author:		Hwang
-- Create date: 2013.05.22
-- Description:	agency item sale
--				(random sockets added)
-- =============================================
CREATE OR ALTER PROCEDURE [dbo].[P_GPShopItem_INT_DEL_B_New]
	@UnitUID_S		bigint			-- seller
,	@UnitUID_R		bigint			-- buyer
,	@Quantity		int				-- quantity bought
,	@ItemUID_P		bigint 			-- seller's item UID
,	@InsertType		tinyint			-- item creation reason
,	@ItemUID		bigint = 0 OUTPUT
AS
	SET NOCOUNT ON
	
	DECLARE @Result			int
	
	DECLARE @sdtNow			smalldatetime
		,	@sdtEndDate		smalldatetime
		,	@ItemID			int
		,	@UsageType		tinyint
		,	@Quantity_P_Old	int
		,	@Attribute1		tinyint
		,	@Attribute2		tinyint
		,	@Attribute3		tinyint
		,	@ELevel			smallint
		,	@SealCnt		tinyint
		,	@Socket1		int
		,	@Socket2		int
		,	@Socket3		int
		,	@Socket4		int
		,	@Socket5		int
		,	@ExpandedSocketNum	tinyint
		,	@SocketR1		int
		,	@SocketR2		int
		,	@SocketR3		int
		,	@SocketR4		int
		,	@SocketR5		int
		,	@Deleted		tinyint
		
	SET @sdtNow = CURRENT_TIMESTAMP
	
	SELECT	@ItemID = ItemID, @UsageType = UsageType, @Quantity_P_Old = Quantity, @ELevel = ELevel, @SealCnt = SealCnt,
			@Socket1 = Socket1, @Socket2 = Socket2, @Socket3 = Socket3, @Socket4 = Socket4, @Socket5 = ISNULL(Socket5, 0), @ExpandedSocketNum = ISNULL(ExpandedSocketNum, 0),
			@SocketR1 = SocketR1, @SocketR2 = SocketR2, @SocketR3 = SocketR3, @SocketR4 = SocketR4, @SocketR5 = SocketR5, 
			@Attribute1 = Attribute1, @Attribute2 = Attribute2, @Attribute3 = Attribute3, @Deleted = Deleted
		FROM dbo.GPShopItem WITH(NOLOCK)
			WHERE UnitUID = @UnitUID_S AND ItemUID = @ItemUID_P
	
		
	-- the item does not exist.
	IF @@ROWCOUNT = 0
		SET @Result = -1
	ELSE IF @UsageType = 2 AND @Quantity_P_Old - @Quantity < 0
		SET @Result = -2
	ELSE IF @Deleted = 1
		SET @Result = -3
	ELSE IF @Quantity_P_Old = 0
		SET @Result = -4
	ELSE BEGIN
		BEGIN TRY
			BEGIN TRAN
			
				-- register the item
				SET @Result = -11

				-- put the bought item in the inventory
				INSERT INTO dbo.GItem ( UnitUID, ItemID, InventoryCategory, UsageType, Quantity, Endurance, SlotID, RegDate, Inserted )
				SELECT @UnitUID_R, @ItemID, 0, @UsageType
					, CASE WHEN @UsageType = 2 THEN @Quantity ELSE 0 END
					, CASE WHEN @UsageType = 1 THEN 10000 ELSE -1 END
					, 0
					, @sdtNow
					, @InsertType

				SET @ItemUID = @@IDENTITY
				
				-- move the enchant level
				IF  @ELevel > 0   BEGIN
					SET @Result = -12
					
					INSERT INTO dbo.GItemEnchant ( UnitUID, ItemUID, Elevel )
					VALUES( @UnitUID_R, @ItemUID, @ELevel )

				END

				-- move the attributes
				IF ( @Attribute1 > 0 OR @Attribute2 > 0 OR @Attribute3 > 0 ) BEGIN
					SET @Result = -13
					
					INSERT INTO dbo.GItemAttribute (UnitUID, ItemUID, SlotNo, Attribute)
					SELECT @UnitUID_R, @ItemUID, SlotNo, Attribute
					FROM (
						SELECT 0 AS SlotNo, @Attribute1 AS Attribute
						UNION ALL
						SELECT 1 AS SlotNo, @Attribute2 AS Attribute
						UNION ALL
						SELECT 2 AS SlotNo, @Attribute3 AS Attribute
					) AS TempTable
					WHERE Attribute > 0 
				END
				
				-- move the sockets
				IF ( @Socket1 > 0 OR @Socket2 > 0 OR @Socket3 > 0 OR @Socket4 > 0 OR @Socket5 > 0 OR @ExpandedSocketNum > 0 ) BEGIN
					SET @Result = -14
					
					INSERT INTO dbo.GItemSocket ( UnitUID, ItemUID, Socket1, Socket2, Socket3, Socket4, Socket5, ExpandedSocketNum )
					VALUES ( @UnitUID_R, @ItemUID, @Socket1, @Socket2, @Socket3, @Socket4, @Socket5, @ExpandedSocketNum )

				END
				
				-- move the random sockets (added)
				IF ( @SocketR1 > 0 OR @SocketR2 > 0 OR @SocketR3 > 0 OR @SocketR4 > 0 OR @SocketR5 > 0 ) BEGIN
					SET @Result = -14
					
					INSERT INTO dbo.GItemSocket_Random ( UnitUID, ItemUID, Socket1, Socket2, Socket3, Socket4, Socket5 )
					VALUES ( @UnitUID_R, @ItemUID, @SocketR1, @SocketR2, @SocketR3, @SocketR4, @SocketR5 )

				END

				-- move the seal count
				IF ( @SealCnt <> 0 ) BEGIN
					SET @Result = -15
					
					INSERT INTO dbo.GItemSeal ( ItemUID, SealCnt )
					VALUES ( @ItemUID, @SealCnt - 100 )

				END
		
				--> from here, delete the inventory item
				-- items left on sale
				SET @Result = -16
					
				UPDATE dbo.GPShopItem
					SET Quantity = Quantity - @Quantity
					,	TotalSellEDIn = TotalSellEDIn + ( PricePerOne * @Quantity )
				WHERE UnitUID = @UnitUID_S AND ItemUID = @ItemUID_P					
				
			COMMIT TRAN
			SET @Result = 0
		END TRY
		BEGIN CATCH
				DECLARE @NO_XACT_STATE		smallint
				SET @NO_XACT_STATE			= XACT_STATE()		-- transaction state

				IF @NO_XACT_STATE <> 0
					ROLLBACK TRAN

				EXEC dbo.SYS_SP_ErrorLog_INT @NO_XACT_STATE

				SET @Result = -1
		END CATCH
	END

	RETURN @Result

GO

-- =============================================
-- Author:		Hwang
-- Create date: 2013.05.22
-- Description:	agency item take-back
--				(random sockets added)
-- =============================================
CREATE OR ALTER PROCEDURE [dbo].[P_GPShopItem_INT_DEL_C_New]
	@UnitUID_S		bigint		-- seller
,	@UnitUID_R		bigint		-- buyer
,	@Quantity		int			-- quantity bought
,	@ItemUID_P		bigint		-- seller's item UID
,	@InsertType		tinyint
,	@ItemUID		bigint = 0 OUTPUT
AS
	SET NOCOUNT ON
	
	DECLARE @Result			int
	
	DECLARE @sdtNow			smalldatetime
		,	@sdtEndDate		smalldatetime
		,	@ItemID			int
		,	@UsageType		tinyint
		,	@Quantity_P_Old	int
		,	@Attribute1		tinyint
		,	@Attribute2		tinyint
		,	@Attribute3		tinyint
		,	@ELevel			smallint
		,	@SealCnt		tinyint
		,	@Socket1		int
		,	@Socket2		int
		,	@Socket3		int
		,	@Socket4		int
		,	@Socket5		int
		,	@ExpandedSocketNum	tinyint
		,	@SocketR1		int
		,	@SocketR2		int
		,	@SocketR3		int
		,	@SocketR4		int
		,	@SocketR5		int
		,	@Deleted		tinyint
		
	SET @sdtNow = CURRENT_TIMESTAMP
	
	SELECT	@ItemID = ItemID, @UsageType = UsageType, @Quantity_P_Old = Quantity, @ELevel = ELevel, @SealCnt = SealCnt,
			@Socket1 = Socket1, @Socket2 = Socket2, @Socket3 = Socket3, @Socket4 = Socket4, @Socket5 = ISNULL(Socket5, 0), @ExpandedSocketNum = ISNULL(ExpandedSocketNum, 0),
			@SocketR1 = SocketR1, @SocketR2 = SocketR2, @SocketR3 = SocketR3, @SocketR4 = SocketR4, @SocketR5 = SocketR5, 
			@Attribute1 = Attribute1, @Attribute2 = Attribute2, @Attribute3 = Attribute3, @Deleted = Deleted
		FROM dbo.GPShopItem WITH(NOLOCK)
			WHERE UnitUID = @UnitUID_S AND ItemUID = @ItemUID_P
	
		
	-- the item does not exist.
	IF @@ROWCOUNT = 0
		SET @Result = -1
	ELSE IF @Quantity_P_Old - @Quantity < 0
		SET @Result = -2
	ELSE IF @Deleted = 1
		SET @Result = -3
	ELSE BEGIN
		BEGIN TRY
			BEGIN TRAN
			
				-- register the item
				SET @Result = -11
				IF ( @Quantity > 0 ) BEGIN

					-- put the bought item in the inventory
					INSERT INTO dbo.GItem ( UnitUID, ItemID, InventoryCategory, UsageType, Quantity, Endurance, SlotID, RegDate, Inserted )
					SELECT @UnitUID_R, @ItemID, 0, @UsageType
						, CASE WHEN @UsageType = 2 THEN @Quantity ELSE 0 END
						, CASE WHEN @UsageType = 1 THEN 10000 ELSE -1 END
						, 0
						, @sdtNow
						, @InsertType

					SET @ItemUID = @@IDENTITY
					
					-- move the enchant level
					IF  @ELevel > 0   BEGIN
						SET @Result = -12
						
						INSERT INTO dbo.GItemEnchant ( UnitUID, ItemUID, Elevel )
						VALUES( @UnitUID_R, @ItemUID, @ELevel )

					END

					-- move the attributes
					IF ( @Attribute1 > 0 OR @Attribute2 > 0 OR @Attribute3 > 0 ) BEGIN
						SET @Result = -13
						
						INSERT INTO dbo.GItemAttribute (UnitUID, ItemUID, SlotNo, Attribute)
						SELECT @UnitUID_R, @ItemUID, SlotNo, Attribute
						FROM (
							SELECT 0 AS SlotNo, @Attribute1 AS Attribute
							UNION ALL
							SELECT 1 AS SlotNo, @Attribute2 AS Attribute
							UNION ALL
							SELECT 2 AS SlotNo, @Attribute3 AS Attribute
						) AS TempTable
						WHERE Attribute > 0 
					END
					
					-- move the sockets
					IF ( @Socket1 > 0 OR @Socket2 > 0 OR @Socket3 > 0 OR @Socket4 > 0 OR @Socket5 > 0 OR @ExpandedSocketNum > 0 ) BEGIN
						SET @Result = -14
						
						INSERT INTO dbo.GItemSocket ( UnitUID, ItemUID, Socket1, Socket2, Socket3, Socket4, Socket5, ExpandedSocketNum )
						VALUES ( @UnitUID_R, @ItemUID, @Socket1, @Socket2, @Socket3, @Socket4, @Socket5, @ExpandedSocketNum )

					END
					
					-- move the random sockets
					IF ( @SocketR1 > 0 OR @SocketR2 > 0 OR @SocketR3 > 0 OR @SocketR4 > 0 OR @SocketR5 > 0 ) BEGIN
						SET @Result = -14
						
						INSERT INTO dbo.GItemSocket_Random ( UnitUID, ItemUID, Socket1, Socket2, Socket3, Socket4, Socket5 )
						VALUES ( @UnitUID_R, @ItemUID, @SocketR1, @SocketR2, @SocketR3, @SocketR4, @SocketR5 )

					END
			
					-- move the seal count (take-back)
					IF ( @SealCnt <> 0 ) BEGIN
						SET @Result = -15
						
						INSERT INTO dbo.GItemSeal ( ItemUID, SealCnt )
						VALUES ( @ItemUID, @SealCnt )

					END
					
				END
				
				-- delete what is left in the agency
				SET @Result = -17
					
				UPDATE dbo.GPShopItem
					SET DelDate = @sdtNow
					,	Deleted = 1
				WHERE UnitUID = @UnitUID_S AND ItemUID = @ItemUID_P
				
			COMMIT TRAN
			SET @Result = 0
		END TRY
		BEGIN CATCH
				DECLARE @NO_XACT_STATE		smallint
				SET @NO_XACT_STATE			= XACT_STATE()		-- transaction state

				IF @NO_XACT_STATE <> 0
					ROLLBACK TRAN

				EXEC dbo.SYS_SP_ErrorLog_INT @NO_XACT_STATE

				SET @Result = -1
		END CATCH
	END

	RETURN @Result
GO
