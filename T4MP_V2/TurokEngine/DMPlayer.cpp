#include <stdio.h>
#include "../3rdparty\MinHook_133_lib\include\MinHook.h"
#include "../GUI/imgui_debugconsole.h"

#include "../T4MP/T4MP_Engine.h"
#include "../T4MP/Networking/network_player.h"
#include "DMPlayer.h"

const void* DMPlayer_::ADDR_UPDATECAMERAS = (void*)0x004D8F20;
const void* DMPlayer_::ADDR_RENDERDAMAGEOSD = (void*)0x004EE7F0;
const void* DMPlayer_::ADDR_UPDATEOSDATDEATH = (void*)0x004EEA50;
const void* DMPlayer_::ADDR_UPDATEOSDATDEATH2 = (void*)0x004EE850;
const void* DMPlayer_::ADDR_RENDERDEATHMSG = (void*)0x004DCFC0;
const void* DMPlayer_::ADDR_UPDATEDEATHMODE = (void*)0x004D9130;
const void* DMPlayer_::ADDR_UPDATERESPAWNMODE = (void*)0x004D91D0;
const void* DMPlayer_::ADDR_ISWEAPONVALID = (void*)0x004E3720;
const void* DMPlayer_::ADDR_SWITCHWEAPON = (void*)0x004E3AF0;
const void* DMPlayer_::ADDR_FIREWEAPON = (void*)0x004DBC70;
const void* DMPlayer_::ADDR_HOLDFIRE = (void*)0x004D6DF0;
const void* DMPlayer_::ADDR_RELEASEFIRE = (void*)0x004D6E40;
const void* DMPlayer_::ADDR_MODIFYWEAPON = (void*)0x4C7700;
const void* DMPlayer_::ADDR_RENDERHEALTH = (void*)0x004EBFB0;
const void* DMPlayer_::ADDR_RENDERAMMO = (void*)0x4EBD40;

UpdateCameras_t DMPlayer_::oUpdateCameras = nullptr;
RenderDamageOSD_t DMPlayer_::oRenderDamageOSD = nullptr;
UpdateOSDAtDeath_t DMPlayer_::oUpdateOSDAtDeath = nullptr;
UpdateOSDAtDeath2_t DMPlayer_::oUpdateOSDAtDeath2 = nullptr;
RenderDeathMessage_t DMPlayer_::oRenderDeathMessage = nullptr;
UpdateDeathMode_t DMPlayer_::oUpdateDeathMode = nullptr;
UpdateRespawnMode_t DMPlayer_::oUpdateRespawnMode = nullptr;
IsWeaponValid_t DMPlayer_::oIsWeaponValid = nullptr;
SwitchWeapon_t DMPlayer_::oSwitchWeapon = nullptr;
FireWeapon_t DMPlayer_::oFireWeapon = nullptr;
ReleaseFire_t DMPlayer_::oReleaseFire = nullptr;
HoldFire_t DMPlayer_::oHoldFire = nullptr;
ModifyWeapon_t DMPlayer_::oModifyWeapon = nullptr;
RenderHealth_t DMPlayer_::oRenderHealth = nullptr;
RenderAmmo_t DMPlayer_::oRenderAmmo = nullptr;

extern LocalPlayer g_LocalPlayer;

