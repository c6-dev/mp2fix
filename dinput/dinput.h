#pragma once

#include <dinput.h>

#include "Logging.h"
#include "types.h"
#include "SafeWrite.h"
#include "patches.h"

typedef HRESULT(WINAPI *DirectInputCreateAProc)(HINSTANCE, DWORD, LPDIRECTINPUTA*, LPUNKNOWN);

