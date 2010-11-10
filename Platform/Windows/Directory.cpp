#include "Platform/Directory.h"

#include "Platform/Error.h"
#include "Platform/Exception.h"
#include "Platform/Windows/Windows.h"

using namespace Helium;

void CopyFromWindowsStruct( const WIN32_FIND_DATA& windowsFile, FileFindData& ourFile )
{
    ourFile.m_Filename = windowsFile.cFileName;
    ourFile.m_CreationTime = ( (uint64_t)windowsFile.ftCreationTime.dwHighDateTime << 32 ) | windowsFile.ftCreationTime.dwLowDateTime;
    ourFile.m_ModificationTime = ( (uint64_t)windowsFile.ftLastWriteTime.dwHighDateTime << 32 ) | windowsFile.ftLastWriteTime.dwLowDateTime;
    ourFile.m_AccessTime = ( (uint64_t)windowsFile.ftLastAccessTime.dwHighDateTime << 32 ) | windowsFile.ftLastAccessTime.dwLowDateTime;
    ourFile.m_FileSize = ( (uint64_t)windowsFile.nFileSizeHigh << 32 ) | windowsFile.nFileSizeLow;

    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_READONLY ? FileAttributes::ReadOnly : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ? FileAttributes::Hidden : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ? FileAttributes::System : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? FileAttributes::Directory : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE ? FileAttributes::Archive : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_DEVICE ? FileAttributes::Device : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_NORMAL ? FileAttributes::Normal : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY ? FileAttributes::Temporary : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE ? FileAttributes::Sparse : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT ? FileAttributes::Redirect : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED ? FileAttributes::Compressed : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE ? FileAttributes::Offline : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED ? FileAttributes::NonIndexed : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED ? FileAttributes::Encrypted : 0;
    ourFile.m_FileAttributes |= windowsFile.dwFileAttributes & FILE_ATTRIBUTE_VIRTUAL ? FileAttributes::Virtual : 0;
}

DirectoryHandle Helium::FindFirst( const tstring& spec, FileFindData& data )
{
    // check that the input is not larger than allowed
    if ( spec.size() > MAX_PATH )
    {
        throw Helium::Exception( TXT( "Query string is too long (max buffer length is %d): %s" ), ( int ) MAX_PATH, spec.c_str() );
    }

    DirectoryHandle handle = NULL;
    
    WIN32_FIND_DATA foundFile;
    handle = ::FindFirstFile( spec.c_str(), &foundFile );

    if ( handle )
    {
        CopyFromWindowsStruct( foundFile, data );
    }
    else
    {
        DWORD error = GetLastError();
        if ( error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND ) 
        {
            throw Exception( TXT( "Error calling ::FindFirstFile: %s" ), GetErrorString( error ).c_str() );
        }
    }

    return handle;
}

bool Helium::FindNext( DirectoryHandle& handle, FileFindData& data )
{
    WIN32_FIND_DATA foundFile;
    if ( !::FindNextFile( handle, &foundFile ) )
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
    return TRUE == ::FindClose( handle );
}
