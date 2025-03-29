#include <windows.h>
#include "3rdparty\MinHook_133_lib\include\MinHook.h"
#include "3rdparty\ImGui\imgui_impl_dx9.h"
#include "3rdparty\ImGui\imgui_impl_win32.h"
#include "3rdparty\D3D9\d3d9.h"
#include "3rdparty\D3D9\IDIn8.h"
#include "T4MP\Networking\enet_wrapper.h"
#include <unordered_map>

hkIDirectInput8 hkDInput8;
IDirectInput8 *g_pDInInt8 = NULL;
hkIDirectInput8 *phkDInput8 = NULL;
IDirectInputDevice8 *g_pDInDev8 = NULL;
hkIDirectInputDevice8 *phkDInputDevice8 = NULL;
LPVOID *Value;

#pragma comment(lib, "dxguid.lib")

typedef HWND(__stdcall* tCreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
typedef BOOL(__stdcall* tPeekMessageA)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
typedef void(__stdcall* tCreateWindowInstalledHook)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, HWND result);
typedef void(__stdcall* tPeekMessageAInstalledHook)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg, BOOL* result);
typedef HRESULT(WINAPI* DirectInput8Create_t)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
tCreateWindowExA pCreateWindowExA;
tPeekMessageA pPeekMessageA;
HWND WINAPI CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL WINAPI PeekMessageAHook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
DirectInput8Create_t original_DirectInput8Create = nullptr;
typedef BOOL(WINAPI *tGetCursorPos)(LPPOINT lpPoint);
tGetCursorPos pGetCursorPos = NULL;

extern bool g_RenderSpawnActor;
extern bool g_RenderDebugConsole;
extern bool g_IsHost;

// Hook function for DirectInput8Create
HRESULT WINAPI DirectInput8Create_Hook(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
	// Call original function
	HRESULT hr = original_DirectInput8Create(hinst, dwVersion, riidltf, (LPVOID*)&g_pDInInt8, punkOuter);
	phkDInput8 = &hkDInput8;
	*ppvOut = phkDInput8;
	Value = ppvOut;
	if (SUCCEEDED(hr) && ppvOut && *ppvOut)
	{
		// Successfully created DirectInput - now hook the interface
		IDirectInput8* pDI8 = (IDirectInput8*)*ppvOut;
		printf("[+] DirectInput8Create_Hook called!\n");
	}

	return hr;
}

BOOL GetCursorPosHook(LPPOINT lpPoint)
{
	if(ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard)
	{
		lpPoint->x = 0;
		lpPoint->y = 0;
	}

	return pGetCursorPos(lpPoint);
}

// Set up the hook in your initialization
void SetupDirectInputHooks()
{
	HMODULE dinput8 = LoadLibrary("dinput8.dll");
	if (dinput8)
	{
		DirectInput8Create_t pDirectInput8Create = (DirectInput8Create_t)GetProcAddress(dinput8, "DirectInput8Create");
		if (pDirectInput8Create)
		{
			MH_CreateHook(pDirectInput8Create, DirectInput8Create_Hook, (LPVOID*)&original_DirectInput8Create);
			MH_EnableHook(pDirectInput8Create);
			printf("[+] DirectInput8Create hook installed\n");
		}
	}
}