void DMPlayer_::InstallHooks()
{
	MH_STATUS status = MH_ERROR_DISABLED;

	status = MH_CreateHook((LPVOID)ADDR_UPDATECAMERAS, (LPVOID)DMPlayer_::UpdateCameras, (PVOID*)&oUpdateCameras);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::UpdateCameras!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::UpdateCameras hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_RENDERDAMAGEOSD, (LPVOID)DMPlayer_::RenderDamageOSD, (PVOID*)&oRenderDamageOSD);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::RenderDamageOSD!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::RenderDamageOSD hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_UPDATEOSDATDEATH, (LPVOID)DMPlayer_::UpdateOSDAtDeath, (PVOID*)&oUpdateOSDAtDeath);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::UpdateOSDAtDeath!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::UpdateOSDAtDeath hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_UPDATEOSDATDEATH2, (LPVOID)DMPlayer_::UpdateOSDAtDeath2, (PVOID*)&oUpdateOSDAtDeath2);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::UpdateOSDAtDeath2!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::UpdateOSDAtDeath2 hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_RENDERDEATHMSG, (LPVOID)DMPlayer_::RenderDeathMessage, (PVOID*)&oRenderDeathMessage);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::RenderDeathMessage!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::RenderDeathMessage hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_UPDATEDEATHMODE, (LPVOID)DMPlayer_::UpdateDeathMode, (PVOID*)&oUpdateDeathMode);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::UpdateDeathMode!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::UpdateDeathMode hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_UPDATERESPAWNMODE, (LPVOID)DMPlayer_::UpdateRespawnMode, (PVOID*)&oUpdateRespawnMode);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::UpdateRespawnMode!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::UpdateRespawnMode hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_ISWEAPONVALID, (LPVOID)DMPlayer_::IsWeaponValid, (PVOID*)&oIsWeaponValid);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::IsWeaponValid!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::IsWeaponValid hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_SWITCHWEAPON, (LPVOID)DMPlayer_::SwitchWeapon, (PVOID*)&oSwitchWeapon);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::SwitchWeapon!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::SwitchWeapon hook installed\n");
	}


	status = MH_CreateHook((LPVOID)ADDR_FIREWEAPON, (LPVOID)DMPlayer_::FireWeapon, (PVOID*)&oFireWeapon);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::FireWeapon!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::FireWeapon hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_RELEASEFIRE, (LPVOID)DMPlayer_::ReleaseFire, (PVOID*)&oReleaseFire);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::ReleaseFire!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::ReleaseFire hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_HOLDFIRE, (LPVOID)DMPlayer_::HoldFire, (PVOID*)&oHoldFire);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::HoldFire!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::HoldFire hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_MODIFYWEAPON, (LPVOID)DMPlayer_::ModifyWeapon, (PVOID*)&oModifyWeapon);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::ModifyWeapon!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::ModifyWeapon hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_RENDERHEALTH, (LPVOID)DMPlayer_::RenderHealth, (PVOID*)&oRenderHealth);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::RenderHealth!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::RenderHealth hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_RENDERAMMO, (LPVOID)DMPlayer_::RenderAmmo, (PVOID*)&oRenderAmmo);
	if (status != MH_OK)
	{
		g_Console.AddLog("[DMPlayer] MH_CreateHook failed on DMPlayer::RenderAmmo!\n");
	}
	else
	{
		g_Console.AddLog("[DMPlayer] ::RenderAmmo hook installed\n");
	}

	// Enable all hooks
	status = MH_EnableHook(MH_ALL_HOOKS);
	if (status != MH_OK)
	{
		printf("[-] MH_EnableHook failed in DMPlayer::InstallHooks()!\n");
	}
}

void DMPlayer_::UpdateCameras(void* pThis, void* _, int a3, int a4)
{	
	g_Console.AddLog("[DMPlayer] ::UpdateCameras (pThis: %08X, a3: %i X:(%08X), a4: %i X:(%08X) ) ", pThis, a3, a3, a4, a4);

	return oUpdateCameras(pThis, _, a3, a4);
}

/*
	Original routine seems to take in the pCameraOSD we could've created in the spawn and avoid doing.
	Meaning pThis is null when we reach here because we disabled the OSD from being created.
*/
void DMPlayer_::RenderDamageOSD(void* pThis, void* _, int a2)
{
	g_Console.AddLog("[DMPlayer] ::RenderDamageOSD for player pThis: %08X, a2: %08X",pThis,a2);
	if (!pThis)
		return;
	else
		return oRenderDamageOSD(pThis, _, a2);
}

/*
	pCamerOSD is the only parameter passed here but we didn't intiialize it for everyone else.
	This seems to de-init it.
*/
void DMPlayer_::UpdateOSDAtDeath(void* pThis, void* _)
{
	if (!pThis)
		return;
	else
		return oUpdateOSDAtDeath(pThis, _);
}

void DMPlayer_::UpdateOSDAtDeath2(void* pThis, void* _, int a2)
{
	if (!pThis)
		return;
	else
		return oUpdateOSDAtDeath2(pThis, _, a2);
}

