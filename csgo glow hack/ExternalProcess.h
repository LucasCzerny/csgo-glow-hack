#pragma once

#include <vector>

#include <Windows.h>
#include <TlHelp32.h>

class ExternalProcess
{
private:
	HANDLE m_hProc = 0x0;
	DWORD m_ProcId = 0;

public:
	ExternalProcess(const wchar_t* procName);
	~ExternalProcess();

	template <typename value>
	value ReadMemory(uintptr_t location)
	{
		value val;
		ReadProcessMemory(m_hProc, (void*)location, &val, sizeof(val), nullptr);
		return val;
	}

	template <typename value>
	void WriteMemory(uintptr_t destination, value val)
	{
		WriteProcessMemory(m_hProc, (void*)destination, &val, sizeof(val), nullptr);
	}

	uintptr_t GetModuleBaseAddress(const wchar_t* moduleName);

	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);

	inline DWORD GetProcessId() const { return m_ProcId; }

private:
	DWORD GetProcId(const wchar_t* procName);
};