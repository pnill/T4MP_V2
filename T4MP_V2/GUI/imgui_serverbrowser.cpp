#include <vector>
#include <string>
#include <algorithm>
#include <cctype>   
#include <cstring>   
#include "../3rdparty\ImGui\imgui.h"
#include "../3rdparty\ImGui\imgui_impl_dx9.h"
#include "../3rdparty\ImGui\imgui_impl_win32.h"
#include "../T4MP/Networking/enet_wrapper.h"
#include "../TurokEngine/Level.h"
#include "../TurokEngine/Game.h"
#include "../TurokEngine/types.h"
#include "../TurokEngine/OldEngineStruct.h"


extern void RenderSettingsWindow();
bool g_RenderBrowser = true;
extern bool g_RenderSettings;
extern bool g_RenderDirectConnect;

extern int g_Port;

extern Network g_Network;

// Server data structure
struct ServerInfo {
	std::string name;
	std::string gameType;
	int players;
	int maxPlayers;
	std::string map;
	int latency; // in ms
};

// Sample server data (replace with your actual server discovery logic)
std::vector<ServerInfo> GetServers() {
	// This would be replaced with actual network code to discover servers
	std::vector<ServerInfo> servers;
	servers.push_back({ "Casual Gaming", "Deathmatch", 12, 16, "Arena", 45 });
	servers.push_back({ "Pro Players Only", "Capture the Flag", 8, 10, "Fortress", 78 });
	servers.push_back({ "Newbie Welcome", "Team Deathmatch", 6, 24, "City", 33 });
	servers.push_back({ "Late Night Gaming", "Domination", 16, 32, "Wastelands", 56 });
	servers.push_back({ "Weekend Warriors", "Survival", 4, 8, "Island", 92 });
	return servers;
}

// Global variables to maintain state between frames
static std::vector<ServerInfo> g_serverList;    // Persistent server list
static std::vector<ServerInfo> g_filteredList;  // Filtered server list based on search
static int g_selectedServerIndex = -1;          // Currently selected server
static bool g_initialized = false;              // Track if we've initialized
static int g_sortColumn = -1;                   // Current sort column (-1 = no sort)
static bool g_sortAscending = true;            // Sort direction
static char g_searchBuffer[128] = "";          // Search filter text

