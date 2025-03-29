#include <stdio.h>
#include "../3rdparty\MinHook_133_lib\include\MinHook.h"
#include "../GUI/imgui_debugconsole.h"
#include "../T4MP/Networking/network_handlers.h"
#include "../T4MP/T4MP_Engine.h"
#include "OldEngineStruct.h"
#include "Game.h"

T4Engine * g_Game = (T4Engine*)0x6B52E4;
T4Game** g_Turok4Game = (T4Game**)0x006B52B4;


const void* CGame::ADDR_PUSHLEVEL = (void*)0x00503FD0;
const void* CGame::ADDR_POPLEVEL = (void*)0x00501F40;
const void* CGame::ADDR_POPLEVELS = (void*)0x00501F60;
const void* CGame::ADDR_PUSHLEVELSOON = (void*)0x00502800;
const void* CGame::ADDR_PUSHPENDINGLEVEL = (void*)0x00503830;
const void* CGame::ADDR_UPDATE = (void*)0x004849C0;
const void* CGame::ADDR_TRIGGERHISTORYLINKSINLEVELS = (void*)0x005021A0;
const void* CGame::ADDR_JOYSTICKHANDLERUPDATEOBJECT = (void*)0x004FAA20;

PushLevel_t CGame::oPushLevel = nullptr;
PushLevelSoon_t CGame::oPushLevelSoon = nullptr;
PushPendingLevel_t CGame::oPushPendingLevel = nullptr;
PopLevel_t CGame::oPopLevel = nullptr;
PopLevels_t CGame::oPopLevels = nullptr;
Update_t CGame::oUpdate = nullptr;
TriggerHistoryLinksInLevels_t CGame::oTriggerHistoryLinksInLevels = nullptr;
JoyStickHandlerUpdateObject_t CGame::oJoyStickHandlerUpdateObject = nullptr;

bool g_HistoryHasUpdate = false;

bool CGame::levelPopLocked = false;

void CGame::LockLevelPop(bool bIsLocked)
{
	levelPopLocked = bIsLocked;
}

void CGame::ForceInGamePause()
{
	oPushLevelSoon(*(g_Turok4Game), 0, "$/Data/Levels/u_jcowlishaw/screens/ingamepause/ingamepause.atr", 1, 0, "");
}

void CGame::ForceToMultiplayerJoin()
{
	//CGame::oPopLevels(*(g_Turok4Game), 0);
	//CGame::oPushLevelSoon(*(g_Turok4Game), 0, "$/Data/Levels/u_jcowlishaw/screens/multiplayerjoin/multiplayerjoin.atr", 0, 0, "");
	CGame::oPushPendingLevel(*(g_Turok4Game), 0, "$/Data/Levels/u_jcowlishaw/screens/multiplayerjoin/multiplayerjoin.atr", 1, 1);
	CGame::oPopLevel(*(g_Turok4Game), 0);
	CGame::oPushLevelSoon(*(g_Turok4Game), 0, "$/Data/Levels/u_jcowlishaw/screens/multiplayerjoin/multiplayerjoin.atr", 1, 1, "");
}

void CGame::ForceToMainMenu()
{
	CGame::oPopLevels(*(g_Turok4Game), 0);
	CGame::oPushLevelSoon(*(g_Turok4Game), 0, "$/Data/Levels/u_jcowlishaw/screens/FirstInteractiveScreen/firstinteractivescreen.atr", 0, 0, "");
	CGame::oPushLevelSoon(*(g_Turok4Game), 0, "$/Data/Levels/u_jcowlishaw/screens/frontendbackground/frontendbackground.atr", 0, 0, "");
}

void CGame::PopOneLevel()
{
	CGame::oPopLevel(*(g_Turok4Game), 0);
}

// Function to create a new x_std_string with metadata
x_std_string* create_x_std_string(const char* text) {
	size_t length = strlen(text);
	size_t maxLength = length + 10;  // Add some buffer, or set as needed

									 // Allocate memory for the metadata + structure
									 // We need 8 bytes for metadata (length + maxLength) + structure size
	char* memory = (char*)malloc(8 + sizeof(x_std_string));

	// Set the metadata
	int* lengthPtr = (int*)memory;
	int* maxLengthPtr = (int*)(memory + 4);
	*lengthPtr = length;
	*maxLengthPtr = maxLength;

	// Get pointer to the actual structure (after metadata)
	x_std_string* str = (x_std_string*)(memory + 8);

	// Copy the string data
	strcpy(str->m_Data, text);

	return str;
}

// Function to free the allocated memory properly
void free_x_std_string(x_std_string* str) {
	// We need to get the original allocation pointer
	// which is 8 bytes before the structure
	void* originalPtr = (char*)str - 8;
	free(originalPtr);
}


