#include "RCSHooks.h"

#include "HookAPI/HookAPI.h"

HANDLE WINAPI HookedCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
  ::MessageBox( NULL, "CreateFileA", "Hooked", MB_OK );
	return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI HookedCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
  ::MessageBox( NULL, "CreateFileW", "Hooked", MB_OK );
	return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HOOKINFO g_APIToHook[] =
{
	{"KERNEL32.DLL", "CreateFileA", 7, "HookedCreateFileA", &HookedCreateFileA},
	{"KERNEL32.DLL", "CreateFileW", 7, "HookedCreateFileW", &HookedCreateFileW},
};

static int g_InitCount = 0;

void RCSHooks::Initialize()
{
  if ( ++g_InitCount == 1 )
  {

  }
}

void RCSHooks::Cleanup()
{
  if ( --g_InitCount == 0 )
  {

  }
}

bool RCSHooks::HookProcess(DWORD process_id)
{
  if ( g_InitCount > 0 )
  {
    return HookAPI::HookProcess(process_id);
  }

  return false;
}

bool RCSHooks::UnhookProcess(DWORD process_id)
{
  if ( g_InitCount > 0 )
  {
    return HookAPI::UnhookProcess(process_id);
  }

  return false;
}

void RCSHooks::HookAllProcesses()
{
  if ( g_InitCount > 0 )
  {
    HookAPI::HookAllProcesses();
  }
}

void RCSHooks::UnhookAllProcesses()
{
  if ( g_InitCount > 0 )
  {
    HookAPI::UnhookAllProcesses();
  }
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
    {
      char file[MAX_PATH];
      GetModuleFileName( hInstance, file, sizeof(file) );

      HookAPI::Initialize( file, g_APIToHook, sizeof( g_APIToHook ) / sizeof( HOOKINFO ) );
      HookAPI::HookAllAPI();

      break;
    }

  case DLL_PROCESS_DETACH:
    {
      HookAPI::UnhookAllAPI();

      break;
    }
  }

  return TRUE;
}
