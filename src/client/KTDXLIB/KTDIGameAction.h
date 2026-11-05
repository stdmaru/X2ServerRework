#pragma once

// Game actions and joystick inputs of the action map (split from KTDIManager.h so that KTDIDevice.h can be included on
// its own; docs/porting/client-input.md).
//
// Every action used in the game, by function rather than by key. The map goes GameAction -> key, so several actions can
// share one key, but an action has a single key. To add a configurable action, add it to GAME_ACTION, add a GA_ macro
// (for compatibility with the older code) and give it a key in CKTDIManager::SetDefaultMap. To read a key directly, as
// before, use Getkeyboard()->GetKeyState( DIK_?? ).

#include "DirectInputKeys.h"

// Joystick inputs as stored in the joystick action map: DIJOFS_BUTTON(n) offsets into DIJOYSTATE for buttons, then
// pseudo-inputs read from the axes and the POV.
enum JOYSTICK_BUTTON
{
	JB_BUTTON0 = DIJOFS_BUTTON0,
	JB_BUTTON1,
	JB_BUTTON2,
	JB_BUTTON3,
	JB_BUTTON4,
	JB_BUTTON5,
	JB_BUTTON6,
	JB_BUTTON7,
	JB_BUTTON8,
	JB_BUTTON9,
	JB_BUTTON10,
	JB_BUTTON11,
	JB_BUTTON12,
	JB_BUTTON13,
	JB_BUTTON14,
	JB_BUTTON15,
	JB_BUTTON16,
	JB_BUTTON17,
	JB_BUTTON18,
	JB_BUTTON19,
	JB_BUTTON20,
	JB_BUTTON21,
	JB_BUTTON22,
	JB_BUTTON23,
	JB_BUTTON24,
	JB_BUTTON25,
	JB_BUTTON26,
	JB_BUTTON27,
	JB_BUTTON28,
	JB_BUTTON29,
	JB_BUTTON30,
	JB_BUTTON31,
	JB_UP,
	JB_LEFT,
	JB_RIGHT,
	JB_DOWN,
	JB_LEFT_TRIGGER,
	JB_RIGHT_TRIGGER,
};

enum GAME_ACTION
{

	GAME_ACTION_NONE = -1,

	GAMEACTION_ATTACK_FAST,		/// command, weak attack
	GAMEACTION_ATTACK_STRONG,	/// command, strong attack
	GAMEACTION_SKILL1,			/// skill slot 1
	GAMEACTION_SKILL2,			/// skill slot 2
	GAMEACTION_SKILL3,			/// skill slot 3
	GAMEACTION_SKILL4,			/// skill slot 4
	GAMEACTION_SKILL_EX1,		/// extended skill slot 1
	GAMEACTION_SKILL_EX2,		/// extended skill slot 2
	GAMEACTION_SKILL_EX3,		/// extended skill slot 3
	GAMEACTION_SKILL_EX4,		/// extended skill slot 4

	GAMEACTION_SLOT_CHANGE,		/// swap skill slots
	GAMEACTION_PVE,				/// dungeon tool
	GAMEACTION_PVP,				/// PvP tool
	GAMEACTION_ZOOMIN,			/// zoom in
	GAMEACTION_ZOOMOUT,			/// zoom out
	GAMEACTION_QUEST,			/// quest window
	GAMEACTION_MAP,				/// world map, dungeon progress
	GAMEACTION_COMMUNITY,		/// community window
	GAMEACTION_INFO,			/// character information window
	GAMEACTION_INVENTORY,		/// inventory window
	GAMEACTION_OPTION,			/// system options
	GAMEACTION_PET,				/// pet window
	GAMEACTION_GUILD,			/// guild window
	GAMEACTION_SKILL_TREE,		/// skill window
	GAMEACTION_NAME,			/// monster names on/off
	GAMEACTION_FRIEND,			/// friend window
	GAMEACTION_AWAKE,			/// awakening

	GAMEACTION_QUICKSLOT1,		/// quick slot 1
	GAMEACTION_QUICKSLOT2,		/// quick slot 2
	GAMEACTION_QUICKSLOT3,		/// quick slot 3
	GAMEACTION_QUICKSLOT4,		/// quick slot 4
	GAMEACTION_QUICKSLOT5,		/// quick slot 5
	GAMEACTION_QUICKSLOT6,		/// quick slot 6

	GAMEACTION_UP,				/// up
	GAMEACTION_DOWN,			/// down
	GAMEACTION_LEFT,			/// left
	GAMEACTION_RIGHT,			/// right

	GAMEACTION_SIT,				/// sit

	GAMEACTION_RIDING,			/// ride

	GAMEACTION_RELATION,		/// couple/solo community window (marriage system)

	GAMEACTION_WARP,			/// Cobo Express (SERV_ADD_WARP_BUTTON)

	GAMEACTION_RETURN,			/// confirm, except in the chat box
	GAMEACTION_ESCAPE,			/// cancel; unlike the keyboard's Esc, it does not open the system menu or call LastMsgByEsc
	GAMEACTION_PARTYREADY,		/// F8

	GAMEACTION_END,
	GAMEACTION_TOTAL,			// total
};
