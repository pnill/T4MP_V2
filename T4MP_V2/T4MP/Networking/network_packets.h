#pragma once
#include <stdint.h>

enum class PacketMessageType : uint8_t {
	NICKNAME_UPDATE = 1,
	HISTORY_UPDATE = 2,
	GAME_STATE_UPDATE = 3,
	SERVER_SNAPSHOT = 4,
	CLIENT_SNAPSHOT = 5,
	PEER_ID_UPDATE = 6
};

enum class GameStateUpdateStates : uint8_t {
	MULTIPLAYER_JOIN = 1,
	PAUSEGAME = 2,
	RESUME_GAME = 3,
	QUIT_GAME = 4
};

// Base packet header structure
#pragma pack(push, 1)  // Ensure consistent packing
struct PacketHeader {
	PacketMessageType type;
	uint32_t size;  // Size of the payload data
};

// History update packet
struct HistoryUpdatePacket {
	PacketHeader header;
	uint8_t historyType;  // 0 = Multiplayer, 1 = MultiAdvancedOptions, etc.
	uint8_t data[1];  // Variable length array, will be sized appropriately
};


struct NetworkWeaponWheel {
	uint32_t weaponFlags;

	enum WeaponBits {
		HAS_WAR_CLUB = 1 << 0,
		HAS_BOW = 1 << 1,
		HAS_TEK_BOW = 1 << 2,
		HAS_SPIKE_GRENADE = 1 << 3,
		HAS_PISTOL = 1 << 4,
		HAS_SHOTGUN = 1 << 5,
		HAS_FLECHETTE = 1 << 6,
		HAS_ROCKET_LAUNCHER = 1 << 7,
		HAS_PLASMA_CANNON = 1 << 8,
		HAS_FLAMETHROWER = 1 << 9,
		HAS_SPIDER_MINE = 1 << 10,
		HAS_GRAVITY_GUN = 1 << 11,
		HAS_DARK_MATTER = 1 << 12,
		HAS_CROSSBOW = 1 << 13
	};

	int16_t ammo[32];
	uint8_t currentWeaponTypes[14];
};

struct PlayerSnapshot {
	float x, y, z;
	float viewX, viewY;
	
	uint16_t flags; 

	/*
		Temporarily allow a player to control their own health
	*/
	float currentHealth;
	float maxHealth;
	int deathMode;

	enum FlagBits {
		WALK_FORWARD = 1 << 0,
		WALK_BACKWARD = 1 << 1,
		WALK_LEFT = 1 << 2,
		WALK_RIGHT = 1 << 3,
		JUMP = 1 << 4,
		CROUCH = 1 << 5,
		FIRE = 1 << 6,
		FIRE_HOLD = 1 << 7,
		FIRE_RELEASE = 1 << 8,
		MODIFY_WEAPON = 1 << 9
	};

	float fireHoldTime;
	float fireReleaseTime;
	int32_t weaponSwitch;
	float weaponSwitchTime;
	int32_t weaponSlot;
	uint16_t peerID;
	NetworkWeaponWheel weaponWheel;
};

struct ClientSnapshotPacket {
	PacketHeader header;
	PlayerSnapshot snapshot;
};

struct ServerPlayerData {
	PlayerSnapshot playerState;  

	float maxHealth;
	float currentHealth;
	NetworkWeaponWheel weapons;
	int32_t m_DMPlayerIdx;
	int32_t deathType;
};

struct ServerSnapshotPacket {
	PacketHeader header;
	uint8_t playerCount;
	// Variable-length array of player data follows
	// ServerPlayerData players[playerCount];
};

#pragma pack(pop)