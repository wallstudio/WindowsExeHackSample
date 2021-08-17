#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <psapi.h>
#include <array>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        // プロセスのファイル名を表示しておく
        auto processName = std::array<wchar_t, MAX_PATH>();
        GetProcessImageFileNameW(GetCurrentProcess(), processName.data(), processName.size());
        MessageBoxW(nullptr, processName.data(), L"ParasiteDll", MB_OK);
    }
    return TRUE;
}

