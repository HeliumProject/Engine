#pragma once

#include <windows.h>

bool InjectLib(DWORD process_id, char *lib_name);
bool InjectLib(HANDLE hProcess, char *lib_name);
bool EjectLib(DWORD process_id, char *lib_name);