#include <stdio.h>
#include <string.h>
#include "../3rdparty/MinHook_133_lib/include/MinHook.h"
#include "../GUI/imgui_debugconsole.h"
#include "Level.h"
#include "Game.h"
#include "OldEngineStruct.h"
#include "../T4MP/Networking/network_packets.h"
#include "../T4MP/Networking/network_serialization.h"
#include "../T4MP/Networking/enet_wrapper.h"
#include "../T4MP/T4MP_Engine.h"

const void* CLevel::ADDR_SPAWNACTOR = (void*)0x5120E0;
const void* CLevel::ADDR_CREATEACTOR = (void*)0x50DEE0;
const void* CLevel::ADDR_ADDACTORINSTANCES = (void*)0x00512AF0;
const void* CLevel::ADDR_ADDCAMERA = (void*)0x00510AA0;
const void* CLevel::ADDR_ADDPOSTEFFECTSCAMERA = (void*)0x0050CEA0;

SpawnActor_t CLevel::oSpawnActor = nullptr;
CreateActor_t CLevel::oCreateActor = nullptr;
AddActorInstances_t CLevel::oAddActorInstances = nullptr;
AddCamera_t CLevel::oAddCamera = nullptr;
AddPostEffectsCamera_t CLevel::oAddPostEffectsCamera = nullptr;

extern Network g_Network;
extern int g_gameState;

