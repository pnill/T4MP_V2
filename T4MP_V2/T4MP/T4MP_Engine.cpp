#include <windows.h>
#include <stdio.h>
#include "T4MP_Engine.h"
#include "../TurokEngine/Game.h"
#include "../TurokEngine/Level.h"
#include "../TurokEngine/OldEngineStruct.h"


// During the player inititialization there's a jump that checks if a player should have OSD.
// It only fails if the constructor fails, we force it to believe it did.
const void* T4MP::ADDR_PLAYERCAMERAOSDINITJMP = (void*)0x004DAD49;

DMPlayer* T4MP::GetLocalPlayer()
{
	T4Game** g_Turok4Game = (T4Game**)0x006B52B4;
	return (*g_Turok4Game)->pDMPlayer;
}

DMPlayer* T4MP::GetDMPlayer(int index)
{
	T4Engine * TurokEngine = (T4Engine*)0x6B52E4;
	if (TurokEngine->pT4Game)
		if (TurokEngine->pT4Game->pEngineObjects)
			if (TurokEngine->pT4Game->pEngineObjects->pCameraArray[index])
				if (TurokEngine->pT4Game->pEngineObjects->pCameraArray[index]->pActor)
				{
					if (TurokEngine->pT4Game->pEngineObjects->pCameraArray[index]->pActor->pDMPlayer)
						return TurokEngine->pT4Game->pEngineObjects->pCameraArray[index]->pActor->pDMPlayer;
				}
				else
				{
					return (DMPlayer*)TurokEngine->pT4Game->pEngineObjects->pCameraArray[index]->pPlayer;
				}


	return 0;
}


DMPlayer* T4MP::SpawnPlayer()
{
	T4Engine * TurokEngine = (T4Engine*)0x6B52E4;
	char *spawn_path = new char[255];
	ZeroMemory(spawn_path, 255);

	sprintf_s(spawn_path, strlen("$/Data/Actors/multiplayer\\Players\\TalSetPlayer\\TalSetPlayer.atr") + 1, "%s", "$/Data/Actors/multiplayer\\Players\\TalSetPlayer\\TalSetPlayer.atr");

	char *spawn_name = new char[20];
	ZeroMemory(spawn_name, 20);

	sprintf_s(spawn_name, strlen("DMPlayer") + 1, "%s", "DMPlayer");

	/* A position must be supplied to the function we're using to create the player so we'll use a random one. */
	vector3 npos_struct;
	npos_struct.x = 12.0f;
	npos_struct.y = 10.0f;
	npos_struct.z = 5.0f;

	DWORD dwOld;
	VirtualProtect((BYTE*)ADDR_PLAYERCAMERAOSDINITJMP, 1, PAGE_EXECUTE_READWRITE, &dwOld);

	BYTE OrigByte = *(BYTE*)ADDR_PLAYERCAMERAOSDINITJMP;
	*(BYTE*)ADDR_PLAYERCAMERAOSDINITJMP = 0xEB; 

	DMPlayer* nPlayer = (DMPlayer*)CLevel::oSpawnActor(TurokEngine->pT4Game, 0, 0, spawn_name, spawn_path, npos_struct, 0);

	*(BYTE*)ADDR_PLAYERCAMERAOSDINITJMP = OrigByte;


	return nPlayer;
}