void CGame::InstallHooks()
{
	MH_STATUS status = MH_ERROR_DISABLED;

	// Hook for PushLevel
	status = MH_CreateHook((LPVOID)ADDR_PUSHLEVEL, (LPVOID)CGame::PushLevel, (PVOID*)&oPushLevel);
	if (status != MH_OK)
	{
		g_Console.AddLog("[CGame] MH_CreateHook failed on CGame::PushLevel!\n");
	}
	else
	{
		g_Console.AddLog("[CGame] ::PushLevel hook installed\n");
	}

	// Hook for PushPendingLevel
	status = MH_CreateHook((LPVOID)ADDR_PUSHPENDINGLEVEL, (LPVOID)CGame::PushPendingLevel, (PVOID*)&oPushPendingLevel);
	if (status != MH_OK)
	{
		g_Console.AddLog("[CGame] MH_CreateHook failed on CGame::PushPendingLevel!\n");
	}
	else
	{
		g_Console.AddLog("[CGame] ::PushPendingLevel hook installed\n");
	}

	// Hook for PopLevel
	status = MH_CreateHook((LPVOID)ADDR_POPLEVEL, (LPVOID)CGame::PopLevel, (PVOID*)&oPopLevel);
	if (status != MH_OK)
	{
		g_Console.AddLog("[CGame] MH_CreateHook failed on CGame::PopLevel!\n");
	}
	else
	{
		g_Console.AddLog("[CGame] ::PopLevel hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_POPLEVELS, (LPVOID)CGame::PopLevels, (PVOID*)&oPopLevels);
	if (status != MH_OK)
	{
		g_Console.AddLog("[CGame] MH_CreateHook failed on CGame::PopLevels!\n");
	}
	else
	{
		g_Console.AddLog("[CGame] ::PopLevels hook installed\n");
	}

	// Hook for PushLevelSoon
	status = MH_CreateHook((LPVOID)ADDR_PUSHLEVELSOON, (LPVOID)CGame::PushLevelSoon, (PVOID*)&oPushLevelSoon);
	if (status != MH_OK)
	{
		g_Console.AddLog("[CGame] MH_CreateHook failed on CGame::PushLevelSoon!\n");
	}
	else
	{
		g_Console.AddLog("[CGame] ::PushLevelSoon hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_UPDATE, (LPVOID)CGame::Update, (PVOID*)&oUpdate);
	if (status != MH_OK)
	{
		g_Console.AddLog("[CGame] MH_CreateHook failed on CGame::Update!\n");
	}
	else
	{
		g_Console.AddLog("[CGame] ::Update hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_TRIGGERHISTORYLINKSINLEVELS, (LPVOID)CGame::TriggerHistoryLinksInLevels, (PVOID*)&oTriggerHistoryLinksInLevels);
	if (status != MH_OK)
	{
		g_Console.AddLog("[CGame] MH_CreateHook failed on CGame::TriggerHistoryLinksInLevels!\n");
	}
	else
	{
		g_Console.AddLog("[CGame] ::TriggerHistoryLinksInLevels hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_JOYSTICKHANDLERUPDATEOBJECT, (LPVOID)CGame::JoyStickHandlerUpdateObject, (PVOID*)&oJoyStickHandlerUpdateObject);
	if (status != MH_OK)
	{
		g_Console.AddLog("[CGame] MH_CreateHook failed on CGame::JoyStickHandlerUpdateObject!\n");
	}
	else
	{
		g_Console.AddLog("[CGame] ::JoyStickHandlerUpdateObject hook installed\n");
	}

	// Enable all hooks
	status = MH_EnableHook(MH_ALL_HOOKS);
	if (status != MH_OK)
	{
		printf("[-] MH_EnableHook failed in CGame::InstallHooks()!\n");
	}
}


void CGame::Update(void* pThis, void* _, float unk)
{
	g_Console.AddLog("[CGame] :: Update unk float: %f",unk);
	NetworkHandlers::Update();
	return oUpdate(pThis, _, unk);
}

void* CGame::PushLevel(void* pThis, void* _, CLevelCreationInfo* pLevelCreationInfo)
{
	return oPushLevel(pThis, _, pLevelCreationInfo);
}

void CGame::PushPendingLevel(void* pThis, void* _, char* pszLevelPath, char a3, char a4)
{
	g_Console.AddLog("[CGame] ::PushPendingLevel (pThis: %08X, pszLevelPath: %s, a3: %i, a4: %i)",pThis,pszLevelPath,a3,a4);
	return oPushPendingLevel(pThis, _, pszLevelPath, a3, a4);
}

void* CGame::PopLevel(void* pThis, void* _)
{

	g_Console.AddLog("[CGame] ::PopLevel (pThis: %08X)", pThis);
	if (levelPopLocked)
	{
		g_Console.AddLog("[CGame] ::PopLevel was locked by levelPopLocked");
		return nullptr;
	}
	return oPopLevel(pThis, _);
}

void* CGame::PopLevels(void* pThis, void* _)
{
	g_Console.AddLog("[CGame] ::PopLevels (pThis: %08X)", pThis);

	return oPopLevels(pThis, _);
}


void* CGame::PushLevelSoon(void* pThis, void* _, char* pszLevelPath, int a3, int a4, char* pszUnk)
{
	g_Console.AddLog("[CGame] ::PushLevelSoon (pThis: %08X, pszLevelPath: %s, a3: %i, a4: %i, pszUnk: %s)", pThis, pszLevelPath, a3, a4, pszUnk);
	
	return oPushLevelSoon(pThis, _, pszLevelPath, a3, a4, pszUnk);
}

void* CGame::TriggerHistoryLinksInLevels(void* pThis, void* _, int a2, int a3)
{
	g_Console.AddLog("[CGame] ::TriggerHistoryLinksInLevels( pThis: %08X, a2: %08X, a3: %08X )", pThis, a2, a3);
	return oTriggerHistoryLinksInLevels(pThis, _, a2, a3);
}

void CGame::JoyStickHandlerUpdateObject(void* pThis, void* _, void* ObjectUpdated)
{
	DMPlayer* player = T4MP::GetLocalPlayer();
	g_Console.AddLog("CGame::JoyStickHandlerUpdateObject - Player: %08X, object: %08X, pThis: %08X", player, ObjectUpdated, pThis);

	if ((DMPlayer*)ObjectUpdated != T4MP::GetDMPlayer(0))
		return;

	return oJoyStickHandlerUpdateObject(pThis, _, ObjectUpdated);
}