void CLevel::InstallHooks()
{
	MH_STATUS status = MH_ERROR_DISABLED;
	status = MH_CreateHook((LPVOID)ADDR_SPAWNACTOR, (LPVOID)CLevel::SpawnActor, (PVOID*)&oSpawnActor);
	if (status != MH_OK)
	{
		g_Console.AddLog("[-] MH_CreateHook failed on CLevel::SpawnActor!\n");
	}
	else
	{
		g_Console.AddLog("[*] Clevel::SpawnActor hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_CREATEACTOR, (LPVOID)CLevel::CreateActor, (PVOID*)&oCreateActor);
	if (status != MH_OK)
	{
		g_Console.AddLog("[-] MH_CreateHook failed on CLevel::CreateActor!\n");
	}
	else
	{
		g_Console.AddLog("[*] Clevel::CreateActor hook installed\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_ADDACTORINSTANCES, (LPVOID)CLevel::AddActorInstances, (PVOID*)&oAddActorInstances);
	if (status != MH_OK)
	{
		g_Console.AddLog("[-] MH_CreateHook failed on CLevel::AddActorInstances!\n");
	}
	else
	{
		g_Console.AddLog("[*] CLevel::AddActorInstances installed!\n");
	}

	status = MH_CreateHook((LPVOID)ADDR_ADDCAMERA, (LPVOID)CLevel::AddCamera, (PVOID*)&oAddCamera);
	if (status != MH_OK)
	{
		g_Console.AddLog("[-] MH_CreateHook failed on CLevel::AddCamera!\n");
	}
	else
	{
		g_Console.AddLog("[*] CLevel::AddCamera installed!\n");
	}


	status = MH_CreateHook((LPVOID)ADDR_ADDPOSTEFFECTSCAMERA, (LPVOID)CLevel::AddPostEffectsCamera, (PVOID*)&oAddPostEffectsCamera);
	if (status != MH_OK)
	{
		g_Console.AddLog("[-] MH_CreateHook failed on CLevel::AddPostEffectsCamera!\n");
	}
	else
	{
		g_Console.AddLog("[*] CLevel::AddPostEffectsCamera installed!\n");
	}

	status = MH_EnableHook(MH_ALL_HOOKS);
	if (status != MH_OK)
	{
		g_Console.AddLog("[-] MH_EnableHook failed in CLevel::InstallHooks()!\n");
	}
}


// Technically these return CActor but we have no representation of that yet...
/*
	CLevel::SpawnActor(void* unk, char* pszActorType, char* pszActorPath, const vector3& position, void* pParent)

	This actually spawns as specified actor and adds it to the level,
	actors are .atr files not "ati" which is the actor instance file.

	This resolves data about them, it seems to not really care what type is passed.

	The only time I've ever seen "unk" passed is when it's set to 1 and it seems to be on things with a parent or if they belong to the player.
	Most of the time pParent is null.

	Both unk and pParent can be null the rest of the data must be filled but pszActorType is pretty irrelevant it seems most of the time.
*/
void* CLevel::SpawnActor(void* pThis, void* _, void* unk, char* pszActorName, char* pszActorPath, const vector3& position, void* pParent)
{
	g_Console.AddLog("[CLevel] ::SpawnActor( pThis = %08X, unk = %08X, pszActorName: %s, pszActorPath: %s, position -  x: %f, y: %f, z: %f, pParent: %08X\n", pThis, unk, pszActorName, pszActorPath, position.x, position.y, position.z, pParent);

	return oSpawnActor(pThis, _, unk, pszActorName, pszActorPath, position, pParent);
}

void CLevel::AddCamera(void* pThis, void* _, void* pCamera)
{
	g_Console.AddLog("[CLevel] ::AddCamera pThis: %08X, pCamera: %08X", pThis, pCamera);

	if (T4MP::GetLocalPlayer())
	{
		if (pCamera == (void*)T4MP::GetDMPlayer(0)->pBlendedCamera)
		{
			g_Console.AddLog("[CLevel] ::AddCamera pCamera = DMPlayer(0)->pCamera");
		}
		else
		{
			g_Console.AddLog("[CLevel] ::AddCamera pCamera != DMPlayer(0)->pCamera");
			// What about the spidermine??
			return;
		}
	}
	return oAddCamera(pThis, _, pCamera);
}

void CLevel::AddPostEffectsCamera(void* pThis, void* _, void* pCamera)
{
	g_Console.AddLog("[CLevel] ::AddPostEffectsCamera pThis: %08X, pCamera: %08X", pThis, pCamera);

	if (T4MP::GetLocalPlayer())
	{
		if (pCamera == (void*)T4MP::GetDMPlayer(0)->pBlendedCamera)
		{
			g_Console.AddLog("[CLevel] ::AddPostEffectsCamera pCamera = DMPlayer(0)->pCamera");
		}
		else
		{
			g_Console.AddLog("[CLevel] ::AddPostEffectsCamera pCamera != DMPlayer(0)->pCamera");
			// What about the spidermine??
			return;
		}
	}
	return oAddPostEffectsCamera(pThis, _, pCamera);
}
/*
	CLevel::CreateActor(char* pszActorType, char* pszActorPath)
	
	This one seems to create the object in memory and return a CActor object.
	It does not however, add the object into the level 
	You have to do that part yourself by calling: CLevel::AddActor.
	
	Additionally, it seems CActorBasicPhysicsBehavior::AddActorBasicPhysics...
	and CBasicPhysics::SetMass usually follow as well.

	I'm not sure what happens if you simply CreateActor, AddActor and don't do those.
*/
// both strings appear to be path, not sure what the difference is
void* CLevel::CreateActor(void* pThis, void* _, char* pszActorType, char* pszActorPath)
{
	g_Console.AddLog("[CLevel] ::CreateActor( pThis= %08X, pszActorType: %s, pszActorPath: %s)\n", pThis, pszActorType, pszActorPath);
	return oCreateActor(pThis, _, pszActorType, pszActorPath);
}

void *CLevel::AddActorInstances(void* pThis, void* _, char* pszAtiPath, int a3)
{
	g_Console.AddLog("[CLevel] ::AddActorInstances( pThis = %08X, pszInstancePath = %s, a3=  %i)\n", pThis, pszAtiPath, a3);

	if (strstr(pszAtiPath, "Multiplayer.ati"))
	{
		strcpy(pszAtiPath, "$\\T4MP\\multiplayer.ati");
	}

	/*
	if (strstr(pszAtiPath, "InGamePause.ati"))
	{

		CGame::oPopLevels(*(g_Turok4Game), 0);
		CGame::oPushLevelSoon(*(g_Turok4Game), 0, "$/Data/Levels/u_jcowlishaw/screens/FirstInteractiveScreen/firstinteractivescreen.atr", 0, 0, "");
		CGame::oPushLevelSoon(*(g_Turok4Game), 0, "$/Data/Levels/u_jcowlishaw/screens/frontendbackground/frontendbackground.atr", 0, 0, "");

	}
	*/

	if (strstr(pszAtiPath, "FirstInteractiveScreen.ati"))
	{
		//strcpy(pszAtiPath, "$\\T4MP\\multiplayer.ati");
		CGame::oPushPendingLevel(*(g_Turok4Game), 0, "$/Data/Levels/u_jcowlishaw/screens/multiplayer/multiplayer.atr", 1, 1);
		CGame::oPopLevel(*(g_Turok4Game), 0);
		CGame::oPushLevelSoon(*(g_Turok4Game), 0, "$/Data/Levels/u_jcowlishaw/screens/multiplayer/multiplayer.atr", 1, 1, "");
	}

	if (strstr(pszAtiPath, "MultiPlayerJoin.ati"))
	{
		strcpy(pszAtiPath, "$\\T4MP\\multiplayerjoin.ati");
		std::vector<uint8_t> packet = NetworkSerializer::CreateGameStateUpdatePacket((uint8_t)GameStateUpdateStates::MULTIPLAYER_JOIN);
		g_Network.broadcastData(packet, 0, true);

		g_gameState = (int)GameStateUpdateStates::MULTIPLAYER_JOIN;
	}
	
	return oAddActorInstances(pThis, _, pszAtiPath, a3);
}

/*
	Additional CLevel calls that are either not hooked or unknown where they are in the engine.
	NOTE: These may not exist in Turok they're from other games in the same engine.

	Game.cpp

	char const * __thiscall CLevel::Name(void)const
	char const * __thiscall CLevel::FileName(void)const
	int __thiscall CLevel::Number(void)const
	float __thiscall CLevel::Time(void)const
	BOOL __thiscall CLevel::IsUpdated(void)const
	BOOL __thiscall CLevel::IsRendered(void)const
	BOOL __thiscall CLevel::AllowsOthersToUpdate(void)const
	BOOL __thiscall CLevel::AllowsOthersToRender(void)const
	BOOL __thiscall CLevel::ReusesZBuffer(void)const
	BOOL __thiscall CLevel::IsDead(void)const
	BOOL __thiscall CLevel::IsPendingStandby(void)const
	int __thiscall CLevel::IsMenu(void)const
	unsigned long __thiscall CLevel::HashedId(void)const
	unsigned long __thiscall CLevel::Types(void)const
	unsigned long __thiscall CLevel::Subtypes(void)const
	BOOL __thiscall CLevel::HasFlag(enum CLevel::LevelFlagType)const
	void __thiscall CLevel::SetEntranceName(char const *)
	class CPreCacheManager * __thiscall CLevel::PreCacheManager(void)
	enum CLevel::LoadProgressType __thiscall CLevel::LoadingProgress(void)const
	int __thiscall CLevel::IsLoaded(void)const


	This stuff probably only exists in debug builds, I believe it was used with their world builder and that they could do some form of live editing/movement of objects.

	ToolsConnection.cpp

	class CActor * __thiscall CLevel::FindActor(unsigned long)
	class CInventoryItem & __thiscall x_std::vector<class CInventoryItem,class x_std::allocator<class CInventoryItem> >::operator[](unsigned int)
	unsigned int __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::size(void)
	class CLevel::CActorInfo & __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::operator[](unsigned int)
	unsigned int __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::size(void)
	class CLevel::CActorInfo & __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::operator[](unsigned int)

	Font.cpp
	float __thiscall CLevel::TextAlphaMultiplier(void)

	Actor.cpp

	struct vector3 const & __thiscall CLevel::Gravity(void)
	class CCamera * __thiscall CLevel::CurrentCamera(void)
	class CCollision const * __thiscall CLevel::CurrentCollision(void)
	void __thiscall CLevel::SetCurrentCollision(class CCollision const *)
	class CParticleContainer * __thiscall CLevel::Particles(void)
	class CPaths const & __thiscall CLevel::Paths(void)
	class CActorCollisionGrids & __thiscall CLevel::SetCollisionGrids(void) 
	class CActorUpdateGrids & __thiscall CLevel::SetUpdateGrids(void)
	class CActorRenderGrids & __thiscall CLevel::SetRenderGrids(void) 

	Pre-CacheManager.cpp

	class CActor const * __thiscall CLevel::WorldActor(void)
	class CEffectSys * __thiscall CLevel::ParticleEffectSystem(void)
	class CLoadParticle * __thiscall CLevel::ParticleLoader(void)

	PlayerPathMonitor.cpp
	
	void __thiscall CLevel::AddSphereDebugMarker(struct vector3 const &,float,float,float,struct color)
	void __thiscall CLevel::AddLineDebugMarker(struct vector3 const &,struct vector3 const &,float,float,struct color)

	LinkEvent.cpp

	class CActor * __thiscall CLevel::CIteratorByRunTimeName::operator*(void)
	class CDeity * __thiscall CLevel::Deity(void)
	class CActor * __thiscall CLevel::Focus(void)
	class CActor * __thiscall CLevel::OldFocus(void)
	class CActor * __thiscall CLevel::FingeredActor(int)

	NavData.cpp

	__thiscall CLevel::CIterator::CIterator(class CActor const *)
	class CActor * __thiscall CLevel::CIterator::operator*(void)
	class CActor * __thiscall CLevel::FirstActor(void)
	class CLevel::CIterator & __thiscall CLevel::CIterator::operator++(void)

	Widget.cpp

	__thiscall CLevel::CDeepIterator::CDeepIterator(void)
	__thiscall CLevel::CDeepIterator::CDeepIterator(class CActor const *)
	class CActor * __thiscall CLevel::CDeepIterator::operator*(void)
	BOOL __thiscall CLevel::CDeepIterator::operator!=(class CLevel::CDeepIterator const &)
	class CScreenProxy * __thiscall CLevel::ScreenProxy(void)

	PredefinedLinkCauses.cpp

	class CGeneralClock const & __thiscall CLevel::Clock(void)
	virtual int __thiscall CLevelLinkCause::ParseProperty(class CTextFileParser &)
	virtual int __thiscall CLevelLinkCause::ConditionIsTrue(class CActor const *,class CActor const *,void const *)

	Level.cpp

	float __cdecl x_sqr(float)
	class CMaterialChanges const * __thiscall CActorDrawInfo::MaterialChanges(void)const
	class x_std::string::const_iterator & __thiscall x_std::string::const_iterator::operator++(void)
	BOOL __thiscall x_std::string::const_iterator::operator!=(class x_std::string::const_iterator const &)const
	__thiscall x_std::string::const_iterator::const_iterator(char const *)
	class x_std::string::const_iterator __thiscall x_std::string::begin(void)const
	class x_std::string::const_iterator __thiscall x_std::string::end(void)const
	class x_std::string const & __thiscall x_std::strings::operator[](int)const
	void __thiscall CMaterialManager::SetClothWind(struct vector3 const &)
	void __thiscall sphere::Set(struct vector3 const &,float)
	int __thiscall sphere::Intersects(class sphere const &)const
	float __thiscall CActor::DeferredUpdateDelay(void)const
	float __thiscall CActor::LevelTimeLastUpdated(void)const
	float __thiscall CActor::DeferredUpdatePortion(void)const
	void __thiscall CActor::SetDeferredUpdateDelay(float)
	void __thiscall CActor::SetLevelTimeLastUpdated(float)
	class sphere const & __thiscall CActor::CollisionSphere(void)const
	void __thiscall CActor::GetPosition(struct vector3 *)const
	int __thiscall CActor::Casts3dShadow(void)const
	float __thiscall CActor::Distance(struct vector3 const &)const
	enum CActor::TimeScaleClassType __thiscall CActor::TimeScaleClass(void)const
	unsigned short __thiscall CActor::RenderListIndex(void)const
	unsigned short __thiscall CActor::UpdateListIndex(void)const
	void __thiscall CActor::SetUpdateListIndex(unsigned short)
	void __thiscall CActor::SetRenderListIndex(unsigned short)
	unsigned long __thiscall CActor::CollisionTestNumber(void)const
	void __thiscall CActor::SetCollisionTestNumber(unsigned long)
	int __thiscall CActor::GroupNumber(void)const
	signed char __thiscall CActor::UpdateOrder(void)const
	BOOL __thiscall CActor::IsLight(void)const
	BOOL __thiscall CActor::IsPortal(void)const
	BOOL __thiscall CActor::IsCamera(void)const
	BOOL __thiscall CActor::IsMicrophone(void)const
	int __thiscall CActor::GhostTypes(void)const
	int __thiscall CActor::HasOverlayRenderOrder(void)const
	int __thiscall CActor::CollidesWith(class CActor const &)const
	int __thiscall CActor::HasGhostCollisionsWith(class CActor const &)const
	BOOL __thiscall CActor::HasRunTimeName(void)const
	void __thiscall CActor::PrivatelySetLevel(class CLevel *)
	void __thiscall Intersection::CContact::SetFraction(float)
	BOOL __thiscall Intersection::CBox::MightIntersect(class sphere const &)const
	int __thiscall CActorGridSet::GridCount(void)const
	unsigned long __thiscall CStringHash::Hash(void)const
	__thiscall CStringHash::CStringHash(char const *)
	char const * __thiscall CActorTypeInfo::LevelFragmentName(void)const
	void __thiscall CAlphaManager::Clear(void)
	void __thiscall CShadowManager::IncrementCasters(void)
	int __thiscall CInventory::Size(void)const
	void __thiscall CCamera::SetOrderedRenderId(int)
	int __thiscall CCamera::GetOrderedRenderId(void)
	class view const & __thiscall CCamera::GetView(void)const
	void __thiscall CCamera::SetView(class view const &)
	struct vector3 const & __thiscall CCamera::RecentForwardUnitDirection(void)const
	enum CCamera::PortalKindType __thiscall CCamera::PortalKind(void)const
	char const * __thiscall CCinema::Filename(void)const
	void __thiscall CCinema::Stop(void)
	int __thiscall CCinema::HasStopped(void)
	void __thiscall CCollision::SetStartPosition(struct vector3 const &)
	void __thiscall CCollision::SetToWorst(void)
	void __thiscall CCollision::SetFraction(float)
	class sphere __thiscall CCollisionGeometry::SphereInWorld(void)const
	class CCollisionOctree const * __thiscall COctreeCollisionPart::Octree(void)const
	unsigned long __thiscall CFog::Flags(void)const
	class CLevelProperties const & __thiscall CDeity::Properties(void)const
	int __thiscall CGame::WarningCount(void)const
	float __thiscall CGeneralClockTime::Seconds(void)const
	int __thiscall CGeneralClock::SecondsPerMinute(void)const
	float __thiscall CGeneralClock::Seconds(void)const
	int __thiscall CGeneralClock::SecondsPerDay(void)const
	int __thiscall CGeneralClock::SecondsIn(int,int,int)const
	int __thiscall CGeneralClock::SecondOfDay(void)const
	void __thiscall CPaths::Add(class CPath *)
	struct vector3 __thiscall CSimpleParticle::GetPos(void)const
	void __thiscall CParticleContainer::ClearShimmer(void)
	float __thiscall CLevelTimeWarp::Duration(void)const
	float __thiscall CLevelTimeWarp::CameraTimeScale(void)const
	float __thiscall CLevelTimeWarp::OtherTimeScale(void)const
	BOOL __thiscall CLevelTimeWarp::IsActive(void)const
	void __thiscall CLevelTimeWarp::SetDuration(float)
	static void __cdecl CLevel::IncrementLoading(void)
	static void __cdecl CLevel::DecrementLoading(void)
	int __thiscall CLevel::HasCamera(void)const
	void __thiscall CLevel::SetCurrentRenderCamera(class CCamera *)
	virtual void __thiscall CLevel::PrepareForPlayerEntry(int)
	int __thiscall CLevel::ActorCount(void)const
	__thiscall CLevel::CIterator::CIterator(void)
	BOOL __thiscall CLevel::CIterator::operator!=(class CLevel::CIterator const &)const
	__thiscall CLevel::CIteratorByRunTimeName::CIteratorByRunTimeName(int,class CLevel const &)
	class CPerformanceGroups & __thiscall CLevel::PerformanceGroups(void)
	class CPortalManager & __thiscall CLevel::PortalManager(void)
	__thiscall CLevel::CResourceUsage::CResourceUsage(void)
	__thiscall CLevel::CMenuInfo::CMenuInfo(class CWidget * (__cdecl*)(void))
	__thiscall CLevel::CTaskInfo::CTaskInfo(class CSimpleTask *,int,class CActor *)
	__thiscall CLevel::CActorInfo::CActorInfo(void)
	__thiscall CLevel::CGroupedActors::CGroupedActors(class CQuickVector<class CActor *> &,class CQuickVector<class CActor *> &)
	BOOL __thiscall CLevel::IsPendingReload(void)const
	BOOL __thiscall CLevel::WasJustMovedOffStandby(void)const
	int __thiscall CLevel::IsGame(void)const
	class CCollisionOctree const * __thiscall CLevel::WorldOctree(void)const
	class CGeneralClock & __thiscall CLevel::Clock(void)
	float __thiscall CLevel::ClockSpeed(void)const
	int __thiscall CLevel::CurrentFragmentNumber(void)const
	unsigned long __thiscall CLevel::CollisionTestNumber(void)const
	void __thiscall CLevel::AddFlag(enum CLevel::LevelFlagType)
	void __thiscall CLevel::RemoveFlag(enum CLevel::LevelFlagType)
	void __thiscall CLevel::SetTypes(unsigned long)
	void __thiscall CLevel::StartNewCollisionTest(void)const
	class CLightManager & __thiscall CLevel::LightManager(void)
	class CLevelFragments & __thiscall CLevel::Fragments(void)
	class CSounds * __thiscall CLevel::Sounds(void)
	class CFog & __thiscall CLevel::Fog(void)
	class CLighting & __thiscall CLevel::Lighting(void)
	class CLevelProperties & __thiscall CLevel::Properties(void)
	virtual void __thiscall CLevel::Print(char const *,int,int)const
	int __thiscall CLevel::HasPublicSetting(enum CLevel::PublicSettingType)const
	struct vector3 const & __thiscall CLevel::GetClothWindDirection(void)const
	class CActor * __thiscall CRegion::Owner(void)const
	void __thiscall CLevelFragment::SetFileName(char const *)
	void __thiscall CLevelFragment::SetNickName(char const *)
	int __thiscall CLevelFragments::Count(void)const
	class CLevelFragment * __thiscall CLevelFragments::Fragment(int)
	int __thiscall CSimpleTask::IsDone(void)const
	void (__cdecl*__thiscall CStreamingVideoManager::GetEndCallback(void)const )(void *)
	__thiscall CSceneRenderProxy::CSceneRenderProxy(void)
	virtual void * __thiscall CSceneRenderProxy::`scalar deleting destructor'(unsigned int)
	virtual void * __thiscall CSceneRenderProxy::`vector deleting destructor'(unsigned int)
	virtual __thiscall CSceneRenderProxy::~CSceneRenderProxy(void)
	virtual void __thiscall CSceneRenderProxy::Render(void)
	void __thiscall CSceneRenderProxy::SetLevel(class CLevel *)
	void __thiscall CSceneRenderProxy::SetCamera(class CCamera *)
	__thiscall CActorRenderProxy::CActorRenderProxy(class CActor *)
	virtual void * __thiscall CActorRenderProxy::`vector deleting destructor'(unsigned int)
	virtual void * __thiscall CActorRenderProxy::`scalar deleting destructor'(unsigned int)
	virtual __thiscall CActorRenderProxy::~CActorRenderProxy(void)
	virtual void __thiscall CActorRenderProxy::Render(void)
	__thiscall CParticleRenderProxy::CParticleRenderProxy(void)
	virtual void * __thiscall CParticleRenderProxy::`vector deleting destructor'(unsigned int)
	virtual void * __thiscall CParticleRenderProxy::`scalar deleting destructor'(unsigned int)
	virtual __thiscall CParticleRenderProxy::~CParticleRenderProxy(void)
	virtual void __thiscall CParticleRenderProxy::Render(void)
	void __thiscall CParticleRenderProxy::SetParticleSystem(class CParticleSystem *)
	void __thiscall CParticleRenderProxy::SetCamera(class CCamera *)
	static struct vector3 const & __cdecl CLevel::XAxis(void)
	static struct vector3 const & __cdecl CLevel::YAxis(void)
	static struct vector3 const & __cdecl CLevel::ZAxis(void)
	__thiscall CLevel::CLevel(char const *)
	virtual void * __thiscall CLevel::`vector deleting destructor'(unsigned int)
	virtual void * __thiscall CLevel::`scalar deleting destructor'(unsigned int)
	void __thiscall CLevel::Initialize(void)
	void __thiscall CLevel::Reset(void)
	class CLevelProperties & __thiscall CLevelProperties::operator=(class CLevelProperties const &)
	__thiscall CActorCollisionGrids::CActorCollisionGrids(void)
	__thiscall CActorUpdateGrids::CActorUpdateGrids(void)
	__thiscall CQuickActorVector::CQuickActorVector(void)
	void __thiscall CLevel::CreateWorldActor(class CActorTypeInfo const &)
	void __thiscall CLevel::AddActorInstancesFromType(class CActorTypeInfo const &)
	int __thiscall CLevel::GetPreCacheFileName(char *,char const *)const
	void __thiscall CLevel::LoadPreCacheFiles(void)
	void __thiscall CLevel::FinalizeLevelLoad(void)
	void __thiscall CLevel::CreateLevel(char const *)
	void __thiscall CLevel::Load(char const *)
	virtual void __thiscall CLevel::Reload(int)
	void __thiscall CLevel::LoadVisibilityInfo(char const *)
	void __thiscall CLevel::AddActorInstancesFromType(char const *,class CActor const *)
	void __thiscall CLevel::AddActorInstancesFromFile(class CTextFileParser &,class CActor const *)
	void __thiscall CLevel::AddActorInstances(char const *,class CActor const *)
	void __thiscall CLevel::ShowStatistics(char const *)
	void __thiscall CLevel::DoSetup(void)
	virtual void __thiscall CLevel::EnableParticleSystem(void)
	virtual void __thiscall CLevel::InitializeParticleSystem(void)
	virtual void __thiscall CLevel::Setup(void)
	void * __thiscall CBlurEffect::`scalar deleting destructor'(unsigned int)
	void __thiscall CLevel::SetupLevelForPerformanceAnalysis(void)
	void __thiscall CLevel::SetupWorldCollisionOctree(void)
	void __thiscall CLevel::SetupActors(void)
	void __thiscall CLevel::SetupProps(void)
	void __thiscall CLevel::SetupActor(class CActor *)
	void __thiscall CLevel::DeleteActor(class CActor *)
	void __thiscall CLevel::RemoveEndingActors(void)
	void __thiscall CLevel::DeleteActors(void)
	void __thiscall CLevel::DeleteProps(void)
	virtual __thiscall CLevel::~CLevel(void)
	void * __thiscall CCinema::`scalar deleting destructor'(unsigned int)
	int __thiscall CLevel::GetWorldLimits(struct vector3 &,struct vector3 &)const
	class CLevel * __thiscall CLevel::Child(int)const
	void __thiscall CLevel::AddChild(class CLevel *)
	int __thiscall CLevel::RemoveChild(class CLevel *)
	void __thiscall CLevel::AddChild(char const *)
	int __thiscall CLevel::RemoveChild(char const *)
	int __thiscall CLevel::CameraCount(void)const
	class CCamera * __thiscall CLevel::Camera(int)const
	virtual class CCamera * __thiscall CLevel::DefaultCamera(void)const
	void __thiscall CLevel::AddCamera(class CCamera *)
	int __thiscall CLevel::RemoveCamera(class CCamera *)
	void __thiscall CLevel::SetCamera(class CCamera *)
	int __thiscall CLevel::MicrophoneCount(void)const
	class CMicrophone * __thiscall CLevel::Microphone(int)const
	void __thiscall CLevel::AddMicrophone(class CMicrophone *)
	int __thiscall CLevel::RemoveMicrophone(class CMicrophone *)
	void __thiscall CLevel::SetMicrophone(class CMicrophone *)
	void __thiscall CLevel::ParseNextActor(class CTextFileParser &,char const *,class CActor const *)
	class CActorTypeInfo * __thiscall CLevel::LoadActorTypes(char const *)
	void __thiscall CLevel::AddBasicActor(class CActor *)
	void __thiscall CLevel::RemoveBasicActor(class CActor *)
	class CActor * __thiscall CLevel::AddActor(class CActor *,class CActor *)
	virtual void __thiscall CLevel::HandleActorBeingAddedToLevel(class CActor *)
	void __thiscall CLevel::AddChildren(class CActor *)
	void __thiscall CLevel::RemoveActor(class CActor *)
	virtual void __thiscall CLevel::HandleActorBeingRemovedFromLevel(class CActor *)
	virtual void __thiscall CLevel::HandleBeingMovedToStandby(void)
	void __thiscall CLevel::RemoveChildren(class CActor *)
	class CActor * __thiscall CLevel::SpawnActor(void const *,char const *,struct vector3 const &,class CActor *)
	class CActor * __thiscall CLevel::SpawnActor(class CActor *,class CActor *)
	class CActor * __thiscall CLevel::CreateActor(char const *)const
	class CActor * __thiscall CLevel::CreateActor(class CActorTypeInfo const &)const
	class CProp * __thiscall CLevel::CreateProp(char const *)const
	class CProp * __thiscall CLevel::CreateProp(class CActorTypeInfo const &)const
	class CActor * __thiscall CLevel::CreateActorFromClassName(char const *)const
	void __thiscall CLevel::EndActor(class CActor *)
	void __thiscall CLevel::DoCleanUpAndInitialUpdateManagement(void)
	float __thiscall CLevel::SetupTiming(float,float * const)
	void __thiscall CLevel::DoNotUpdate(void)
	void __thiscall CLevel::PreUpdate(float)
	virtual void __thiscall CLevel::Update(float)
	void __thiscall CLevel::UpdateParticles(float)
	int __thiscall CLevel::FindUpdateReferencePoint(struct vector3 &)const
	int __thiscall CLevel::FindRenderReferencePoint(struct vector3 &,class CCamera *)const
	void __thiscall CLevel::ListAllUpdatingActors(class CQuickVector<class CActor *> &)const
	void __thiscall CLevel::ListAllRenderingActors(class CQuickVector<class CActor *> &,class CQuickVector<class CActor *> &)const
	class CQuickVector<class CActor *> & __thiscall CLevel::ListUpdatingActors(void)const
	class CLevel::CGroupedActors __thiscall CLevel::ListRenderedActors(class CCamera *)const
	class CLevel::CGroupedActors __thiscall CLevel::ListFarPortalRenderedActors(class CCamera *)const
	void __thiscall CLevel::UpdateActors(float * const)
	void __thiscall CLevel::UpdateNarrative(void)
	void __thiscall CLevel::UpdateDeferredActor(class CActor &)
	float __thiscall CLevel::CalculateActorDeferredUpdateDelay(class CActor &)
	float __thiscall CLevel::CalculateActorDeferredUpdateDelayFromVectors(class CActor &,struct vector3 const &,struct vector3 const &)
	void __thiscall CLevel::UpdateActor(class CActor &,float)
	void __thiscall CLevel::DoNotUpdateActor(class CActor &)
	void __thiscall CLevel::UpdateLastActors(float * const)
	void __thiscall CLevel::UpdateRegularActors(float * const,class CQuickVector<class CActor *> const &)
	void __thiscall CLevel::UpdateFirstActors(float * const)
	void __thiscall CLevel::UpdateSounds(float)
	void __thiscall CLevel::UpdateNavigationData(float)
	void __thiscall CLevel::UpdateFog(float)
	void __thiscall CLevel::UpdatePerformanceGroups(float)
	void __thiscall CLevel::UpdateClock(float)
	void __thiscall CLevel::CleanUp(void)
	virtual void __thiscall CLevel::Render(void)
	virtual void __thiscall CLevel::Render(class CCamera *)
	void __thiscall CLevel::RenderActorsAndProps(class CCamera *)
	int __thiscall CLevel::RenderProps(class CCamera *)
	void __thiscall CLevel::RenderActor(class CActor &)
	void __thiscall CLevel::RenderInPortalActor(class CActor &)
	void __thiscall CLevel::DoNotRenderActor(class CActor &)
	int __thiscall CLevel::RenderFirstActors(class CActor ** &,class CQuickVector<class CActor *> &)
	int __thiscall CLevel::RenderInPortalFirstActors(class CActor ** &,class CQuickVector<class CActor *> &)
	int __thiscall CLevel::RenderActors(class CQuickVector<class CActor *> &)
	int __thiscall CLevel::RenderInPortalActors(class CQuickVector<class CActor *> &)
	int __thiscall CLevel::RenderSkyActors(class CActor ** &,class CQuickVector<class CActor *> &)
	int __thiscall CLevel::RenderInPortalSkyActors(class CActor ** &,class CQuickVector<class CActor *> &)
	int __thiscall CLevel::RenderLastActors(class CActor ** &,class CQuickVector<class CActor *> &)
	int __thiscall CLevel::RenderInPortalLastActors(class CActor ** &,class CQuickVector<class CActor *> &)
	void __thiscall CLevel::BeginDeferredRender(void)
	void __thiscall CLevel::StartNewDeferredRenderPhase(int &)
	void __thiscall CLevel::EndDeferredRender(void)
	void __thiscall CLevel::RenderFogPostEffect(class CCamera *)
	void __thiscall CLevel::RenderDebugInfo(class CCamera *)
	void __thiscall CLevel::RenderPortals(class CCamera *)
	void __thiscall CLevel::RenderAlphaTriangles(void)
	void __thiscall CLevel::RenderParticlesIfAppropriate(class CCamera *)
	void __thiscall CLevel::RenderOverlays(class CCamera *)
	void __thiscall CLevel::RenderActorsDrawnAfterOverlays(class CCamera *)
	void __thiscall CLevel::BeginRender(class CCamera *)
	void __thiscall CLevel::SetupPostRenderSpecialEffects(class CCamera *)
	void __thiscall CLevel::FinishPostRenderSpecialEffects(void)
	void __thiscall CLevel::FinishPostRenderSpecialEffectsAfterOverlays(void)
	int __thiscall CLevel::GetCurrentCameraMatrix(struct matrix4 &)
	class CVisibility * __thiscall CLevel::GetVis(void)
	void __thiscall CLevel::EndRender(class CCamera *)
	int __thiscall CLevel::FindClosestSolidCollision(class CCollision &,class CActor const &,struct vector3 const &)const
	int __thiscall CLevel::FindClosestSolidCollision(class CCollision &,class CActor const &,struct vector3 const &,class CQuickVector<class CActor *> const &)const
	int __thiscall CLevel::FindClosestSolidCollision(class CCollision &,class CActor const &,struct vector3 const &,class CCollisionSubspace &)const
	int __thiscall CLevel::FindClosestSolidCollisionOnScale(class CCollision &,class CActor const *,struct vector3 const &)const
	int __thiscall CLevel::FindClosestSolidCollisionOnTurn(class CCollision &,struct vector3 &,class CActor const *,struct quaternion const &,class CQuickVector<class CActor *> const &)const
	int __thiscall CLevel::FindRegions(class CQuickVector<class CRegion *> &,struct vector3 const &,class CCollisionSubspace const *,class CActor const *)const
	int __thiscall CLevel::FindRegions(class CActorList<class CRegion> &,struct vector3 const &,class CCollisionSubspace const *,class CActor const *)const
	class CRegion * __thiscall CLevel::FluidRegionContaining(struct vector3 const &)const
	int __thiscall CLevel::PointIsInFluid(struct vector3 const &)const
	void __thiscall CLevel::HandleGhostCollisions(class CActor &,struct vector3 const &,class CActor const *)
	void __thiscall CLevel::HandleGhostCollisions(class CActor &,struct vector3 const &,class CQuickVector<class CActor *> const &,class CActor const *)
	void __thiscall CLevel::HandleScaleCollisions(class CActor &,struct vector3 const &,BOOL,class CActor const *)
	void __thiscall CLevel::HandleTurnCollisions(class CActor &,struct quaternion const &,BOOL,class CActor const *)
	void __thiscall CLevel::HandleMoveCollision(class CActor &,class CActor &,class CCollision &,class CCollisionResponse &)
	int __thiscall CLevel::FindClosestBlockingCollisionFromSphere(class CCollision &,class CActor const *,int,struct vector3 const &,struct vector3 const &,float,unsigned long,int,int,class CCollisionSubspace const *,float)const
	int __thiscall CLevel::DamageActorsWithinRadiusOfActor(class CActor const *,float,float,float,int,int,int,signed char,int,class CActor const *)
	int __thiscall CLevel::DamageActorsWithinRadiusOfParticle(class CParticle const *,float,float,float,int,int,int,int,class CActor const *)
	int __thiscall CLevel::DamageActors(class CActor const *,float,int,int,int,int,class CActor const *)
	void __thiscall CLevel::ActivateScreenShake(float,unsigned long,unsigned long,float,float,float,struct vector3 const &,float,float,float,unsigned long)
	class CScreenFlash * __thiscall CLevel::ActivateScreenFlash(float,float,float,float,float,float,float,float,struct vector3 const &,class CCamera const *)
	void __thiscall CLevel::ActivateRumble(float,float,float,float,class CActor *,int)
	void __thiscall CLevel::ActivateRumble(float,float,float,float,struct vector3 const &,int)
	void __thiscall CLevel::ActivateRumble(float,float,enum CPlatformJoystick::Number)
	void __thiscall CLevel::TriggerLinks(short,class CActor *,void const *)
	void __thiscall CLevel::TriggerHistoryEvents(void const *)
	void __thiscall CLevel::TriggerAllHistoryEvents(void)
	int __thiscall CLevel::PlayerCount(void)const
	class CActor * __thiscall CLevel::Player(int)const
	virtual class CActor * __thiscall CLevel::FindPlayerN(int)const
	virtual int __thiscall CLevel::ParseProperty(class CTextFileParser &)
	void __thiscall CLevel::SetDeity(class CDeity *)
	void __thiscall CLevel::PlayPerformanceGroup(char const *)
	void __thiscall CLevel::StopPerformanceGroup(char const *)
	void __thiscall CLevel::PlayCinema(char const *,class CActor *)
	void __thiscall CLevel::StopCinema(char const *)
	void __thiscall CLevel::UpdateCinemas(float)
	void __thiscall CLevel::PushMenu(class CWidget * (__cdecl*)(void))
	void __thiscall CLevel::ClearMenus(void)
	int __thiscall CLevel::IsEmpty(void)const
	void __thiscall CLevel::ActivateTimeWarp(class CLevelTimeWarp const &)
	void __thiscall CLevel::SetClock(class CGeneralClock *)
	void __thiscall CLevel::SetClockFractionOfDay(float)
	void __thiscall CLevel::TriggerClockEvents(class CGeneralClockTime const *)
	void __thiscall CLevel::CTaskInfo::CleanUp(void)
	void __thiscall CLevel::AddExpensiveTask(class CSimpleTask *,int,class CActor *)
	__thiscall CLevel::CTaskInfo::~CTaskInfo(void)
	int __thiscall CLevel::CancelExpensiveTask(class CSimpleTask *)
	int __thiscall CLevel::CancelExpensiveTasks(class CActor const *)
	void __thiscall CLevel::CancelExpensiveTasks(void)
	void __thiscall CLevel::UpdateExpensiveTasks(float)
	class CCamera * __thiscall CLevel::AddMenuCamera(void)
	class CMicrophone * __thiscall CLevel::AddMenuMicrophone(void)
	void __thiscall CLevel::ListCollidingActors(class CQuickVector<class CActor *> &,class Intersection::CBox const &,int)const
	virtual void __thiscall CLevel::HandleEndOfVideoStream(void)
	void __thiscall CLevel::SetHandlerForEndOfVideoStream(void)
	void __thiscall CLevel::SetCurrentFragmentNumber(int)
	int __thiscall CLevel::AddFragment(char const *,char const *,int)
	void __thiscall CLevel::SetFingeredActor(int,class CActor *)
	void __thiscall CLevel::SetFocus(class CActor *,int)
	void __thiscall CLevel::SetFocus(int,class CActor *,int)
	void __thiscall CLevel::LoadAsynchronous(char const *)
	void __thiscall CLevel::UpdateLevelLoad(void)
	struct CLevel::CResourceUsage __thiscall CLevel::ActorResourceUsage(void)const
	struct CLevel::CResourceUsage __thiscall CLevel::LevelResourceUsage(void)const
	struct CLevel::CResourceUsage __thiscall CLevel::PreCacheResourceUsage(void)const
	void __thiscall CLevel::SetUpdated(BOOL)
	void __thiscall CLevel::SetRendered(BOOL)
	void __thiscall CLevel::SetAllowsOthersToUpdate(BOOL)
	void __thiscall CLevel::SetAllowsOthersToRender(BOOL)
	void __thiscall CLevel::SetIsPortalTarget(BOOL)
	void __thiscall CLevel::SetDead(BOOL)
	void __thiscall CLevel::SetPendingStandby(BOOL)
	void __thiscall CLevel::SetPendingReload(BOOL)
	void __thiscall CLevel::SetWasJustMovedOffStandby(BOOL)
	void __thiscall CLevel::SuppressSounds(BOOL)
	static unsigned long __cdecl CLevel::HashedId(char const *)
	unsigned long __thiscall CLevel::LastActorId(void)const
	void __thiscall CLevel::CreatePreCacheManager(void)
	int __thiscall CLevel::WasJustLoaded(void)
	int __thiscall CLevel::ResourcesHaveBeenLoaded(void)const
	void __thiscall CLevel::CancelLoad(void)
	void __thiscall CLevel::RemoveFromRenderList(class CActor &)
	void __thiscall CLevel::SetOriginatingLevelHashedId(unsigned long)
	void __thiscall CLevel::AddAutoDestructLevel(unsigned long)
	void __thiscall CLevel::SetSubtypes(unsigned long)
	virtual void __thiscall CLevel::HandlePortalAboutToBeOpened(class CActor &,char const *)
	unsigned long __thiscall CLevel::MemorySequenceId(void)const
	void __thiscall CLevel::SetMemorySequenceId(unsigned long)
	void __thiscall CLevel::StopAllPlayingSounds(void)
	void __thiscall CLevel::StopAndFreeAllSounds(void)
	class CActor * __thiscall CLevel::FindActor(char const *)const
	void __thiscall CLevel::RemoveActorFromRunTimeNameList(class CActor &)
	void __thiscall CLevel::AddActorToRunTimeNameList(class CActor &)
	class CLevel::CIteratorByRunTimeName __thiscall CLevel::IteratorByRunTimeName(char const *)const
	class CLevel::CIteratorByRunTimeName __thiscall CLevel::CIteratorByRunTimeName::Next(char const *)const
	class CLevel::CDeepIterator & __thiscall CLevel::CDeepIterator::operator++(void)
	void __thiscall CLevel::CollectShadows(int,int)
	struct vector3 __thiscall CLevel::AveragePlayerPosition(BOOL)const
	void __thiscall CLevel::RenderActorSummariesAndMarkers(void)const
	void __thiscall CLevel::AddActorToBeRenderedAfterOverlays(class CActor *)
	void __thiscall CLevel::RemoveActorToBeRenderedAfterOverlays(class CActor *)
	void __thiscall CLevel::SetPublicSettings(unsigned long)
	void __thiscall CLevel::AddPublicSetting(enum CLevel::PublicSettingType)
	void __thiscall CLevel::RemovePublicSetting(enum CLevel::PublicSettingType)
	void __thiscall CLevel::CleanUpMemory(void)
	__thiscall x_std::allocator<class CLevel::CMenuInfo>::allocator<class CLevel::CMenuInfo>(void)
	__thiscall x_std::allocator<class CProp *>::allocator<class CProp *>(void)
	__thiscall x_std::allocator<class CLevel::CActorInfo>::allocator<class CLevel::CActorInfo>(void)
	__thiscall x_std::allocator<class CCamera *>::allocator<class CCamera *>(void)
	__thiscall x_std::allocator<class CMicrophone *>::allocator<class CMicrophone *>(void)
	__thiscall x_std::allocator<class CActor *>::allocator<class CActor *>(void)
	__thiscall x_std::allocator<class CCinema *>::allocator<class CCinema *>(void)
	__thiscall x_std::allocator<class CLevel::CTaskInfo>::allocator<class CLevel::CTaskInfo>(void)
	__thiscall x_std::allocator<class CActorTypeInfo const *>::allocator<class CActorTypeInfo const *>(void)
	__thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::reverse_iterator::reverse_iterator(void)
	class x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::reverse_iterator & __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::reverse_iterator::operator++(void)
	class CCamera * & __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::reverse_iterator::operator*(void)const
	BOOL __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::reverse_iterator::operator!=(class x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::reverse_iterator const &)const
	__thiscall owned_ptr<class CMaterialChanges>::operator class CMaterialChanges const *(void)const
	unsigned long const * __thiscall x_std::vector<unsigned long,class x_std::allocator<unsigned long> >::begin(void)const
	unsigned long * __thiscall x_std::vector<unsigned long,class x_std::allocator<unsigned long> >::begin(void)
	unsigned long const * __thiscall x_std::vector<unsigned long,class x_std::allocator<unsigned long> >::end(void)const
	unsigned long * __thiscall x_std::vector<unsigned long,class x_std::allocator<unsigned long> >::end(void)
	class CActor * __thiscall CInlineList<class CActor,class CActorSiblingLinks>::pop_back(void)
	unsigned int __thiscall x_std::vector<class CActorGrid *,class x_std::allocator<class CActorGrid *> >::size(void)const
	BOOL __thiscall CQuickVector<class CActor *>::full(void)const
	int __thiscall CQuickVector<class CActor *>::capacity(void)const
	void __thiscall CQuickVector<class CActor *>::clear(void)
	class CPreCacheManager * __thiscall owned_ptr<class CPreCacheManager>::operator->(void)
	BOOL __thiscall owned_ptr<class CPreCacheManager>::operator==(class CPreCacheManager const *)const
	class CPreCacheManager * __thiscall owned_ptr<class CPreCacheManager>::release(void)
	void __thiscall x_std::vector<class CPath *,class x_std::allocator<class CPath *> >::push_back(class CPath * const &)
	class CParticleSystem * __thiscall owned_ptr<class CParticleSystem>::operator->(void)
	BOOL __thiscall owned_ptr<class CParticleSystem>::operator!=(class CParticleSystem const *)const
	class CSwooshSystem * __thiscall owned_ptr<class CSwooshSystem>::operator->(void)
	BOOL __thiscall owned_ptr<class CSwooshSystem>::operator!=(class CSwooshSystem const *)const
	__thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >(class x_std::allocator<class CLevel::CMenuInfo> const &)
	__thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::~vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >(void)
	BOOL __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::empty(void)const
	class CLevel::CMenuInfo & __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::back(void)
	void __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::push_back(class CLevel::CMenuInfo const &)
	void __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::pop_back(void)
	void __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::clear(void)
	__thiscall owned_ptr<class CActorCollisionGrids>::owned_ptr<class CActorCollisionGrids>(void)
	__thiscall owned_ptr<class CActorCollisionGrids>::~owned_ptr<class CActorCollisionGrids>(void)
	class CActorCollisionGrids const * __thiscall owned_ptr<class CActorCollisionGrids>::operator->(void)const
	class CActorCollisionGrids * __thiscall owned_ptr<class CActorCollisionGrids>::operator->(void)
	class owned_ptr<class CActorCollisionGrids> & __thiscall owned_ptr<class CActorCollisionGrids>::operator=(class CActorCollisionGrids *)
	__thiscall owned_ptr<class CActorUpdateGrids>::owned_ptr<class CActorUpdateGrids>(void)
	__thiscall owned_ptr<class CActorUpdateGrids>::~owned_ptr<class CActorUpdateGrids>(void)
	class CActorUpdateGrids const * __thiscall owned_ptr<class CActorUpdateGrids>::operator->(void)const
	class CActorUpdateGrids * __thiscall owned_ptr<class CActorUpdateGrids>::operator->(void)
	BOOL __thiscall owned_ptr<class CActorUpdateGrids>::operator!=(class CActorUpdateGrids const *)const
	class owned_ptr<class CActorUpdateGrids> & __thiscall owned_ptr<class CActorUpdateGrids>::operator=(class CActorUpdateGrids *)
	__thiscall owned_ptr<class CActorRenderGrids>::owned_ptr<class CActorRenderGrids>(void)
	__thiscall owned_ptr<class CActorRenderGrids>::~owned_ptr<class CActorRenderGrids>(void)
	class CActorRenderGrids const * __thiscall owned_ptr<class CActorRenderGrids>::operator->(void)const
	class CActorRenderGrids * __thiscall owned_ptr<class CActorRenderGrids>::operator->(void)
	BOOL __thiscall owned_ptr<class CActorRenderGrids>::operator!=(class CActorRenderGrids const *)const
	class owned_ptr<class CActorRenderGrids> & __thiscall owned_ptr<class CActorRenderGrids>::operator=(class CActorRenderGrids *)
	__thiscall owned_ptr<class CRenderOptimizer>::owned_ptr<class CRenderOptimizer>(void)
	__thiscall owned_ptr<class CRenderOptimizer>::~owned_ptr<class CRenderOptimizer>(void)
	class CRenderOptimizer * __thiscall owned_ptr<class CRenderOptimizer>::operator->(void)
	class owned_ptr<class CRenderOptimizer> & __thiscall owned_ptr<class CRenderOptimizer>::operator=(class CRenderOptimizer *)
	__thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::vector<class CProp *,class x_std::allocator<class CProp *> >(class x_std::allocator<class CProp *> const &)
	__thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::~vector<class CProp *,class x_std::allocator<class CProp *> >(void)
	class CProp ** __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::begin(void)
	class CProp ** __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::end(void)
	BOOL __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::empty(void)const
	void __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::reserve(unsigned int)
	class CProp * & __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::back(void)
	void __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::push_back(class CProp * const &)
	void __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::pop_back(void)
	__thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >(class x_std::allocator<class CLevel::CActorInfo> const &)
	__thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::~vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >(void)
	unsigned int __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::capacity(void)const
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::reserve(unsigned int)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::resize(unsigned int,class CLevel::CActorInfo const &)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::clear(void)
	__thiscall owned_ptr<class CPaths>::owned_ptr<class CPaths>(void)
	__thiscall owned_ptr<class CPaths>::~owned_ptr<class CPaths>(void)
	class CPaths * __thiscall owned_ptr<class CPaths>::operator->(void)
	class owned_ptr<class CPaths> & __thiscall owned_ptr<class CPaths>::operator=(class CPaths *)
	__thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::vector<class CCamera *,class x_std::allocator<class CCamera *> >(class x_std::allocator<class CCamera *> const &)
	__thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::~vector<class CCamera *,class x_std::allocator<class CCamera *> >(void)
	class CCamera ** __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::begin(void)
	class CCamera ** __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::end(void)
	class x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::reverse_iterator __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::rbegin(void)
	class x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::reverse_iterator __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::rend(void)
	unsigned int __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::size(void)const
	BOOL __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::empty(void)const
	class CCamera * const & __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::operator[](unsigned int)const
	class CCamera * & __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::front(void)
	class CCamera * & __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::back(void)
	void __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::push_back(class CCamera * const &)
	void __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::pop_back(void)
	void __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::clear(void)
	class CCamera ** __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::erase(class CCamera * *)
	__thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >(class x_std::allocator<class CMicrophone *> const &)
	__thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::~vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >(void)
	class CMicrophone ** __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::begin(void)
	class CMicrophone ** __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::end(void)
	unsigned int __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::size(void)const
	class CMicrophone * const & __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::operator[](unsigned int)const
	void __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::push_back(class CMicrophone * const &)
	void __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::clear(void)
	class CMicrophone ** __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::erase(class CMicrophone * *)
	__thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::vector<class CActor *,class x_std::allocator<class CActor *> >(class x_std::allocator<class CActor *> const &)
	__thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::~vector<class CActor *,class x_std::allocator<class CActor *> >(void)
	class CActor * const * __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::begin(void)const
	class CActor ** __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::begin(void)
	class CActor * const * __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::end(void)const
	class CActor ** __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::end(void)
	BOOL __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::empty(void)const
	unsigned int __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::capacity(void)const
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::reserve(unsigned int)
	class CActor * & __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::operator[](unsigned int)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::push_back(class CActor * const &)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::clear(void)
	class CActor ** __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::insert(class CActor * *,class CActor * const &)
	class CActor ** __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::erase(class CActor * *)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::swap(class x_std::vector<class CActor *,class x_std::allocator<class CActor *> > &)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::x_free(void)
	__thiscall owned_ptr<class CNavData>::owned_ptr<class CNavData>(void)
	__thiscall owned_ptr<class CNavData>::~owned_ptr<class CNavData>(void)
	class CNavData * __thiscall owned_ptr<class CNavData>::operator->(void)
	BOOL __thiscall owned_ptr<class CNavData>::operator!=(class CNavData const *)const
	class owned_ptr<class CNavData> & __thiscall owned_ptr<class CNavData>::operator=(class CNavData *)
	__thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::vector<class CCinema *,class x_std::allocator<class CCinema *> >(class x_std::allocator<class CCinema *> const &)
	__thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::~vector<class CCinema *,class x_std::allocator<class CCinema *> >(void)
	class CCinema ** __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::begin(void)
	class CCinema ** __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::end(void)
	BOOL __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::empty(void)const
	class CCinema * & __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::back(void)
	void __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::push_back(class CCinema * const &)
	void __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::pop_back(void)
	void __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::clear(void)
	class CCinema ** __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::erase(class CCinema * *)
	__thiscall owned_ptr<class CPerformanceGroups>::owned_ptr<class CPerformanceGroups>(void)
	__thiscall owned_ptr<class CPerformanceGroups>::~owned_ptr<class CPerformanceGroups>(void)
	class CPerformanceGroups & __thiscall owned_ptr<class CPerformanceGroups>::operator*(void)
	class owned_ptr<class CPerformanceGroups> & __thiscall owned_ptr<class CPerformanceGroups>::operator=(class CPerformanceGroups *)
	__thiscall owned_ptr<class CGeneralClock>::owned_ptr<class CGeneralClock>(void)
	__thiscall owned_ptr<class CGeneralClock>::~owned_ptr<class CGeneralClock>(void)
	class CGeneralClock & __thiscall owned_ptr<class CGeneralClock>::operator*(void)
	class owned_ptr<class CGeneralClock> & __thiscall owned_ptr<class CGeneralClock>::operator=(class CGeneralClock *)
	__thiscall owned_ptr<class CCalculator>::owned_ptr<class CCalculator>(void)
	__thiscall owned_ptr<class CCalculator>::~owned_ptr<class CCalculator>(void)
	class owned_ptr<class CCalculator> & __thiscall owned_ptr<class CCalculator>::operator=(class CCalculator *)
	__thiscall owned_ptr<class CLightManager>::owned_ptr<class CLightManager>(void)
	__thiscall owned_ptr<class CLightManager>::~owned_ptr<class CLightManager>(void)
	__thiscall owned_ptr<class CLightManager>::operator class CLightManager *(void)
	class CLightManager & __thiscall owned_ptr<class CLightManager>::operator*(void)
	class owned_ptr<class CLightManager> & __thiscall owned_ptr<class CLightManager>::operator=(class CLightManager *)
	__thiscall owned_ptr<class CPortalManager>::owned_ptr<class CPortalManager>(void)
	__thiscall owned_ptr<class CPortalManager>::~owned_ptr<class CPortalManager>(void)
	class CPortalManager & __thiscall owned_ptr<class CPortalManager>::operator*(void)
	class owned_ptr<class CPortalManager> & __thiscall owned_ptr<class CPortalManager>::operator=(class CPortalManager *)
	__thiscall owned_ptr<class CLighting>::owned_ptr<class CLighting>(void)
	__thiscall owned_ptr<class CLighting>::~owned_ptr<class CLighting>(void)
	class CLighting & __thiscall owned_ptr<class CLighting>::operator*(void)
	class owned_ptr<class CLighting> & __thiscall owned_ptr<class CLighting>::operator=(class CLighting *)
	__thiscall owned_ptr<class CVisibility>::owned_ptr<class CVisibility>(void)
	__thiscall owned_ptr<class CVisibility>::~owned_ptr<class CVisibility>(void)
	__thiscall owned_ptr<class CVisibility>::operator class CVisibility *(void)
	class CVisibility * __thiscall owned_ptr<class CVisibility>::operator->(void)
	BOOL __thiscall owned_ptr<class CVisibility>::operator!=(class CVisibility const *)const
	class owned_ptr<class CVisibility> & __thiscall owned_ptr<class CVisibility>::operator=(class CVisibility *)
	__thiscall owned_ptr<class CLevelFragments>::owned_ptr<class CLevelFragments>(void)
	__thiscall owned_ptr<class CLevelFragments>::~owned_ptr<class CLevelFragments>(void)
	class CLevelFragments & __thiscall owned_ptr<class CLevelFragments>::operator*(void)
	class CLevelFragments * __thiscall owned_ptr<class CLevelFragments>::operator->(void)
	class owned_ptr<class CLevelFragments> & __thiscall owned_ptr<class CLevelFragments>::operator=(class CLevelFragments *)
	__thiscall owned_ptr<class CParticleContainer>::owned_ptr<class CParticleContainer>(void)
	__thiscall owned_ptr<class CParticleContainer>::~owned_ptr<class CParticleContainer>(void)
	class CParticleContainer * __thiscall owned_ptr<class CParticleContainer>::operator->(void)
	class owned_ptr<class CParticleContainer> & __thiscall owned_ptr<class CParticleContainer>::operator=(class CParticleContainer *)
	class CParticleContainer * __thiscall owned_ptr<class CParticleContainer>::release(void)
	__thiscall owned_ptr<class CPostEffects>::owned_ptr<class CPostEffects>(void)
	__thiscall owned_ptr<class CPostEffects>::~owned_ptr<class CPostEffects>(void)
	class owned_ptr<class CPostEffects> & __thiscall owned_ptr<class CPostEffects>::operator=(class CPostEffects *)
	__thiscall owned_ptr<class CDebugDemons>::owned_ptr<class CDebugDemons>(void)
	__thiscall owned_ptr<class CDebugDemons>::~owned_ptr<class CDebugDemons>(void)
	class owned_ptr<class CDebugDemons> & __thiscall owned_ptr<class CDebugDemons>::operator=(class CDebugDemons *)
	__thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >(class x_std::allocator<class CLevel::CTaskInfo> const &)
	__thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::~vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >(void)
	class CLevel::CTaskInfo * __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::begin(void)
	class CLevel::CTaskInfo * __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::end(void)
	BOOL __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::empty(void)const
	class CLevel::CTaskInfo & __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::front(void)
	class CLevel::CTaskInfo & __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::back(void)
	void __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::push_back(class CLevel::CTaskInfo const &)
	void __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::pop_back(void)
	class CLevel::CTaskInfo * __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::erase(class CLevel::CTaskInfo *)
	__thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >(class x_std::allocator<class CActorTypeInfo const *> const &)
	__thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::~vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >(void)
	unsigned int __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::size(void)const
	class CActorTypeInfo const * & __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::operator[](unsigned int)
	void __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::push_back(class CActorTypeInfo const * const &)
	void __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::x_free(void)
	unsigned int __thiscall x_std::vector<class CLevelFragment *,class x_std::allocator<class CLevelFragment *> >::size(void)const
	class CLevelFragment * & __thiscall x_std::vector<class CLevelFragment *,class x_std::allocator<class CLevelFragment *> >::operator[](unsigned int)
	void __cdecl x_std::fill<class safe_ptr<class CActor> *,class CActor *>(class safe_ptr<class CActor> *,class safe_ptr<class CActor> *,class CActor * const &)
	class CCamera ** __cdecl x_std::find<class CCamera * *,class CCamera *>(class CCamera * *,class CCamera * *,class CCamera * const &)
	class CMicrophone ** __cdecl x_std::find<class CMicrophone * *,class CMicrophone *>(class CMicrophone * *,class CMicrophone * *,class CMicrophone * const &)
	class CActor ** __cdecl x_std::find<class CActor * *,class CActor *>(class CActor * *,class CActor * *,class CActor * const &)
	class CCamera ** __cdecl x_std::find<class CCamera * *,class CActor *>(class CCamera * *,class CCamera * *,class CActor * const &)
	class CMicrophone ** __cdecl x_std::find<class CMicrophone * *,class CActor *>(class CMicrophone * *,class CMicrophone * *,class CActor * const &)
	__thiscall CQuickVector<class CRegion *>::CQuickVector<class CRegion *>(class CRegion * *,int)
	BOOL __thiscall CQuickVector<class CRegion *>::full(void)const
	void __thiscall CQuickVector<class CRegion *>::push_back(class CRegion * const &)
	BOOL __thiscall CQuickVector<class CRegion *>::has(class CRegion * const &)const
	void __thiscall CActorList<class CRegion>::Add(class CRegion *)
	BOOL __thiscall CActorList<class CRegion>::Has(class CRegion const *)
	int __cdecl Abs<int>(int)
	unsigned long * __cdecl x_std::find<unsigned long *,unsigned long>(unsigned long *,unsigned long *,unsigned long const &)
	void * __thiscall CActorCollisionGrids::`scalar deleting destructor'(unsigned int)
	void * __thiscall CActorUpdateGrids::`scalar deleting destructor'(unsigned int)
	void * __thiscall CActorRenderGrids::`scalar deleting destructor'(unsigned int)
	void * __thiscall CRenderOptimizer::`scalar deleting destructor'(unsigned int)
	void * __thiscall CPaths::`scalar deleting destructor'(unsigned int)
	void * __thiscall CPerformanceGroups::`scalar deleting destructor'(unsigned int)
	void * __thiscall CCalculator::`scalar deleting destructor'(unsigned int)
	void * __thiscall CLightManager::`scalar deleting destructor'(unsigned int)
	void * __thiscall CPortalManager::`scalar deleting destructor'(unsigned int)
	void * __thiscall CLighting::`scalar deleting destructor'(unsigned int)
	void * __thiscall CVisibility::`scalar deleting destructor'(unsigned int)
	void * __thiscall CLevelFragments::`scalar deleting destructor'(unsigned int)
	void * __thiscall CParticleContainer::`scalar deleting destructor'(unsigned int)
	void * __thiscall CPostEffects::`scalar deleting destructor'(unsigned int)
	void * __thiscall CDebugDemons::`scalar deleting destructor'(unsigned int)
	__thiscall CActorCollisionGrids::~CActorCollisionGrids(void)
	__thiscall CActorUpdateGrids::~CActorUpdateGrids(void)
	__thiscall CActorRenderGrids::~CActorRenderGrids(void)
	__thiscall CDebugDemons::~CDebugDemons(void)
	void __thiscall x_std::allocator<class CPath *>::construct(class CPath * *,class CPath * const &)
	void __thiscall x_std::allocator<class CLevel::CMenuInfo>::construct(class CLevel::CMenuInfo *,class CLevel::CMenuInfo const &)
	void __thiscall x_std::allocator<class CLevel::CMenuInfo>::destroy(class CLevel::CMenuInfo *)
	__thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::this_type::this_type(class x_std::allocator<class CLevel::CMenuInfo>)
	void __thiscall x_std::allocator<class CProp *>::construct(class CProp * *,class CProp * const &)
	void __thiscall x_std::allocator<class CProp *>::destroy(class CProp * *)
	__thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::this_type::this_type(class x_std::allocator<class CProp *>)
	__thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::this_type::this_type(class x_std::allocator<class CLevel::CActorInfo>)
	void __thiscall x_std::allocator<class CCamera *>::construct(class CCamera * *,class CCamera * const &)
	void __thiscall x_std::allocator<class CCamera *>::destroy(class CCamera * *)
	__thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::this_type::this_type(class x_std::allocator<class CCamera *>)
	void __thiscall x_std::allocator<class CMicrophone *>::construct(class CMicrophone * *,class CMicrophone * const &)
	__thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::this_type::this_type(class x_std::allocator<class CMicrophone *>)
	void __thiscall x_std::allocator<class CActor *>::construct(class CActor * *,class CActor * const &)
	__thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::this_type::this_type(class x_std::allocator<class CActor *>)
	void __thiscall x_std::allocator<class CCinema *>::construct(class CCinema * *,class CCinema * const &)
	void __thiscall x_std::allocator<class CCinema *>::destroy(class CCinema * *)
	__thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::this_type::this_type(class x_std::allocator<class CCinema *>)
	void __thiscall x_std::allocator<class CLevel::CTaskInfo>::construct(class CLevel::CTaskInfo *,class CLevel::CTaskInfo const &)
	void __thiscall x_std::allocator<class CLevel::CTaskInfo>::destroy(class CLevel::CTaskInfo *)
	__thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::this_type::this_type(class x_std::allocator<class CLevel::CTaskInfo>)
	void __thiscall x_std::allocator<class CActorTypeInfo const *>::construct(class CActorTypeInfo const * *,class CActorTypeInfo const * const &)
	__thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::this_type::this_type(class x_std::allocator<class CActorTypeInfo const *>)
	__thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::reverse_iterator::reverse_iterator(class CCamera * *)
	class x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::iterator & __thiscall x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::iterator::operator++(void)
	class safe_ptr<class CActor> & __thiscall x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::iterator::operator*(void)const
	BOOL __thiscall x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::iterator::operator!=(class x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::iterator const &)const
	void __thiscall x_std::vector<class CPath *,class x_std::allocator<class CPath *> >::reserve(unsigned int)
	unsigned int __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::size(void)const
	unsigned int __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::capacity(void)const
	void __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::reserve(unsigned int)
	void __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::erase(unsigned int,unsigned int)
	void __thiscall x_std::allocator<class CLevel::CMenuInfo>::deallocate(class CLevel::CMenuInfo *,unsigned int)
	unsigned int __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::size(void)const
	unsigned int __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::capacity(void)const
	void __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::clear(void)
	void __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::allocate_new(unsigned int)
	void __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::construct_up(class CProp * *,class CProp * *,class CProp * const *)
	void __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::destroy_up(class CProp * *,class CProp * *)
	void __thiscall x_std::allocator<class CProp *>::deallocate(class CProp * *,unsigned int)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::insert(unsigned int,unsigned int,class CLevel::CActorInfo const &)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::erase(unsigned int,unsigned int)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::allocate_new(unsigned int)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::construct_up(class CLevel::CActorInfo *,class CLevel::CActorInfo *,class CLevel::CActorInfo const *)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::destroy_up(class CLevel::CActorInfo *,class CLevel::CActorInfo *)
	void __thiscall x_std::allocator<class CLevel::CActorInfo>::deallocate(class CLevel::CActorInfo *,unsigned int)
	unsigned int __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::capacity(void)const
	void __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::reserve(unsigned int)
	void __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::erase(unsigned int,unsigned int)
	void __thiscall x_std::allocator<class CCamera *>::deallocate(class CCamera * *,unsigned int)
	unsigned int __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::capacity(void)const
	void __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::reserve(unsigned int)
	void __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::erase(unsigned int,unsigned int)
	void __thiscall x_std::allocator<class CMicrophone *>::deallocate(class CMicrophone * *,unsigned int)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::insert(unsigned int,unsigned int,class CActor * const &)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::erase(unsigned int,unsigned int)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::allocate_new(unsigned int)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::construct_up(class CActor * *,class CActor * *,class CActor * const *)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::destroy_up(class CActor * *,class CActor * *)
	void __thiscall x_std::allocator<class CActor *>::deallocate(class CActor * *,unsigned int)
	unsigned int __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::size(void)const
	unsigned int __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::capacity(void)const
	void __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::reserve(unsigned int)
	void __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::erase(unsigned int,unsigned int)
	void __thiscall x_std::allocator<class CCinema *>::deallocate(class CCinema * *,unsigned int)
	unsigned int __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::size(void)const
	unsigned int __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::capacity(void)const
	void __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::reserve(unsigned int)
	void __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::clear(void)
	void __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::erase(unsigned int,unsigned int)
	void __thiscall x_std::allocator<class CLevel::CTaskInfo>::deallocate(class CLevel::CTaskInfo *,unsigned int)
	unsigned int __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::capacity(void)const
	void __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::reserve(unsigned int)
	void __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::clear(void)
	void __thiscall x_std::allocator<class CActorTypeInfo const *>::deallocate(class CActorTypeInfo const * *,unsigned int)
	class x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::iterator __thiscall x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::begin(void)
	class x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::iterator __thiscall x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::end(void)
	void __thiscall x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::push_back(class safe_ptr<class CActor> const &)
	__thiscall x_std::vector<class CDebugDemon *,class x_std::allocator<class CDebugDemon *> >::~vector<class CDebugDemon *,class x_std::allocator<class CDebugDemon *> >(void)
	void * __cdecl x_std_placement_new<class CPath *>(class CPath * *,class CPath * const &)
	void * __cdecl x_std_placement_new<class CLevel::CMenuInfo>(class CLevel::CMenuInfo *,class CLevel::CMenuInfo const &)
	0void * __cdecl x_std_placement_new<class CProp *>(class CProp * *,class CProp * const &)
	void * __cdecl x_std_placement_new<class CCamera *>(class CCamera * *,class CCamera * const &)
	void * __cdecl x_std_placement_new<class CMicrophone *>(class CMicrophone * *,class CMicrophone * const &)
	void * __cdecl x_std_placement_new<class CActor *>(class CActor * *,class CActor * const &)
	void * __cdecl x_std_placement_new<class CCinema *>(class CCinema * *,class CCinema * const &)
	void * __cdecl x_std_placement_new<class CLevel::CTaskInfo>(class CLevel::CTaskInfo *,class CLevel::CTaskInfo const &)
	void * __cdecl x_std_placement_new<class CActorTypeInfo const *>(class CActorTypeInfo const * *,class CActorTypeInfo const * const &)
	__thiscall CLevel::CTaskInfo::CTaskInfo(class CLevel::CTaskInfo const &)
	struct x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::node_type * __thiscall x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::allocate_node(class safe_ptr<class CActor> const &)
	void __thiscall x_std::allocator<class CLevel::CActorInfo>::construct(class CLevel::CActorInfo *,class CLevel::CActorInfo const &)
	void __thiscall x_std::allocator<class CLevel::CActorInfo>::destroy(class CLevel::CActorInfo *)
	void __thiscall x_std::allocator<class CActor *>::destroy(class CActor * *)
	__thiscall x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::iterator::iterator(struct x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::node_type *)
	void __thiscall x_std::vector<class CPath *,class x_std::allocator<class CPath *> >::allocate_new(unsigned int)
	void __thiscall x_std::vector<class CPath *,class x_std::allocator<class CPath *> >::construct_up(class CPath * *,class CPath * *,class CPath * const *)
	void __thiscall x_std::vector<class CPath *,class x_std::allocator<class CPath *> >::destroy_up(class CPath * *,class CPath * *)
	void __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::allocate_new(unsigned int)
	void __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::copy_up_n(class CLevel::CMenuInfo *,class CLevel::CMenuInfo const *,unsigned int)
	void __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::destroy_up_n(class CLevel::CMenuInfo *,unsigned int)
	void __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::construct_up(class CLevel::CMenuInfo *,class CLevel::CMenuInfo *,class CLevel::CMenuInfo const *)
	void __thiscall x_std::vector<class CLevel::CMenuInfo,class x_std::allocator<class CLevel::CMenuInfo> >::destroy_up(class CLevel::CMenuInfo *,class CLevel::CMenuInfo *)
	void __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::erase(unsigned int,unsigned int)
	class CProp ** __thiscall x_std::allocator<class CProp *>::allocate(unsigned int,void const *)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::construct_up_n(class CLevel::CActorInfo *,class CLevel::CActorInfo const *,unsigned int)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::construct_up_n(class CLevel::CActorInfo *,class CLevel::CActorInfo const &,unsigned int)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::construct_down_n(class CLevel::CActorInfo *,class CLevel::CActorInfo const *,unsigned int)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::copy_up_n(class CLevel::CActorInfo *,class CLevel::CActorInfo const *,unsigned int)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::copy_up_n(class CLevel::CActorInfo *,class CLevel::CActorInfo const &,unsigned int)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::copy_down_n(class CLevel::CActorInfo *,class CLevel::CActorInfo const *,unsigned int)
	void __thiscall x_std::vector<class CLevel::CActorInfo,class x_std::allocator<class CLevel::CActorInfo> >::destroy_up_n(class CLevel::CActorInfo *,unsigned int)
	class CLevel::CActorInfo * __thiscall x_std::allocator<class CLevel::CActorInfo>::allocate(unsigned int,void const *)
	void __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::allocate_new(unsigned int)
	void __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::copy_up_n(class CCamera * *,class CCamera * const *,unsigned int)
	void __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::destroy_up_n(class CCamera * *,unsigned int)
	void __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::construct_up(class CCamera * *,class CCamera * *,class CCamera * const *)
	void __thiscall x_std::vector<class CCamera *,class x_std::allocator<class CCamera *> >::destroy_up(class CCamera * *,class CCamera * *)
	void __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::allocate_new(unsigned int)
	void __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::copy_up_n(class CMicrophone * *,class CMicrophone * const *,unsigned int)
	void __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::destroy_up_n(class CMicrophone * *,unsigned int)
	void __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::construct_up(class CMicrophone * *,class CMicrophone * *,class CMicrophone * const *)
	void __thiscall x_std::vector<class CMicrophone *,class x_std::allocator<class CMicrophone *> >::destroy_up(class CMicrophone * *,class CMicrophone * *)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::construct_up_n(class CActor * *,class CActor * const *,unsigned int)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::construct_up_n(class CActor * *,class CActor * const &,unsigned int)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::construct_down_n(class CActor * *,class CActor * const *,unsigned int)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::copy_up_n(class CActor * *,class CActor * const *,unsigned int)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::copy_up_n(class CActor * *,class CActor * const &,unsigned int)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::copy_down_n(class CActor * *,class CActor * const *,unsigned int)
	void __thiscall x_std::vector<class CActor *,class x_std::allocator<class CActor *> >::destroy_up_n(class CActor * *,unsigned int)
	class CActor ** __thiscall x_std::allocator<class CActor *>::allocate(unsigned int,void const *)
	void __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::allocate_new(unsigned int)
	void __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::copy_up_n(class CCinema * *,class CCinema * const *,unsigned int)
	void __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::destroy_up_n(class CCinema * *,unsigned int)
	void __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::construct_up(class CCinema * *,class CCinema * *,class CCinema * const *)
	void __thiscall x_std::vector<class CCinema *,class x_std::allocator<class CCinema *> >::destroy_up(class CCinema * *,class CCinema * *)
	void __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::allocate_new(unsigned int)
	void __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::copy_up_n(class CLevel::CTaskInfo *,class CLevel::CTaskInfo const *,unsigned int)
	void __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::destroy_up_n(class CLevel::CTaskInfo *,unsigned int)
	void __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::construct_up(class CLevel::CTaskInfo *,class CLevel::CTaskInfo *,class CLevel::CTaskInfo const *)
	void __thiscall x_std::vector<class CLevel::CTaskInfo,class x_std::allocator<class CLevel::CTaskInfo> >::destroy_up(class CLevel::CTaskInfo *,class CLevel::CTaskInfo *)
	void __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::erase(unsigned int,unsigned int)
	void __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::allocate_new(unsigned int)
	void __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::construct_up(class CActorTypeInfo const * *,class CActorTypeInfo const * *,class CActorTypeInfo const * const *)
	void __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::destroy_up(class CActorTypeInfo const * *,class CActorTypeInfo const * *)
	void __thiscall x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::insert_before(struct x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::node_type *,struct x_std::list<class safe_ptr<class CActor>,class x_std::allocator<class safe_ptr<class CActor> > >::node_type *)
	unsigned int __thiscall x_std::vector<class CDebugDemon *,class x_std::allocator<class CDebugDemon *> >::capacity(void)const
	void __thiscall x_std::vector<class CDebugDemon *,class x_std::allocator<class CDebugDemon *> >::clear(void)
	void __thiscall x_std::allocator<class CDebugDemon *>::deallocate(class CDebugDemon * *,unsigned int)
	void * __thiscall x_std::allocator<class safe_ptr<class CActor> >::x_allocate_char(unsigned int)
	void * __cdecl x_std_placement_new<class CLevel::CActorInfo>(class CLevel::CActorInfo *,class CLevel::CActorInfo const &)
	class CLevel::CTaskInfo & __thiscall CLevel::CTaskInfo::operator=(class CLevel::CTaskInfo const &)
	void __thiscall x_std::allocator<class CMicrophone *>::destroy(class CMicrophone * *)
	void __thiscall x_std::allocator<class CActorTypeInfo const *>::destroy(class CActorTypeInfo const * *)
	class CPath ** __thiscall x_std::allocator<class CPath *>::allocate(unsigned int,void const *)
	class CLevel::CMenuInfo * __thiscall x_std::allocator<class CLevel::CMenuInfo>::allocate(unsigned int,void const *)
	void __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::copy_up_n(class CProp * *,class CProp * const *,unsigned int)
	void __thiscall x_std::vector<class CProp *,class x_std::allocator<class CProp *> >::destroy_up_n(class CProp * *,unsigned int)
	class CCamera ** __thiscall x_std::allocator<class CCamera *>::allocate(unsigned int,void const *)
	class CMicrophone ** __thiscall x_std::allocator<class CMicrophone *>::allocate(unsigned int,void const *)
	class CCinema ** __thiscall x_std::allocator<class CCinema *>::allocate(unsigned int,void const *)
	class CLevel::CTaskInfo * __thiscall x_std::allocator<class CLevel::CTaskInfo>::allocate(unsigned int,void const *)
	void __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::copy_up_n(class CActorTypeInfo const * *,class CActorTypeInfo const * const *,unsigned int)
	void __thiscall x_std::vector<class CActorTypeInfo const *,class x_std::allocator<class CActorTypeInfo const *> >::destroy_up_n(class CActorTypeInfo const * *,unsigned int)
	class CActorTypeInfo const ** __thiscall x_std::allocator<class CActorTypeInfo const *>::allocate(unsigned int,void const *)
	unsigned int __thiscall x_std::vector<class CDebugDemon *,class x_std::allocator<class CDebugDemon *> >::size(void)const
	void __thiscall x_std::vector<class CDebugDemon *,class x_std::allocator<class CDebugDemon *> >::erase(unsigned int,unsigned int)
	void __thiscall x_std::vector<class CDebugDemon *,class x_std::allocator<class CDebugDemon *> >::copy_up_n(class CDebugDemon * *,class CDebugDemon * const *,unsigned int)
	void __thiscall x_std::vector<class CDebugDemon *,class x_std::allocator<class CDebugDemon *> >::destroy_up_n(class CDebugDemon * *,unsigned int)
	void __thiscall x_std::allocator<class CDebugDemon *>::destroy(class CDebugDemon * *)

	ScreenProxy.cpp

	void __thiscall CLevel::SetScreenProxy(class CScreenProxy *)

	ActorJoystick.cpp
	
	unsigned long __thiscall CLevel::FrameCount(void)

	LinkConditional.cpp

	static class CLinkConditional * __cdecl CLevelLinkConditional::Make(void)

	PIPCamera.cpp

	enum CLevel::RenderPhaseType __thiscall CLevel::RenderPhase(void)

	portal.cpp

	void __thiscall CLevel::SetCurrentCamera(class CCamera *)

	LinkCause.cpp

	static class CLinkCause * __cdecl CLevelLinkCause::Make(void)

	BlendedCamera.cpp

	void __thiscall CLevel::DisplayDebugMessage(char const *,float)

	ActorRegions.cpp

	class CActorCollisionGrids const & __thiscall CLevel::CollisionGrids(void)

	ActorChannels.cpp

	class CCalculator & __thiscall CLevel::Calculator(void)
	float __thiscall CLevel::OverallTimeScale(void)const
	float __thiscall CLevel::ParticleTimeScale(void)const
	void __thiscall CLevel::SetClockSpeed(float)
	void __thiscall CLevel::SetOverallTimeScale(float)
	void __thiscall CLevel::SetParticleTimeScale(float)
	void __thiscall CLevel::SetClothWindDirection(struct vector3 const &)

	ParticleContainer.cpp

	public: class CPostEffects & __thiscall CLevel::SetPostEffects(void)

	PredefinedLinkEvents.cpp

	class CActor * __thiscall CLevel::Focus(int)const
	unsigned long __thiscall CLevel::PublicSettings(void)

	LevelProperties.cpp

	int __thiscall CLevelProperties::ParseProperty(class CTextFileParser &)

	Prop.cpp

	class CRenderOptimizer & __thiscall CLevel::SetRenderOptimizer(void)

	ParticleSystem.cpp

	int __thiscall CLevel::FindClosestBlockingCollisionFromSphere(class CCollision &,class CActor const *,int,int,struct vector3 const &,struct vector3 const &,float)

	LevelFragment.cpp

	char const * __thiscall CLevelFragment::NickName(void)const
	class CLevelFragment const * __thiscall CLevelFragments::Fragment(int)const
	BOOL __thiscall CLevelFragment::IsRendered(void)const
	BOOL __thiscall CLevelFragment::IsUpdated(void)const
	__thiscall CLevelFragment::CLevelFragment(void)
	__thiscall CLevelFragment::~CLevelFragment(void)
	void __thiscall CLevelFragment::Load(void)
	void __thiscall CLevelFragment::Unload(void)
	void __thiscall CLevelFragment::Update(void)
	void __thiscall CLevelFragment::SetRendered(BOOL)
	void __thiscall CLevelFragment::SetUpdated(BOOL)
	void __thiscall CLevelFragment::RemoveActors(void)
	void __thiscall CLevelFragment::Add(class CActor *)
	__thiscall CLevelFragments::CLevelFragments(void)
	__thiscall CLevelFragments::~CLevelFragments(void)
	void * __thiscall CLevelFragment::`scalar deleting destructor'(unsigned int)
	void __thiscall CLevelFragments::Update(void)
	class CLevelFragment * __thiscall CLevelFragments::Add(int)
	class CLevelFragment const * __thiscall CLevelFragments::FindByNickName(char const *)const
	class CLevelFragment * __thiscall CLevelFragments::FindByNickName(char const *)
	class CLevelFragment * __thiscall CLevelFragments::FindByGroup(int)
	int __thiscall CLevelFragments::FindNumberByGroup(int)const
	int __thiscall CLevelFragments::FindNumberByNickName(char const *)const

	LevelTimeWarp.cpp

	int __thiscall CLevelTimeWarp::ParseProperty(class CTextFileParser &)

	ScreenFlash.cpp

	void __thiscall CLevel::SetTextAlphaMultiplier(float)

	PredefinedLinkConditionals.cpp

	virtual int __thiscall CLevelLinkConditional::ParseProperty(class CTextFileParser &)
	virtual BOOL __thiscall CLevelLinkConditional::IsTrue(class CActor &)

	LightningBolt.cpp
	
	class CParticleSystem * __thiscall CLevel::ParticleSystem(void)

	RedStarLevel.cpp / TurokLevel.cpp (Possibly the turok naming)
	
	unsigned long __thiscall CLevel::OriginatingLevelHashedId(void)
	BOOL __thiscall CLevel::IsPortalTarget(void)

	GameEvent.cpp

	static class CGameEvent * __cdecl CLevelInformationLinkEvent::Make(void)

	GameChannels.cpp

	class CBlurEffect * __thiscall CLevel::GetBlurEffect(void)

	LevelEntryTasks.cpp

	void __thiscall CLevelEntryTask::Execute(class CLevel &)
	void __thiscall CLevelEntryTasks::Execute(class CLevel &)

	LevelEntryTasks.cpp

	void __thiscall CLevelEntryTasks::Add(class CLevelEntryTask *)
	void __thiscall CLevelEntryTasks::Clear(void)

	ActorFlasher.cpp

	float __thiscall CLevel::LastUnadjustedUpdateTime(void)

	GameEvents.cpp

	virtual int __thiscall CLevelInformationLinkEvent::ParseProperty(class CTextFileParser &)
	virtual void __thiscall CLevelInformationLinkEvent::Apply(class CActor *,class CActor *,class CActor *)

	SpawnTrackingInfo.cpp

	class CNavData * __thiscall CLevel::NavigationData(void)

	Indicators.cpp

	class CActorRenderGrids const & __thiscall CLevel::RenderGrids(void)

	SpawnArena.cpp

	class CNavData const * __thiscall CLevel::NavigationData(void)const

	GameEvents.cpp

	static class CProperty const * const CLevelInformationLinkEvent::m_MemberProperties

	PredefinedLinkCauses.cpp

	static class CProperty const * const CLevelLinkCause::m_MemberProperties

	Level.cpp

	static class CProperty const * const CLevel::m_MemberProperties

	LevelProperties.cpp

	static class CProperty const * const CLevelProperties::m_MemberProperties

	LevelTimeWarp.cpp

	static class CProperty const * const CLevelTimeWarp::m_MemberProperties

	PredefinedLinkConditionals.cpp

	static class CProperty const * const CLevelLinkConditional::MemberProperties

	Level.cpp

	static unsigned long CLevel::m_LoadingCount

	GameEvent.cpp

	static class CGameEvent * __cdecl CLevelInformationLinkEvent::Make(void)

	CIL module:
	void __thiscall CLevel::AddExpensiveTask(class CSimpleTask *,int,class CActor *)
	void __thiscall CLevel::TriggerAllHistoryEvents(void)
	void __thiscall CLevel::BeginRender(class CCamera *)
	void __thiscall CLevel::StopCinema(char const *)
	void __thiscall CLevel::StopPerformanceGroup(char const *)
	void __thiscall CLevel::PlayPerformanceGroup(char const *)
	class CMicrophone * __thiscall CLevel::AddMenuMicrophone(void)
	class CCamera * __thiscall CLevel::AddMenuCamera(void)
	class CScreenFlash * __thiscall CLevel::ActivateScreenFlash(float,float,float,float,float,float,float,float,struct vector3 const &,class CCamera const *)
	void __thiscall CLevel::ActivateScreenShake(float,unsigned long,unsigned long,float,float,float,struct vector3 const &,float,float,float,unsigned long)
	void __thiscall CLevel::ParseNextActor(class CTextFileParser &,char const *,class CActor const *)
	void __thiscall CLevel::AddActorInstancesFromFile(class CTextFileParser &,class CActor const *)
	void __thiscall CLevel::Reset(void)
	void __thiscall CLevel::PlayCinema(char const *,class CActor *)
	virtual __thiscall CLevel::~CLevel(void)
	void __thiscall CLevel::AddActorInstances(char const *,class CActor const *)
	void __thiscall CLevel::Initialize(void)
	void __thiscall CLevel::LoadPreCacheFiles(void)
	class CLevel * __thiscall CGame::PushLevel(class CGame::CLevelCreationInfo const &)
	virtual void __thiscall CLevel::Reload(int)
*/