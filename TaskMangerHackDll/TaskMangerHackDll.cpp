#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <stdlib.h>
#include <iostream>
#include <codecvt>
#include <vector>
#include <format>
#include <map>

// https://snoozy.hatenablog.com/entry/2020/03/28/001631
// https://qiita.com/cha1aza/items/f64dc4351517a2477ef1
// https://tech.blog.aerie.jp/entry/2016/01/13/013206

typedef struct Entry
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER Reserved[3];
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    ULONG BasePriority;
    HANDLE ProcessId;
    HANDLE InheritedFromProcessId;
};

std::map<std::wstring, std::wstring> g_FakeNames;

NTSTATUS WINAPI _NtQuerySystemInformation(SYSTEM_INFORMATION_CLASS infoClass, Entry* entry, ULONG numEntry, PULONG requierNumEntry)
{
    static auto base = (decltype(&_NtQuerySystemInformation))GetProcAddress(GetModuleHandle(L"ntdll"), "NtQuerySystemInformation"); // 上書き後もこれなら元の関数が取れる
    auto status = base(infoClass, entry, numEntry, requierNumEntry);

    if (STATUS_SUCCESS == status && SystemProcessInformation == infoClass)
    {
        // notepad.exeを隠蔽する
        auto notepadName = std::wstring(L"notepad.exe");
        for (auto _entry = entry;;)
        {
            if (_entry->NextEntryOffset == 0) break;

            auto next = (Entry*)((SIZE_T)_entry + (SIZE_T)_entry->NextEntryOffset);
            if (next->ImageName.Length > 0 && notepadName == next->ImageName.Buffer)
            {
                _entry->NextEntryOffset = next->NextEntryOffset == 0 ? 0 : (_entry->NextEntryOffset + next->NextEntryOffset);
            }
            // 次のEntry
            _entry = next;
        }

        // 名前を反転させる
        for (auto _entry = entry;;)
        {
            if (_entry->ImageName.Length > 0)
            {
                auto name = std::wstring(_entry->ImageName.Buffer);
                if (!g_FakeNames.contains(name))
                {
                    auto fake = name;
                    std::reverse(fake.begin(), fake.end());
                    g_FakeNames[name] = fake;
                }
                _entry->ImageName.Buffer = (wchar_t*)g_FakeNames[name].c_str();
            }
            // 次のEntry
            if (_entry->NextEntryOffset == 0) break;
            _entry = (Entry*)((SIZE_T)_entry + (SIZE_T)_entry->NextEntryOffset);
        }
    }
    return status;
}

IMAGE_THUNK_DATA* FindImportFromIAT(std::string functionName)
{
    OutputDebugStringA(std::format("search function ({0})", functionName).c_str());

    // ImportAddressTable（DLLの関数ポインタがマッピングされているテーブル）を取得
    auto head = (size_t)GetModuleHandleW(nullptr);
    auto dosHeader = (PIMAGE_DOS_HEADER)head; // dosHeader->e_magic == IMAGE_DOS_SIGNATURE
    auto ntHeader = (IMAGE_NT_HEADERS*)(head + dosHeader->e_lfanew);
    auto importDescs = (IMAGE_IMPORT_DESCRIPTOR*)(head + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    // 検索
    for (auto importDesc = importDescs; importDesc->Name != NULL; importDesc++)
    {
        auto dllName = (char*)(head + importDesc->Name);
        auto importNameTable = (IMAGE_THUNK_DATA*)(head + importDesc->OriginalFirstThunk); // 名前or序数が記録されたテーブル
        auto imprortAddressTable = (IMAGE_THUNK_DATA*)(head + importDesc->FirstThunk); // 解決された実アドレスが記録されたテーブル
        for (auto i = 0; importNameTable[i].u1.AddressOfData != 0; i++)
        {
            auto nameEntry = importNameTable[i];
            auto addressEntry = &imprortAddressTable[i];
            if (nameEntry.u1.Ordinal >> 63 == 1) continue; // 序数Import （アドレスの場合は64bit使い切らないのでUnionできる）

            auto name = (IMAGE_IMPORT_BY_NAME*)(head + nameEntry.u1.AddressOfData);
            if (functionName == name->Name)
            {
                return addressEntry;
            }
        }
    }

    throw std::runtime_error("not found function");
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID _)
{
    // TaskManagerに寄生して、ntdll.dllのIATのNtQuerySystemInformationを書き換えて以下のいたずらをする
    // - notepad.exeを隠蔽
    // - プロセスのImage名をリバース

    if (reason == DLL_PROCESS_ATTACH)
    {
        try
        {
            auto funcptr = FindImportFromIAT("NtQuerySystemInformation");
            DWORD oldProtection;
            VirtualProtect(funcptr, sizeof(LPVOID), PAGE_READWRITE, &oldProtection);
            funcptr->u1.Function = (LONGLONG)_NtQuerySystemInformation;
        }
        catch (std::exception e)
        {
            OutputDebugStringA(e.what());
        }
    }
    return TRUE;
}