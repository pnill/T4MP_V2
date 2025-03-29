#include <stdio.h>
#include "../3rdparty\MinHook_133_lib\include\MinHook.h"
#include "../GUI/imgui_debugconsole.h"

#include "../T4MP/T4MP_Engine.h"
#include "Actor.h"


const void* CActor_::ADDR_SETRENDERED = (void*)0x0051E3B0;
const void* CActor_::ADDR_HANDLEBEINGPICKEDUP = (void*)0x00495A60;
const void* CActor_::ADDR_HANDLEFAILEDPICKUP = (void*)0x00495C80;

SetRendered_t CActor_::oSetRendered = nullptr;
HandleBeingPickedUp_t CActor_::oHandleBeingPickedUp = nullptr;
HandleFailedPickup_t CActor_::oHandleFailedPickup = nullptr;

void CActor_::InstallHooks()
{
	MH_STATUS status = MH_ERROR_DISABLED;

	status = MH_CreateHook((LPVOID)ADDR_SETRENDERED, (LPVOID)CActor_::SetRendered, (PVOID*)&oSetRendered);
	if (status != MH_OK)
	{
		g_Console.AddLog("[CActor] MH_CreateHook failed on CActor::SetRendered!\n");
	}
	else
	{
		g_Console.AddLog("[CActor] ::SetRendered hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_HANDLEBEINGPICKEDUP, (LPVOID)CActor_::HandleBeingPickedUp, (PVOID*)&oHandleBeingPickedUp);
	if (status != MH_OK)
	{
		g_Console.AddLog("[CActor] MH_CreateHook failed on CActor::HandleBeingPickedUp!\n");
	}
	else
	{
		g_Console.AddLog("[CActor] ::HandleBeingPickedUp hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_HANDLEFAILEDPICKUP, (LPVOID)CActor_::HandleFailedPickup, (PVOID*)&oHandleFailedPickup);
	if (status != MH_OK)
	{
		g_Console.AddLog("[CActor] MH_CreateHook failed on CActor::HandleBeingPickedUp!\n");
	}
	else
	{
		g_Console.AddLog("[CActor] ::HandleBeingPickedUp hook installed\n");
	}

	// Enable all hooks
	status = MH_EnableHook(MH_ALL_HOOKS);
	if (status != MH_OK)
	{
		printf("[-] MH_EnableHook failed in CACtor::InstallHooks()!\n");
	}
}

void CActor_::HandleFailedPickup(void* pThis, void* _, void* pPlayerPickingUp)
{
	if (pPlayerPickingUp != T4MP::GetDMPlayer(0))
		return;

	return oHandleFailedPickup(pThis, _, pPlayerPickingUp);
}

void CActor_::HandleBeingPickedUp(void* pThis, void* _, void* pPlayerPickingUp)
{
	if (pPlayerPickingUp != T4MP::GetDMPlayer(0))
	{
		// Some flag that causes it not to draw pickup text.
		*(BYTE*)((BYTE*)pThis + 0x238) = 0x35;
	}
	
	return oHandleBeingPickedUp(pThis, _, pPlayerPickingUp);
}

char CActor_::SetRendered(void* pThis, void* _, char bIsRendered)
{
	// Safety check for null pointer
	if (pThis == nullptr)
		return 0;

	if (T4MP::GetLocalPlayer())
	{
		// Get the type string pointer
		char* type = *(char**)((BYTE*)pThis + 0x5C);

		// Check if it's a weapon (first character is 'W' which is 0x57 in ASCII)
		// AND if the owner is the current player
		if (type && *type == 0x57 && *(DWORD*)((BYTE*)pThis + 0x234) != (DWORD)T4MP::GetDMPlayer(0)->pBlendedCamera->pPlayer)
		{
			// If it's a weapon but NOT owned by current player, force non-rendered
			return oSetRendered(pThis, _, 0);
		}
	}

	// For all other cases, use the provided render state
	return oSetRendered(pThis, _, bIsRendered);
}