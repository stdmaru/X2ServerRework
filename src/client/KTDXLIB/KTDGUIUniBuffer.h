#pragma once

// IMM32 is emulated on the SDL3 IME state (DXUT/Ime.h) instead of loaded from imm32.dll.
#define EMULATEDPROC( APIName ) \
	_##APIName = x2::dxut::imm::APIName
// Back to the edit box's own do-nothing version (Uninitialize).
#define PLACEHOLDERPROC( APIName ) \
	_##APIName = Dummy_##APIName

class CKTDGUIUniBuffer
{
	public:
		CKTDGUIUniBuffer( int nInitialSize = 1 );
		~CKTDGUIUniBuffer(void);

		static	void	Initialize();
		static	void	Uninitialize();

				int		GetBufferSize() { return m_nBufferSize; }
				bool	SetBufferSize( int nSize );
				int		GetTextSize()  { return (int) wcslen( m_pwszBuffer ); }
		const	WCHAR*	GetBuffer() { return m_pwszBuffer; }

		const	WCHAR& operator[]( int n ) const { return m_pwszBuffer[n]; }
				WCHAR& operator[]( int n );

				CKTDGFontManager::CUKFont* GetFontNode() { return m_pFontNode; }
				void SetFontNode( CKTDGFontManager::CUKFont* pFontNode ) { m_pFontNode = pFontNode; }
				void		Clear();

				bool		InsertChar( int nIndex, WCHAR wChar ); // Inserts the char at specified index. If nIndex == -1, insert to the end.
				bool		RemoveChar( int nIndex );  // Removes the char at specified index. If nIndex == -1, remove the last char.
				bool		InsertString( int nIndex, const WCHAR *pStr, int nCount = -1 );  // Inserts the first nCount characters of the string pStr at specified index.  If nCount == -1, the entire string is inserted. If nIndex == -1, insert to the end.
				bool		SetText( LPCWSTR wszText );

				// Uniscribe
                HRESULT     CPtoY( int nCP, BOOL bTrail, int *pY );
				HRESULT		CPtoX( int nCP, BOOL bTrail, int *pX );
                HRESULT		XtoCP( int nX, int *pCP, int *pnTrail );
                HRESULT     XYtoCP( int nX, int nY, int *pCP, int *pnTrail );
				void		GetPriorItemPos( int nCP, int *pPrior );
				void		GetNextItemPos( int nCP, int *pPrior );
                int         GetLineWidth(int nCP);
                int         GetCaretLineMove( int nCP, int stepLine );
                int         GetLineCP(int nLine);
                int         GetLineCount(int nCP);

				// Hide Char
		int		m_nBufferSize;			// Size of the buffer allocated, in characters
		int		m_nLengthLimit;			// 글자 제한
		bool	m_bOneCharMode;			// 한문자 모드
				void		SetHideChar( bool bUseHideChar, WCHAR cHideChar );
				void		SetLengthLimit( int val ) { m_nLengthLimit = val; }
				void		SetOneCharMode( bool val ) { m_bOneCharMode = val; }
				void		SetOnlyNumMode( bool val ) { m_bOnlyNumMode = val; }
				void		SetWidthLimit( int val ) { m_WidthLimit = val; }
                void		SetLineLimit( int val ) { m_LineLimit = val; }
				void		SetByteLimit( int val ) { m_bByteLimit = true, m_ByteLimit = val; }
				int			GetByteLimit() { return m_bByteLimit ? m_ByteLimit : m_nLengthLimit; }
				void		SetOnlyNumAndEnglishMode( bool val, bool bCheckEnglishBigAndSmall, bool bBigEnglish )
				{
					m_bOnlyNumAndEnglishMode = val;
					m_bCheckEnglishBigAndSmall = bCheckEnglishBigAndSmall;
					m_bBigEnglish = bBigEnglish;
				}

				void		SetAutoChangeBigEnglish( bool val ) { m_bAutoChangeBigEnglish = val; }

	private:
		bool isEnglish( WCHAR cWChar );

		WCHAR*	m_pwszBuffer;			// Buffer to hold text
		WCHAR*	m_pwszBufferHC;			// Buffer to hold text
		bool	m_bOnlyNumMode;			// 숫자 입력 모드
		int		m_WidthLimit;
        int     m_LineLimit;
		bool	m_bOnlyNumAndEnglishMode;
		bool	m_bCheckEnglishBigAndSmall;
		bool	m_bBigEnglish;	//숫자와 영어 모드에서 영어가 대문자로 할것인지?
		bool	m_bByteLimit;
		int		m_ByteLimit;
		bool	m_bAutoChangeBigEnglish;

		// Hide Char
		WCHAR	m_cHideChar;
		bool	m_bUseHideChar;

		// Uniscribe-specific
		WCHAR					Defence1[100];
		CKTDGFontManager::CUKFont*	    m_pFontNode;        // Font node for the font that this buffer uses

		// The original measured the text with Uniscribe (usp10.dll: ScriptStringAnalyse and the caret functions) on a
		// GDI device context; the port measures it with the engine's fonts (the font step, docs/porting/client-ktdxlib.md).
		bool							m_bAnalyseRequired;         // True if the string has changed since last analysis.

		WCHAR					Defence2[100];

};
