#pragma once
/**	@file	: 2012/04/12
	@desc	: �˾����� ����ϴ� �ߺ��Ǵ� ����� �ϳ��� Ŭ������ ����( 9��� ��Ų, ����Ǵ� �Լ� �� )
	@author	: JHKang
*/

/** @class	: CX2PopupUIBase
	@desc	: �˾��� ���Ǵ� ���ҽ� �� ����� ������.
	@date	: 2012/04/12
*/

//#pragma region CX2PopupUIBase
class CX2PopupUIBase
{
//#pragma region publice function
public:
	CX2PopupUIBase(void);
	virtual ~CX2PopupUIBase(void);

	bool GetShowMenu()
	{
		return m_bShow;
	}
	CKTDGUIDialogType GetDialog()
	{
		return m_pDlgPopup;
	}

	virtual void ClosePopupMenu();
	virtual void OpenUserPopupMenu( UidType /*iUnitUID*/, bool /*bPartyMember*/ = false ) { };
	virtual void SetStage( CKTDXStage* pNowState  );

	void ChangeTopImage( IN bool bChange_ );
//#pragma endregion

protected:
	bool	m_bShow;

	CKTDGUIDialogType	m_pDlgPopup;		/// �˾� �޴��� ���� Dialog( 9���� )

	CKTDGUIControl::CPictureData*	m_pPicLeftTop;		/// ��� ����( �׶��̼� ȿ���� ���� 2ĭ�� ũ�⸦ ���� )
	CKTDGUIControl::CPictureData*	m_pPicCenterTop;	/// ��� �߾�
	CKTDGUIControl::CPictureData*	m_pPicRightTop;		/// ��� ������

	CKTDGUIControl::CPictureData*	m_pPicLeftMiddle;	/// �ߴ� ����
	CKTDGUIControl::CPictureData*	m_pPicCenterMiddle;	/// �ߴ� �߾�
	CKTDGUIControl::CPictureData*	m_pPicRightMiddle;	/// �ߴ� ������

	CKTDGUIControl::CPictureData*	m_pPicLeftBottom;	/// �ϴ� ����
	CKTDGUIControl::CPictureData*	m_pPicCenterBottom;	/// �ϴ� �߾�
	CKTDGUIControl::CPictureData*	m_pPicRightBottom;	/// �ϴ� ������

	CX2State*	m_pNowState;

	int		m_nMenuCount;
};
//#pragma endregion
