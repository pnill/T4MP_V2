#pragma once
#include <vector>
#include "../../TurokEngine/OldEngineStruct.h"

class NetworkSerializer {
public:
	static std::vector<uint8_t> CreateNicknameUpdatePacket(const std::string& nickname);
	static std::vector<uint8_t> CreatePeerIDUpdatePacket(uint16_t peerID);
	static std::vector<uint8_t> CreateHistoryUpdatePacket(uint8_t historyType, const uint8_t* data, size_t dataSize);
	static std::vector<uint8_t> CreateGameStateUpdatePacket(uint8_t gameState);
	static std::vector<uint8_t> CreateClientSnapshotPacket(
		float x, float y, float z,
		float viewX, float viewY,
		bool walkForward, bool walkBackward, bool walkLeft, bool walkRight,
		bool jump, bool crouch, bool fire, bool fireHold, float fireHoldTime,
		bool fireRelease, float fireReleaseTime, int32_t weaponSwitch,
		float weaponSwitchTime, int32_t weaponSlot, bool modifyWeapon,
		uint16_t peerID, const WeaponWheel* playerWeaponWheel,
		float currHealth, float maxHealth);
};