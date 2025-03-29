#include "imgui.h"
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include "../TurokEngine/Level.h"
#include "../TurokEngine/Game.h"
#include "../TurokEngine/types.h"
#include "../TurokEngine/OldEngineStruct.h"

// Input buffer for actor type
char actorTypeBuffer[256] = "";

// Actor path handling
std::string basePath = "Data\\Actors\\";
std::string selectedActorPath = "";
char actorPathBuffer[256] = "$\\Data\\Actors\\";  // Preset with base directory
std::vector<std::string> actorFiles;
std::vector<std::string> filteredActorFiles; // For storing filtered results
int currentActorIndex = -1;

// Filter input buffer
char filterBuffer[256] = "";

// Position using engine vector3 type
vector3 position;

// Window state
bool g_RenderSpawnActor = false;
bool spawnOnPlayer = false;

// Helper function to check if a file has .atr extension
bool HasAtrExtension(const std::string& filename) {
	size_t dot = filename.find_last_of('.');
	if (dot == std::string::npos)
		return false;

	std::string ext = filename.substr(dot);
	// Convert to lowercase for case-insensitive comparison
	for (auto& c : ext)
		c = tolower(c);

	return ext == ".atr";
}

// Helper function to convert string to lowercase for case-insensitive comparison
std::string ToLowerCase(const std::string& str) {
	std::string result = str;
	for (auto& c : result) {
		c = tolower(c);
	}
	return result;
}

// Function to recursively scan directory for .atr files using Windows API
void ScanDirectoryForActorFiles(const std::string& directory, const std::string& relativePath = "") {
	WIN32_FIND_DATA findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	std::string searchPath = directory + "\\*";

	hFind = FindFirstFile(searchPath.c_str(), &findData);

	if (hFind == INVALID_HANDLE_VALUE) {
		printf("Error scanning directory: %s\n", directory.c_str());
		return;
	}

	do {
		if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
			continue;

		std::string fullPath = directory + "\\" + findData.cFileName;
		std::string newRelativePath = relativePath.empty() ? findData.cFileName : relativePath + "\\" + findData.cFileName;

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// Recursively scan subdirectories
			ScanDirectoryForActorFiles(fullPath, newRelativePath);
		}
		else {
			// Check if file has .atr extension
			if (HasAtrExtension(findData.cFileName)) {
				actorFiles.push_back(newRelativePath);
			}
		}
	} while (FindNextFile(hFind, &findData) != 0);

	FindClose(hFind);
}

// Function to scan for all .atr files
void ScanForActorFiles() {
	actorFiles.clear();

	// Start the recursive scanning from the base path
	ScanDirectoryForActorFiles(basePath);

	// Sort files alphabetically
	std::sort(actorFiles.begin(), actorFiles.end());

	// Reset filter
	filterBuffer[0] = '\0';

	// Initialize filtered list with all files
	filteredActorFiles = actorFiles;
}

// Function to filter actor files based on search string
void FilterActorFiles(const char* filter) {
	filteredActorFiles.clear();

	if (filter[0] == '\0') {
		// If filter is empty, show all files
		filteredActorFiles = actorFiles;
		return;
	}

	std::string lowerFilter = ToLowerCase(filter);

	for (const auto& file : actorFiles) {
		std::string lowerFile = ToLowerCase(file);
		if (lowerFile.find(lowerFilter) != std::string::npos) {
			filteredActorFiles.push_back(file);
		}
	}
}

void SpawnActor() {
	std::string actorType(actorTypeBuffer);
	std::string actorPath(actorPathBuffer);

	printf("Spawning actor of type: %s\n", actorType.c_str());
	printf("Actor path: %s\n", actorPath.c_str());
	printf("Position: X=%.2f, Y=%.2f, Z=%.2f\n", position.x, position.y, position.z);

	// Call the engine's spawn actor function
	CLevel::oSpawnActor(g_Game->pT4Game, 0, 0, (char*)actorType.c_str(), (char*)actorPath.c_str(), position, 0);
}

