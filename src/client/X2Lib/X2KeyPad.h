#ifndef X2KEY_PAD_H_
#define X2KEY_PAD_H_
#pragma once

#include <boost/unordered_map.hpp>
#include <boost/bimap/bimap.hpp>

//#pragma region CX2KeyPad
/** @class : CX2KeyPad
	@brief : Ű���� UI Ŭ����
	@date  : 2012/05/08
*/
class CX2KeyPad
{
//#pragma region enum, const, typedef
public:
	enum KEY_BUTTON_INDEX
	{
		KBI_NONE,

		KBI_GRAVE = 1,		/// ~
		KBI_1,				/// 1
		KBI_2,				/// 2
		KBI_3,				/// 3
		KBI_4,				/// 4
		KBI_5,				/// 5
		KBI_6,				/// 6
		KBI_7,				/// 7
		KBI_8,				/// 8
		KBI_9,				/// 9
		KBI_0,				/// 0
		KBI_MINUS,			/// -
		KBI_EQUAL,			/// =
		KBI_BACKSPACE,		/// <--
		KBI_TAB,			/// Tab
		KBI_Q,				/// Q
		KBI_W,				/// W
		KBI_E,				/// E
		KBI_R,				/// R
		KBI_T,				/// T
		KBI_Y,				/// Y
		KBI_U,				/// U
		KBI_I,				/// I
		KBI_O,				/// O
		KBI_P,				/// P
		KBI_LBRACKET,		/// [
		KBI_RBRACKET,		/// ]
		KBI_ROOT,			/// |
		KBI_CAPITAL,		/// Caps Lock
		KBI_A,				/// A
		KBI_S,				/// S
		KBI_D,				/// D
		KBI_F,				/// F
		KBI_G,				/// G
		KBI_H,				/// H
		KBI_J,				/// J
		KBI_K,				/// K
		KBI_L,				/// L
		KBI_SEMICOLON,		/// ;
		KBI_APOSTROPHE,		/// '
		KBI_RETURN,			/// Enter
		KBI_LSHIFT,			/// Left Shift
		KBI_Z,				/// Z
		KBI_X,				/// X
		KBI_C,				/// C
		KBI_V,				/// V
		KBI_B,				/// B
		KBI_N,				/// N
		KBI_M,				/// M
		KBI_COMMA,			/// ,
		KBI_PERIOD,			/// .
		KBI_SLASH,			/// /
		KBI_RSHIFT,			/// Right Shift
		KBI_UP,				/// Up Key
		KBI_LCTRL,			/// Left Control
		KBI_LALT,			/// Left Alt
		KBI_SPACE,			/// Space
		KBI_RALT,			/// Right Alt
		KBI_RCTRL,			/// Right Control
		KBI_LEFT,			/// Left Key
		KBI_DOWN,			/// Down Key
		KBI_RIGHT,			/// Right Key
		KBI_NUMLOCK,		// Num Lock
		KBI_NSLASH,			// KeyPad Slash
		KBI_NMULTIPLY,		// KeyPad Multiply
		KBI_NMINUS,			// KeyPad Minus
		KBI_NPLUS,			// KeyPad Plus
		KBI_N1,				// KeyPad 1
		KBI_N2,				// KeyPad 2
		KBI_N3,				// KeyPad 3
		KBI_N4,				// KeyPad 4
		KBI_N5,				// KeyPad 5
		KBI_N6,				// KeyPad 6
		KBI_N7,				// KeyPad 7
		KBI_N8,				// KeyPad 8
		KBI_N9,				// KeyPad 9
		KBI_N0,				// KeyPad 0
		KBI_NDEL,			// KeyPad Delete
		KBI_NENTER,			// KeyPad Enter

		KBI_END,
	};

	enum KEY_ACTION_ID
	{
		KAI_NONE,			/// ���� ����

		KAI_QUICKSLOT1,		/// ������ 1
		KAI_QUICKSLOT2,		/// ������ 2
		KAI_QUICKSLOT3,		/// ������ 3
		KAI_QUICKSLOT4,		/// ������ 4
		KAI_QUICKSLOT5,		/// ������ 5
		KAI_QUICKSLOT6,		/// ������ 6

		KAI_COMMAND_FAST,	/// Ŀ�ǵ� ��
		KAI_COMMAND_STRONG,	/// Ŀ�ǵ� ��
		KAI_HYPER,			/// ����
		KAI_CHANGE_SLOT,	/// ���� ü����
		KAI_ZOOM_IN,		/// ȭ�� Ȯ��
		KAI_ZOOM_OUT,		/// ȭ�� ���
		KAI_TOGGLE_NAME,	/// ���� �̸� ����

		KAI_SKILLSLOT1,		/// ��ų ���� 1
		KAI_SKILLSLOT2,		/// ��ų ���� 2
		KAI_SKILLSLOT3,		/// ��ų ���� 3
		KAI_SKILLSLOT4,		/// ��ų ���� 4

		KAI_EX_SKILLSLOT1,	/// Ȯ�� ��ų ���� 1
		KAI_EX_SKILLSLOT2,	/// Ȯ�� ��ų ���� 2
		KAI_EX_SKILLSLOT3,	/// Ȯ�� ��ų ���� 3
		KAI_EX_SKILLSLOT4,	/// Ȯ�� ��ų ���� 4

