#include <vector>
#include <algorithm>
#include "../../TurokEngine/History.h"
#include "../../TurokEngine/Game.h"
#include "../../TurokEngine/DMPlayer.h"
#include "../../TurokEngine/OldEngineStruct.h"
#include "../../GUI/imgui_debugconsole.h"
#include "../T4MP_Engine.h"
#include "enet_wrapper.h"
#include "network_player.h"
#include "network_handlers.h"
#include "network_packets.h"
#include "network_serialization.h"
#define NOMINMAX
#include <enet/enet.h>

//T4Engine * g_Game = (T4Engine*)0x6B52E4;
extern T4Engine* g_Game;
LocalPlayer g_LocalPlayer;

int g_gameState = -1;

extern Network g_Network;
extern T4MPConsole g_Console;
extern void DrawConnectionNotification(const char* username, bool disconnect = false, float duration = 5.0f);
std::unordered_map<uint16_t, NetworkPlayer*> netPlayers;


void ResetLocalPlayer()
{
	g_LocalPlayer.isJumping = false;
	g_LocalPlayer.isCrouching = false;
	g_LocalPlayer.isFiring = false;
	g_LocalPlayer.isHoldingFire = false;
	g_LocalPlayer.isModifyingWeapon = false;
	g_LocalPlayer.isReleasingFire = false;
	g_LocalPlayer.isFiring = false;
	g_LocalPlayer.fireReleaseTime = 0.0f;
	g_LocalPlayer.fireHoldingTime = 0.0f;
}

