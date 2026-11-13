#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

class CKTDGUIDialog;

// Horizontal anchors (T44): where a dialog or control sits on a screen wider than 4:3. Center keeps the 1024-wide
// layout column centered (the default, and the original layout at 4:3); left and right hold it to that edge of the
// screen. Stretch is drawn as center until the stretched pieces exist (client/widescreen-stretch).
enum class UIAnchor : std::uint8_t { Center, Left, Right, Stretch };

// "CENTER", "LEFT", "RIGHT" or "STRETCH", in any case.
bool ParseUIAnchor( const char* pszName, UIAnchor& eAnchor );

// The anchors declared in UIAnchors.lua, by dialog script:
//
//     UI_ANCHORS =
//     {
//         ["DLG_UI_Quick_Slot.lua"] = { DIALOG = "LEFT", CONTROLS = { Static_Exp = "STRETCH" } },
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
	};

private:
	static const std::unordered_map<std::wstring, Entry>& Get();
	static std::unordered_map<std::wstring, Entry> Load();
};
