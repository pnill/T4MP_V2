#include <string>
#include <unordered_map>
#include <vector>
#include "imgui.h"
#include "imgui_notify.h"

void NotificationSystem::AddConnectionNotification(const char* username, float duration, bool disconnect) {
		// Add a new notification
		Notification notification;
		notification.username = username;
		notification.startTime = ImGui::GetTime();
		notification.duration = duration;
		notification.isDisconnect = disconnect;
		activeNotifications.push_back(notification);
}

void NotificationSystem::Draw() {
		ImVec2 windowSize = ImGui::GetIO().DisplaySize;
		ImVec2 boxSize(200, 40);
		float padding = 10.0f;
		float currentY = windowSize.y - padding;

		// Process notifications from bottom to top
		for (int i = activeNotifications.size() - 1; i >= 0; i--) {
			Notification& notification = activeNotifications[i];
			float elapsedTime = ImGui::GetTime() - notification.startTime;

			// Remove expired notifications
			if (elapsedTime >= notification.duration) {
				activeNotifications.erase(activeNotifications.begin() + i);
				continue;
			}

			// Calculate fade-in and fade-out effect
			float alpha = 1.0f;
			if (elapsedTime < 0.5f) {
				// Fade in during first 0.5 seconds
				alpha = elapsedTime / 0.5f;
			}
			else if (elapsedTime > notification.duration - 1.0f) {
				// Fade out during last 1.0 second
				alpha = 1.0f - (elapsedTime - (notification.duration - 1.0f));
			}

			// Position for this notification
			currentY -= boxSize.y;
			ImVec2 position(windowSize.x - boxSize.x - padding, currentY);

			// Set window position and style
			ImGui::SetNextWindowPos(position);
			ImGui::SetNextWindowSize(boxSize);
			ImGui::SetNextWindowBgAlpha(0.75f * alpha);

			// Create the notification box
			ImGuiWindowFlags flags =
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_NoNav |
				ImGuiWindowFlags_NoMove;

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.75f * alpha));
			std::string windowId = "##ConnectionNotification" + std::to_string(i);
			if (ImGui::Begin(windowId.c_str(), nullptr, flags)) {
				ImGui::SetCursorPos(ImVec2(10, (boxSize.y - ImGui::GetTextLineHeight()) * 0.5f));
				if (notification.isDisconnect)
				{
					ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, alpha), "%s has disconnected", notification.username.c_str());
				}
				else
					ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, alpha), "%s has connected", notification.username.c_str());

				ImGui::End();
			}
			ImGui::PopStyleColor();

			// Add spacing between notifications
			currentY -= padding;
		}
}


class NotificationSystem g_Notification;

// Simple usage example:
void DrawConnectionNotification(const char* username, bool disconnect = false, float duration = 5.0f) {
	//static NotificationSystem notificationSystem;

	// Only create a new notification if username is provided
	if (username != nullptr) {
		g_Notification.AddConnectionNotification(username, duration,disconnect);
	}

	// Always draw all active notifications
	//notificationSystem.Draw();
}