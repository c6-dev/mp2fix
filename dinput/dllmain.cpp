/**
* Copyright (C) 2020 Elisha Riedlinger
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/
#define _CRT_SECURE_NO_WARNINGS
#include "dinput.h"


std::ofstream Log::LOG("dinput.log");


DirectInputCreateAProc m_pDirectInputCreateA;


BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	static HMODULE dinputdll;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Load dll
		char path[MAX_PATH];
		GetSystemDirectoryA(path, MAX_PATH);
		strcat_s(path, "\\dinput.dll");
		Log() << "Loading " << path;
		dinputdll = LoadLibraryA(path);

		// Get function addresses
		m_pDirectInputCreateA = (DirectInputCreateAProc)GetProcAddress(dinputdll, "DirectInputCreateA");
		createLoaderHook();
		break;

	case DLL_PROCESS_DETACH:
		FreeLibrary(dinputdll);
		break;
	}

	return TRUE;
}

HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* lplpDirectInput, LPUNKNOWN punkOuter)
{
	if (!m_pDirectInputCreateA)
	{
		return E_FAIL;
	}

	return m_pDirectInputCreateA(hinst, dwVersion, lplpDirectInput, punkOuter);

}


