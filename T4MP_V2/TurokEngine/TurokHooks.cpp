#include <windows.h>
#include <stdio.h>
#include "History.h"
#include "Level.h"
#include "Game.h"
#include "DMPlayer.h"
#include "Actor.h"

void InstallTurokHooks()
{
	printf("[*] Installing CHistories hooks\n");
	CHistories::InstallHooks();
	printf("[*] Installing CLevel hooks\n");
	CLevel::InstallHooks();
	printf("[*] Installing CGame hooks\n");
	CGame::InstallHooks();
	printf("[*] Installing DMPlayer hooks\n");
	DMPlayer_::InstallHooks();
	printf("[*] Installing Actor hooks\n");
	CActor_::InstallHooks();
}