void DMPlayer_::RenderDeathMessage(void* pThis, void* _)
{
	// Only render the local player's death message.
	if (pThis != T4MP::GetDMPlayer(0))
	{
		return;
	}
	return oRenderDeathMessage(pThis, _);
}


/*
	Unsure what the real purpose of this is, it seems to toggle whether or not sepecific properties are rendered.
	It utilizes CActor::SetRendered() on various objects related to the player.
	It's called as the second potential call when referencing the "Death" mode setup.

	Sometimes pThis will get passed with null or 0x1B20 is null based on old code,
	rather than handling it at all I just check if it's the local player being passed if pThis is not null.
*/
void DMPlayer_::UpdateDeathMode(void* pThis, void* _, float a2)
{
	if (pThis == nullptr)
		return;
	if (pThis != T4MP::GetDMPlayer(0))
		return;

	return oUpdateDeathMode(pThis, _, a2);
}

/*
	As above unsure what the actual purpoose of it is, 
	just know it screws with player 'mode' or 'animation' related to death/respawn.
*/
void DMPlayer_::UpdateRespawnMode(void* pThis, void* _)
{
	// This check is technically redudnant with the one after, but it should work.
	if (pThis == nullptr)
		return;
	if (pThis != T4MP::GetDMPlayer(0))
		return;

	return oUpdateRespawnMode(pThis, _);
}

void* DMPlayer_::IsWeaponValid(void* pThis, void* _, void* WeaponID, void* Unk)
{
	return oIsWeaponValid(pThis, _, WeaponID, Unk);
}

void* DMPlayer_::SwitchWeapon(void* pThis, void* _, void* WeaponID, void* Unk)
{
	g_Console.AddLog("[DMPlayer] ::SwitchWeapon(pThis: %08X, WeaponID: %08X, Unk: %08X)",pThis,WeaponID,Unk);

	return oSwitchWeapon(pThis, _, WeaponID, Unk);
}

void DMPlayer_::FireWeapon(void* pThis, void* _, int a1, int a2)
{
	g_Console.AddLog("[DMPlayer] ::FireWeapon(pThis: %08X, a1: %08X, a2: %08X)", pThis, a1, a2);
	g_Console.AddLog("[DMPlayer] Player firing view X: %f, view Y: %f", T4MP::GetDMPlayer(0)->ViewX, T4MP::GetDMPlayer(0)->ViewY);

	if(pThis == T4MP::GetDMPlayer(0))
		g_LocalPlayer.isFiring = true;

	return oFireWeapon(pThis, _, a1, a2);
}

void DMPlayer_::ReleaseFire(void* pThis, void* _, float HeldTime)
{
	if (pThis == T4MP::GetDMPlayer(0))
	{
		g_LocalPlayer.fireReleaseTime = HeldTime;
		g_LocalPlayer.isReleasingFire = true;
	}
	return oReleaseFire(pThis, _, HeldTime);
}

void DMPlayer_::HoldFire(void* pThis, void* _, float heldTime, int a2)
{
	if (pThis == T4MP::GetDMPlayer(0))
	{
		g_LocalPlayer.fireHoldingTime = heldTime;
		g_LocalPlayer.isHoldingFire = true;
	}
	return oHoldFire(pThis, _, heldTime, a2);
}

void DMPlayer_::ModifyWeapon(void* pThis, void* _, int a1, int a2)
{
	if (pThis == T4MP::GetDMPlayer(0))
	{
		g_Console.AddLog("[DMPlayer] ::ModifyWeapon(pThis: %08X, a1: %08X, a2: %08X)", pThis, a1, a2);
		g_LocalPlayer.isModifyingWeapon = true;
	}

	return oModifyWeapon(pThis, _, a1, a2);
}


// Separate function call that renders additional player's health for some reason.
// Not even the main player uses this same routine to render.
// These could break other things but hopefully not.
void DMPlayer_::RenderHealth(void* pThis, void* _)
{
	//g_Console.AddLog("[DMPlayer] ::RenderHealth(pThis: %08X)", pThis);
	return;
}

void DMPlayer_::RenderAmmo(void* pThis, void* _)
{
	return;
}