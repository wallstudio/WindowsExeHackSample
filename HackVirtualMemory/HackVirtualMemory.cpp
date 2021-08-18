#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <stdlib.h>
#include <iostream>
#include <codecvt>
#include <thread>
#include <format>
#include <functional>
#include <chrono>

std::byte* SearchAddress(std::function<bool(std::byte*)> predicate)
{
	// ヒープ内のRegion順番に捜索する
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	MEMORY_BASIC_INFORMATION info;
	for (std::byte* address = nullptr; VirtualQueryEx(handle, address, &info, sizeof(info));)
	{
		// 実行可能な命令はMEM_IMAGE、一般データがMEM_PRIVATE
		if (info.Type == MEM_PRIVATE && info.State == MEM_COMMIT && info.Protect == PAGE_READWRITE)
		{
			for (size_t i = 0; i < info.RegionSize; i++)
			{
				// 1byteずつずらしながら照合
				if (predicate(address + i))
				{
					return address + i;
				}
			}
		}
		address += info.RegionSize;
	}
	throw std::runtime_error("not found match data in memory");
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID _)
{
	// Notepadに寄生して、VirtualMemoryを書き換える
	// このサンプルでは、データを書き換えているけど、プログラム、命令も書き換えることが可能
    // https://github.com/processhacker/processhacker/blob/e96989/ProcessHacker/memprv.c#L757
	// http://kylehalladay.com/blog/2020/05/20/Rendering-With-Notepad.html

	static bool isAlive = true;
	static std::thread thread;

    if (reason == DLL_PROCESS_ATTACH)
    {
		// 1秒感覚で「ゆかいあ」を「ゆかマキ」に書き換えるスレッド
		thread = std::thread([&]()
		{
			while (isAlive)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				try
				{
					while (true)
					{
						static auto targetPattern = std::wstring(L"ゆかいあ");
						static auto replacement = std::wstring(L"ゆかマキ");
						auto target = SearchAddress([=](std::byte* pointer)
						{
							return pointer != nullptr
								&& (void*)pointer != targetPattern.data()
								&& 0 == memcmp(pointer, targetPattern.data(), (targetPattern.size() - 1) * sizeof(wchar_t));
						});
						DWORD oldProtection;
						VirtualProtect(target, sizeof(replacement), PAGE_READWRITE, &oldProtection);
						memcpy(target, replacement.data(), sizeof(replacement) * sizeof(char));
					}
				}
				catch (std::exception e)
				{
					OutputDebugStringA(e.what());
				}
			}
		});
    }

	if (reason == DLL_PROCESS_DETACH)
	{
		isAlive = false;
		thread.join();
	}

    return TRUE;
}