void NetworkHandlers::Update() {

	/*
		Update Player Data
	*/
	if (!g_Network.isHosting() && g_Network.isConnected())
	{
		if(T4MP::GetLocalPlayer())
		{ 

			DMPlayer* player = T4MP::GetDMPlayer(0);
			std::vector<uint8_t> packet = NetworkSerializer::CreateClientSnapshotPacket(
					player->POS.x,
					player->POS.y,
					player->POS.z,
					player->ViewX,
					player->ViewY,
					player->Walk_forward,
					player->Walk_backward,
					player->Walk_left,
					player->Walk_right,
					g_LocalPlayer.isJumping,
					g_LocalPlayer.isCrouching,
					g_LocalPlayer.isFiring,
					g_LocalPlayer.isHoldingFire,
					g_LocalPlayer.fireHoldingTime,
					g_LocalPlayer.isReleasingFire,
					g_LocalPlayer.fireReleaseTime,
					player->Weapon_switch,
					player->Weapon_switch_time,
					player->Weapon_slot,
					g_LocalPlayer.isModifyingWeapon,
					g_LocalPlayer.peerID,
					player->pWeaponWheel,
					player->pHealth->Current,
					player->pHealth->Max);

				g_Console.AddLog("[Network Client Snapshot] player->Weapon_slot: %i, player->Weapon_switch: %i", player->Weapon_slot, player->Weapon_switch);

				g_Network.sendData(packet, 1, true);
				ResetLocalPlayer();
		}
	}
	else
	{
		// host update everyone
		if (T4MP::GetLocalPlayer())
		{
			DMPlayer* player = T4MP::GetDMPlayer(0);

			std::vector<uint8_t> packet = NetworkSerializer::CreateClientSnapshotPacket(
				player->POS.x,
				player->POS.y,
				player->POS.z,
				player->ViewX,
				player->ViewY,
				player->Walk_forward,
				player->Walk_backward,
				player->Walk_left,
				player->Walk_right,
				g_LocalPlayer.isJumping,
				g_LocalPlayer.isCrouching,
				g_LocalPlayer.isFiring,
				g_LocalPlayer.isHoldingFire,
				g_LocalPlayer.fireHoldingTime,
				g_LocalPlayer.isReleasingFire,
				g_LocalPlayer.fireReleaseTime,
				player->Weapon_switch,
				player->Weapon_switch_time,
				player->Weapon_slot,
				g_LocalPlayer.isModifyingWeapon,
				999,
				player->pWeaponWheel,
				player->pHealth->Current,
				player->pHealth->Max);
			
			g_Network.broadcastData(packet, 1, true);
			ResetLocalPlayer();
		}
	}


	std::vector<NetworkPacket> packets = g_Network.pollEvents(0);
	
	for (const auto& packet : packets) {
		if (packet.type == PacketType::DATA && packet.data.size() >= sizeof(PacketHeader)) {
			const PacketHeader* header = reinterpret_cast<const PacketHeader*>(packet.data.data());

			switch (header->type) {
			case PacketMessageType::HISTORY_UPDATE:
				//g_Console.AddLog("[Networking] - NetworkHandlers::Update() received packet HISTORY_UPDATE");
				NetworkHandlers::HandleHistoryUpdate(packet.data);
				break;
			case PacketMessageType::NICKNAME_UPDATE:
				g_Console.AddLog("[Networking] - NetworkHnadlers::Update() received packet NICKNAME_UPDATE");
				HandleNicknameUpdate(packet.data, packet.peer);
				break;
			case PacketMessageType::GAME_STATE_UPDATE:
				HandleGameStateUpdate(packet.data);
				break;
			case PacketMessageType::CLIENT_SNAPSHOT:
				HandleClientSnapshot(packet.data, packet.peer);
				break;
			case PacketMessageType::PEER_ID_UPDATE:
				HandlePeerUpdatePacket(packet.data);
				break;
			}
		}
		else if (packet.type == PacketType::CONNECT) {
	
			g_Console.AddLog("[Networking] - NetworkHandlers::Update() received PacketType::CONNECT");
			if (g_Network.isHosting())
			{
				uint16_t peerID = packet.peer->incomingPeerID;
				std::vector<uint8_t> peerUpdatePacket = NetworkSerializer::CreatePeerIDUpdatePacket(peerID);
				if (!g_Network.sendData(peerUpdatePacket, 0, true, packet.peer))
				{
					g_Console.AddLog("[Networking] Sending peerID failed!?, peerID: %i", peerID);
				}

				g_Console.AddLog("[Networking] Sent peer back their peerID: %i", peerID);

				if (g_gameState != -1)
				{

					// They wouldn't have received our history yet, if we're already in-game and they're connecting.
					std::vector<uint8_t> HistoryUpdatePacket = NetworkSerializer::CreateHistoryUpdatePacket(
						0,
						(uint8_t*)CHistories::multiplayer_options,
						sizeof(CHistories::multiplayer_options)
						);
					if (!g_Network.sendData(HistoryUpdatePacket, 0, true, packet.peer))
					{
						g_Console.AddLog("[Networking] Could not send history update packet. ");
					}

					std::vector<uint8_t> HistoryUpdatePacket2 = NetworkSerializer::CreateHistoryUpdatePacket(
						1,
						(uint8_t*)CHistories::multiadvanced_options,
						sizeof(CHistories::multiadvanced_options)
						);
					if (!g_Network.sendData(HistoryUpdatePacket2, 0, true, packet.peer)); 
					{
						g_Console.AddLog("[Networking] Could not send history update packet. ");

					}

					// We need to make sure they receive whatever state we've set incase they're joining late.
					//std::vector<uint8_t> packet = NetworkSerializer::CreateGameStateUpdatePacket((uint8_t)g_gameState);
					// Networking doesn't support sending to individual peers yet from server, only broadcast.
					//g_Network.sendData(packet, 0, true);
					g_Console.AddLog("[Networking] Peer connected and our g_gameState != -1 - sending updated g_gameState = %i",g_gameState);
					std::vector<uint8_t> gameStatePacket = NetworkSerializer::CreateGameStateUpdatePacket((uint8_t)g_gameState);
					if (!g_Network.sendData(gameStatePacket, 0, true, packet.peer))
					{
						g_Console.AddLog("[Networking] Could not send gamestate update packet.");
					};
				}
				else
				{
					g_Console.AddLog("[Networking] Peer connected but our g_GameState == -1");
				}
			}

	
	
		}
		else if (packet.type == PacketType::DISCONNECT) {
			g_Console.AddLog("[Networking] - NetworkHandlers::Update() received PacketType::DISCONNECT");
			// Handle disconnection
		}
	}

}

void NetworkHandlers::HandlePeerUpdatePacket(const std::vector<uint8_t>& data) {
	if (data.size() < sizeof(PacketHeader)) return;

	const PacketHeader* header = reinterpret_cast<const PacketHeader*>(data.data());
	uint16_t peerID = data[sizeof(PacketHeader)];

	if (peerID != 0)
	{
		g_Console.AddLog("[Networking] Updated local peerID: %i", peerID);
		g_LocalPlayer.peerID = peerID;
	}
	else {
		g_Console.AddLog("[Networking] PeerID 0!");
	}

}

