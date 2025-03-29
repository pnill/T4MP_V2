#pragma once

// network.h
#ifndef NETWORK_H
#define NETWORK_H

// Forward declarations to avoid including ENet in the header
typedef struct _ENetHost ENetHost;
typedef struct _ENetPeer ENetPeer;

#include <string>
#include <functional>
#include <memory>
#include <vector>

enum class PacketType {
	CONNECT,
	DISCONNECT,
	DATA,
	TIMEOUT
};

struct NetworkPacket {
	PacketType type;
	ENetPeer* peer;
	std::vector<uint8_t> data;
};

class Network {
public:
	Network();
	~Network();

	// Initialize ENet
	bool initialize();

	// Host a server on the specified port
	bool hostServer(uint16_t port, size_t maxClients = 32);

	// Connect to a server at the specified IP and port
	bool connectToServer(const std::string& ip, uint16_t port, uint32_t timeout_ms = 5000);

	// Disconnect from the server
	void disconnect();

	// Send data to a specific peer (or to server if client)
	bool sendData(const std::vector<uint8_t>& data, uint8_t channelId = 0, bool reliable = true, ENetPeer* peer = nullptr);

	// Send data to all connected peers (server only)
	bool broadcastData(const std::vector<uint8_t>& data, uint8_t channelId = 0, bool reliable = true);

	// Poll for new events
	std::vector<NetworkPacket> pollEvents(uint32_t timeout_ms = 0);

	// Check if connected to server (client) or if hosting (server)
	bool isConnected() const;
	bool isHosting() const;

	// Get list of connected peers (server only)
	std::vector<ENetPeer*> getConnectedPeers() const;

private:
	ENetHost* m_host;
	ENetPeer* m_serverPeer;  // Only used when client is connected to a server
	bool m_isServer;
	bool m_initialized;

	// Helper method to clean up resources
	void cleanup();
};

#endif // NETWORK_H