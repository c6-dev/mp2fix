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
#include "dsound.h"


std::ofstream Log::LOG("dsound.log");


DirectSoundEnumerateAProc m_pDirectSoundEnumerateA;
DirectSoundCreate8Proc m_pDirectSoundCreate8;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	static HMODULE dsounddll;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Load dll
		char path[MAX_PATH];
		GetSystemDirectoryA(path, MAX_PATH);
		strcat_s(path, "\\dsound.dll");
		Log() << "Loading " << path;
		dsounddll = LoadLibraryA(path);

		// Get function addresses
		m_pDirectSoundEnumerateA = (DirectSoundEnumerateAProc)GetProcAddress(dsounddll, "DirectSoundEnumerateA");
		m_pDirectSoundCreate8 = (DirectSoundCreate8Proc)GetProcAddress(dsounddll, "DirectSoundCreate8");
		loadIniOptions();
		writePatches();
		break;

	case DLL_PROCESS_DETACH:
		FreeLibrary(dsounddll);
		break;
	}

	return TRUE;
}



HRESULT WINAPI DirectSoundEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext)
{
	Log() << "DirectSoundEnumerateA";
	if (!m_pDirectSoundEnumerateA)
	{
		return E_FAIL;
	}

	return m_pDirectSoundEnumerateA(pDSEnumCallback, pContext);
}


HRESULT WINAPI DirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter)
{
	Log() << "DirectSoundCreate8";
	if (!m_pDirectSoundCreate8)
	{
		return E_FAIL;
	}

	HRESULT hr = m_pDirectSoundCreate8(pcGuidDevice, ppDS8, pUnkOuter);

	return hr;
}


