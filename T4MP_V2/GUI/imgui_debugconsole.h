#pragma once
#include "../3rdparty/ImGui/imgui.h"
#include <stdio.h>

// Forward declarations
struct ImGuiInputTextCallbackData;

// Debug console visibility flag
extern bool g_RenderDebugConsole;

struct T4MPConsole
{
	char                  InputBuf[256];
	ImVector<char*>       Items;
	ImVector<const char*> Commands;
	ImVector<char*>       History;
	int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter       Filter;
	bool                  AutoScroll;
	bool                  ScrollToBottom;

	T4MPConsole();
	~T4MPConsole();

	// Portable helpers
	static int   Stricmp(const char* s1, const char* s2);
	static int   Strnicmp(const char* s1, const char* s2, int n);
	static char* Strdup(const char* s);
	static void  Strtrim(char* s);

	void    ClearLog();
	void    AddLog(const char* fmt, ...) IM_FMTARGS(2);
	void    Draw(const char* title, bool* p_open);
	void    ExecCommand(const char* command_line);

	// Callbacks
	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
	int     TextEditCallback(ImGuiInputTextCallbackData* data);
};

// Global console instance
extern T4MPConsole g_Console;

// Function to render the debug console
void RenderDebugConsole();