bool bAlreadyPushed = false;
bool bAlreadyPopped = false;
bool shouldRejectPopLevels = false;

void RenderSpawnActor() {
	// Initialize actor files list if it's empty
	if (actorFiles.empty()) {
		ScanForActorFiles();
	}

	// Set window flags for non-resizable window
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;

	// Begin ImGui window with fixed size
	ImGui::SetNextWindowSize(ImVec2(800, 0), ImGuiCond_FirstUseEver);  // Width of 800, auto height

	if (ImGui::Begin("Spawn Actor", &g_RenderSpawnActor, window_flags)) {
		// Text input fields
		ImGui::Text("Actor Properties:");
		ImGui::InputText("Actor Type", actorTypeBuffer, IM_ARRAYSIZE(actorTypeBuffer));

		// Actor path dropdown
		ImGui::Text("Actor Path:");

		// Add filter input
		if (ImGui::InputText("Filter", filterBuffer, IM_ARRAYSIZE(filterBuffer))) {
			// Filter the actor files when the filter text changes
			FilterActorFiles(filterBuffer);
		}

		// Create a preview value that shows the selected path or a default message
		const char* previewValue = (currentActorIndex >= 0) ?
			selectedActorPath.c_str() : "Select an actor file...";

		// Show file count info
		ImGui::Text("Showing %d of %d actor files", filteredActorFiles.size(), actorFiles.size());
		// Dropdown for actor selection
		if (ImGui::BeginCombo("##ActorPathCombo", previewValue)) {
			for (int i = 0; i < (int)filteredActorFiles.size(); i++) {
				const bool isSelected = (selectedActorPath == filteredActorFiles[i]);
				if (ImGui::Selectable(filteredActorFiles[i].c_str(), isSelected)) {
					currentActorIndex = i; // This now references the filtered list index
					selectedActorPath = filteredActorFiles[i];

					// Update the buffer with the full path including the $ prefix
					std::string fullPath = std::string("$\\") + basePath + selectedActorPath;
					strcpy(actorPathBuffer, fullPath.c_str());
				}

				// Set the initial focus when opening the combo
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		// Display the full path with $ prefix (read-only)
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f)); // Darker background for read-only
		ImGui::InputText("##FullPath", actorPathBuffer, IM_ARRAYSIZE(actorPathBuffer), ImGuiInputTextFlags_ReadOnly);
		ImGui::PopStyleColor();

		if (g_Game && g_Game->pT4Game && g_Game->pT4Game->pEngineObjects && g_Game->pT4Game->pEngineObjects->pCameraArray && spawnOnPlayer)
		{
			printf("player pos ptr: %08X\n", g_Game->pT4Game->pEngineObjects->pCameraArray[0]->x);
			position.x = g_Game->pT4Game->pEngineObjects->pCameraArray[0]->x;
			position.y = g_Game->pT4Game->pEngineObjects->pCameraArray[0]->y;
			position.z = g_Game->pT4Game->pEngineObjects->pCameraArray[0]->z;
		}
		// Position inputs
		ImGui::Separator();
		ImGui::Text("Position:");
		ImGui::InputFloat("X", &position.x, 0.1f, 1.0f, "%.2f");
		ImGui::InputFloat("Y", &position.y, 0.1f, 1.0f, "%.2f");
		ImGui::InputFloat("Z", &position.z, 0.1f, 1.0f, "%.2f");
		// Spawn button
		ImGui::Separator();
		ImGui::Checkbox("Spawn On Player", &spawnOnPlayer);

		if (ImGui::Button("Spawn", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			SpawnActor();
		}

		// Refresh button for rescanning directory
		if (ImGui::Button("Refresh Actor List", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			ScanForActorFiles();
			currentActorIndex = -1;
			selectedActorPath = "";
		}


	}
	ImGui::End();
}