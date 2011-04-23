#include "Platform/Directory.h"

#include "Platform/Assert.h"
#include "Platform/Error.h"
#include "Platform/Exception.h"
#include "Platform/Windows/Windows.h"

using namespace Helium;

void CopyFromWindowsStruct( const WIN32_FIND_DATA& windowsFile, FileFindData& ourFile )
{
    ourFile.m_Filename = windowsFile.cFileName;

#pragma TODO( "FileFindData should have a flag set as to whether or not we have file stat data retrieved." )
#ifndef _DEBUG
    ourFile.m_Stat.m_CreatedTime = ( (uint64_t)windowsFile.ftCreationTime.dwHighDateTime << 32 ) | windowsFile.ftCreationTime.dwLowDateTime;
    ourFile.m_Stat.m_ModifiedTime = ( (uint64_t)windowsFile.ftLastWriteTime.dwHighDateTime << 32 ) | windowsFile.ftLastWriteTime.dwLowDateTime;
    ourFile.m_Stat.m_AccessTime = ( (uint64_t)windowsFile.ftLastAccessTime.dwHighDateTime << 32 ) | windowsFile.ftLastAccessTime.dwLowDateTime;
    ourFile.m_Stat.m_Size = ( (uint64_t)windowsFile.nFileSizeHigh << 32 ) | windowsFile.nFileSizeLow;

    ourFile.m_Stat.m_Mode |= ( windowsFile.dwFileAttributes & FILE_ATTRIBUTE_READONLY ) ? FileModeFlags::Read : ( FileModeFlags::Read | FileModeFlags::Write );
    ourFile.m_Stat.m_Mode |= ( windowsFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? FileModeFlags::Directory : FileModeFlags::None;
    ourFile.m_Stat.m_Mode |= ( windowsFile.dwFileAttributes & FILE_ATTRIBUTE_NORMAL ) ? FileModeFlags::File : FileModeFlags::None;
    ourFile.m_Stat.m_Mode |= ( windowsFile.dwFileAttributes & FILE_ATTRIBUTE_DEVICE ) ? FileModeFlags::Special : FileModeFlags::None;
    ourFile.m_Stat.m_Mode |= ( windowsFile.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT ) ? FileModeFlags::Special : FileModeFlags::None;
    ourFile.m_Stat.m_Mode |= ( windowsFile.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) ? FileModeFlags::Special : FileModeFlags::None;
#endif
}

bool Helium::FindFirst( DirectoryHandle& handle, FileFindData& data )
{
    WIN32_FIND_DATA foundFile;
    handle.m_Handle = ::FindFirstFile( tstring( handle.m_Path + TXT( "/*" ) ).c_str(), &foundFile );

    if ( handle.m_Handle == INVALID_HANDLE_VALUE )
    {
        DWORD error = GetLastError();
        if ( error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND || error == ERROR_ACCESS_DENIED ) 
        {
            return false;
        }
        else
        {
            throw Exception( TXT( "Error calling ::FindFirstFile: %s" ), GetErrorString( error ).c_str() );
        }
    }

    CopyFromWindowsStruct( foundFile, data );
    return true;
}

bool Helium::FindNext( DirectoryHandle& handle, FileFindData& data )
{
    WIN32_FIND_DATA foundFile;
    if ( !::FindNextFile( handle.m_Handle, &foundFile ) )
    {
        DWORD error = GetLastError();
        if ( error != ERROR_NO_MORE_FILES ) 
        {
            throw Exception( TXT( "Error calling ::FindNextFile: %s" ), GetErrorString( error ).c_str() );
        }

        return false;
    }

    CopyFromWindowsStruct( foundFile, data );
    return true;
}

bool Helium::CloseFind( DirectoryHandle& handle )
{
    if ( ::FindClose( handle.m_Handle ) == 0 )
    {
        DWORD error = GetLastError();
        throw Exception( TXT( "Error calling ::FindClose: %s" ), GetErrorString( error ).c_str() );

        return false;
    }
    return true;
}

bool Helium::GetExtendedData( DirectoryHandle& handle, FileFindData& data )
{
#ifdef _DEBUG
    if ( !Helium::StatPath( tstring( handle.m_Path + TXT( "/" ) + data.m_Filename.c_str() ).c_str(), data.m_Stat ) )
    {
        return false;
    }

#endif

    return true;
}