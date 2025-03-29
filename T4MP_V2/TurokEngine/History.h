#pragma once
#include <unordered_map>

typedef void* (__fastcall* Find_t)(void*,void*,const char* histPath);
typedef const char* (__fastcall* ItemName_t)(void*, void*, int idx);
typedef void* (__fastcall* FindHistoryItem_t)(void*, void*, char*, unsigned __int16 key);



struct CHistoryItem {
	unsigned int value;
	unsigned int index;
};
struct CHistory {
	void* vtable;
	CHistoryItem* items;
};


class CHistories {
public:
	enum multiadavancedoptions;
	enum multiarsenal;
	enum multiplayeroptions;

	static const void* ADDR_FIND;
	static const void* ADDR_ITEMNAME;
	static const void* ADDR_FINDHISTORYITEM;
	
	static Find_t oFind;
	static ItemName_t oItemName;
	static FindHistoryItem_t oFindHistoryItem;

	// Network tracking and option storage
	static char multiadvanced_options[sizeof(multiadavancedoptions)];
	static char multiplayer_options[sizeof(multiplayeroptions)];
	static char multiaresnal_options[sizeof(multiarsenal)];
	
	static void InstallHooks();

	static void* __fastcall Find(void*, void*, const char* histPath);
	static const char* __fastcall ItemName(void*, void*, unsigned int idx);
	static void* __fastcall FindHistoryItem(void*, void*, char*, unsigned __int16 key);

	enum controlsetup {
		normal1,
		flight1,
		lookspring1,
		reverse1,
		vibration1,
		h_speed1,
		v_speed1,
		dummy8,
		dummy9,
		dummy10,

		normal2,
		flight2,
		lookspring2,
		reverse2,
		vibration2,
		h_speed2,
		v_speed2,
		dummy18,
		dummy19,
		dummy20,

		normal3,
		flight3,
		lookspring3,
		reverse3,
		vibration3,
		h_speed3,
		v_speed3,
		dummy28,
		controlsetup_dummy29,
		controlsetup_dummy30,

		normal4,
		flight4,
		lookspring4,
		reverse4,
		vibration4,
		h_speed4,
		v_speed4,
		dummy38,
		dummy39,
		dummy40,

		normal,
		control_setup_flight,
		lookspring,
		reverse,
		vibration,
		h_speed,
		v_speed,
		dummy48,
		dummy49,
		dummy50,

		active_controller,
		was_back_pressed,
		display_index
	};

	enum multiadvanced {
		teamplay,
		friendly_fire,
		rage,
		full_weapons,
		auto_balance,
		radar,
		head_shots_only,
		weapons_stay,
		no_health,
		no_powerups,
		one_shot_kill,
		ulimited_ammo
	};

	enum multiplayerjoin {
		control1,
		model1,
		team1,
		control2,
		model2,
		team2,
		control3,
		model3,
		team3,
		control4,
		model4,
		team4,
		join1,
		join2,
		join3,
		join4,
		dummy1,
		dummy2,
		dummy3,
		not_accept,
		accept,
		loading
	};

	enum multiplayeroptions {
		game_type,
		map,
		frag_limit,
		time_limit,
		bots,
		is_flight
	};

	enum multiarsenal {
		crossbow,
		spike_mine,
		sniper_psitol,
		shotgun,
		minigun,
		launcher,
		tek,
		flamethrower,
		remote_device,
		gravity_disruptor,
		darkmatter_cube,
		dummy12,
		dummy13,
		dummy14,
		dummy15,
		dummy16,
		dummy17,
		crossbow_spawn,
		spike_mine_spawn,
		sniper_pistol_spawn,
		shotgun_spawn,
		minigun_spawn,
		launcher_spawn,
		tek_spawn,
		flamethrower_spawn,
		remote_device_spawn,
		gravity_spawn,
		darkmatter_cube_spawn,
		multiarsenal_dummy29,
		multiarsenal_dummy30,
		dummy31,
		dummy32,
		dummy33,
		dummy34
	};

	enum options {
		autoaim,
		hud,
		crosshair,
		violence,
		sound_mode,
		language,
		sfx_volume,
		music_volume,
		gamma,
		aspect_ratio
	};

};
