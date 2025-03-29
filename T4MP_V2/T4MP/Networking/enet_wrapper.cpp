#include <iostream>
#include <enet/enet.h>
#include "network_packets.h"
#include "enet_wrapper.h"
#include "../../GUI/imgui_debugconsole.h"

bool g_IsHost = true;
Network g_Network;

Network::Network() : m_host(nullptr), m_serverPeer(nullptr), m_isServer(false), m_initialized(false) {
}

Network::~Network() {
	cleanup();
}

bool Network::initialize() {
	if (m_initialized) {
		return true;
	}

	if (enet_initialize() != 0) {
		std::cerr << "Failed to initialize ENet." << std::endl;
		return false;
	}

	m_initialized = true;
	
	return true;
}

bool Network::hostServer(uint16_t port, size_t maxClients) {
	g_Console.AddLog("[Networking] hostServer port :%i, maxClients: %i", port, maxClients);
	if (!m_initialized) {
		if (!initialize()) {
			g_Console.AddLog("[Networking] Networking was not intitalized aborting!");
			return false;
		}
	}

	// Clean up any existing resources
	cleanup();

	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;

	m_host = enet_host_create(&address, maxClients, 2, 0, 0);

	if (m_host == nullptr) {
		g_Console.AddLog("[Networking] Failed to create ENet server host.");
		return false;
	}

	g_Console.AddLog("[Networking] Hosting succesfully.");
	m_isServer = true;
	g_IsHost = true;
	return true;
}

bool Network::connectToServer(const std::string& ip, uint16_t port, uint32_t timeout_ms) {
	g_Console.AddLog("[Networking] connectToServer - ip: %s, port: %i, time_out: %i", ip.c_str(), port, timeout_ms);

	if (!m_initialized) {
		if (!initialize()) {
			g_Console.AddLog("[Networking] [ERROR] - Networking is not initialized!\n");
			return false;
		}
	}

	// Clean up any existing resources
	cleanup();

	m_host = enet_host_create(nullptr, 1, 2, 0, 0);

	if (m_host == nullptr) {
		g_Console.AddLog("[Networking] Failed to create ENet client host.");
		return false;
	}

	ENetAddress address;
	enet_address_set_host(&address, ip.c_str());
	address.port = port;

	m_serverPeer = enet_host_connect(m_host, &address, 2, 0);

	if (m_serverPeer == nullptr) {
		g_Console.AddLog("[Networking] Failed to initiate ENet connection.");
		enet_host_destroy(m_host);
		m_host = nullptr;
		return false;
	}

	// Wait for the connection to complete or timeout
	ENetEvent event;
	if (enet_host_service(m_host, &event, timeout_ms) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
		g_Console.AddLog("[Networking] Succesfully connected to server.");
		m_isServer = false;
		g_IsHost = false;
		return true;
	}
	else {
		enet_peer_reset(m_serverPeer);
		m_serverPeer = nullptr;
		g_Console.AddLog("[Networking] Connection to server timed out.");
		return false;
	}
}

void Network::disconnect() {
	if (m_host != nullptr) {
		if (!m_isServer && m_serverPeer != nullptr) {
			enet_peer_disconnect(m_serverPeer, 0);

			// Wait for the disconnect event
			ENetEvent event;
			while (enet_host_service(m_host, &event, 3000) > 0) {
				if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
					break;
				}

				if (event.type == ENET_EVENT_TYPE_RECEIVE) {
					enet_packet_destroy(event.packet);
				}
			}

			// Force disconnect if timeout
			enet_peer_reset(m_serverPeer);
			m_serverPeer = nullptr;
		}

		cleanup();
	}
}

bool Network::sendData(const std::vector<uint8_t>& data, uint8_t channelId, bool reliable, ENetPeer* peer) {
	if (m_host == nullptr) {
		return false;
	}

	// If client, send to server
	if (!m_isServer && m_serverPeer != nullptr) {
		ENetPacket* packet = enet_packet_create(
			data.data(),
			data.size(),
			reliable ? ENET_PACKET_FLAG_RELIABLE : 0
			);

		if (enet_peer_send(m_serverPeer, channelId, packet) < 0) {
			enet_packet_destroy(packet);
			return false;
		}

		return true;
	} 
	else
	{
		if (peer != nullptr)
		{
			ENetPacket* packet = enet_packet_create(
				data.data(),
				data.size(),
				reliable ? ENET_PACKET_FLAG_RELIABLE : 0
				);

			if (enet_peer_send(peer, channelId, packet) < 0) {
				enet_packet_destroy(packet);
				return false;
			}

			return true;
		}
	}

	return false; // Not connected or no valid target
}

bool Network::broadcastData(const std::vector<uint8_t>& data, uint8_t channelId, bool reliable) {
	if (!m_isServer || m_host == nullptr) {
		return false;
	}

	ENetPacket* packet = enet_packet_create(
		data.data(),
		data.size(),
		reliable ? ENET_PACKET_FLAG_RELIABLE : 0
		);

	enet_host_broadcast(m_host, channelId, packet);
	return true;
}

std::vector<NetworkPacket> Network::pollEvents(uint32_t timeout_ms) {
	std::vector<NetworkPacket> packets;

	if (m_host == nullptr) {
		return packets;
	}

	ENetEvent event;
	while (enet_host_service(m_host, &event, timeout_ms) > 0) {
		NetworkPacket packet;
		packet.peer = event.peer;

		switch (event.type) {
		case ENET_EVENT_TYPE_CONNECT:
			packet.type = PacketType::CONNECT;
			g_Console.AddLog("[Networking] ENET_EVENT_TYPE_CONNECT!");
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			packet.type = PacketType::DISCONNECT;
			g_Console.AddLog("[Networking] ENET_EVENT_TYPE_DISCONNECT!");
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			packet.type = PacketType::DATA;
			packet.data.resize(event.packet->dataLength);
			memcpy(packet.data.data(), event.packet->data, event.packet->dataLength);
			enet_packet_destroy(event.packet);
			break;

		case ENET_EVENT_TYPE_NONE:
			continue;
		}

		packets.push_back(packet);

		// Only process one event per timeout if timeout > 0
		if (timeout_ms > 0) {
			timeout_ms = 0;
		}
	}

	return packets;
}

bool Network::isConnected() const {
	return !m_isServer && m_host != nullptr && m_serverPeer != nullptr;
}

bool Network::isHosting() const {
	return m_isServer && m_host != nullptr;
}

std::vector<ENetPeer*> Network::getConnectedPeers() const {
	std::vector<ENetPeer*> peers;

	if (m_isServer && m_host != nullptr) {
		for (size_t i = 0; i < m_host->peerCount; ++i) {
			if (m_host->peers[i].state == ENET_PEER_STATE_CONNECTED) {
				peers.push_back(&m_host->peers[i]);
			}
		}
	}

	return peers;
}

void Network::cleanup() {
	if (m_host != nullptr) {
		enet_host_destroy(m_host);
		m_host = nullptr;
	}

	m_serverPeer = nullptr;
	m_isServer = false;
	g_IsHost = false;
}