void RenderServerBrowser() {
	// Initialize server list if needed (only do this once)
	if (!g_initialized) {
		g_serverList = GetServers();
		g_filteredList = g_serverList; // Initialize filtered list with all servers
		g_initialized = true;
	}
	// Set up full-screen window flags
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;         // Enable menu bar
	window_flags |= ImGuiWindowFlags_NoTitleBar;      // No title bar
	window_flags |= ImGuiWindowFlags_NoResize;        // No resizing
	window_flags |= ImGuiWindowFlags_NoMove;          // No moving
	window_flags |= ImGuiWindowFlags_NoScrollbar;     // No scrollbars
	window_flags |= ImGuiWindowFlags_NoCollapse;      // No collapse
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	// Get window size for full-screen window
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(windowSize);

	// Create a full-screen window
	ImGui::Begin("Server Browser", nullptr, window_flags);

	// Menu Bar
	if (ImGui::BeginMenuBar()) {
		if (ImGui::MenuItem("Settings")) {
			g_RenderSettings = true;
		}
		if (ImGui::MenuItem("Host Server")) {
			
			g_Network.hostServer(g_Port);
			g_RenderBrowser = false;
			
			CGame::oPushPendingLevel(*(g_Turok4Game),0, "$/Data/Levels/u_jcowlishaw/screens/multiplayer/multiplayer.atr", 1, 1);
			CGame::oPopLevel(*(g_Turok4Game),0);
			CGame::oPushLevelSoon(*(g_Turok4Game), 0, "$/Data/Levels/u_jcowlishaw/screens/multiplayer/multiplayer.atr", 1, 1, "");

			/*
			CGame::PushPendingLevel(v4, *(const char **)&cActor3a->bThirdPerson, v7, v6);
			Cgame::PopLevel(g_Game);
			CGame::PushLevelSoon((int)v4, *(const char **)(this + 44), v7, v6, *(const char **)(this + 48));
			*/
		}
		if (ImGui::MenuItem("Direct Connect")) {
			// Handle direct connect action
			g_RenderDirectConnect = true;
		}
		if (ImGui::MenuItem("Quit")) {
			exit(0);
			// Handle quit action
			// e.g., game_running = false;
		}
		ImGui::EndMenuBar();
	}

	// Title
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("SERVER BROWSER").x) * 0.5f);
	ImGui::Text("SERVER BROWSER");
	ImGui::Separator();

	// Filtering/search controls
	ImGui::Text("Filter: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);

	// Store previous search buffer to detect changes
	char prevSearchBuffer[128];
	strcpy(prevSearchBuffer, g_searchBuffer);

	// Input text for search
	if (ImGui::InputText("##search", g_searchBuffer, IM_ARRAYSIZE(g_searchBuffer))) {
		// Filter has changed, apply filtering
		g_filteredList.clear();

		std::string filter = g_searchBuffer;
		// Convert to lowercase for case-insensitive search
		std::transform(filter.begin(), filter.end(), filter.begin(),
			[](unsigned char c) { return std::tolower(c); });

		// Filter the server list
		for (const auto& server : g_serverList) {
			// Convert server properties to lowercase for comparison
			std::string serverName = server.name;
			std::string gameType = server.gameType;
			std::string mapName = server.map;

			std::transform(serverName.begin(), serverName.end(), serverName.begin(),
				[](unsigned char c) { return std::tolower(c); });
			std::transform(gameType.begin(), gameType.end(), gameType.begin(),
				[](unsigned char c) { return std::tolower(c); });
			std::transform(mapName.begin(), mapName.end(), mapName.begin(),
				[](unsigned char c) { return std::tolower(c); });

			// Add server to filtered list if it matches the filter
			if (serverName.find(filter) != std::string::npos ||
				gameType.find(filter) != std::string::npos ||
				mapName.find(filter) != std::string::npos) {
				g_filteredList.push_back(server);
			}
		}

		// Reset selection as the indexes may have changed
		g_selectedServerIndex = -1;
	}

	ImGui::PopItemWidth();

	ImGui::SameLine(ImGui::GetWindowWidth() * 0.3f + 120);
	// Refresh button action
	if (ImGui::Button("Refresh")) {
		g_serverList = GetServers();

		// Apply current filter to the refreshed list
		std::string filter = g_searchBuffer;
		if (!filter.empty()) {
			// Convert to lowercase for case-insensitive search
			std::transform(filter.begin(), filter.end(), filter.begin(),
				[](unsigned char c) { return std::tolower(c); });

			g_filteredList.clear();
			for (const auto& server : g_serverList) {
				// Convert server properties to lowercase for comparison
				std::string serverName = server.name;
				std::string gameType = server.gameType;
				std::string mapName = server.map;

				std::transform(serverName.begin(), serverName.end(), serverName.begin(),
					[](unsigned char c) { return std::tolower(c); });
				std::transform(gameType.begin(), gameType.end(), gameType.begin(),
					[](unsigned char c) { return std::tolower(c); });
				std::transform(mapName.begin(), mapName.end(), mapName.begin(),
					[](unsigned char c) { return std::tolower(c); });

				// Add server to filtered list if it matches the filter
				if (serverName.find(filter) != std::string::npos ||
					gameType.find(filter) != std::string::npos ||
					mapName.find(filter) != std::string::npos) {
					g_filteredList.push_back(server);
				}
			}
		}
		else {
			g_filteredList = g_serverList;
		}

		// Reset selection as the server list has changed
		g_selectedServerIndex = -1;
	}

	// Server list
	// (using the persistent global list instead of getting a new one each frame)

	// Table for the server list
	// Set up flags for columns - if your version doesn't have ImGuiTableFlags,
	// we'll use a simpler approach with child windows and columns

	// Create a child window to contain the scrollable list
	ImGui::BeginChild("ServerListRegion", ImVec2(0, ImGui::GetContentRegionAvail().y - 35), true, ImGuiWindowFlags_HorizontalScrollbar);

	// Set up columns for the list
	ImGui::Columns(5, "ServerListColumns");

	// Set column widths (proportional)
	float windowWidth = ImGui::GetWindowWidth();
	ImGui::SetColumnWidth(0, windowWidth * 0.4f);  // Server Name
	ImGui::SetColumnWidth(1, windowWidth * 0.2f);  // Game Type
	ImGui::SetColumnWidth(2, windowWidth * 0.1f);  // Players
	ImGui::SetColumnWidth(3, windowWidth * 0.2f);  // Map
	ImGui::SetColumnWidth(4, windowWidth * 0.1f);  // Latency

												   // Column headers with sorting capability
												   // Server Name
	if (ImGui::Selectable("Server Name", g_sortColumn == 0)) {
		if (g_sortColumn == 0) {
			g_sortAscending = !g_sortAscending;
		}
		else {
			g_sortColumn = 0;
			g_sortAscending = true;
		}

		// Sort by server name
		std::sort(g_filteredList.begin(), g_filteredList.end(),
			[](const ServerInfo& a, const ServerInfo& b) {
			if (g_sortAscending)
				return a.name < b.name;
			else
				return a.name > b.name;
		});
	}
	ImGui::NextColumn();

	// Game Type
	if (ImGui::Selectable("Game Type", g_sortColumn == 1)) {
		if (g_sortColumn == 1) {
			g_sortAscending = !g_sortAscending;
		}
		else {
			g_sortColumn = 1;
			g_sortAscending = true;
		}

		// Sort by game type
		std::sort(g_filteredList.begin(), g_filteredList.end(),
			[](const ServerInfo& a, const ServerInfo& b) {
			if (g_sortAscending)
				return a.gameType < b.gameType;
			else
				return a.gameType > b.gameType;
		});
	}
	ImGui::NextColumn();

	// Players
	if (ImGui::Selectable("Players", g_sortColumn == 2)) {
		if (g_sortColumn == 2) {
			g_sortAscending = !g_sortAscending;
		}
		else {
			g_sortColumn = 2;
			g_sortAscending = true;
		}

		// Sort by player count
		std::sort(g_filteredList.begin(), g_filteredList.end(),
			[](const ServerInfo& a, const ServerInfo& b) {
			if (g_sortAscending)
				return a.players < b.players;
			else
				return a.players > b.players;
		});
	}
	ImGui::NextColumn();

	// Map
	if (ImGui::Selectable("Map", g_sortColumn == 3)) {
		if (g_sortColumn == 3) {
			g_sortAscending = !g_sortAscending;
		}
		else {
			g_sortColumn = 3;
			g_sortAscending = true;
		}

		// Sort by map name
		std::sort(g_filteredList.begin(), g_filteredList.end(),
			[](const ServerInfo& a, const ServerInfo& b) {
			if (g_sortAscending)
				return a.map < b.map;
			else
				return a.map > b.map;
		});
	}
	ImGui::NextColumn();

	// Latency
	if (ImGui::Selectable("Latency", g_sortColumn == 4)) {
		if (g_sortColumn == 4) {
			g_sortAscending = !g_sortAscending;
		}
		else {
			g_sortColumn = 4;
			g_sortAscending = true;
		}

		// Sort by latency
		std::sort(g_filteredList.begin(), g_filteredList.end(),
			[](const ServerInfo& a, const ServerInfo& b) {
			if (g_sortAscending)
				return a.latency < b.latency;
			else
				return a.latency > b.latency;
		});
	}
	ImGui::NextColumn();

	ImGui::Separator();

	// Populate rows (using the filtered list)
	for (int i = 0; i < g_filteredList.size(); i++) {
		ServerInfo& server = g_filteredList[i];

		// Create unique ID for the selectable
		std::string id = "##" + std::to_string(i);

		// Check if this is the selected server
		bool isSelected = (i == g_selectedServerIndex);

		// Make the entire row selectable
		if (ImGui::Selectable(id.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
			g_selectedServerIndex = i;  // Update the selection index in the filtered list
		}

		// Handle double-click to join
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
			// Handle server join action
			// e.g., JoinServer(server);
		}

		// Draw the server name text right after the selectable
		ImGui::SameLine();
		ImGui::Text("%s", server.name.c_str());

		ImGui::NextColumn();

		// Game Type
		ImGui::Text("%s", server.gameType.c_str());
		ImGui::NextColumn();

		// Players
		ImGui::Text("%d/%d", server.players, server.maxPlayers);
		ImGui::NextColumn();

		// Map
		ImGui::Text("%s", server.map.c_str());
		ImGui::NextColumn();

		// Latency with color coding
		ImVec4 latencyColor;
		if (server.latency < 50) latencyColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);       // Green for good
		else if (server.latency < 100) latencyColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow for medium
		else latencyColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);                           // Red for bad

		ImGui::TextColored(latencyColor, "%d ms", server.latency);
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
	ImGui::EndChild();

	// Status bar at the bottom with three sections: left, center, right
	float bottomPanelHeight = 30;

	// Left section for Total Servers (showing filtered/total count)
	ImGui::SetCursorPos(ImVec2(10, windowSize.y - bottomPanelHeight));
	if (strlen(g_searchBuffer) > 0) {
		ImGui::Text("Servers: %d/%d", (int)g_filteredList.size(), (int)g_serverList.size());
	}
	else {
		ImGui::Text("Total Servers: %d", (int)g_serverList.size());
	}

	// Center section for Selected Server (if any)
	if (g_selectedServerIndex >= 0 && g_selectedServerIndex < g_filteredList.size()) {
		ServerInfo& selectedServer = g_filteredList[g_selectedServerIndex];

		// Calculate the position to center the "Selected: ServerName" text
		std::string selectedText = "Selected: " + selectedServer.name;
		float textWidth = ImGui::CalcTextSize(selectedText.c_str()).x;
		float centerPosX = (windowSize.x - textWidth) * 0.5f;

		ImGui::SetCursorPos(ImVec2(centerPosX, windowSize.y - bottomPanelHeight));
		ImGui::Text("%s", selectedText.c_str());

		// Right section for Join Button
		float joinButtonWidth = 100;
		float joinButtonHeight = 25;
		float joinButtonPosX = windowSize.x - joinButtonWidth - 20;
		float joinButtonPosY = windowSize.y - joinButtonHeight - (bottomPanelHeight - joinButtonHeight) / 2;

		ImGui::SetCursorPos(ImVec2(joinButtonPosX, joinButtonPosY));
		if (ImGui::Button("Join Server", ImVec2(joinButtonWidth, joinButtonHeight))) {
			// Join the selected server
			// JoinServer(selectedServer);
		}
	}

	ImGui::End();
}

// Implementation of the server browser UI
// Include this in your own code and call RenderServerBrowser() from your render loop