#include "InjectLib.h"
#include "ProcessModule.h"

#include <stdio.h>

bool InjectLib(DWORD process_id, char *lib_name)
{
	PTHREAD_START_ROUTINE pfnRemote = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");
  if(pfnRemote ==NULL)
  {
		return false;
  }

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if(hProcess ==NULL)
	{
		return false;
	}

	int mem_size = strlen(lib_name)+1;
	void *premote_mem = VirtualAllocEx(hProcess, NULL, mem_size, MEM_COMMIT, PAGE_READWRITE);
	if (premote_mem ==NULL)
	{
		CloseHandle(hProcess);
		return false;
	}

	int ret = WriteProcessMemory(hProcess, premote_mem, lib_name, mem_size,NULL);
	if(ret == STATUS_ACCESS_VIOLATION || ret ==false)
	{
		VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
		CloseHandle(hProcess); 
		return false;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pfnRemote, premote_mem, 0, NULL);
	if(hThread ==NULL)
	{
		VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return false;
	}
	
	WaitForSingleObject(hThread, INFINITE);

	VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
	CloseHandle(hProcess);
	CloseHandle(hThread);

	return true;
}

bool InjectLib(HANDLE hProcess, char *lib_name)
{
	if(hProcess == NULL)
	{
		return false;
	}

	PTHREAD_START_ROUTINE pfnRemote = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");
	if(pfnRemote ==NULL)
  {
		return false;
  }

	int mem_size =strlen(lib_name)+1;
	void *premote_mem = VirtualAllocEx(hProcess, NULL, mem_size, MEM_COMMIT, PAGE_READWRITE);
	if(premote_mem == NULL)
	{
		CloseHandle(hProcess);
		return false;
	}

	int ret = WriteProcessMemory(hProcess, premote_mem, lib_name, mem_size,NULL);
	if(ret == STATUS_ACCESS_VIOLATION || ret ==false)
	{
		VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
		return false;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pfnRemote, premote_mem, 0, NULL);
	if(hThread ==NULL)
	{
		VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
		return false;
	}
	
	WaitForSingleObject(hThread, INFINITE);

	VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
	CloseHandle(hThread);

	return true;
}

bool EjectLib(DWORD process_id, char *lib_name)
{
	PTHREAD_START_ROUTINE pfnRemote = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("Kernel32"), "FreeLibrary");
	if(pfnRemote ==NULL)
	{
		return false;
	}

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if(hProcess == NULL)
	{
		return false;
	}

	ProcessModule pm;
	HANDLE hModule = pm.GetProcessModuleHandle(process_id, lib_name);
	if(hModule ==NULL)
	{
		CloseHandle(hProcess);
		return true;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pfnRemote, hModule, 0, NULL);
	if(hThread ==NULL)
	{
		CloseHandle(hProcess);
		return false;
	}

  WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hProcess);
	CloseHandle(hThread);

	return true;
}