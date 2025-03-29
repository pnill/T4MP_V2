#pragma once
#include "types.h"

struct x_std_string {
	char m_Data[256];
};

x_std_string* create_x_std_string(const char* text);
void free_x_std_string(x_std_string*);

struct CLevelCreationInfo
{
	x_std_string *field_0;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
};

typedef void* (__fastcall* PushLevel_t)(void*, void*, CLevelCreationInfo*);
typedef void  (__fastcall* PushPendingLevel_t)(void*, void*, char* pszLevelPath, char a3, char a4);
typedef void* (__fastcall* PopLevel_t)(void*, void*);
typedef void* (__fastcall* PushLevelSoon_t)(void*, void*, char*, int, int, char*);
typedef void* (__fastcall* PopLevels_t)(void*, void*);
typedef void  (__fastcall* Update_t)(void*, void*, float);
typedef void* (__fastcall* TriggerHistoryLinksInLevels_t)(void*, void*, int, int);
typedef void  (__fastcall* JoyStickHandlerUpdateObject_t)(void*, void*, void*);

class CGame {
public:
	typedef CLevelCreationInfo CLevelCreationInfo;

	/*
		CGame::PushPendingLevel(v4, *(const char **)&cActor3a->bThirdPerson, v7, v6);
		Cgame::PopLevel(g_Game);
		CGame::PushLevelSoon((int)v4, *(const char **)(this + 44), v7, v6, *(const char **)(this + 48));
	*/
	static const void* ADDR_PUSHLEVEL;
	static const void* ADDR_PUSHLEVELSOON;
	static const void* ADDR_PUSHPENDINGLEVEL;
	static const void* ADDR_POPLEVEL;
	static const void* ADDR_POPLEVELS;
	static const void* ADDR_UPDATE;
	static const void* ADDR_TRIGGERHISTORYLINKSINLEVELS;
	static const void* ADDR_JOYSTICKHANDLERUPDATEOBJECT;

	static bool levelPopLocked;

	static PushLevel_t oPushLevel;
	static PushPendingLevel_t oPushPendingLevel;
	static PopLevel_t oPopLevel;
	static PopLevels_t oPopLevels;
	static PushLevelSoon_t oPushLevelSoon;
	static Update_t oUpdate;
	static TriggerHistoryLinksInLevels_t oTriggerHistoryLinksInLevels;
	static JoyStickHandlerUpdateObject_t oJoyStickHandlerUpdateObject;

	static void InstallHooks();

	static void ForceInGamePause();
	static void ForceToMainMenu();
	static void LockLevelPop(bool bIsLocked);
	static void PopOneLevel();
	static void ForceToMultiplayerJoin();

	static void* __fastcall PushLevel(void* pThis, void* _, CLevelCreationInfo* pLevelCreationInfo);
	static void  __fastcall PushPendingLevel(void* pThis, void* _, char* pszLevelPath, char a3, char a4);
	static void* __fastcall PopLevel(void* pThis, void* _);
	static void* __fastcall PopLevels(void* pThis, void* _);
	static void* __fastcall PushLevelSoon(void* pThis, void* _, char* pszLevelPath, int a3, int a4, char* pszUnk);
	static void  __fastcall Update(void* pThis, void* _, float unk);
	static void* __fastcall TriggerHistoryLinksInLevels(void* pThis, void* _, int a2, int a3);
	static void	 __fastcall JoyStickHandlerUpdateObject(void* pThis, void* _, void* updateObject);
};