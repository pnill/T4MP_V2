#pragma once
#include "types.h"


typedef void* (__fastcall* SpawnActor_t)(void*, void*, void*, char*,char*, const vector3&,void*);
typedef void* (__fastcall* CreateActor_t)(void*, void*, char* pszActorType, char* pszActorPath);
typedef void* (__fastcall* AddActorInstances_t)(void*, void*, char*, int a3);
typedef void  (__fastcall* AddCamera_t)(void*, void*, void*);
typedef void(__fastcall* AddPostEffectsCamera_t)(void*, void*, void*);

class CLevel {
public:
	static const void* ADDR_SPAWNACTOR;
	static const void* ADDR_CREATEACTOR;
	static const void* ADDR_ADDACTORINSTANCES;
	static const void* ADDR_ADDCAMERA;
	static const void* ADDR_ADDPOSTEFFECTSCAMERA;

	static SpawnActor_t oSpawnActor;
	static CreateActor_t oCreateActor;
	static AddActorInstances_t oAddActorInstances;
	static AddCamera_t oAddCamera;
	static AddPostEffectsCamera_t oAddPostEffectsCamera;

	static void InstallHooks();

	static void* __fastcall SpawnActor(void* pThis, void* _, void* unk, char* pszActorType, char* pszActorPath, const vector3& position, void* pParent);
	static void* __fastcall CreateActor(void* pThis, void* _, char* pszActorType, char* pszActorPath);
	static void* __fastcall AddActorInstances(void* pThis, void* _, char* atiPath, int a3);
	static void  __fastcall AddCamera(void* pThis, void* _, void* pCamera);
	static void  __fastcall AddPostEffectsCamera(void* pThis, void* _, void* pCamera);
};