void NetworkHandlers::HandleNicknameUpdate(const std::vector<uint8_t>& data, ENetPeer* peer) {
	if (data.size() < sizeof(PacketHeader)) return;

	const PacketHeader* header = reinterpret_cast<const PacketHeader*>(data.data());

	// Extract the nickname string
	std::string nickname(
		reinterpret_cast<const char*>(data.data() + sizeof(PacketHeader)),
		header->size
		);

	// Store the nickname associated with this peer
	// You'll need some data structure to associate peers with nicknames
	// For example:
	if (peer != nullptr) {
		// You could store it in a map
		//peerNicknames[peer->incomingPeerID] = nickname;

		g_Console.AddLog("[Networking] Player connected with nickname: %s peerID: %i", nickname.c_str(),peer->incomingPeerID);
		DrawConnectionNotification(nickname.c_str());
	}
}

void NetworkHandlers::HandleGameStateUpdate(const std::vector<uint8_t>& data) {
	if (data.size() < sizeof(PacketHeader) + 1) return;

	const PacketHeader* header = reinterpret_cast<const PacketHeader*>(data.data());
	uint8_t gameState = data[sizeof(PacketHeader)];
	
	switch (gameState)
	{
		case 1:
			CGame::ForceToMultiplayerJoin();
		break;
	}
}



void NetworkHandlers::HandleHistoryUpdate(const std::vector<uint8_t>& data) {
		if (data.size() < sizeof(PacketHeader) + 1) return;

		const PacketHeader* header = reinterpret_cast<const PacketHeader*>(data.data());
		uint8_t historyType = data[sizeof(PacketHeader)];
		const uint8_t* historyData = data.data() + sizeof(PacketHeader) + 1;
		size_t historyDataSize = header->size - 1;

		switch (historyType) {
		case 0: // Multiplayer.hst
			memcpy(CHistories::multiplayer_options, historyData, std::min(historyDataSize, sizeof(CHistories::multiplayer_options)));
			break;
		case 1: // MultiAdvancedOptions.hst
			memcpy(CHistories::multiadvanced_options, historyData, std::min(historyDataSize, sizeof(CHistories::multiadvanced_options)));
			break;
			// Add more history types as needed
		}
}

