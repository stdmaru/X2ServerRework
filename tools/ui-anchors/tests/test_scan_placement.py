import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import scan_placement  # noqa: E402

HEADER = """
class CX2State
{
public:
	class MouseClickUI
	{
	public:
		MouseClickUI( CKTDXStage* pStage, D3DXVECTOR2 vPos, float fTime, int nCount );
	};
	void CreateMouseClickUI( CKTDXStage* pStage, D3DXVECTOR2 vPos, float fTime = 0.5f, int nCount = 1000 )
	{
		m_pMouseClickUI = new MouseClickUI( pStage, vPos, fTime, nCount );
	}
	CKTDGUIDialogType KTDGUIOKMsgBox( D3DXVECTOR2 pos, const WCHAR* pText );
	CKTDGXMeshPlayer::CXMeshInstance* m_pMeshInst;
};
"""

SOURCE = """
CX2State::MouseClickUI::MouseClickUI( CKTDXStage* pStage, D3DXVECTOR2 vPos, float fTime, int nCount )
{
	m_pDLGMouseClick = new CKTDGUIDialog( pStage, L"DLG_Mouse_Click.lua" );
	m_pDLGMouseClick->SetPos( vPos );
	vPos -= D3DXVECTOR2( 10, 10 );
	m_pDLGMouseClick->Move( vPos, D3DXCOLOR(1,1,1,1), fTime, false );
}

	CKTDGUIDialogType CX2State::KTDGUIOKMsgBox( D3DXVECTOR2 pos, const WCHAR* pText )
{
	CKTDGUIDialogType pDialog = new CKTDGUIDialog( this, L"DLG_UI_OKMsgBox.lua" );
	pos.y -= 50.f;
	pDialog->SetPos( pos );
	return pDialog;
}

void CX2State::Guide( int step )
{
	D3DXVECTOR2 vPos = D3DXVECTOR2( 0, 0 );
	switch( step )
	{
	case 0: vPos = D3DXVECTOR2( 783, 233 ); break;
	case 1: vPos = D3DXVECTOR2( 135, 150 ); break;
	}
	CreateMouseClickUI( this, vPos, 0.5f, 1000 );
	CreateMouseClickUI( this, D3DXVECTOR2( 805, 215 ) );
	KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), L"hello" );
	KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), L"default" );
}

void CX2State::Layout()
{
	m_pDLGQuest = new CKTDGUIDialog( this, L"DLG_Quest.lua" );
	m_pDLGQuest->SetPos( D3DXVECTOR2( 810, 200 ) );
	m_pDLGQuest->SetPos( D3DXVECTOR2( g_pKTDXApp->GetMousePos().x, 1024 - 40 ) );
	m_pMeshInst->SetPos( D3DXVECTOR3( 0, 0, 0 ) );     // a mesh: not UI
	pStatic->SetOffsetPos( D3DXVECTOR2( 4, 8 ) );
	pPicture->SetSize( vSize );
	// m_pDLGQuest->SetPos( D3DXVECTOR2( 1, 1 ) );
#if 0
	m_pDLGQuest->SetPos( D3DXVECTOR2( 2, 2 ) );
#endif
	m_pDLGQuest->SetAnchor( UIAnchor::Left );
	m_pDLGEmpty = new CKTDGUIDialog( this, NULL );
	CKTDGUIStatic* pMade = new CKTDGUIStatic();
	g_pData->GetUIMajorParticle()->CreateSequence( NULL, L"QuestMark", 512, 384, 0 );
}
"""


class ScanPlacementTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        root = Path(self.tmp.name)
        (root / "X2State.h").write_text(HEADER, encoding="utf-8")
        (root / "X2State.cpp").write_text(SOURCE, encoding="utf-8")
        self.sites = scan_placement.scan([root])

    def tearDown(self):
        self.tmp.cleanup()

    def find(self, **want):
        return [s for s in self.sites if all(getattr(s, k) == v for k, v in want.items())]

    def test_fixed_dialog_spot(self):
        [site] = self.find(call="SetPos", receiver="m_pDLGQuest", kind="A")
        self.assertEqual(site.script, "DLG_Quest.lua")
        self.assertEqual(site.function, "CX2State::Layout")

    def test_derived_and_screen_literals(self):
        [site] = self.find(call="SetPos", receiver="m_pDLGQuest", kind="C")
        self.assertIn("mouse", site.flags)
        self.assertIn("screen", site.flags)

    def test_comments_disabled_code_and_meshes_are_skipped(self):
        self.assertFalse([s for s in self.sites if "( 1, 1 )" in s.args or "( 2, 2 )" in s.args])
        self.assertFalse(self.find(receiver="m_pMeshInst"))

    def test_spots_through_functions(self):
        # CreateMouseClickUI (inline in the header) -> MouseClickUI -> SetPos( vPos ): the spots are at the callers.
        spots = self.find(call="CreateMouseClickUI", kind="A")
        self.assertEqual(len(spots), 2)
        self.assertTrue(all(s.script == "DLG_Mouse_Click.lua" for s in spots))
        via_switch = [s for s in spots if "783" in s.args][0]
        self.assertIn("D3DXVECTOR2( 135, 150 )", via_switch.args)
        self.assertTrue(any("805, 215" in s.args for s in spots))

    def test_message_box_default_spot(self):
        [fixed] = self.find(call="KTDGUIOKMsgBox", kind="A")
        self.assertIn("250, 300", fixed.args)
        [default] = self.find(call="KTDGUIOKMsgBox", kind="C")
        self.assertIn("default", default.flags)

    def test_other_kinds(self):
        self.assertTrue(self.find(call="SetOffsetPos", kind="D"))
        self.assertTrue(self.find(call="SetSize", kind="data"))
        self.assertTrue(self.find(call="SetAnchor", kind="F"))
        self.assertTrue(self.find(call="new CKTDGUIDialog", kind="G"))
        self.assertTrue(self.find(call="new CKTDGUIStatic", kind="G"))
        [particle] = self.find(kind="E")
        self.assertEqual(particle.script, "QuestMark")
        self.assertIn("screen", particle.flags)


if __name__ == "__main__":
    unittest.main()
