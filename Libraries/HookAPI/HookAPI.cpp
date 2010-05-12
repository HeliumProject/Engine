#include "HookApi.h"

#include "APIInfo.h"
#include "InjectLib.h"
#include "ProcessModule.h"

#include <time.h>
#include <stdio.h>

#pragma check_stack(off)

APIInfo g_APIInfo;
char g_DLLName[MAX_PATH];

bool HookAPI::Initialize( const char* dll_name, HOOKINFO* info, int count )
{
  strcpy(g_DLLName, dll_name);

  for ( int i=0; i<count; i++ )
  {
    if ( info[i].module_name == NULL )
    {
      continue;
    }

    HMODULE hModule;
    if ( (hModule = GetModuleHandle(info[i].module_name)) == NULL )
    {
      printf( "Error: Unable to get module %s\n", info[i].module_name );
      continue;
    }

    APIFUNC old_api;
    if ( (old_api = (APIFUNC)GetProcAddress(hModule, info[i].api_name)) == NULL )
    {
      printf( "Error: Unable to find %s in module %s\n", info[i].api_name, info[i].module_name );
      continue;
    }

    APIFUNC new_api;
    if ( (new_api = (APIFUNC)info[i].new_api) == NULL )
    {
      printf( "Error: Null hooking API for %s\n", info[i].api_name );
      continue;
    }

    APIINFO* pAPIInfo;
    if ( (pAPIInfo = g_APIInfo.Add(info[i].module_name, info[i].api_name, info[i].my_api_name, info[i].param_count, old_api, new_api, info[i].friend_my_api_name, info[i].start_pos)) == NULL )
    {
      printf( "Error: Unable to add APIINFO for %s\n", info[i].api_name );
      break;
    }
  }

  HMODULE hModule;
  APIFUNC old_api;
  APIINFO* pAPIInfo;

  if ( (hModule = GetModuleHandle("Kernel32.dll")) == NULL )
  {
    printf( "Error: Unable to get module Kernel32.dll\n" );
    return false;
  }

  if ( (old_api = (APIFUNC)GetProcAddress(hModule, "CreateProcessA")) == NULL )
  {
    printf( "Error: Unable to find CreateProcessA in module Kernel32.dll\n" );
    return false;
  }

  if ( (pAPIInfo = g_APIInfo.Add("Kernel32.dll", "CreateProcessA", "CreateprocessA", 10, old_api, (APIFUNC)CreateProcessA)) == NULL )
  {
    printf( "Error: Unable to add APIINFO for CreateProcessA\n" );
    return false;
  }

  if ( (old_api = (APIFUNC)GetProcAddress(hModule, "CreateProcessW")) == NULL )
  {
    printf( "Error: Unable to find CreateProcessW in module Kernel32.dll\n" );
    return false;
  }

  if ( (pAPIInfo = g_APIInfo.Add("Kernel32.dll", "CreateProcessW", "CreateprocessW", 10, old_api, (APIFUNC)CreateProcessW)) == NULL )
  {
    printf( "Error: Unable to add APIINFO for CreateProcessW\n" );
    return false;
  }

  return true;
}

void HookAPI::HookOneAPI(APIINFO *pAPIInfo)
{
  if ( pAPIInfo->f_hooked )
  {
    return;
  }

  BYTE *papi = (BYTE *)pAPIInfo->old_api + pAPIInfo->start_pos;

  if ( !RemoveProtection( pAPIInfo ) )
  {
    printf("Error: RemoveProtection failed for %s\n", pAPIInfo->api_name);
    return;
  }

  memcpy( pAPIInfo->save_bytes, papi, CALL_BYTES_SIZE );

  papi[0] =0xE8;
  *(DWORD *)&papi[1] = (DWORD)ProcessCall - (DWORD)papi - CALL_BYTES_SIZE;

  pAPIInfo->f_hooked = true;

  printf("Hooked API: hModule=%x, %s - %x (%x, %x, %x, %x), start_pos: %d\n", 
    GetModuleHandle(pAPIInfo->module_name), pAPIInfo->api_name,
    papi, papi[0], papi[1], pAPIInfo->save_bytes[0], pAPIInfo->save_bytes[1], pAPIInfo->start_pos);
}

