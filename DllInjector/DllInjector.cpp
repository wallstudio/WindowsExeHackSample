#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <psapi.h>
#include <iostream>
#include <format>
#include <array>
#include <regex>
#include <filesystem>
#include <exception>


HANDLE FindProcess(std::wstring pattern)
{
    std::wcout << std::format(L"search process ({0})", pattern) << std::endl;

    // アクティブなプロセスを列挙
    DWORD sizeInByte;
    auto processeIds = std::vector<DWORD>(2048);
    EnumProcesses(processeIds.data(), processeIds.size() * sizeof(DWORD), &sizeInByte);
    processeIds.resize(sizeInByte);

    // 正規表現にマッチするプロセスを検索
    auto patternReg = std::wregex(pattern);
    for (auto processId : processeIds)
    {
        auto processName = std::array<wchar_t, MAX_PATH>();
        HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        GetProcessImageFileNameW(handle, processName.data(), processName.size());
        if (std::regex_match(processName.data(), patternReg))
        {
            std::wcout << std::format(L"found process ({0} {1})", processName.data(), handle) << std::endl;
            return handle;
        }
    }
    throw std::runtime_error("not found process");
}

bool InjectDll(HANDLE target, std::filesystem::path dll)
{
    // ターゲットプロセスに新たなHeapを確保し、LoadLibraryWの引数を書き込む
    dll = std::filesystem::absolute(dll);
    SIZE_T size = ((dll.wstring().size() / 1024) + 1) * 1024 * sizeof(wchar_t);
    auto targetHeap = VirtualAllocEx(target, 0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (targetHeap == nullptr) throw std::runtime_error("failed alloc heap in target process");
    WriteProcessMemory(target, targetHeap, dll.c_str(), size, &size);

    // ターゲットプロセスでLoadLibraryWを実行するプロセスを起動
    auto kernel32 = GetModuleHandleW(L"kernel32");
    if (kernel32 == nullptr) throw std::runtime_error("failed load kernel32.dll");
    auto loadLibraryW = GetProcAddress(kernel32, "LoadLibraryW"); // 本プロセスとターゲットプロセスで同じアドレスにマッピングされている前提
    if (loadLibraryW == nullptr) throw std::runtime_error("failed load LoadLibraryW");
    auto thread = CreateRemoteThread(target, nullptr, 0, (LPTHREAD_START_ROUTINE)loadLibraryW, targetHeap, 0, nullptr);
    if (thread == nullptr) throw std::runtime_error("failed create thread in target process");
    
    // スレッド終了まで待機
    WaitForSingleObject(thread, INFINITE);
    std::wcout << L"finish thread" << std::endl;

    CloseHandle(thread);
    VirtualFreeEx(target, targetHeap, 0, MEM_RELEASE);
    return true;
}

int main()
{
    // 外部プロセスから、任意のDLLをロードさせる（DllMainにより任意の処理をさせることが可能）
    // https://github.com/i-saint/RemoteTalk/blob/89fa111/.RemoteTalk/Plugin/RemoteTalkVOICEROID/RemoteTalkVOICEROIDEx.cpp#L7
    // http://titech-ssr.blog.jp/archives/1047454763.html

    try
    {
        auto target = FindProcess(L".*Taskmgr.exe");
        InjectDll(target, L"TaskMangerHackDll.dll");
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
