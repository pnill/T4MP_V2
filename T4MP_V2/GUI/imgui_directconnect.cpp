// imgui_directconnect.cpp
#include "imgui.h"
#include "../T4MP/Networking/enet_wrapper.h"
#include "../T4MP/Networking/network_serialization.h"


// Global variables
extern bool g_RenderBrowser;
bool g_RenderDirectConnect = false;
static char g_IpAddress[64] = "127.0.0.1";
static int g_Port = 12071;
extern Network g_Network;
extern std::string g_Username;

void RenderDirectConnect()
{
	if (!g_RenderDirectConnect)
		return;

	// Set window size constraints to prevent tiny windows
	ImGui::SetNextWindowSize(ImVec2(400.0f, 200.0f), ImGuiCond_FirstUseEver);

	// Create the Direct Connect window
	if (ImGui::Begin("Direct Connect", &g_RenderDirectConnect, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Enter the IP address of the server you want to connect to:");

		// IP Address input field
		ImGui::InputText("IP Address", g_IpAddress, IM_ARRAYSIZE(g_IpAddress));

		// Port number input
		ImGui::InputInt("Port", &g_Port,0,0);

		ImGui::Separator();

		// Center the Connect button
		float windowWidth = ImGui::GetWindowSize().x;
		float buttonWidth = 120.0f;
		float buttonX = (windowWidth - buttonWidth) * 0.5f;

		ImGui::SetCursorPosX(buttonX);
		ImGui::Spacing();
		ImGui::Spacing();

		// Connect button (without custom styling)
		if (ImGui::Button("Connect", ImVec2(buttonWidth, 30.0f)))
		{
			if (g_Network.connectToServer(g_IpAddress, g_Port))
			{
				g_RenderBrowser = false;
				g_RenderDirectConnect = false;
				std::vector<uint8_t> packet = NetworkSerializer::CreateNicknameUpdatePacket(g_Username);
				g_Network.sendData(packet, 0, true);
			}
		}

		ImGui::End();
	}
}