// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "utils/addr.h"
#include "utils/MemoryMgr.h"
#include "utils/Trampoline.h"
#include "utils/Patterns.h"
#include "exports.h"
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

    setFrameSkippingPtr = (uintptr_t)get_pattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 0F B6 D9 8B FA 0F B6 0D ? ? ? ? 8D 41 FE 3C 01 40 0F 96 C6 83 FA 08");

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

        gamelogicJumpPtr = (uintptr_t)get_pattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 40 33 ED 45 33 FF 45 33 E4 48 89 6C 24 ? 4C 89 7C 24");

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