void ApplyNetworkWeaponWheel(WeaponWheel* targetWheel, const NetworkWeaponWheel& networkWheel) {
	// Set weapon availability based on flags
	targetWheel->bHas_WarClub = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_WAR_CLUB) ? 1 : 0;
	targetWheel->bHas_WarClub2 = targetWheel->bHas_WarClub;  // Set duplicate flags

	targetWheel->bHas_Bow1 = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_BOW) ? 1 : 0;
	targetWheel->bHas_Bow2 = targetWheel->bHas_Bow1;

	targetWheel->bHas_TekBow = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_TEK_BOW) ? 1 : 0;
	targetWheel->bHas_Tekbow2 = targetWheel->bHas_TekBow;

	targetWheel->bHas_SpikeGrenade = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_SPIKE_GRENADE) ? 1 : 0;
	targetWheel->bHas_SpikeGrenade2 = targetWheel->bHas_SpikeGrenade;

	targetWheel->bHas_Pistol = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_PISTOL) ? 1 : 0;
	targetWheel->bHas_Pistol2 = targetWheel->bHas_Pistol;

	targetWheel->bHas_ShotGun = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_SHOTGUN) ? 1 : 0;
	targetWheel->bHas_ShotGun2 = targetWheel->bHas_ShotGun;

	targetWheel->bHas_Fleechete = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_FLECHETTE) ? 1 : 0;
	targetWheel->bHas_Fleechete2 = targetWheel->bHas_Fleechete;

	targetWheel->bHas_RocketLauncher = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_ROCKET_LAUNCHER) ? 1 : 0;
	targetWheel->bHas_RocketLauncher2 = targetWheel->bHas_RocketLauncher;

	targetWheel->bHas_PlasmaCannon = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_PLASMA_CANNON) ? 1 : 0;
	targetWheel->bHas_PlasmaCannon2 = targetWheel->bHas_PlasmaCannon;

	targetWheel->bHas_FlameThrower = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_FLAMETHROWER) ? 1 : 0;
	targetWheel->bHas_FlameThrower2 = targetWheel->bHas_FlameThrower;

	targetWheel->bHas_SpiderMine = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_SPIDER_MINE) ? 1 : 0;
	targetWheel->bHas_SpiderMine2 = targetWheel->bHas_SpiderMine;

	targetWheel->bHas_GravityGun = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_GRAVITY_GUN) ? 1 : 0;
	targetWheel->bHas_GravityGun2 = targetWheel->bHas_GravityGun;

	targetWheel->bHas_DarkMatterCube = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_DARK_MATTER) ? 1 : 0;
	targetWheel->bHas_DarkMatterCube2 = targetWheel->bHas_DarkMatterCube;

	targetWheel->bHas_CrossBow = (networkWheel.weaponFlags & NetworkWeaponWheel::HAS_CROSSBOW) ? 1 : 0;
	targetWheel->bHas_CrossBow2 = targetWheel->bHas_CrossBow;

	// Copy ammo values from the ammo array
	int ammoIndex = 0;

	// Bow ammo
	targetWheel->Bow_Normal_Ammo = networkWheel.ammo[ammoIndex++];
	targetWheel->Bow_Explosive_Ammo = networkWheel.ammo[ammoIndex++];
	targetWheel->Bow_Poison_Ammo = networkWheel.ammo[ammoIndex++];

	// TekBow ammo
	targetWheel->Tekbow_Normal_Ammo = networkWheel.ammo[ammoIndex++];
	targetWheel->TekBow_Explosive_Ammo = networkWheel.ammo[ammoIndex++];
	targetWheel->TekBow_Poison_Ammo = networkWheel.ammo[ammoIndex++];

	// SpikeGrenade ammo
	targetWheel->SpikeGrenade_Ammo = networkWheel.ammo[ammoIndex++];

	// Pistol ammo
	targetWheel->Pistol_Ammo = networkWheel.ammo[ammoIndex++];

	// Shotgun ammo
	targetWheel->ShotGun_Ammo = networkWheel.ammo[ammoIndex++];

	// Flechette/MiniGun ammo
	targetWheel->Fleechete_Ammo = networkWheel.ammo[ammoIndex++];
	targetWheel->MiniGun_Ammo = networkWheel.ammo[ammoIndex++];

	// Rocket Launcher ammo
	targetWheel->RocketLauncher_Ammo = networkWheel.ammo[ammoIndex++];
	targetWheel->RocketLauncher_Ammo2 = networkWheel.ammo[ammoIndex++];
	targetWheel->RocketLauncher_Ammo3 = networkWheel.ammo[ammoIndex++];

	// Plasma Cannon ammo
	targetWheel->PlasmaCannon_Ammo = networkWheel.ammo[ammoIndex++];
	targetWheel->PlasmaCannon_Ammo2 = networkWheel.ammo[ammoIndex++];
	targetWheel->PlasmaCannon_Ammo3 = networkWheel.ammo[ammoIndex++];

	// Flamethrower ammo
	targetWheel->FlameThrower_Ammo = networkWheel.ammo[ammoIndex++];

	// Spider Mine ammo
	targetWheel->SpiderMine_Ammo = networkWheel.ammo[ammoIndex++];

	// Gravity Gun ammo
	targetWheel->GravityGun_Ammo = networkWheel.ammo[ammoIndex++];
	targetWheel->GravityGun_Ammo2 = networkWheel.ammo[ammoIndex++];

	// Dark Matter Cube ammo
	targetWheel->DarkMatterCube_Ammo = networkWheel.ammo[ammoIndex++];

	// CrossBow ammo
	targetWheel->CrossBow_Normal_Ammo = networkWheel.ammo[ammoIndex++];
	targetWheel->CrossBow_Explosive_Ammo = networkWheel.ammo[ammoIndex++];
	targetWheel->CrossBow_Poison_Ammo = networkWheel.ammo[ammoIndex++];

	// Set the "has ammo" flags based on ammo counts
	targetWheel->bBow_Has_Normal = (targetWheel->Bow_Normal_Ammo > 0) ? 1 : 0;
	targetWheel->bBow_Has_Explosive = (targetWheel->Bow_Explosive_Ammo > 0) ? 1 : 0;
	targetWheel->bBow_Has_Poison = (targetWheel->Bow_Poison_Ammo > 0) ? 1 : 0;

	targetWheel->bTekbow_Has_Normal = (targetWheel->Tekbow_Normal_Ammo > 0) ? 1 : 0;
	targetWheel->bTekBow_Has_Explosive = (targetWheel->TekBow_Explosive_Ammo > 0) ? 1 : 0;
	targetWheel->bTekBow_Has_Poison = (targetWheel->TekBow_Poison_Ammo > 0) ? 1 : 0;

	targetWheel->bSpikeGrenade_Has_Ammo = (targetWheel->SpikeGrenade_Ammo > 0) ? 1 : 0;

	targetWheel->bPistol_Has_Ammo = (targetWheel->Pistol_Ammo > 0) ? 1 : 0;

	targetWheel->bShotGun_Has_Ammo1 = (targetWheel->ShotGun_Ammo > 0) ? 1 : 0;

	targetWheel->bFleechete_Has_Ammo = (targetWheel->Fleechete_Ammo > 0) ? 1 : 0;
	targetWheel->bMiniGun_Has_Ammo = (targetWheel->MiniGun_Ammo > 0) ? 1 : 0;

	targetWheel->bRocketLauncher_Has_Ammo = (targetWheel->RocketLauncher_Ammo > 0) ? 1 : 0;
	targetWheel->bRocketLauncher_Has_Ammo1 = (targetWheel->RocketLauncher_Ammo2 > 0) ? 1 : 0;
	targetWheel->bRocketLauncher_Has_Ammo2 = (targetWheel->RocketLauncher_Ammo3 > 0) ? 1 : 0;

	targetWheel->bPlasmaCannon_Has_Ammo = (targetWheel->PlasmaCannon_Ammo > 0) ? 1 : 0;
	targetWheel->bPlasmaCannon_Has_Ammo2 = (targetWheel->PlasmaCannon_Ammo2 > 0) ? 1 : 0;
	targetWheel->bPlasmaCannon_Has_Ammo3 = (targetWheel->PlasmaCannon_Ammo3 > 0) ? 1 : 0;

	targetWheel->bFlameThrower_Has_Ammo = (targetWheel->FlameThrower_Ammo > 0) ? 1 : 0;

	targetWheel->bSpiderMine_Has_Ammo = (targetWheel->SpiderMine_Ammo > 0) ? 1 : 0;

	targetWheel->bGravityGun_Has_Ammo = (targetWheel->GravityGun_Ammo > 0) ? 1 : 0;
	targetWheel->bGravityGun_Has_Ammo2 = (targetWheel->GravityGun_Ammo2 > 0) ? 1 : 0;

	targetWheel->bDarkMatterCube_Has_Ammo = (targetWheel->DarkMatterCube_Ammo > 0) ? 1 : 0;

	targetWheel->bCrossBow_Has_Normal = (targetWheel->CrossBow_Normal_Ammo > 0) ? 1 : 0;
	targetWheel->bCrossBow_Has_Explosive = (targetWheel->CrossBow_Explosive_Ammo > 0) ? 1 : 0;
	targetWheel->bCrossBow_Has_Poison = (targetWheel->CrossBow_Poison_Ammo > 0) ? 1 : 0;

}

