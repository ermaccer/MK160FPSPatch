// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "utils/addr.h"
#include "utils/MemoryMgr.h"
#include "utils/Trampoline.h"
#include "utils/Patterns.h"
#include "exports.h"
#include "minhook/include/MinHook.h"

using namespace Memory::VP;
using namespace hook::txn;

static void (*orgSetFrameSkipping)(int status, int flags);
void SetFrameSkipping(int mode, int flags)
{
    mode = 1;
    orgSetFrameSkipping(mode, flags);
}

void Init()
{
    MH_Initialize();
    static uintptr_t setFrameSkippingPtr = 0;

    setFrameSkippingPtr = (uintptr_t)get_pattern("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 0F B6 D9 8B FA 0F B6 0D ? ? ? ? 8D 41 FE");

    if (setFrameSkippingPtr)
    {
        MH_STATUS s = MH_CreateHook((void*)setFrameSkippingPtr, SetFrameSkipping, (void**)&orgSetFrameSkipping);

        if (s == MH_OK)
            MH_EnableHook((void*)setFrameSkippingPtr);
    }
    else
        MessageBoxA(0, "Failed to find SetFrameSkip pattern!", "MK1.60FPSPatch", MB_ICONERROR);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        LoadOriginalDLL();
        Init();
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