void HookAPI::UnhookOneAPI(APIINFO *pAPIInfo)
{
  if (!pAPIInfo->f_hooked)
  {
    return;
  }

  APIINFO *pinfoitr = g_APIInfo.Get();
  int f_already_unhooked = 0;
  while(pinfoitr)
  {
    if (pinfoitr == pAPIInfo)
    {
      break;
    }

    if (pinfoitr->old_api ==pAPIInfo->old_api)
    {
      f_already_unhooked = true;
      break;
    }

    pinfoitr = pinfoitr->pnext;
  }

  if (f_already_unhooked)
  {
    pAPIInfo->f_hooked = false;
  }

  BYTE *papi =(BYTE *)pAPIInfo->old_api +pAPIInfo->start_pos;

  if ( RemoveProtection(pAPIInfo) )
  {
    memcpy((PBYTE)papi, pAPIInfo->save_bytes, CALL_BYTES_SIZE);
  }
  else
  {
    printf("Error: RemoveProtection failed for %s\n", pAPIInfo->api_name);
  }

  DWORD dwScratch;
  VirtualProtect(papi, 20, pAPIInfo->old_protection_flags, &dwScratch);
  pAPIInfo->f_hooked =false;
}

void HookAPI::HookAllAPI()
{
  APIINFO *pAPIInfo =g_APIInfo.Get();

  while (pAPIInfo !=NULL)
  {
    HookOneAPI(pAPIInfo);
    pAPIInfo =pAPIInfo->pnext;
  }
}

void HookAPI::UnhookAllAPI()
{
  APIINFO *pAPIInfo =g_APIInfo.Get();

  while (pAPIInfo !=NULL)
  {
    UnhookOneAPI(pAPIInfo);
    pAPIInfo =pAPIInfo->pnext;
  }
}

bool HookAPI::HookProcess(DWORD process_id)
{
  HookAPI::ObtainSeDebugPrivilege();
  return InjectLib(process_id, g_DLLName);
}

bool HookAPI::UnhookProcess(DWORD process_id)
{
  HookAPI::ObtainSeDebugPrivilege();
  return EjectLib(process_id, g_DLLName);
}

static bool IfCanHook(DWORD process_id)
{
  DWORD cur_pid = GetCurrentProcessId();
  if ( process_id ==0 || process_id ==cur_pid )
  {
    return false;
  }

  ProcessModule pm;

  if ( pm.GetProcessModuleHandle(process_id, "smss.exe")
    || pm.GetProcessModuleHandle(process_id, "csrss.exe")
    || pm.GetProcessModuleHandle(process_id, "lsass.exe")	)
  {
    return false;
  }

  return true;
}

void HookAPI::HookAllProcesses()
{
  bool result;
  HookAPI::ObtainSeDebugPrivilege();
  ProcessModule pm;
  pm.EnumProcess();
  for(int i =0; i<(int)pm.m_dwProcessCount; i++)
  {
    if (IfCanHook(pm.m_dwProcessIDs[i]))
    {
      result = InjectLib(pm.m_dwProcessIDs[i], g_DLLName);
    }
  }
}

void HookAPI::UnhookAllProcesses()
{
  bool result;
  ProcessModule pm;
  pm.EnumProcess();
  for(int i =0; i<(int)pm.m_dwProcessCount; i++)
  {
    if (IfCanHook(pm.m_dwProcessIDs[i]))
    {
      result = EjectLib(pm.m_dwProcessIDs[i], g_DLLName);
    }
  }
}

#pragma warning ( disable : 4731 )