void ApplyPlayerUpdates(DMPlayer* netPlayer,const ClientSnapshotPacket* packet)
{
	netPlayer->POS.x = packet->snapshot.x;
	netPlayer->POS.y = packet->snapshot.y;
	netPlayer->POS.z = packet->snapshot.z;
	netPlayer->ViewX = packet->snapshot.viewX;
	netPlayer->ViewY = packet->snapshot.viewY;
	netPlayer->pHealth->Current = packet->snapshot.currentHealth;
	netPlayer->pHealth->Max = packet->snapshot.maxHealth;

	// Let the game update/assign this.
	//netPlayer->Weapon_slot = packet->snapshot.weaponSlot;
	//netPlayer->Weapon_switch = packet->snapshot.weaponSwitch;
	//netPlayer->Weapon_switch_time = packet->snapshot.weaponSwitchTime;

	netPlayer->Walk_backward = (packet->snapshot.flags & PlayerSnapshot::WALK_BACKWARD) != 0;
	netPlayer->Walk_forward = (packet->snapshot.flags & PlayerSnapshot::WALK_FORWARD) != 0;
	netPlayer->Walk_left = (packet->snapshot.flags & PlayerSnapshot::WALK_LEFT) != 0;
	netPlayer->Walk_right = (packet->snapshot.flags & PlayerSnapshot::WALK_RIGHT) != 0;

	ApplyNetworkWeaponWheel(netPlayer->pWeaponWheel, packet->snapshot.weaponWheel);


	// Only do this after the weapon wheel has been updated
	if (netPlayer->Weapon_slot != packet->snapshot.weaponSlot)
	{
		if (DMPlayer_::oIsWeaponValid((void*)&netPlayer->pWeapon, 0, (void*)(packet->snapshot.weaponSlot + 1), 0))
		{
			DMPlayer_::oSwitchWeapon((void*)&netPlayer->pWeapon, 0, (void*)(packet->snapshot.weaponSlot + 1), 0);
		}
	}

	if ((packet->snapshot.flags & PlayerSnapshot::MODIFY_WEAPON))
		DMPlayer_::oModifyWeapon(netPlayer, 0, 0, 0);
	if ((packet->snapshot.flags & PlayerSnapshot::FIRE) != 0)
	{
		g_Console.AddLog("[Networking] Player firing view X: %f, view Y: %f", netPlayer->ViewX, netPlayer->ViewY);
		DMPlayer_::oFireWeapon(netPlayer, 0, 0, 0);
	}
	if ((packet->snapshot.flags & PlayerSnapshot::FIRE_RELEASE) != 0)
		DMPlayer_::oReleaseFire(netPlayer, 0, packet->snapshot.fireReleaseTime);
	if ((packet->snapshot.flags & PlayerSnapshot::FIRE_HOLD) != 0)
		DMPlayer_::oHoldFire(netPlayer, 0, packet->snapshot.fireHoldTime, 0);
	
}

