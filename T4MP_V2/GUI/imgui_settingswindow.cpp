#include "imgui.h"
#include <stdio.h>
#include <string>

bool g_RenderSettings = false;
bool g_invertControls = false;

std::string g_Username;

int g_Port = 12071;

void RenderSettings()
{
	static char username[64] = "";
	
	// Set window size constraints to prevent tiny windows
	ImGui::SetNextWindowSize(ImVec2(500.0f, 400.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Settings", &g_RenderSettings, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		// General Settings Category - using TreeNodeEx with special flags to make it non-collapsible
		ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		// You can use this if you want the arrow to be hidden:
		// headerFlags |= ImGuiTreeNodeFlags_Leaf;

		// Or this if you want the arrow to be visible but disabled:
		//headerFlags |= ImGuiTreeNodeFlags_NoButton;

		// For the heading style without collapsing functionality

		// Now we manually handle what would normally be in the tree node
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f)); // Highlight category text
		ImGui::Text("User Information");
		ImGui::PopStyleColor();
		ImGui::Separator();

		// Username input field with better layout
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Username:");
		ImGui::SameLine(150);
		ImGui::SetNextItemWidth(200);
		ImGui::InputText("##username", username, IM_ARRAYSIZE(username));
		ImGui::Spacing();

		// No need for TreePop() since we used NoTreePushOnOpen

		// Control Settings Category - same approach

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f)); // Highlight category text
		ImGui::Text("Control Options");
		ImGui::PopStyleColor();
		ImGui::Separator();

		// Invert controls checkbox with better layout
		ImGui::AlignTextToFramePadding();
		ImGui::Checkbox("Invert Controls", &g_invertControls);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Inverts mouse/keyboard controls");
		ImGui::Spacing();

		// No need for TreePop() since we used NoTreePushOnOpen



		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f)); // Highlight category text
		ImGui::Text("Network Information");
		ImGui::PopStyleColor();
		ImGui::Separator();

		// Username input field with better layout
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Network Port:");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("This is the port your server will be hosted on default 12071");
		ImGui::SameLine(150);
		ImGui::SetNextItemWidth(200);
		ImGui::InputInt("##port", &g_Port, 0, 0);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("This is the port your server will be hosted on default 12071");
		ImGui::Spacing();

		g_Username = std::string(username);
		ImGui::End();
	}
	// Window exit handling is done via the g_RenderSettings global variable
	// When the user clicks the X, ImGui will set g_RenderSettings to false
}