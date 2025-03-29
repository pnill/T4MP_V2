#pragma once

typedef void  (__fastcall* UpdateCameras_t)(void*, void*, int a3, int a4);
typedef void  (__fastcall* RenderDamageOSD_t)(void*, void*, int a2);
typedef void  (__fastcall* UpdateOSDAtDeath_t)(void*, void*);
typedef void  (__fastcall* UpdateOSDAtDeath2_t)(void*, void*, int a2);
typedef void  (__fastcall* RenderDeathMessage_t)(void*, void*);
typedef void  (__fastcall* UpdateDeathMode_t)(void*, void*,float);
typedef void  (__fastcall* UpdateRespawnMode_t)(void*, void*);
typedef void* (__fastcall* IsWeaponValid_t)(void*, void*, void*, void*);
typedef void* (__fastcall* SwitchWeapon_t)(void*, void*, void*, void*);
typedef void  (__fastcall* FireWeapon_t)(void*, void*, int a1, int a2);
typedef void  (__fastcall* ReleaseFire_t)(void*, void*, float heldTime);
typedef void  (__fastcall* HoldFire_t)(void*, void*, float heldTime, int a2);
typedef void  (__fastcall* ModifyWeapon_t)(void*, void*, int a1, int a2);
typedef void  (__fastcall* RenderHealth_t)(void*, void*);
typedef void (__fastcall* RenderAmmo_t)(void*, void*);

// Until I get rid of the original "oldstruct" I need to rename this to "_"
class DMPlayer_ {
public:
	static const void* ADDR_UPDATECAMERAS;
	static const void* ADDR_RENDERDAMAGEOSD;
	static const void* ADDR_UPDATEOSDATDEATH;
	static const void* ADDR_UPDATEOSDATDEATH2;
	static const void* ADDR_RENDERDEATHMSG;
	static const void* ADDR_UPDATEDEATHMODE;
	static const void* ADDR_UPDATERESPAWNMODE;
	static const void* ADDR_ISWEAPONVALID;
	static const void* ADDR_SWITCHWEAPON;
	static const void* ADDR_FIREWEAPON;
	static const void* ADDR_HOLDFIRE;
	static const void* ADDR_RELEASEFIRE;
	static const void* ADDR_MODIFYWEAPON;
	static const void* ADDR_RENDERHEALTH;
	static const void* ADDR_RENDERAMMO;

	static UpdateCameras_t oUpdateCameras;
	static RenderDamageOSD_t oRenderDamageOSD;
	static UpdateOSDAtDeath_t oUpdateOSDAtDeath;
	static UpdateOSDAtDeath2_t oUpdateOSDAtDeath2;
	static RenderDeathMessage_t oRenderDeathMessage;
	static UpdateDeathMode_t oUpdateDeathMode;
	static UpdateRespawnMode_t oUpdateRespawnMode;
	static IsWeaponValid_t oIsWeaponValid;
	static SwitchWeapon_t oSwitchWeapon;
	static FireWeapon_t oFireWeapon;
	static ReleaseFire_t oReleaseFire;
	static HoldFire_t oHoldFire;
	static ModifyWeapon_t oModifyWeapon;
	static RenderHealth_t oRenderHealth;
	static RenderAmmo_t oRenderAmmo;

	static void InstallHooks();

	static void  __fastcall UpdateCameras(void* pThis, void* _, int a3, int a4);
	static void  __fastcall RenderDamageOSD(void* pThis, void*, int a2);
	static void  __fastcall UpdateOSDAtDeath(void* pThis, void*);
	static void  __fastcall UpdateOSDAtDeath2(void* pThis, void*, int a2);
	static void  __fastcall RenderDeathMessage(void* pThis, void*);
	static void  __fastcall UpdateDeathMode(void* pThis, void*,float);
	static void  __fastcall UpdateRespawnMode(void* pThis, void*);
	static void* __fastcall IsWeaponValid(void* pThis, void* _, void* pWeaponID, void* Unk);
	static void* __fastcall SwitchWeapon(void* pThis, void* _, void* pWeaponId, void* Unk);
	static void  __fastcall FireWeapon(void* pThis, void* _, int a1, int a2);
	static void  __fastcall ReleaseFire(void* pThis, void* _, float heldTime);
	static void  __fastcall HoldFire(void* pThis, void* _, float heldTime, int a2);
	static void  __fastcall ModifyWeapon(void* pThis, void* _, int a1, int a2);
	static void  __fastcall RenderHealth(void* pThis, void* _);
	static void  __fastcall RenderAmmo(void* pThis, void* _);
};