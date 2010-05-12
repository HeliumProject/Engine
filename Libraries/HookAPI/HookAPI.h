#pragma once

#include <windows.h>

#include "APIInfo.h"

struct HOOKINFO
{
	char* module_name;
	char* api_name;
	int   param_count;
	char* my_api_name;
	void* new_api;
	int   start_pos;
	char* friend_my_api_name;
};

namespace HookAPI  
{
  bool Initialize(const char* dll_name, HOOKINFO* info, int count);

  void HookOneAPI(APIINFO *api_info);
  void UnhookOneAPI(APIINFO *api_info);

  void HookAllAPI();
  void UnhookAllAPI();

  bool HookProcess(DWORD process_id);
  bool UnhookProcess(DWORD process_id);

  void HookAllProcesses();
  void UnhookAllProcesses();

  void ProcessCall();

  void ChangeAPICodes(char *my_api_name);
  void RestoreAPICodes(char *my_api_name);
  bool RemoveProtection(APIINFO *);

  DWORD WINAPI CreateProcessW(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine, 
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation);

  DWORD WINAPI CreateProcessA(
    LPCSTR lpApplicationName,
    LPSTR lpCommandLine, 
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCSTR lpCurrentDirectory,
    LPSTARTUPINFO lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation);

  BOOL ObtainSeDebugPrivilege();
};