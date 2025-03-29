#pragma once
#include <unordered_map>
#include "../../TurokEngine/OldEngineStruct.h"
#include "enet_wrapper.h"

class LocalPlayer {
public:
	bool isCrouching = false;
	bool isJumping = false;
	bool isFiring = false;
	bool isHoldingFire = false;
	float fireHoldingTime = 0.0f;
	bool isModifyingWeapon = false;
	bool isReleasingFire = false;
	float fireReleaseTime = 0.0f;
	int deathIdx;
	uint16_t peerID;
};

extern LocalPlayer g_LocalPlayer;

class NetworkPlayer {
	public:
		LocalPlayer lPlayer;
		DMPlayer* dmPlayer;
		ENetPeer* enetPeeer;
		int m_DMPlayerIdx;
};

