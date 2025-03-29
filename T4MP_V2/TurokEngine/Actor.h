#pragma once

typedef char(__fastcall* SetRendered_t)(void*, void*, char bIsRendered);
typedef void(__fastcall* HandleBeingPickedUp_t)(void*, void*, void*);
typedef void(__fastcall* HandleFailedPickup_t)(void*, void*, void*);

class CActor_ {
public:
	static const void* ADDR_SETRENDERED;
	static const void* ADDR_HANDLEBEINGPICKEDUP;
	static const void* ADDR_HANDLEFAILEDPICKUP;

	static SetRendered_t oSetRendered;
	static HandleBeingPickedUp_t oHandleBeingPickedUp;
	static HandleFailedPickup_t oHandleFailedPickup;

	static void InstallHooks();

	static char __fastcall SetRendered(void*, void*, char bIsRendered);
	static void __fastcall HandleBeingPickedUp(void* pThis, void* _, void* pPlayerPickingUp);
	static void __fastcall HandleFailedPickup(void* pThis, void* _, void* pPlayerPickingUp);
};