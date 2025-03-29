#include "History.h"
#include <stdio.h>
#include "../3rdparty\MinHook_133_lib\include\MinHook.h"
#include "../GUI/imgui_debugconsole.h"
#include "../T4MP/Networking/enet_wrapper.h"
#include "../T4MP/Networking/network_serialization.h"
#include "../TurokEngine/Game.h"
#include "../TurokEngine/types.h"
#include "../TurokEngine/OldEngineStruct.h"

const void* CHistories::ADDR_FIND = (void*)0x529650;
const void* CHistories::ADDR_ITEMNAME = (void*)0x5298E0;
const void* CHistories::ADDR_FINDHISTORYITEM = (void*)0x5296A0;

Find_t CHistories::oFind = nullptr;
ItemName_t CHistories::oItemName = nullptr;
FindHistoryItem_t CHistories::oFindHistoryItem = nullptr;
extern bool g_IsHost;
extern bool g_invertControls;
extern bool g_HistoryHasUpdate;

// Network tracking and option storage
char CHistories::multiadvanced_options[sizeof(CHistories::multiadavancedoptions)] = { 0 };
char CHistories::multiplayer_options[sizeof(CHistories::multiplayeroptions)] = { 0 };
char CHistories::multiaresnal_options[sizeof(CHistories::multiarsenal)] = { 0 };

extern Network g_Network;

void CHistories::InstallHooks()
{
	MH_STATUS status = MH_ERROR_DISABLED;
	status = MH_CreateHook((LPVOID)ADDR_FIND, (LPVOID)CHistories::Find, (PVOID*)&oFind);
	if (status != MH_OK)
	{
		g_Console.AddLog("[-] MH_CreateHook failed on CHistories::Find!\n");
	}
	else 
	{
		g_Console.AddLog("[*] CHistories::Find hook installed\n");
	}

	/*
	status = MH_CreateHook((LPVOID)ADDR_FINDHISTORYITEM, (LPVOID)CHistories::FindHistoryItem, (PVOID*)&oFindHistoryItem);
	if (status != MH_OK)
	{
		printf("[-] MH_CreateHook failed on CHistories::FindHistoryItem\n");
	}
	else
	{
		printf("[*] CHistories::FindHistoryItem hook installed\n");
	}*/

	/*
		status = MH_CreateHook((LPVOID)ADDR_ITEMNAME, (LPVOID)CHistories::ItemName, (PVOID*)&oItemName);
		if (status != MH_OK)
		{
			printf("[-] MH_CreateHook failed on CHistories::ItemName!\n");
		}
		else
		{
			printf("[*] CHistories::ItemName hook installed\n");
		}
	*/
	status = MH_EnableHook(MH_ALL_HOOKS);
	if (status != MH_OK)
	{
		g_Console.AddLog("[-] MH_EnableHook failed in CHistories::InstallHooks()!\n");
	}
}

