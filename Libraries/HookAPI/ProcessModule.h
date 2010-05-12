#pragma once

#include <windows.h>
#include <tlhelp32.h>
typedef HANDLE (WINAPI *CREATETOOLHELP32SNAPSHOT_PROC)(DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL (WINAPI *PROCESS32FIRST_PROC)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI *PROCESS32NEXT_PROC)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI *MODULE32FIRST_PROC)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef BOOL (WINAPI *MODULE32NEXT_PROC)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

#define MAX_PROCESS_COUNT	512
#define MAX_MODULE_COUNT	500

class ProcessModule
{
public:
	ProcessModule();
	~ProcessModule();
	
private:
	char m_mod_name[MAX_MODULE_COUNT][100];
	CREATETOOLHELP32SNAPSHOT_PROC pCreateToolhelp32Snapshot;
	PROCESS32FIRST_PROC pProcess32First;
	PROCESS32NEXT_PROC pProcess32Next;
	MODULE32FIRST_PROC pModule32First;
	MODULE32NEXT_PROC pModule32Next;

public:
	DWORD m_dwProcessIDs[MAX_PROCESS_COUNT];
	DWORD m_dwProcessCount;
	HMODULE m_hMods[MAX_MODULE_COUNT];
	int m_nModCount;

	int EnumProcess();
	int EnumProcessModules(DWORD process_id);
	int GetModuleBaseNameA(int num, char *mod_name);
	void EndEnumProcessModules();

	HANDLE GetProcessModuleHandle(DWORD process_id, char *mod_name);
	DWORD GetProcessID(char *exe_name);
	BOOL ObtainSeDebugPrivilege(HANDLE hProcess);
};