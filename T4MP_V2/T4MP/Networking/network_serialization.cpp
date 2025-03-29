// network_serialization.h/cpp
#include <vector>
#include "network_packets.h"
#include "network_serialization.h"
#include "../../TurokEngine/OldEngineStruct.h"

std::vector<uint8_t> NetworkSerializer::CreateHistoryUpdatePacket(uint8_t historyType, const uint8_t* data, size_t dataSize) {
		std::vector<uint8_t> packet(sizeof(PacketHeader) + 1 + dataSize);

		PacketHeader* header = reinterpret_cast<PacketHeader*>(packet.data());
		header->type = PacketMessageType::HISTORY_UPDATE;
		header->size = 1 + dataSize;  
		packet[sizeof(PacketHeader)] = historyType;

		memcpy(packet.data() + sizeof(PacketHeader) + 1, data, dataSize);

		return packet;
};

std::vector<uint8_t> NetworkSerializer::CreateNicknameUpdatePacket(const std::string& nickname) {
	// Calculate the size: header + nickname length (including null terminator)
	std::vector<uint8_t> packet(sizeof(PacketHeader) + nickname.length() + 1);

	// Set up the header
	PacketHeader* header = reinterpret_cast<PacketHeader*>(packet.data());
	header->type = PacketMessageType::NICKNAME_UPDATE;
	header->size = nickname.length() + 1;  // Include null terminator

										   // Copy the nickname data (including null terminator)
	memcpy(packet.data() + sizeof(PacketHeader), nickname.c_str(), nickname.length() + 1);

	return packet;
}

std::vector<uint8_t> NetworkSerializer::CreatePeerIDUpdatePacket(uint16_t peerID) {
	std::vector<uint8_t> packet(sizeof(PacketHeader) + 2);
	PacketHeader* header = reinterpret_cast<PacketHeader*>(packet.data());
	header->type = PacketMessageType::PEER_ID_UPDATE;
	header->size = 2;
	packet[sizeof(PacketHeader)] = peerID;
	
	return packet;
}

std::vector<uint8_t> NetworkSerializer::CreateGameStateUpdatePacket(uint8_t gameState) {
	std::vector<uint8_t> packet(sizeof(PacketHeader) + 1);
	PacketHeader* header = reinterpret_cast<PacketHeader*>(packet.data());
	header->type = PacketMessageType::GAME_STATE_UPDATE;
	header->size = 1;
	packet[sizeof(PacketHeader)] = gameState;

	return packet;
}

