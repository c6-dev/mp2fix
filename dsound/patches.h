#pragma once
#include "ModUtils/Patterns.h"
#include "calls.h"
#include "borderless.h"


UInt32 callAdr = NULL;
UInt32 jumpAddr = NULL;

bool bSkipLauncher = 0;
bool bSkipIntro = 0;
bool bHideCursorInWindowed = 0;

__declspec(naked) void SkipLauncher() {
	__asm {
		call callAdr
		add esp, 4
		mov eax, dword ptr ds:[ebp+10]
		mov ebx, 1
		jmp jumpAddr
	}
}


void loadIniOptions() {
	char filename[MAX_PATH];
	GetModuleFileNameA(NULL, filename, MAX_PATH);
	strcpy((char*)(strrchr(filename, '\\') + 1), "dsound.ini");
	bSkipLauncher = GetPrivateProfileIntA("MAIN", "bSkipLauncher", 0, filename);
	bSkipIntro = GetPrivateProfileIntA("MAIN", "bSkipIntro", 0, filename);
	borderless::bBorderlessWindowed = GetPrivateProfileIntA("MAIN", "bBorderlessWindowed", 0, filename);
	bHideCursorInWindowed = GetPrivateProfileIntA("MAIN", "bHideCursorInWindowed", 0, filename);
}
void writePatches() {
	if (bSkipLauncher) {
		auto skipLauncher = hook::pattern("C2 10 00 83 F8 4E").get_first(-19);
		callAdr = (UInt32)hook::pattern("6A 0C 68 0D 04 00 00 53 33 FF").get_first(-48);
		jumpAddr = (UInt32)hook::pattern("68 F0 00 00 00 68 25 04 00").get_first(-7);
		WriteRelJump((UInt32)skipLauncher, (UInt32)SkipLauncher);
	}
	if (bSkipIntro) {
		auto skipIntro = hook::pattern("8B 55 00 D9 EE 8B 42 48 51").get_first(-30);
		auto skipIntroJump = hook::pattern("8B 44 24 14 85 C0 ? ? 50").get_first();
		WriteRelJump((UInt32)skipIntro, (UInt32)skipIntroJump);
	}

	if (bHideCursorInWindowed) {
		PatchMemoryNop(0x457A34, 8);
	}

	WriteRelLibCall(0x60F16F, (UInt32)borderless::CreateWindowExA_Hook);
	WriteRelLibCall(0x60F08F, (UInt32)borderless::AdjustWindowRect_Hook);
	WriteRelLibCall(0x60F235, (UInt32)borderless::SetWindowPos_Hook);

}