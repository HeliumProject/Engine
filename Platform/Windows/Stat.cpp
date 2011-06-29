#include "PlatformPch.h"
#include "Platform/Stat.h"

#include <sys/stat.h>

bool Helium::StatPath( const tchar_t* path, Helium::Stat& stat )
{
    struct _stati64 windowsStats;
    bool result = ( _tstati64( path, &windowsStats ) == 0 );

    if ( result )
    {
        stat.m_Mode = 0;

        if ( ( windowsStats.st_mode & _S_IFREG ) == _S_IFREG )
            stat.m_Mode |= FileModeFlags::File;
        if ( ( windowsStats.st_mode & _S_IFDIR ) == _S_IFDIR )
            stat.m_Mode |= FileModeFlags::Directory;
        if ( ( windowsStats.st_mode & _S_IFIFO ) == _S_IFIFO )
            stat.m_Mode |= FileModeFlags::Pipe;
        if ( ( windowsStats.st_mode & _S_IFCHR ) == _S_IFCHR )
            stat.m_Mode |= FileModeFlags::Special;


        if ( ( windowsStats.st_mode & _S_IREAD ) == _S_IREAD )
            stat.m_Mode |= FileModeFlags::Read;
        if ( ( windowsStats.st_mode & _S_IWRITE ) == _S_IWRITE )
            stat.m_Mode |= FileModeFlags::Write;
        if ( ( windowsStats.st_mode & _S_IEXEC ) == _S_IEXEC )
            stat.m_Mode |= FileModeFlags::Execute;

        stat.m_AccessTime = windowsStats.st_atime;
        stat.m_CreatedTime = windowsStats.st_ctime;
        stat.m_ModifiedTime = windowsStats.st_mtime;
        stat.m_Size = windowsStats.st_size;
    }

    return result;
}

