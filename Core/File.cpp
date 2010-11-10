//----------------------------------------------------------------------------------------------------------------------
// File.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/File.h"

#include "Core/Path.h"
#include "Foundation/Stream/FileStream.h"

namespace Lunar
{
    /// Free any statically allocated resources.
    ///
    /// This should only be called immediately prior to application exit.
    void File::Shutdown()
    {
        PlatformShutdown();
    }

    /// Attempt to open a file with a new file stream object.
    ///
    /// @param[in] pPath      Path name of the file to open.
    /// @param[in] modeFlags  Combination of FileStream::EMode flags specifying the mode in which to open the file.
    /// @param[in] bTruncate  If the FileStream::MODE_WRITE flag is set, true to truncate any existing file, false to
    ///                       append to any existing file.  This is ignored if FileStream::MODE_WRITE is not set.
    ///
    /// @return  Pointer to a FileStream instance opened for the specified file if it was successfully opened, null if
    ///          opening failed.  Note that the caller is responsible for deleting the FileStream instance when it is no
    ///          longer needed.
    FileStream* File::Open( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate )
    {
        FileStream* pStream = CreateStream();
        HELIUM_ASSERT( pStream );
        if( !pStream->Open( pPath, modeFlags, bTruncate ) )
        {
            delete pStream;
            return NULL;
        }

        return pStream;
    }

    /// Attempt to open a file with a new file stream object.
    ///
    /// @param[in] rPath      Path name of the file to open.
    /// @param[in] modeFlags  Combination of FileStream::EMode flags specifying the mode in which to open the file.
    /// @param[in] bTruncate  If the FileStream::MODE_WRITE flag is set, true to truncate any existing file, false to
    ///                       append to any existing file.  This is ignored if FileStream::MODE_WRITE is not set.
    ///
    /// @return  Pointer to a FileStream instance opened for the specified file if it was successfully opened, null if
    ///          opening failed.  Note that the caller is responsible for deleting the FileStream instance when it is no
    ///          longer needed.
    FileStream* File::Open( const String& rPath, uint32_t modeFlags, bool bTruncate )
    {
        return Open( *rPath, modeFlags, bTruncate );
    }

    /// Test whether a file or directory exists
    ///
    /// @param[in] rPath  Path name of the file or directory to check.
    ///
    /// @return  True if the file or directory exists, false if not.
    bool File::Exists( const String& rPath )
    {
        return Exists( *rPath );
    }

    /// Get the type of a file entry (i.e. regular file, directory, etc.).
    ///
    /// @param[in] rPath  Path name of the file or directory to check.
    ///
    /// @return  Type of the entry at the specified path, or TYPE_INVALID if the entry does not exist.
    File::EType File::GetFileType( const String& rPath )
    {
        return GetFileType( *rPath );
    }

    /// Get the size of a file.
    ///
    /// @param[in] rPath  Path name of the file to check.
    ///
    /// @return  Size of the file if it exists, is a file, and the size could be retrieved successfully, or -1 if the
    ///          file size could not be retrieved.
    int64_t File::GetSize( const String& rPath )
    {
        return GetSize( *rPath );
    }

    /// Get the timestamp of a file or directory.
    ///
    /// @param[in] rPath  Path name of the file or directory to check.
    ///
    /// @return  64-bit timestamp value, or INT64_MIN if acquiring the timestamp failed.  Note that the interpretation
    ///          of this value is platform dependent, but results should be consistent across the same platform.
    int64_t File::GetTimestamp( const String& rPath )
    {
        return GetTimestamp( *rPath );
    }

