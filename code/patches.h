#pragma once
#include "calls.h"
#define QPC_HOOK (void*)0x4E4040

__declspec(naked) void TextureFilteringHook() {
	__asm {
		mov cl, byte ptr ds:[0x53E8AB]
		test cl, cl
		je DONE
		mov cl, byte ptr ds:[0x556985]
		test cl, cl
		jne checkNext
		cmp eax, 1
		je RETZERO
	checkNext:
		mov cl, byte ptr ds:[0x556986]
		test cl, cl
		jne DONE
		cmp eax, 2
		jne DONE
	RETZERO:
		xor eax, eax
	DONE:
		ret
	}
}

__declspec(naked) void TextureColorDepthHook() {
	__asm {
		mov cl, byte ptr ds:[0x53E8AB]
		test cl, cl
		je DONE
		mov cl, byte ptr ds:[0x556987]
		test cl, cl
		jne DONE
		test eax, eax
		jle DONE
		xor eax, eax
	DONE :
		ret
	}
}

__declspec(naked) void AntiAliasingHook() {
	__asm {
		mov cl, byte ptr ds:[0x53E8AB]
		test cl, cl
		je DONE
		mov ecx, eax
		mov edx, 1
		shl edx, cl
		test dword ptr ds:[0x556980], edx
		jne DONE
		xor eax, eax
	DONE :
		ret
	}
}

void* get_IAT_address(BYTE* base, const char* dll_name, const char* search)
{
	IMAGE_DOS_HEADER* dos_header = (IMAGE_DOS_HEADER*)base;
	IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)(base + dos_header->e_lfanew);
	IMAGE_DATA_DIRECTORY section = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	IMAGE_IMPORT_DESCRIPTOR* import_descriptor = (IMAGE_IMPORT_DESCRIPTOR*)(base + section.VirtualAddress);
	for (size_t i = 0; import_descriptor[i].Name != NULL; i++)
	{
		if (!_stricmp((char*)(base + import_descriptor[i].Name), dll_name))
		{
			if (!import_descriptor[i].FirstThunk) { return nullptr; }
			IMAGE_THUNK_DATA* name_table = (IMAGE_THUNK_DATA*)(base + import_descriptor[i].OriginalFirstThunk);
			IMAGE_THUNK_DATA* import_table = (IMAGE_THUNK_DATA*)(base + import_descriptor[i].FirstThunk);
			for (; name_table->u1.Ordinal != NULL; ++name_table, ++import_table)
			{
				if (!IMAGE_SNAP_BY_ORDINAL(name_table->u1.Ordinal))
				{
					IMAGE_IMPORT_BY_NAME* import_name = (IMAGE_IMPORT_BY_NAME*)(base + name_table->u1.ForwarderString);
					char* func_name = &import_name->Name[0];
					if (!_stricmp(func_name, search)) { return &import_table->u1.AddressOfData; }
				}
			}
		}
	}
	return nullptr;
}

void writePatches() {
	WriteRelJump(0x4076D1, (UInt32)TextureFilteringHook);
	WriteRelJump(0x407641, (UInt32)TextureColorDepthHook);
	WriteRelJump(0x4075EE, (UInt32)AntiAliasingHook);
}

BOOL WINAPI stp_hook(HANDLE hThread, int nPriority)
{
	writePatches();
	BYTE* base = (BYTE*)GetModuleHandle(NULL);
	void* address = get_IAT_address(base, "kernel32.dll", "SetThreadPriority");
	// Restoring the original pointer
	SafeWrite32((UInt32)address, (UInt32)&SetThreadPriority);
	Log() << "Patches installed, original SetThreadPriority pointer restored.";
	return SetThreadPriority(hThread, nPriority);
}

// Hooking SetThreadPriority (called after the exe is unpacked)
void createLoaderHook() {
	BYTE* base = (BYTE*)GetModuleHandle(NULL);
	void* address = get_IAT_address(base, "kernel32.dll", "SetThreadPriority");
	if (address == QPC_HOOK)
	{
		SafeWrite32((UInt32)address, (UInt32)&stp_hook);
		Log() << "SetThreadPriority hook installed.";
	}
}