void InstallAPIHooks()
{
	// Initialize MinHook
	if (MH_Initialize() != MH_OK)
	{
		printf("Failed to intiialize MinHook\r\nTerminating...\n");
		MessageBox(NULL, "Failed to initialize MinHook.\r\nTerminating...", "T4MP INTERNAL ERROR", MB_OK);
		exit(0);
	}
	
	SetupDirectInputHooks();

	HMODULE user32_module = LoadLibrary("user32.dll");
	if (user32_module)
	{
		MH_STATUS status = MH_ERROR_DISABLED;

		// Create hooks for the target functions
		status = MH_CreateHookApi(L"user32.dll","CreateWindowExA",(LPVOID)CreateWindowExAHook, (LPVOID*)&pCreateWindowExA);
		if (status != MH_OK)
		{
			printf("[-] MH_CreateHook - Failed to create hooks.\n");
			printf("[!] MH_STATUS = %d\n", status);
			MessageBox(NULL, "Failed to create hooks.\r\nTerminating...", "T4MP INTERNAL ERROR", MB_OK);
			exit(0);
		}

		status = MH_CreateHookApi(L"user32.dll","PeekMessageA", (LPVOID)PeekMessageAHook, (LPVOID*)&pPeekMessageA);
		if (status  != MH_OK)
		{
			printf("[-] MH_CreateHook - Failed to create hooks.\n");
			printf("[!] MH_STATUS = %d\n", status);
			MessageBox(NULL, "Failed to create hooks.\r\nTerminating...", "T4MP INTERNAL ERROR", MB_OK);
			exit(0);
		}

		status = MH_EnableHook(MH_ALL_HOOKS);
		// Enable the hooks
		if (status != MH_OK)
		{
			printf("[-] MH_EnableHook - Failed to enable hooks.\n");
			printf("[!] MH_STATUS: %d\n", status);
			MessageBox(NULL, "Failed to enable hooks.\r\nTerminating...", "T4MP INTERNAL ERROR", MB_OK);
			exit(0);
		}
	
	}
	else
	{
		printf("[-] Failed to LoadLibrary on user32.dll!\n");
	}
	
	// Our hooks have to happen before the windower does.
	// We should probably just port the windower internally at this point.
	LoadLibrary("d3dhook.dll");

	char sysd3d[320];
	GetSystemDirectory(sysd3d, 320);
	strcat(sysd3d, "\\d3d9.dll");
	HMODULE hMod = LoadLibrary(sysd3d);

	if (hMod)
	{
		if (MH_CreateHook(GetProcAddress(hMod, "Direct3DCreate9"), &hkDirect3DCreate9, reinterpret_cast<void**>(&oDirect3DCreate9)) != MH_OK)
		{
			printf("[-] MH_CreateHook failed on Direct3DCreate9!\n");
		}

		if (MH_EnableHook(GetProcAddress(hMod, "Direct3DCreate9")) != MH_OK)
		{
			printf("[-] MH_EnableHook failed on Direct3DCreate9!\n");
		}

	}

}

static bool g_ImGuiInitialized = false;


HWND WINAPI CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	HWND result = pCreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	// Only initialize ImGui once
	if (!g_ImGuiInitialized && result != NULL) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigNavMoveSetMousePos = true;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.WantCaptureMouse = true;
		io.MouseDrawCursor = false;

		ImGui::StyleColorsDark();
		ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Arrow);
		ImGui_ImplWin32_Init(result);

		g_ImGuiInitialized = true;
	}

	return result;
}


BOOL WINAPI PeekMessageAHook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	BOOL result;
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hWnd, &cursorPos);

	// Update ImGui's mouse position
	ImGui::GetIO().MousePos.x = static_cast<float>(cursorPos.x);
	ImGui::GetIO().MousePos.y = static_cast<float>(cursorPos.y);

	result = pPeekMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	TranslateMessage(lpMsg);

	ImGui_ImplWin32_WndProcHandler(hWnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
	
	switch (lpMsg->message)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_MOUSEMOVE:
			if (ImGui::GetIO().WantCaptureMouse || !g_IsHost)
			{
				lpMsg->message = WM_NULL;
				result = TRUE;
			}
		break;

		case WM_KEYDOWN:
			if (lpMsg->wParam == VK_HOME)
				g_RenderSpawnActor = true;
			if (lpMsg->wParam == VK_OEM_3)
				g_RenderDebugConsole = true;

			if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard || !g_IsHost)
			{
				lpMsg->message = WM_NULL;
				return TRUE;
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_CHAR: 
			if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard || !g_IsHost)
			{
				lpMsg->message = WM_NULL;
				return TRUE;
			}
			break;
		break;
	}

	return result;
}