/*
	CHistories::Find(const char* histPath)
	
	From what I can tell the game uses this to grab history files that are already in memory,
	I guess the idea is they're used by maps, settings menus etc to determine several things.

	Levels use it as a way of handling non-persistent data storage tracking if they have the key or etc.
	It seems these files are created by an external world editor, where they have some ability to add new keys.
	Then they could make their levels read from those keys and commit actions with minimal to no programming.
*/
void* __fastcall CHistories::Find(void* pThis, void* _, const char* path)
{
	g_Console.AddLog("[CHistories] ::Find(pThis: %08X, path: %s)\n",pThis,path);
	CHistory* ret = (CHistory*)oFind(pThis, _, path);


	// The game determines if controls are inverted by reading from here.
	// The actual in-game settings overwrite the file's data.
	// Instead we just let ImGui's global handle it.
	// That way we can adjust control settings from our own menus.
	if (strcmp("$\\Data\\History\\ControlSetup.hst", path) == 0)
	{
		ret->items[controlsetup::reverse].value = g_invertControls;
	}

	// Because the game had a split-screen mode 'hidden'.
	// We're able to get into it by making it believe the second player joined/accepted.
	// We do this by modifying multiplayerjoin.atr.
	// Once the actual match is starting however, we want the game to believe no one joined.
	// This means no one but the first player will be spawned/have properties like a a camera created for them.
	if (strcmp("$\\Data\\History\\MultiPlayerJoin.hst", path) == 0)
	{
		ret->items[multiplayerjoin::join2].value = 0;
		
	}


	// Everything from here has more to do with network syncing than anything else.
	// The host will choose what map to play on and setup various other game settings..
	// When they do this we'll want to keep track of that information for people joining.
	// This is how we're accomplishing that.
	// The network code will then read from these arrays of stored info, or...
	// If they're a client the network code will write these arrays with the host's settings.
	if (strcmp("$\\Data\\History\\Multiplayer.hst", path) == 0)
	{
		for (int i = 0; i < sizeof(multiplayeroptions); i++)
		{
			if (g_IsHost)
			{
				multiplayer_options[i] = ret->items[i].value;
				std::vector<uint8_t> packet = NetworkSerializer::CreateHistoryUpdatePacket(
					0,
					(uint8_t*)multiplayer_options,
					sizeof(multiplayer_options)
					);
				g_Network.broadcastData(packet, 0, true);
			}
			else
			{
				// Make sure if the map has actually changed we force the event
				if(ret->items[multiplayeroptions::map].value != multiplayer_options[multiplayeroptions::map])
					CGame::TriggerHistoryLinksInLevels(*(g_Turok4Game), 0, 0x00070002, multiplayer_options[multiplayeroptions::map]);
					// Just to update the timer.
					CGame::TriggerHistoryLinksInLevels(*(g_Turok4Game), 0, 0x00070004, 1);

		
				ret->items[i].value = multiplayer_options[i];
			}
		}
	}

	if (strcmp("$\\Data\\History\\MultiAdvancedOptions.hst", path) == 0)
	{
		for (int i = 0; i < sizeof(multiadvanced_options); i++)
		{
			if (g_IsHost)
			{
				multiadvanced_options[i] = ret->items[i].value;
				std::vector<uint8_t> packet = NetworkSerializer::CreateHistoryUpdatePacket(
					1,
					(uint8_t*)multiadvanced_options,
					sizeof(multiadvanced_options)
					);
				g_Network.broadcastData(packet, 0, true);
			}
			else
				ret->items[i].value = multiadvanced_options[i];
		}
	

	}

	g_Console.AddLog("[CHistories] ::Find() ret = %08X\n", ret);
	return ret;
}


/*
	CHistories::ItemName(unsigned int unk)

	Originally, I thought this was an "ItemName" being returned based on an index.
	
	The code doesn't seem to indicate that when staring at it and observing the
	response it doesn't seem like the return is actually a "const char" either.
	
*/
const char* __fastcall CHistories::ItemName(void* pThis, void* _, unsigned int idx)
{
	const char* ret = oItemName(pThis, _, idx);
	g_Console.AddLog("[CHistories] ::ItemName(pThis: %08X, idx: %d, idx(hex):  %08X, ret: %08X)\n", ret);
	return ret;
}


/*
	CHIstories::FindHistoryItem(char* histPath, unsigned __int16 key);
	
	This one is mostly wrong too, I don't believe 'key' is an int16.
	It's more than likely an integer.

	The thing that's returned however is a "HistoryItem" object, which I believe
	is each entry we return when resolving the data in "FindItem".

	I just haven't spent enough time reverse engineering those routines to determine how they really work.
*/
void* __fastcall CHistories::FindHistoryItem(void* pThis, void* _, char* histPath, unsigned __int16 key)
{
	void* ret = oFindHistoryItem(pThis, _, histPath, key);
	g_Console.AddLog("[CHistories] ::FindHistoryItem(pThis: %08X, histPath: %s, key: %i)\n",pThis,histPath,key);
	g_Console.AddLog("[CHistories] ::FindHistoryItem ret = %08X\n", ret);
	return ret;
}