#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class CKTDGUIDialog;

// Horizontal anchors (T44): where a dialog or control sits on a screen wider than 4:3. Center keeps the 1024-wide
// layout column centered (the default, and the original layout at 4:3); left and right hold it to that edge of the
// screen. Stretch is drawn as center until the stretched pieces exist (client/widescreen-stretch).
enum class UIAnchor : std::uint8_t { Center, Left, Right, Stretch };

// "CENTER", "LEFT", "RIGHT" or "STRETCH", in any case.
bool ParseUIAnchor( const char* pszName, UIAnchor& eAnchor );

// How a STRETCH piece of a dialog spans the canvas (T44): x in the 1024 layout maps onto the canvas, from its left
// edge to its right. Its zones are the parts that grow (the plain middles between end pieces), in layout units
// relative to the dialog; they share the extra width by length and are linear inside, so a gauge sized by ratio over a
// middle stays proportional, and what lies outside them keeps its size. With no zones the piece is scaled whole: a
// plain fill.
class CKTDGUIStretch
{
public:
	void AddZone( float fMin, float fMax );
	bool HasZones() const { return m_vecZones.empty() == false; }
	// fX in layout units; fBase: the dialog's x; fLeft: the canvas's left edge from the centered column (<= 0);
	// fExtra: the canvas's width beyond 1024. Returns layout units from the centered column.
	float Map( float fX, float fBase, float fLeft, float fExtra ) const;
	float Unmap( float fCanvasX, float fBase, float fLeft, float fExtra ) const;

private:
	std::vector<std::pair<float, float>> m_vecZones;	// sorted, apart
	float m_fZonesWidth = 0.f;
};

// The stretch in effect while a STRETCH piece draws (CKTDXApp::UIAnchorScope).
struct UIStretchState
{
	const CKTDGUIStretch* pZones = nullptr;	// null: no stretch in effect
	float fBase = 0.f, fLeft = 0.f, fExtra = 0.f;
};

// The anchors declared in UIAnchors.lua, by dialog script:
//
//     UI_ANCHORS =
//     {
//         ["DLG_UI_Quick_Slot.lua"] = { DIALOG = "LEFT", CONTROLS = { Static_Exp = "STRETCH" } },
//         ["DLG_Menu_Button_NEW.lua"] = { STRETCH_ZONES = { { 49, 494 }, { 569, 1011 } }, CONTROLS = { ... } },
//     }
//
// Read once, at the first dialog script. A script's own SetAnchor calls take precedence; what neither declares is
// centered.
class CKTDGUIAnchors
{
public:
	// Applies the anchors declared for the script just loaded into the dialog.
	static void Apply( CKTDGUIDialog& kDialog, const wchar_t* pwszScriptName );

	struct Entry
	{
		bool bHasDialog = false;
		UIAnchor eDialog = UIAnchor::Center;
		std::unordered_map<std::wstring, UIAnchor> mapControls;
		std::vector<std::pair<float, float>> vecStretchZones;
	};

private:
	static const std::unordered_map<std::wstring, Entry>& Get();
	static std::unordered_map<std::wstring, Entry> Load();
};

#if X2_DEV_TOOLS
// Dev tool for the anchor migration (tools/ui-anchors): with X2_RECORD_UI_ANCHORS=1, every dialog drawn from a script
// notes what each of its controls drew (in layout units, relative to the dialog) and where the dialog was, into
// UIAnchorsRecord.tsv in the game folder, merged with what earlier runs recorded.
class CKTDGUIAnchorRecorder
{
public:
	static bool IsOn();
	static void Note( CKTDGUIDialog& kDialog );
};
#endif