NetworkWeaponWheel CreateNetworkWeaponWheel(const WeaponWheel* playerWeaponWheel) {
	NetworkWeaponWheel networkWheel;

	// Initialize flags to 0
	networkWheel.weaponFlags = 0;

	// Set weapon availability flags
	if (playerWeaponWheel->bHas_WarClub || playerWeaponWheel->bHas_Bow2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_WAR_CLUB;

	if (playerWeaponWheel->bHas_Bow1 || playerWeaponWheel->bHas_Bow2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_BOW;

	if (playerWeaponWheel->bHas_TekBow || playerWeaponWheel->bHas_Tekbow2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_TEK_BOW;

	if (playerWeaponWheel->bHas_SpikeGrenade || playerWeaponWheel->bHas_SpikeGrenade2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_SPIKE_GRENADE;

	if (playerWeaponWheel->bHas_Pistol || playerWeaponWheel->bHas_Pistol2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_PISTOL;

	if (playerWeaponWheel->bHas_ShotGun || playerWeaponWheel->bHas_ShotGun2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_SHOTGUN;

	if (playerWeaponWheel->bHas_Fleechete || playerWeaponWheel->bHas_Fleechete2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_FLECHETTE;

	if (playerWeaponWheel->bHas_RocketLauncher || playerWeaponWheel->bHas_RocketLauncher2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_ROCKET_LAUNCHER;

	if (playerWeaponWheel->bHas_PlasmaCannon || playerWeaponWheel->bHas_PlasmaCannon2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_PLASMA_CANNON;

	if (playerWeaponWheel->bHas_FlameThrower || playerWeaponWheel->bHas_FlameThrower2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_FLAMETHROWER;

	if (playerWeaponWheel->bHas_SpiderMine || playerWeaponWheel->bHas_SpiderMine2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_SPIDER_MINE;

	if (playerWeaponWheel->bHas_GravityGun || playerWeaponWheel->bHas_GravityGun2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_GRAVITY_GUN;

	if (playerWeaponWheel->bHas_DarkMatterCube || playerWeaponWheel->bHas_DarkMatterCube2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_DARK_MATTER;

	if (playerWeaponWheel->bHas_CrossBow || playerWeaponWheel->bHas_CrossBow2)
		networkWheel.weaponFlags |= NetworkWeaponWheel::HAS_CROSSBOW;

	// Copy ammo values to the ammo array
	// The indices should match your internal ammo ordering
	int ammoIndex = 0;

	// Bow ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->Bow_Normal_Ammo;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->Bow_Explosive_Ammo;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->Bow_Poison_Ammo;

	// TekBow ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->Tekbow_Normal_Ammo;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->TekBow_Explosive_Ammo;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->TekBow_Poison_Ammo;

	// SpikeGrenade ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->SpikeGrenade_Ammo;

	// Pistol ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->Pistol_Ammo;

	// Shotgun ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->ShotGun_Ammo;

	// Flechette/MiniGun ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->Fleechete_Ammo;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->MiniGun_Ammo;

	// Rocket Launcher ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->RocketLauncher_Ammo;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->RocketLauncher_Ammo2;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->RocketLauncher_Ammo3;

	// Plasma Cannon ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->PlasmaCannon_Ammo;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->PlasmaCannon_Ammo2;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->PlasmaCannon_Ammo3;

	// Flamethrower ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->FlameThrower_Ammo;

	// Spider Mine ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->SpiderMine_Ammo;

	// Gravity Gun ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->GravityGun_Ammo;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->GravityGun_Ammo2;

	// Dark Matter Cube ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->DarkMatterCube_Ammo;

	// CrossBow ammo
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->CrossBow_Normal_Ammo;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->CrossBow_Explosive_Ammo;
	networkWheel.ammo[ammoIndex++] = playerWeaponWheel->CrossBow_Poison_Ammo;

	return networkWheel;
}


std::vector<uint8_t> NetworkSerializer::CreateClientSnapshotPacket(
	float x, float y, float z,
	float viewX, float viewY,
	bool walkForward, bool walkBackward, bool walkLeft, bool walkRight,
	bool jump, bool crouch, bool fire, bool fireHold, float fireHoldTime,
	bool fireRelease, float fireReleaseTime, int32_t weaponSwitch,
	float weaponSwitchTime, int32_t weaponSlot, bool modifyWeapon,uint16_t peerID, 
	const WeaponWheel* playerWeaponWheel, float curHealth, float maxHealth) {

	std::vector<uint8_t> packet(sizeof(ClientSnapshotPacket));
	ClientSnapshotPacket* snap = reinterpret_cast<ClientSnapshotPacket*>(packet.data());

	// Set header
	snap->header.type = PacketMessageType::CLIENT_SNAPSHOT;
	snap->header.size = sizeof(ClientSnapshotPacket) - sizeof(PacketHeader);

	// Set position and view
	snap->snapshot.x = x;
	snap->snapshot.y = y;
	snap->snapshot.z = z;
	snap->snapshot.viewX = viewX;
	snap->snapshot.viewY = viewY;

	// Initialize flags to 0
	snap->snapshot.flags = 0;

	// Set flags based on boolean values
	if (walkForward) snap->snapshot.flags |= PlayerSnapshot::WALK_FORWARD;
	if (walkBackward) snap->snapshot.flags |= PlayerSnapshot::WALK_BACKWARD;
	if (walkLeft) snap->snapshot.flags |= PlayerSnapshot::WALK_LEFT;
	if (walkRight) snap->snapshot.flags |= PlayerSnapshot::WALK_RIGHT;
	if (jump) snap->snapshot.flags |= PlayerSnapshot::JUMP;
	if (crouch) snap->snapshot.flags |= PlayerSnapshot::CROUCH;
	if (fire) snap->snapshot.flags |= PlayerSnapshot::FIRE;
	if (fireHold) snap->snapshot.flags |= PlayerSnapshot::FIRE_HOLD;
	if (fireRelease) snap->snapshot.flags |= PlayerSnapshot::FIRE_RELEASE;
	if (modifyWeapon) snap->snapshot.flags |= PlayerSnapshot::MODIFY_WEAPON;

	// Set other values
	snap->snapshot.fireHoldTime = fireHoldTime;
	snap->snapshot.fireReleaseTime = fireReleaseTime;
	snap->snapshot.weaponSwitch = weaponSwitch;
	snap->snapshot.weaponSwitchTime = weaponSwitchTime;
	snap->snapshot.weaponSlot = weaponSlot;
	
	snap->snapshot.peerID = peerID;
	if (playerWeaponWheel)
	{
		snap->snapshot.weaponWheel = CreateNetworkWeaponWheel(playerWeaponWheel);
	}

	snap->snapshot.currentHealth = curHealth;
	snap->snapshot.maxHealth = maxHealth;

	return packet;
}