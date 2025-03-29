#include <windows.h>
#include <stdio.h>
#include "APIHooks.h"

// Dummy function to get us added to the import table.
// Not actually used for anything.
__declspec(dllexport) void Injected(){}
extern void InstallTurokHooks();

// Initialize the mod
bool InitializeMod() 
{
	//MessageBoxA(NULL, "T4MP Injected", "T4MP Status", MB_OK);
	AllocConsole();

	FILE* pFile;
	freopen_s(&pFile, "CONOUT$", "w", stdout);
	freopen_s(&pFile, "CONOUT$", "w", stderr);
	freopen_s(&pFile, "CONIN$", "r", stdin);

	// Set console title
	SetConsoleTitle("Debug Console");

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	consoleInfo.dwSize.Y = 500; // Set buffer to 500 lines
	SetConsoleScreenBufferSize(hConsole, consoleInfo.dwSize);

	printf("[*] Installing API Hooks\n");

	InstallAPIHooks();

	printf("[*] Installing Turok Hooks\n");

	InstallTurokHooks();



	return true;
}

void ShutdownMod()
{

}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			InitializeMod();
			break;
		case DLL_PROCESS_DETACH:
			ShutdownMod();
			break;

	}

	return TRUE;
}