		KAI_COMMUNITY_UI,	/// Ŀ�´�Ƽ â
		KAI_CHARACTER_UI,	/// ĳ���� â
		KAI_INVENTORY_UI,	/// �κ��丮 â
		KAI_OPTION_UI,		/// �ɼ� â
		KAI_PET_UI,			/// �� â
		KAI_PVE_UI,			/// ���� â
		KAI_PVP_UI,			/// ���� â
		KAI_SKILL_UI,		/// ��ų â
		KAI_QUEST_UI,		/// ����Ʈ â
		KAI_MAP_UI,			/// ����� â
		KAI_GUILD_UI,		/// ��� â

		KAI_RINDING,		/// ���̵� �� Ÿ��

		KAI_END,
	};

	enum KEY_PAD_UI_MSG
	{
		KPUIM_EXIT		= 53000,	/// ����
		KPUIM_DEFAULT,				/// �ʱ�ȭ
		KPUIM_CLEAR,				/// ��� ���ֱ�
		KPUIM_PREV,					/// �������
		KPUIM_SAVE,					/// ����
		KPUIM_CANCEL,				/// ���

		KPUIM_USESLOT_DROPPED,		/// �巡�� & ���
		KPUIM_USESLOT_MOUSEIN,		/// ���콺 �� ��Ŀ��
		KPUIM_USESLOT_MOUSEOUT,		/// ���콺 �ƿ� ��Ŀ��
	};

	typedef boost::unordered_map< GAME_ACTION, int > KeyMap;								/// �׼� Ű boost::map
	typedef boost::unordered_map< unsigned char, wstring > KeyStringMap;					/// Key�� �´� String, boost::map
	typedef boost::bimaps::bimap< unsigned char, KEY_BUTTON_INDEX > DeviceKeyMap;			/// DeviceKey�� �´� Pad Slot Index, Boost::bimap
	typedef boost::bimaps::bimap< unsigned char, KEY_BUTTON_INDEX >::value_type value_t;	/// DeviceKeyMap�� ���� �����ϱ� ���� Value_type
//#pragma endregion ����ü, ���, ����� Ÿ��

//#pragma region public function
public:
	CX2KeyPad(void);
	~CX2KeyPad(void);

	void ShowKeyPad( IN bool bShow_ );
	bool GetShowKeyPad() { return m_bShowKeyPad; };
	void LoadSlotData();
	void SaveSlotData( bool bfirst = false );
	void InitKeySlotData();
	wstring GetKeyString ( int iKeyIndex );
	wstring GetKeyString ( GAME_ACTION eActionKey_ );

	bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//#pragma endregion �Լ�

//#pragma region private function
private:
	void ClearSlotData();
	void UpdateSlotData();
	void DefaultSlotData();
	void InitUseKeySlotData();
	void InitDisUseKeySlotData();
	bool ProcessUseSlotDropped( IN LPARAM lParam_ );
	void UpdateEtcString();

	bool Handler_EGS_KEYBOARD_MAPPING_INFO_WRITE_REQ();
	bool Handler_EGS_KEYBOARD_MAPPING_INFO_WRITE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//#pragma endregion �Լ�

//#pragma region public variable
public:
	static const int USE_KEY_NUMBER;				/// ���Ǵ� Ű�� ��
	static const int DISUSE_KEY_NUMBER;				/// ������ �ʴ� Ű�� ��

	static const unsigned char USE_DIK_LIST[KBI_END];	/// DirectX Input Keyboard�� ���ǵ� �� �� ���ӿ� ����ϴ� Ű ����

	static const int KEYPAD_STRING_INDEX[KBI_END];	/// KeyPad String Index
//#pragma endregion ����

//#pragma region private varable
private:
	CKTDGUIDialogType	m_DLGKeyPad;		/// Ű ���� ���̾�α�
	KeyMap				m_KeyPadMap;		/// Ű �׼� ���� ���� �ε��� ���� ����
	DeviceKeyMap		m_DeviceKeyMap;		/// DIK Ű ���� ���� Ű �е� ���� ���� ���� ����
	KeyStringMap		m_KeyStringMap;		/// Key Device�� �´� String ������ ����

	bool				m_bShowKeyPad;		/// KeyPad Ȱ��ȭ ���� Ȯ��
//#pragma endregion ����
};
//#pragma endregion Ŭ����

//#pragma region CX2UseKeySlotData
class CX2UseKeySlotData : public CKTDGUISlotData
{
//#pragma region public function
public:
	CX2UseKeySlotData();
	CX2UseKeySlotData( IN int index_ );
	virtual ~CX2UseKeySlotData();

	virtual void UpdateUI( IN CKTDGUISlot* pSlot_ );
	void Clear( IN int index_ = -1 );
//#pragma endregion �Լ�

//#pragma region public variable
public:
	int	m_iIndex;					/// KeyPad Slot Index
	GAME_ACTION m_eKeyActionID;		/// Key Action ID
//#pragma endregion ����
};
//#pragma endregion Ŭ����

//#pragma region CX2DisUseKeySlotData
class CX2DisUseKeySlotData : public CKTDGUISlotData
{
//#pragma region public function
public:
	CX2DisUseKeySlotData();
	CX2DisUseKeySlotData( IN int index_ );
	virtual ~CX2DisUseKeySlotData();

	virtual void UpdateUI( IN CKTDGUISlot* pSlot_ );
	void Clear( IN int index_ = -1 );
//#pragma endregion �Լ�

//#pragma region public variable
public:
	int	m_iIndex;					/// KeyPad Slot Index
	GAME_ACTION m_eKeyActionID;		/// Key Action ID
//#pragma endregion ����
};
//#pragma endregion Ŭ����


#endif
