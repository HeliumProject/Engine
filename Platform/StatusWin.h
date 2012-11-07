#pragma once

uint64_t FileTimeToUnixTime( FILETIME time );
void CopyFromWindowsAttributes( DWORD attrs, uint32_t mode );