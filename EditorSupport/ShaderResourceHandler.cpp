//----------------------------------------------------------------------------------------------------------------------
// ShaderResourceHandler.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EditorSupportPch.h"

#if L_EDITOR

#include "EditorSupport/ShaderResourceHandler.h"

#include "Foundation/Stream/BufferedStream.h"
#include "Foundation/File/File.h"
#include "Foundation/File/Path.h"
#include "Foundation/Stream/FileStream.h"
#include "Foundation/Stream/Stream.h"
#include "Foundation/StringConverter.h"
#include "Engine/BinarySerializer.h"
#include "PcSupport/ObjectPreprocessor.h"
#include "PcSupport/PlatformPreprocessor.h"

using namespace Lunar;

L_IMPLEMENT_OBJECT( ShaderResourceHandler, EditorSupport, 0 );

/// Constructor.
ShaderResourceHandler::ShaderResourceHandler()
{
}

/// Destructor.
ShaderResourceHandler::~ShaderResourceHandler()
{
}

/// @copydoc ResourceHandler::GetResourceType()
const GameObjectType* ShaderResourceHandler::GetResourceType() const
{
    return Shader::GetStaticType();
}

/// @copydoc ResourceHandler::GetSourceExtensions()
void ShaderResourceHandler::GetSourceExtensions(
    const tchar_t* const*& rppExtensions,
    size_t& rExtensionCount ) const
{
    static const tchar_t* extensions[] = { TXT( ".hlsl" ) };

    rppExtensions = extensions;
    rExtensionCount = HELIUM_ARRAY_COUNT( extensions );
}

/// @copydoc ResourceHandler::CacheResource()
bool ShaderResourceHandler::CacheResource(
    ObjectPreprocessor* pObjectPreprocessor,
    Resource* pResource,
    const String& rSourceFilePath )
{
    HELIUM_ASSERT( pObjectPreprocessor );
    HELIUM_ASSERT( pResource );

    const Shader* pShader = Reflect::AssertCast< const Shader >( pResource );
    GameObjectPath shaderPath = pShader->GetPath();

    HELIUM_TRACE( TRACE_INFO, TXT( "ShaderResourceHandler: Caching \"%s\".\n" ), *shaderPath.ToString() );

    DefaultAllocator allocator;

    FileStream* pSourceFileStream = File::Open( rSourceFilePath, FileStream::MODE_READ );
    if( !pSourceFileStream )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "ShaderResourceHandler: Source file for shader resource \"%s\" failed to open properly.\n" ),
            *shaderPath.ToString() );

        return false;
    }

    // Load the entire shader resource into memory.
    int64_t size64 = pSourceFileStream->GetSize();
    HELIUM_ASSERT( size64 != -1 );

    HELIUM_ASSERT( static_cast< uint64_t >( size64 ) <= static_cast< size_t >( -1 ) );
    if( size64 > static_cast< uint64_t >( static_cast< size_t >( -1 ) ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ShaderResourceHandler: Source file for shader resource \"%s\" is too large to fit into " )
            TXT( "memory for preprocessing.\n" ) ),
            *shaderPath.ToString() );

        delete pSourceFileStream;

        return false;
    }

    size_t size = static_cast< size_t >( size64 );

    void* pShaderData = allocator.Allocate( size );
    HELIUM_ASSERT( pShaderData );
    if( !pShaderData )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ShaderResourceHandler: Failed to allocate %" ) TPRIuSZ TXT( " bytes for loading the source " )
            TXT( "data of \"%s\" for preprocessing.\n" ) ),
            size,
            *shaderPath.ToString() );

        delete pSourceFileStream;

        return false;
    }

    BufferedStream( pSourceFileStream ).Read( pShaderData, 1, size );

    delete pSourceFileStream;

    // Parse all preprocessor toggle and selection tokens from the shader source.
    Shader::PersistentResourceData resourceData;

    const char* pLineEnd = static_cast< const char* >( pShaderData );
    const char* pShaderEnd = pLineEnd + size;

    do
    {
        const char* pLineStart = pLineEnd;
        while( pLineEnd < pShaderEnd )
        {
            char character = *pLineEnd;
            if( character == '\n' || character == '\r' )
            {
                break;
            }

            ++pLineEnd;
        }

        ParseLine( shaderPath, resourceData, pLineStart, pLineEnd );

        while( pLineEnd < pShaderEnd )
        {
            char character = *pLineEnd;
            if( character != '\n' && character != '\r' )
            {
                break;
            }

            ++pLineEnd;
        }
    } while( pLineEnd < pShaderEnd );

    allocator.Free( pShaderData );

    // Serialize the persistent shader resource data for each platform.
    BinarySerializer persistentDataSerializer;
    for( size_t platformIndex = 0; platformIndex < static_cast< size_t >( Cache::PLATFORM_MAX ); ++platformIndex )
    {
        PlatformPreprocessor* pPreprocessor = pObjectPreprocessor->GetPlatformPreprocessor(
            static_cast< Cache::EPlatform >( platformIndex ) );
        if( !pPreprocessor )
        {
            continue;
        }

        persistentDataSerializer.SetByteSwapping( pPreprocessor->SwapBytes() );
        persistentDataSerializer.BeginSerialize();
        resourceData.Serialize( persistentDataSerializer );
        persistentDataSerializer.EndSerialize();

        Resource::PreprocessedData& rPreprocessedData = pResource->GetPreprocessedData(
            static_cast< Cache::EPlatform >( platformIndex ) );
        rPreprocessedData.persistentDataBuffer = persistentDataSerializer.GetPropertyStreamBuffer();
        rPreprocessedData.subDataBuffers.Resize( 0 );
        rPreprocessedData.bLoaded = true;
    }

    return true;
}