void HookAPI::ProcessCall()
{
  PBYTE pbAfterCall;
  PDWORD pdwParam;
  PDWORD pdwESP;
  DWORD dwParamSize;
  void *pvReturnAddr;
  DWORD dwReturnValue;
  BYTE cl_val;
  BYTE *papi;

  DWORD errcode = 0;

  PROCESS_INFORMATION *pi;

  _asm
  {
    Mov [cl_val], CL
    Mov EAX, [EBP + 4]
    Mov [pbAfterCall], EAX

    Mov EAX, [EBP + 8]
    Mov [pvReturnAddr], EAX

    Lea EAX, [EBP + 12]
    Mov [pdwParam], EAX
  }

  APIINFO *pAPIInfo;
  if ( (pAPIInfo = g_APIInfo.FindByOldAPI((APIFUNC)(pbAfterCall-CALL_BYTES_SIZE))) == NULL )
  {
    printf( "Error: ProcessCall cannot find API\n" );
    goto call_ret;
  }

  g_APIInfo.Lock(pAPIInfo);

  papi =(BYTE*) pAPIInfo->old_api + pAPIInfo->start_pos;

  dwParamSize = pAPIInfo->param_count * 4;

  errcode = 0;

  if (!RemoveProtection(pAPIInfo))
  {
    printf("Error: RemoveProtection failed for %s\n", pAPIInfo->api_name);
    goto call_ret;
  }

  memcpy(papi, pAPIInfo->save_bytes, CALL_BYTES_SIZE);

  if (pAPIInfo->my_friend_api_name[0])
  {
    printf("Restoring API: %s\n", pAPIInfo->my_friend_api_name);
    RestoreAPICodes(pAPIInfo->my_friend_api_name);
  }

  _asm
  {
    Sub ESP, [dwParamSize]
    Mov [pdwESP], ESP
  }

  memcpy(pdwESP, pdwParam, dwParamSize);

  _asm
  {
    Mov CL, [cl_val]
  }

  pAPIInfo->new_api();

  _asm
  {
    Push EAX
    Mov [dwReturnValue], EAX
  }

  errcode = GetLastError();

  if (!strcmp(pAPIInfo->api_name, "CreateProcessW") || !strcmp(pAPIInfo->api_name, "CreateProcessA") )
  {
    pi =(PROCESS_INFORMATION *)pdwParam[9];
    if (pi->hProcess)
    {
      ObtainSeDebugPrivilege();
      InjectLib(pi->hProcess, g_DLLName);
    }
    else
    {
      printf("Error: CreateProcess and hProcess == NULL\n");
    }
  }

  if (pAPIInfo->my_friend_api_name[0])
  {
    printf("Restoring API: %s\n", pAPIInfo->my_friend_api_name);
    RestoreAPICodes(pAPIInfo->my_friend_api_name);
  }

  papi[0] =0xE8;
  *(DWORD *)&papi[1] =(DWORD)ProcessCall - (DWORD)papi - CALL_BYTES_SIZE;

  g_APIInfo.Unlock(pAPIInfo);

call_ret:
  SetLastError(errcode);

  _asm
  {
    Pop EAX
    Mov ECX, [dwParamSize]
    Mov EDX, [pvReturnAddr]
    Pop EDI
    Pop ESI
    Pop EBX
    Mov ESP, EBP
    Pop EBP
    Add ESP, 8
    Add ESP, ECX
    Push EDX
    Ret
  }
}

void HookAPI::ChangeAPICodes(char *my_api_name)
{
  APIINFO *pAPIInfo =g_APIInfo.FindByMyAPIName(my_api_name);
  if (pAPIInfo==NULL)
  {
    return;
  }

  BYTE *papi =(BYTE *)pAPIInfo->old_api+pAPIInfo->start_pos;

  papi[0] =0xE8;
  *(DWORD *)&papi[1] =(DWORD)ProcessCall -(DWORD)papi -CALL_BYTES_SIZE;
}