    /// Create a directory.
    ///
    /// @param[in] pPath       Path of the directory to create.
    /// @param[in] bRecursive  True to recursively create parent directories if they don't exist, false to only attempt
    ///                        to create the specified directory.
    ///
    /// @return  Identifier for the result of the directory creation attempt.
    File::EDirectoryCreateResult File::CreateDirectory( const tchar_t* pPath, bool bRecursive )
    {
        HELIUM_ASSERT( pPath );

        if( !bRecursive )
        {
            return PlatformCreateDirectory( pPath );
        }

        StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();
        StackMemoryHeap<>::Marker stackMarker( rStackHeap );

        size_t pathLength = StringLength( pPath );
        size_t pathBufferSize = sizeof( tchar_t ) * ( pathLength + 1 );

        tchar_t* pPathCopy = static_cast< tchar_t* >( rStackHeap.Allocate( pathBufferSize ) );
        HELIUM_ASSERT( pPathCopy );
        MemoryCopy( pPathCopy, pPath, pathBufferSize );

        return CreateDirectoryRecursive( pPathCopy, pathLength );
    }

    /// Create a directory.
    ///
    /// @param[in] rPath       Path of the directory to create.
    /// @param[in] bRecursive  True to recursively create parent directories if they don't exist, false to only attempt
    ///                        to create the specified directory.
    ///
    /// @return  Identifier for the result of the directory creation attempt.
    File::EDirectoryCreateResult File::CreateDirectory( const String& rPath, bool bRecursive )
    {
        const tchar_t* pPath = *rPath;
        HELIUM_ASSERT( pPath );

        if( !bRecursive )
        {
            return PlatformCreateDirectory( pPath );
        }

        StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();
        StackMemoryHeap<>::Marker stackMarker( rStackHeap );

        size_t pathLength = rPath.GetSize();
        size_t pathBufferSize = sizeof( tchar_t ) * ( pathLength + 1 );

        tchar_t* pPathCopy = static_cast< tchar_t* >( rStackHeap.Allocate( pathBufferSize ) );
        HELIUM_ASSERT( pPathCopy );
        MemoryCopy( pPathCopy, pPath, pathBufferSize );

        return CreateDirectoryRecursive( pPathCopy, pathLength );
    }

    /// Iterate over the listing of files in the given directory.
    ///
    /// Note that this will skip over virtual entries for referencing back to the current directory (".") and the parent
    /// directory ("..") if they exist.
    ///
    /// @param[in] rPath  Path of the directory over which to iterate.
    ///
    /// @return  Directory iterator instance.
    DirectoryIterator* File::IterateDirectory( const String& rPath )
    {
        return IterateDirectory( *rPath );
    }

    /// Helper function for recursive directory creation.
    ///
    /// @param[in] pPath       Modifiable path string.
    /// @param[in] pathLength  Cached path string length (path string must still be null-terminated).
    ///
    /// @return  Identifier for the result of the directory creation attempt.
    File::EDirectoryCreateResult File::CreateDirectoryRecursive( tchar_t* pPath, size_t pathLength )
    {
        size_t terminateOffset = pathLength;

        while( terminateOffset != 0 )
        {
            --terminateOffset;
            if( pPath[ terminateOffset ] != L_PATH_SEPARATOR_CHAR &&
                pPath[ terminateOffset ] != L_ALT_PATH_SEPARATOR_CHAR )
            {
                break;
            }
        }

        while( terminateOffset != 0 )
        {
            --terminateOffset;
            if( pPath[ terminateOffset ] == L_PATH_SEPARATOR_CHAR ||
                pPath[ terminateOffset ] == L_ALT_PATH_SEPARATOR_CHAR )
            {
                break;
            }
        }

        if( terminateOffset != 0 )
        {
            tchar_t character = pPath[ terminateOffset ];
            pPath[ terminateOffset ] = TXT( '\0' );

            EDirectoryCreateResult parentCreateResult = CreateDirectoryRecursive( pPath, terminateOffset );

            pPath[ terminateOffset ] = character;

            if( parentCreateResult == DIRECTORY_CREATE_RESULT_FAILED )
            {
                return DIRECTORY_CREATE_RESULT_FAILED;
            }
        }

        EDirectoryCreateResult createResult = PlatformCreateDirectory( pPath );

        return createResult;
    }
}
