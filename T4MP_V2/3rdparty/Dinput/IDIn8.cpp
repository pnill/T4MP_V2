#define INITGUID
#pragma comment(lib, "dxguid.lib")

#include "IDIn8.h"
#include "IDInDev8.h"
#include <stdio.h>


extern IDirectInput8 *g_pDInInt8;
extern hkIDirectInput8 *phkDInput8;
extern LPVOID * Value;

HRESULT APIENTRY hkIDirectInput8::CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8 *lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{

	hkIDirectInputDevice8 *phkDInputDevice8 = new hkIDirectInputDevice8;
	

	if (rguid == GUID_SysMouse)
	{
		//add_log("hooked mouse");
		printf("hooked mouse");
	}
	if (rguid == GUID_SysKeyboard)
	{
		//add_log("hooked Keyboard");
		printf("hooked Keyboard");
	}
	if (rguid == GUID_Joystick)
	{
		//add_log("hooked Joystick");
		printf("hooked Joystick");
	}
	//add_log("GUID 0x%X",rguid);
	printf("GUID 0x%X", rguid);

	HRESULT Result = g_pDInInt8->CreateDevice(rguid, (LPDIRECTINPUTDEVICE8*)&phkDInputDevice8->g_pDInDev8, pUnkOuter);
	printf( "Hooked IDirectInputDevice8: 0x%x -> 0x%x", Value, phkDInputDevice8 );
	*lplpDirectInputDevice = (LPDIRECTINPUTDEVICE8)phkDInputDevice8;
	return Result;


}

HRESULT APIENTRY hkIDirectInput8::QueryInterface(REFIID iid, void ** ppvObject)
{

	printf("dinput8::queryinterface\n");
	HRESULT hRes = g_pDInInt8->QueryInterface(iid, ppvObject); 


	return hRes;

}

ULONG APIENTRY hkIDirectInput8::AddRef(void)
{
	printf("dinput8::AddRef()\n");
	return g_pDInInt8->AddRef();
}

ULONG APIENTRY hkIDirectInput8::Release(void)
{
	printf("dinput8::release\n");
	return g_pDInInt8->Release();
}

HRESULT APIENTRY hkIDirectInput8::ConfigureDevices(LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMS lpdiCDParams, DWORD dwFlags, LPVOID pvRefData)
{
	printf("dinput8::configuredevices\n");
	return g_pDInInt8->ConfigureDevices(lpdiCallback, lpdiCDParams, dwFlags, pvRefData);
}

HRESULT APIENTRY hkIDirectInput8::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	printf("dinput8::enumdevices\n");
	return g_pDInInt8->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
}


struct CallbackContext {
	LPDIENUMDEVICESBYSEMANTICSCB originalCallback;
	LPVOID originalRef;
};

BOOL CALLBACK MyEnumDevicesBySemanticsCallback(
	const DIDEVICEINSTANCE* pdidi,
	LPDIRECTINPUTDEVICE8 pdid,
	DWORD dwFlags,
	DWORD dwRemaining,
	LPVOID pvRef)
{
	CallbackContext* context = static_cast<CallbackContext*>(pvRef);

	// Create your wrapper for this device
	hkIDirectInputDevice8* phkDInputDevice8 = new hkIDirectInputDevice8;

	// Store the original device
	phkDInputDevice8->g_pDInDev8 = pdid;

	// Call the original callback with your wrapped device
	return context->originalCallback(pdidi,
		reinterpret_cast<LPDIRECTINPUTDEVICE8>(phkDInputDevice8),
		dwFlags,
		dwRemaining,
		context->originalRef);
}


HRESULT APIENTRY hkIDirectInput8::EnumDevicesBySemantics(LPCTSTR ptszUserName, LPDIACTIONFORMAT lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags)
{

	CallbackContext context;
	context.originalCallback = lpCallback;
	context.originalRef = pvRef;
	return g_pDInInt8->EnumDevicesBySemantics(ptszUserName, lpdiActionFormat, MyEnumDevicesBySemanticsCallback, &context, dwFlags);
}

HRESULT APIENTRY hkIDirectInput8::FindDevice(REFGUID rguidClass, LPCTSTR ptszName, LPGUID pguidInstance)
{
	printf("dinput8::finddevice\n");
	return g_pDInInt8->FindDevice(rguidClass, ptszName, pguidInstance);
}

HRESULT APIENTRY hkIDirectInput8::GetDeviceStatus(REFGUID rguidInstance)
{
	printf("GetDeviceStatus\n");
	return g_pDInInt8->GetDeviceStatus(rguidInstance);
}

HRESULT APIENTRY hkIDirectInput8::Initialize(HINSTANCE hinst, DWORD dwVersion)
{
	printf("dinput8::init\n");
	return g_pDInInt8->Initialize(hinst, dwVersion);
}

HRESULT APIENTRY hkIDirectInput8::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	printf("dinput8::RunControlPanel\n");
	return g_pDInInt8->RunControlPanel(hwndOwner, dwFlags);
}
