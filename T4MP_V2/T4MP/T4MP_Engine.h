#pragma once
#include "../TurokEngine/OldEngineStruct.h"
class T4MP {
public:
	static const void* ADDR_PLAYERCAMERAOSDINITJMP;
	static const void* ADDR_PUSHLEVELSOON;
	static const void* ADDR_PUSHPENDINGLEVEL;
	static const void* ADDR_POPLEVEL;
	static const void* ADDR_POPLEVELS;
	static const void* ADDR_UPDATE;
	static const void* ADDR_TRIGGERHISTORYLINKSINLEVELS;
	static const void* ADDR_JOYSTICKHANDLERUPDATEOBJECT;

	static DMPlayer* GetLocalPlayer();
	static DMPlayer* GetDMPlayer(int index);
	static DMPlayer* SpawnPlayer();
};