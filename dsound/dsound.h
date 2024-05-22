#pragma once

#include <dsound.h>

#include "Logging.h"
#include "types.h"
#include "SafeWrite.h"
#include "patches.h"

typedef HRESULT(WINAPI *DirectSoundEnumerateAProc)(LPDSENUMCALLBACKA, LPVOID);
typedef HRESULT(WINAPI *DirectSoundCreate8Proc)(LPCGUID, LPDIRECTSOUND8*, LPUNKNOWN);

