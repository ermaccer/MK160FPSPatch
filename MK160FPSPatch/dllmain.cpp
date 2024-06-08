// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "utils/addr.h"
#include "utils/MemoryMgr.h"
#include "utils/Trampoline.h"
#include "utils/Patterns.h"

#include "minhook/include/MinHook.h"
#include "IniReader.h"

using namespace Memory::VP;
using namespace hook::txn;


static bool ms_b60FPSAllowed = true;


static int64(*orgGamelogicJump)(int64, char*, unsigned int, int, int, int, int, int, int);
int64 GamelogicJump(int64 gameInfoPtr, char* mkoName, unsigned int functionHash, int a3, int a4, int a5, int a6, int a7, int a8)
{
    if (strcmp(mkoName, "MapMode.mko") == 0)
    {
        // enter/return/start
        if (functionHash == 0xA1A4DF6C || functionHash == 0x44A477CC || functionHash == 0x4CA174CC || functionHash == 0x7DED6222)
            ms_b60FPSAllowed = true;
        else
            ms_b60FPSAllowed = false;
    }
    else
        ms_b60FPSAllowed = false;
  
    return orgGamelogicJump(gameInfoPtr, mkoName, functionHash, a3, a4, a5, a6, a7, a8);
}

static void (*orgSetFrameSkipping)(int status, int flags);
void SetFrameSkipping(int mode, int flags)
{
    if (ms_b60FPSAllowed)
        mode = 1;
    orgSetFrameSkipping(mode, flags);
}

void Init()
{
    CIniReader ini("");
    bool invasionsOnly = ini.ReadBoolean("Settings", "InvasionsOnly", false);

    MH_Initialize();
    static uintptr_t setFrameSkippingPtr = 0;

    setFrameSkippingPtr = (uintptr_t)get_pattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 8B DA 0F B6 F9 E8 ? ? ? ? 0F B6 F0 83 FB 08 75 09 40 88 3D ? ? ? ? EB 7D 8B C3");

    if (setFrameSkippingPtr)
    {
        MH_STATUS s = MH_CreateHook((void*)setFrameSkippingPtr, SetFrameSkipping, (void**)&orgSetFrameSkipping);

        if (s == MH_OK)
            MH_EnableHook((void*)setFrameSkippingPtr);
    }
    else
        MessageBoxA(0, "Failed to find SetFrameSkip pattern!", "MK1.60FPSPatch", MB_ICONERROR);

    if (invasionsOnly)
    {
        static uintptr_t gamelogicJumpPtr = 0;

        gamelogicJumpPtr = (uintptr_t)get_pattern("4C 8B DC 48 83 EC 68 0F B6 84 24 ? ? ? ? 49 89 5B 18 41 8B D8 49 89 6B 20 48 8B EA 45 88 4B C8 48 8D 15 ? ? ? ? 88 44 24 31 0F B6 84 24 ? ? ? ? 49 89 73 F8 48 8D B1 ? ? ? ? 88 44 24 32 0F B6 84 24");

        if (gamelogicJumpPtr)
        {
            MH_STATUS s = MH_CreateHook((void*)gamelogicJumpPtr, GamelogicJump, (void**)&orgGamelogicJump);

            if (s == MH_OK)
                MH_EnableHook((void*)gamelogicJumpPtr);
        }
        else
            MessageBoxA(0, "Failed to find GamelogicJump pattern!", "MK1.60FPSPatch", MB_ICONERROR);
    }
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Init();
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

