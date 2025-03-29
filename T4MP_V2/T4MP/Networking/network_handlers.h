#pragma once
#include <vector>
#include "enet_wrapper.h"
class NetworkHandlers {
public:
	static void Update();
	static void	HandleNicknameUpdate(const std::vector<uint8_t>& data, ENetPeer* peer);
	static void HandleGameStateUpdate(const std::vector<uint8_t>& data);
	static void HandleHistoryUpdate(const std::vector<uint8_t>& data);
	static void HandleClientSnapshot(const std::vector<uint8_t>& data, ENetPeer* peer);
	static void HandlePeerUpdatePacket(const std::vector<uint8_t>& data);
};