void NetworkHandlers::HandleClientSnapshot(const std::vector<uint8_t>& data, ENetPeer* peer) {
	if (data.size() < sizeof(ClientSnapshotPacket)) return;
	g_Console.AddLog("[Networking] HandleClientSnapshot ");
	const ClientSnapshotPacket* packet = reinterpret_cast<const ClientSnapshotPacket*>(data.data());
	if (g_Network.isHosting())
	{
		// Re-broadcast the snapshot to everyone connected.

		g_Network.broadcastData(data, 1, true);
		if (netPlayers[peer->incomingPeerID])
		{
			// handle updating accordingly.
			DMPlayer* netPlayer = netPlayers[peer->incomingPeerID]->dmPlayer;
			ApplyPlayerUpdates(netPlayer,packet);
		}
		else
		{
			// Basically using as an in-game check.
			if (T4MP::GetLocalPlayer())
			{
				g_Console.AddLog("[Networking] Got first snapshot for player spawning their object accordingly peerID: %i.", peer->incomingPeerID);
				NetworkPlayer *nPlayer = new NetworkPlayer();
				nPlayer->dmPlayer = T4MP::SpawnPlayer();
				netPlayers[peer->incomingPeerID] = nPlayer;
			}
		}
	}
	else
	{
		if (packet->snapshot.peerID != g_LocalPlayer.peerID)
		{
			if (netPlayers[packet->snapshot.peerID])
			{
				DMPlayer* netPlayer = netPlayers[packet->snapshot.peerID]->dmPlayer;
				if (!netPlayer)
				{
					g_Console.AddLog("[Networking] !WARNING! - SOMETHING IS SERIOUSLY WRONG THE PEERID HAS no DMPlayer object!");
					return;
				}

				ApplyPlayerUpdates(netPlayer,packet);
			}
			else
			{
				if (T4MP::GetLocalPlayer())
				{
					g_Console.AddLog("[Networking] Got first snapshot for player spawning their object accordingly peerID: %i.", packet->snapshot.peerID);
					NetworkPlayer *nPlayer = new NetworkPlayer();
					nPlayer->dmPlayer = T4MP::SpawnPlayer();
					netPlayers[packet->snapshot.peerID] = nPlayer;
				}
			}
		}
	}

}