void HookAPI::RestoreAPICodes(char *my_api_name)
{
  APIINFO *pAPIInfo =g_APIInfo.FindByMyAPIName(my_api_name);
  if (pAPIInfo ==NULL || pAPIInfo->f_hooked ==false)
  {
    return;
  }

  g_APIInfo.Lock(pAPIInfo);
  memcpy((PBYTE)pAPIInfo->old_api+pAPIInfo->start_pos, pAPIInfo->save_bytes, CALL_BYTES_SIZE);
  g_APIInfo.Unlock(pAPIInfo);
}

bool HookAPI::RemoveProtection(APIINFO *pAPIInfo)
{
  BYTE *papi =(BYTE *)pAPIInfo->old_api+pAPIInfo->start_pos;

  MEMORY_BASIC_INFORMATION mbi;
  DWORD dwProtectionFlags;
  DWORD dwScratch;

  VirtualQuery(papi, &mbi, sizeof(mbi));

  dwProtectionFlags =mbi.Protect;
  pAPIInfo->old_protection_flags =dwProtectionFlags;

  dwProtectionFlags =PAGE_READWRITE;

  bool ret = VirtualProtect(papi, 20, dwProtectionFlags, &dwScratch) == TRUE;
  if (!ret)
  {
    printf("Error: VirtualProtect failed with %d\n", GetLastError());
  }

  return ret;
}

DWORD WINAPI HookAPI::CreateProcessW(
                                     LPCWSTR lpApplicationName,
                                     LPWSTR lpCommandLine, 
                                     LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                     LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                     BOOL bInheritHandles,
                                     DWORD dwCreationFlags,
                                     LPVOID lpEnvironment,
                                     LPCWSTR lpCurrentDirectory,
                                     LPSTARTUPINFOW lpStartupInfo,
                                     LPPROCESS_INFORMATION lpProcessInformation
                                     )
{
  DWORD result = ::CreateProcessW(lpApplicationName,
    lpCommandLine, lpProcessAttributes,
    lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
    lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

  printf("CreateProcessW happened\n");

  return result;
}

DWORD WINAPI HookAPI::CreateProcessA(
                                     LPCSTR lpApplicationName,
                                     LPSTR lpCommandLine, 
                                     LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                     LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                     BOOL bInheritHandles,
                                     DWORD dwCreationFlags,
                                     LPVOID lpEnvironment,
                                     LPCSTR lpCurrentDirectory,
                                     LPSTARTUPINFO lpStartupInfo,
                                     LPPROCESS_INFORMATION lpProcessInformation
                                     )
{
  DWORD result = ::CreateProcessA(lpApplicationName,
    lpCommandLine, lpProcessAttributes,
    lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
    lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

  printf("CreateProcessA happened\n");

  return result;
}

BOOL HookAPI::ObtainSeDebugPrivilege()
{
  TOKEN_PRIVILEGES TokenPrivileges;
  TOKEN_PRIVILEGES PreviousTokenPrivileges;
  LUID luid;
  HANDLE hToken;
  DWORD dwPreviousTokenPrivilegesSize = sizeof(TOKEN_PRIVILEGES);

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
  {
    return false;
  }

  if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
  {
    return false;
  }

  TokenPrivileges.PrivilegeCount            = 1;
  TokenPrivileges.Privileges[0].Luid        = luid;
  TokenPrivileges.Privileges[0].Attributes  = 0;

  if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), &PreviousTokenPrivileges, &dwPreviousTokenPrivilegesSize))
  {
    return false;
  }

  PreviousTokenPrivileges.PrivilegeCount             = 1;
  PreviousTokenPrivileges.Privileges[0].Luid         = luid;
  PreviousTokenPrivileges.Privileges[0].Attributes  |= SE_PRIVILEGE_ENABLED;

  if (!AdjustTokenPrivileges(hToken, FALSE, &PreviousTokenPrivileges, dwPreviousTokenPrivilegesSize, NULL, NULL))
  {
    return false;
  }

  return true;
}