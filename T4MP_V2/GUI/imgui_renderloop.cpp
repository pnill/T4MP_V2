#include "../3rdparty/ImGui/imgui.h"
#include "imgui_notify.h"

extern bool g_RenderBrowser;
extern bool g_RenderSettings;
extern bool g_RenderSpawnActor;
extern bool g_RenderDebugConsole;
extern bool g_RenderDirectConnect;

extern void RenderServerBrowser();
extern void RenderSettings();
extern void RenderSpawnActor();
extern void RenderDebugConsole();
extern void RenderDirectConnect();

extern void DrawConnectionNotification(const char* username, bool disconnect = false, float duration = 5.0f);
extern class NotificationSystem g_Notification;

void imgui_renderloop()
{
	if (g_RenderBrowser)
		RenderServerBrowser();

	if (g_RenderSettings)
		RenderSettings();

	if (g_RenderSpawnActor)
		RenderSpawnActor();

	if (g_RenderDebugConsole)
		RenderDebugConsole();

	if (g_RenderDirectConnect)
		RenderDirectConnect();

	g_Notification.Draw();
}