/// Parse the given shader source line for toggle and select options.
///
/// @param[in] shaderPath     GameObject path of the shader resource being preprocessed (used for logging purposes
///                           only).
/// @param[in] rResourceData  Persistent shader resource data to update.
/// @param[in] pLineStart     Pointer to the first character in the line.
/// @param[in] pLineEnd       Pointer to the character just past the end of the line.
void ShaderResourceHandler::ParseLine(
                                      GameObjectPath shaderPath,
                                      Shader::PersistentResourceData& rResourceData,
                                      const char* pLineStart,
                                      const char* pLineEnd )
{
    HELIUM_UNREF( shaderPath );  // Not used if logging is disabled.

    HELIUM_ASSERT( pLineStart );
    HELIUM_ASSERT( pLineEnd >= pLineStart );

    const char linePrefix[] = "//!";
    const char toggleUserCommand[] = "@toggle";
    const char selectUserCommand[] = "@select";
    const char toggleSystemCommand[] = "@systoggle";
    const char selectSystemCommand[] = "@sysselect";

    size_t characterCount = static_cast< size_t >( pLineEnd - pLineStart );

    // Only process lines that start with the special comment prefix.
    if( characterCount < HELIUM_ARRAY_COUNT( linePrefix ) - 1 ||
        StringNCompare( pLineStart, linePrefix, HELIUM_ARRAY_COUNT( linePrefix ) - 1 ) != 0 )
    {
        return;
    }

    pLineStart += HELIUM_ARRAY_COUNT( linePrefix ) - 1;
    characterCount -= HELIUM_ARRAY_COUNT( linePrefix ) - 1;

    // Split the line based on groups of whitespaces.
    CharString line( pLineStart, characterCount );

    DynArray< CharString > splitLine;
    line.Split( splitLine, " \t\v\f", Invalid< size_t >(), true );

    // Ignore the first split if it's empty (will occur if the command is preceded by whitespaces).
    size_t splitCount = splitLine.GetSize();
    if( splitCount > 0 && splitLine[ 0 ].IsEmpty() )
    {
        splitLine.Remove( 0 );
        --splitCount;
    }

    // We need at least 2 splits (command and at least one command parameter).
    if( splitCount < 2 )
    {
        return;
    }

    // Process the command.
    DynArray< CharString > splitCommand;
    splitLine[ 0 ].Split( splitCommand, '_' );
    size_t commandSplitCount = splitCommand.GetSize();
    if( commandSplitCount < 1 || commandSplitCount > 2 )
    {
        // Invalid command format.
        return;
    }

    const CharString& rCommand = splitCommand[ 0 ];

    bool bToggleUserCommand = ( rCommand == toggleUserCommand );
    bool bSelectUserCommand = ( !bToggleUserCommand && rCommand == selectUserCommand );
    bool bToggleSystemCommand = ( !( bToggleUserCommand | bSelectUserCommand ) && rCommand == toggleSystemCommand );
    bool bSelectSystemCommand =
        ( !( bToggleUserCommand | bSelectUserCommand | bToggleSystemCommand ) && rCommand == selectSystemCommand );
    if( !( bToggleUserCommand | bSelectUserCommand | bToggleSystemCommand | bSelectSystemCommand ) )
    {
        return;
    }

    /// Make sure the option name (first parameter after the command name) is valid.
    String convertedString;
    HELIUM_VERIFY( ( StringConverter< char, tchar_t >::Convert( convertedString, splitLine[ 1 ] ) ) );
    Name optionName( convertedString );
    if( optionName.IsEmpty() )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "ShaderResourceHandler: Skipping empty option in shader resource \"%s\".\n" ),
            *shaderPath.ToString() );

        return;
    }

    // Make sure an existing toggle or selection option exists with the parsed option name.
    Shader::Options& rSystemOptions = rResourceData.GetSystemOptions();
    Shader::Options& rUserOptions = rResourceData.GetUserOptions();

    DynArray< Shader::Toggle >& rSystemToggles = rSystemOptions.GetToggles();
    DynArray< Shader::Select >& rSystemSelects = rSystemOptions.GetSelects();

    DynArray< Shader::Toggle >& rUserToggles = rUserOptions.GetToggles();
    DynArray< Shader::Select >& rUserSelects = rUserOptions.GetSelects();

    if( ParseLineDuplicateOptionCheck( optionName, rSystemToggles ) ||
        ParseLineDuplicateOptionCheck( optionName, rSystemSelects ) ||
        ParseLineDuplicateOptionCheck( optionName, rUserToggles ) ||
        ParseLineDuplicateOptionCheck( optionName, rUserSelects ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ShaderResourceHandler: Duplicate option name \"%s\" found in shader resource \"%s\".  Only " )
            TXT( "the first option will be used.\n" ) ),
            *optionName,
            *shaderPath.ToString() );

        return;
    }

    // Handle shader-specific command flags (option applies to all shader types if no flags are specified).
    uint32_t shaderFlags = ( 1 << RShader::TYPE_MAX ) - 1;
    if( commandSplitCount > 1 )
    {
        shaderFlags = 0;

        const CharString& rShaderFlags = splitCommand[ 1 ];
        size_t shaderFlagCount = rShaderFlags.GetSize();
        for( size_t flagIndex = 0; flagIndex < shaderFlagCount; ++flagIndex )
        {
            char flagCharacter = rShaderFlags[ flagIndex ];
            if( flagCharacter == 'v' )
            {
                shaderFlags |= ( 1 << RShader::TYPE_VERTEX );
            }
            else if( flagCharacter == 'p' )
            {
                shaderFlags |= ( 1 << RShader::TYPE_PIXEL );
            }
        }
    }

    // Parse the command parameters.
    if( bToggleUserCommand | bToggleSystemCommand )
    {
        Shader::Toggle* pToggle = ( bToggleUserCommand ? rUserToggles : rSystemToggles ).New();
        HELIUM_ASSERT( pToggle );
        pToggle->name = optionName;
        pToggle->shaderTypeFlags = shaderFlags;

        if( splitCount > 2 )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                ( TXT( "ShaderResourceHandler: Extra tokens for toggle command \"%s\" in shader resource \"%s\" " )
                TXT( "ignored.\n" ) ),
                *splitLine[ 1 ],
                *shaderPath.ToString() );
        }
    }
    else
    {
        if( splitCount < 3 || ( splitCount < 4 && splitLine[ 2 ] == "NONE" ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "ShaderResourceHandler: Missing options for select command \"%s\" in shader resource " )
                TXT( "\"%s\".\n" ) ),
                *splitLine[ 1 ],
                *shaderPath.ToString() );

            return;
        }

        Shader::Select* pSelect = ( bSelectUserCommand ? rUserSelects : rSystemSelects ).New();
        HELIUM_ASSERT( pSelect );
        pSelect->name = optionName;

        pSelect->shaderTypeFlags = shaderFlags;
        pSelect->bOptional = ( splitLine[ 2 ] == "NONE" );

        size_t choiceIndex = ( pSelect->bOptional ? 3 : 2 );
        for( ; choiceIndex < splitCount; ++choiceIndex )
        {
            HELIUM_VERIFY( ( StringConverter< char, tchar_t >::Convert( convertedString, splitLine[ choiceIndex ] ) ) );
            pSelect->choices.New( convertedString );
        }
    }
}

/// Check for an existing shader option with the same name in the given list.
///
/// @param[in] optionName  Option name for which to check.
/// @param[in] rOptions    List of existing options against which to check.
///
/// @return  True if a duplicate was found, false if not.
template< typename OptionType >
bool ShaderResourceHandler::ParseLineDuplicateOptionCheck( Name optionName, const DynArray< OptionType >& rOptions )
{
    size_t optionCount = rOptions.GetSize();
    for( size_t optionIndex = 0; optionIndex < optionCount; ++optionIndex )
    {
        if( rOptions[ optionIndex ].name == optionName )
        {
            return true;
        }
    }

    return false;
}

#endif  // L_EDITOR
