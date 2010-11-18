//----------------------------------------------------------------------------------------------------------------------
// D3DIncludeHandler.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "PreprocessingPcPch.h"
#include "PreprocessingPc/D3DIncludeHandler.h"

#include "Core/File.h"
#include "Foundation/Stream/FileStream.h"
#include "Core/Path.h"
#include "Foundation/StringConverter.h"

namespace Lunar
{
    /// Constructor.
    ///
    /// @param[in] rShaderPath  Path to the shader file being processed.
    D3DIncludeHandler::D3DIncludeHandler( const String& rShaderPath )
    {
        Path::GetDirectoryName( m_shaderDirectory, rShaderPath );
    }

    /// Destructor.
    D3DIncludeHandler::~D3DIncludeHandler()
    {
    }

    /// Open a shader include file.
    ///
    /// @param[in]  includeType  Location of the include file.
    /// @param[in]  pFileName    Name of the include file.
    /// @param[in]  pParentData  Pointer to the container that includes the include file.
    /// @param[out] ppData       Pointer to the returned buffer that contains the include directives.  This pointer
    ///                          remains valid until Close() is called.
    /// @param[out] pBytes       Number of bytes returned in ppData.
    ///
    /// @return  S_OK if the include file was loaded successfully, an error code if not.
    ///
    /// @see Close()
    HRESULT D3DIncludeHandler::Open(
        D3D10_INCLUDE_TYPE /*includeType*/,
        LPCSTR pFileName,
        LPCVOID /*pParentData*/,
        LPCVOID* ppData,
        UINT* pBytes )
    {
        HELIUM_ASSERT( pFileName );
        HELIUM_ASSERT( ppData );
        HELIUM_ASSERT( pBytes );

        // Build the path to the file to include.
        String fileName;
        StringConverter< char, tchar_t >::Convert( fileName, pFileName );

        String includePath;
        Path::Combine( includePath, m_shaderDirectory, fileName );

        // Attempt to open and read the contents of the include file.
        FileStream* pIncludeFileStream = File::Open( includePath, FileStream::MODE_READ );
        if( !pIncludeFileStream )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "D3DIncludeHandler::Open(): Failed to open include file \"%s\" for reading.\n" ),
                *includePath );

            return E_FAIL;
        }

        int64_t fileSizeActual = pIncludeFileStream->GetSize();
        HELIUM_ASSERT( fileSizeActual >= 0 );
        if( fileSizeActual > UINT32_MAX )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "D3DIncludeHandler::Open(): Include file \"%s\" is larger than 4 GB and cannot be read.\n" ) ),
                *includePath );

            delete pIncludeFileStream;

            return E_FAIL;
        }

        uint32_t fileSize = static_cast< uint32_t >( fileSizeActual );

        DefaultAllocator allocator;
        void* pBuffer = allocator.Allocate( fileSize );
        if( !pBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "D3DIncludeHandler::Open(): Failed to allocate %" ) TPRIu32 TXT( " bytes for loading include " )
                  TXT( "file \"%s\".\n" ) ),
                fileSize,
                *includePath );

            delete pIncludeFileStream;

            return E_FAIL;
        }

        size_t bytesRead = pIncludeFileStream->Read( pBuffer, 1, fileSize );
        if( bytesRead != fileSize )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                ( TXT( "D3DIncludeHandler::Open(): Include file \"%s\" claimed to be %" ) TPRIu32 TXT( " bytes, but " )
                  TXT( "only %" ) TPRIuSZ TXT( " bytes could be read.\n" ) ),
                *includePath,
                fileSize,
                bytesRead );

            fileSize = static_cast< uint32_t >( bytesRead );
        }

        delete pIncludeFileStream;

        *ppData = pBuffer;
        *pBytes = fileSize;

        return S_OK;
    }

    /// Close a shader include file previously opened using Open().
    ///
    /// @param[in] pData  Pointer to the buffer previously returned by Open() that contains the include directives.
    ///
    /// @return  S_OK if the file was closed successfully, an error code if not.
    ///
    /// @see Open()
    HRESULT D3DIncludeHandler::Close( LPCVOID pData )
    {
        HELIUM_ASSERT( pData );

        DefaultAllocator().Free( const_cast< void* >( pData ) );

        return S_OK;
    }
}
