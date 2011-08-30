//////----------------------------------------------------------------------------------------------------------------------
////// XmlPackageLoader.cpp
//////
////// Copyright (C) 2010 WhiteMoon Dreams, Inc.
////// All Rights Reserved
//////----------------------------------------------------------------------------------------------------------------------
////
//#include "PcSupportPch.h"
//#include "PcSupport/XmlPackageLoader.h"
//
//#include "Foundation/File/File.h"
//#include "Foundation/File/Path.h"
//#include "Foundation/File/Directory.h"
//#include "Foundation/Stream/FileStream.h"
//#include "Foundation/Stream/BufferedStream.h"
//#include "Foundation/AsyncLoader.h"
//#include "Engine/BinaryDeserializer.h"
//#include "Engine/CacheManager.h"
//#include "Engine/Config.h"
//#include "Engine/DirectDeserializer.h"
//#include "Engine/DirectSerializer.h"
//#include "Engine/NullLinker.h"
//#include "Engine/GameObjectLoader.h"
//#include "Engine/Resource.h"
//#include "PcSupport/ObjectPreprocessor.h"
//#include "PcSupport/ResourceHandler.h"
//
//#include "expat.h"
//
//using namespace Helium;
//
//// Property parsing stack layer.
//struct XmlPackageParseProperty
//{
//    // Property name.
//    Name name;
//    // Current array index (invalid if not parsing an array).
//    uint32_t index;
//    // True if we're actively parsing a property value.
//    bool bParsing;
//    // True if the current struct/array property has been closed.
//    bool bClosed;
//};
//
//// Parsing context.
//struct XmlPackageParseContext
//{
//    // XML parser instance.
//    XML_Parser pParser;
//
//    // Package path.
//    GameObjectPath packagePath;
//    // GameObject list.
//    DynArray< XmlPackageLoader::SerializedObjectData >* pObjects;
//
//    // Current parsing depth.
//    size_t depth;
//    // Depth of the last illegal tag encountered (parsing halted until we break out of this tag).
//    size_t illegalDepth;
//
//    // Index of the object currently being parsed.
//    uint32_t currentObjectIndex;
//
//    // Stack specifying the property being parsed (for handling properties within arrays or structs).
//    DynArray< XmlPackageParseProperty > propertyStack;
//    // Buffer of character data streamed in for the current property.
//    DynArray< XML_Char > propertyTextBuffer;
//};
//
///// Parsing handler for start tags.
/////
///// @param[in] pUserData  User data pointer.
///// @param[in] pName      Tag name.
///// @param[in] ppAtts     Array of pairs of tag attribute names and their values.
/////
///// @see XmlPackageEndElementHandler(), XmlPackageCharacterDataHandler()
//static void XMLCALL XmlPackageStartElementHandler(
//    void* pUserData,
//    const XML_Char* pName,
//    const XML_Char** ppAtts )
//{
//    HELIUM_ASSERT( pUserData );
//    HELIUM_ASSERT( pName );
//    HELIUM_ASSERT( ppAtts );
//
//    XmlPackageParseContext* pContext = static_cast< XmlPackageParseContext* >( pUserData );
//
//    uint64_t line = XML_GetCurrentLineNumber( pContext->pParser );
//    uint64_t column = XML_GetCurrentColumnNumber( pContext->pParser );
//    HELIUM_UNREF( line );
//    HELIUM_UNREF( column );
//
//    // Increase the depth and validate the tag.
//    ++pContext->depth;
//
//    if( IsValid( pContext->illegalDepth ) )
//    {
//        // We're still within an illegal tag, so skip over parsing for now.
//        return;
//    }
//
//    if( pContext->depth == 1 )
//    {
//        // Top level tag; must be a "package" tag.
//        if( StringCompare( pName, TXT( "package" ) ) != 0 )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Top-level tag " )
//                TXT( "is \"%s\"; expected \"package\" tag.\n" ) ),
//                line,
//                column,
//                pName );
//
//            pContext->illegalDepth = pContext->depth;
//        }
//
//        return;
//    }
//
//    if( pContext->depth == 2 )
//    {
//        HELIUM_ASSERT( IsInvalid( pContext->currentObjectIndex ) );
//
//        // Second-level tag; must be an "object" tag.
//        if( StringCompare( pName, TXT( "object" ) ) != 0 )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Encountered " )
//                TXT( "\"%s\" tag as a top-level tag within a \"package\" tag; expected \"object\" tag.\n" ) ),
//                line,
//                column,
//                pName );
//
//            pContext->illegalDepth = pContext->depth;
//
//            return;
//        }
//
//        // Parse the name, type, and template attributes.
//        String objectName;
//        Name typeName;
//        GameObjectPath templatePath( NULL_NAME );
//
//        while( *ppAtts != NULL )
//        {
//            const XML_Char* pAttName = *( ppAtts++ );
//            const XML_Char* pAttValue = *( ppAtts++ );
//            HELIUM_ASSERT( pAttValue );
//
//            if( StringCompare( pAttName, TXT( "name" ) ) == 0 )
//            {
//                objectName = HELIUM_OBJECT_PATH_CHAR;
//                objectName += pAttValue;
//
//                continue;
//            }
//
//            if( StringCompare( pAttName, TXT( "type" ) ) == 0 )
//            {
//                typeName.Set( pAttValue );
//                if( !GameObjectType::Find( typeName ) )
//                {
//                    HELIUM_TRACE(
//                        TRACE_ERROR,
//                        ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") \"%s\" " )
//                        TXT( "is not a valid object type.\n" ) ),
//                        line,
//                        column,
//                        pAttValue );
//                }
//
//                continue;
//            }
//
//            if( StringCompare( pAttName, TXT( "template" ) ) == 0 )
//            {
//                templatePath.Clear();
//
//                if( pAttValue[ 0 ] != static_cast< XML_Char >( '\0' ) )
//                {
//                    if( !templatePath.Set( pAttValue ) )
//                    {
//                        HELIUM_TRACE(
//                            TRACE_ERROR,
//                            ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") " )
//                            TXT( "\"%s\" is not a valid object path.\n" ) ),
//                            line,
//                            column,
//                            pAttValue );
//                    }
//                }
//
//                continue;
//            }
//        }
//
//        // Begin parsing the object if we have valid starting data.
//        if( objectName.IsEmpty() )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Missing " )
//                TXT( "\"name\" parameter in \"object\" tag.\n" ) ),
//                line,
//                column );
//
//            pContext->illegalDepth = pContext->depth;
//
//            return;
//        }
//
//        if( typeName.IsEmpty() )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Missing valid " )
//                TXT( "\"type\" parameter in \"object\" tag.\n" ) ),
//                line,
//                column );
//
//            pContext->illegalDepth = pContext->depth;
//
//            return;
//        }
//
//        GameObjectPath objectPath;
//        if( !objectPath.Join( pContext->packagePath, *objectName ) )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") GameObject path is " )
//                TXT( "not valid.\n" ) ),
//                line,
//                column );
//
//            pContext->illegalDepth = pContext->depth;
//
//            return;
//        }
//
//        XmlPackageLoader::SerializedObjectData objectData;
//        objectData.objectPath = objectPath;
//        objectData.typeName = typeName;
//        objectData.templatePath = templatePath;
//
//        DynArray< XmlPackageLoader::SerializedObjectData >& rObjects = *pContext->pObjects;
//        size_t existingObjectCount = rObjects.GetSize();
//        size_t objectIndex;
//        for( objectIndex = 0; objectIndex < existingObjectCount; ++objectIndex )
//        {
//            if( rObjects[ objectIndex ].objectPath == objectPath )
//            {
//                rObjects[ objectIndex ] = objectData;
//
//                break;
//            }
//        }
//
//        if( objectIndex >= existingObjectCount )
//        {
//            HELIUM_ASSERT( objectIndex == existingObjectCount );
//            HELIUM_ASSERT( existingObjectCount < UINT32_MAX );
//            rObjects.Add( objectData );
//        }
//
//        pContext->currentObjectIndex = static_cast< uint32_t >( objectIndex );
//
//        return;
//    }
//
//    // Handle "property" tags.
//    if( StringCompare( pName, TXT( "property" ) ) == 0 )
//    {
//        // Make sure we are editing an object.
//        if( IsInvalid( pContext->currentObjectIndex ) )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Encountered " )
//                TXT( "\"property\" tag outside of a valid \"object\" tag; ignoring.\n" ) ),
//                line,
//                column );
//
//            pContext->illegalDepth = pContext->depth;
//
//            return;
//        }
//
//        DynArray< XmlPackageParseProperty >& rPropertyStack = pContext->propertyStack;
//        HELIUM_ASSERT( !rPropertyStack.IsEmpty() );
//
//        XmlPackageParseProperty& rPropertyTop = rPropertyStack.GetLast();
//
//        // Make sure haven't encountered a property tag within another property tag outside the context of a
//        // struct or array tag.
//        if( rPropertyTop.bParsing || rPropertyTop.bClosed )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Encountered " )
//                TXT( "\"property\" tag within another \"property\" tag outside the context of a \"struct\" or " )
//                TXT( "\"array\" tag; ignoring.\n" ) ),
//                line,
//                column );
//
//            pContext->illegalDepth = pContext->depth;
//
//            return;
//        }
//
//        // If we're parsing an array, simply update the parsing status.
//        if( IsValid( rPropertyTop.index ) )
//        {
//            rPropertyTop.bParsing = true;
//
//            return;
//        }
//
//        HELIUM_ASSERT( rPropertyTop.name.IsEmpty() );
//
//        // Parse the property attributes.
//        Name propertyName( NULL_NAME );
//
//        while( *ppAtts != NULL )
//        {
//            const XML_Char* pAttName = *( ppAtts++ );
//            const XML_Char* pAttValue = *( ppAtts++ );
//            HELIUM_ASSERT( pAttValue );
//
//            if( StringCompare( pAttName, TXT( "name" ) ) == 0 )
//            {
//                propertyName = Name( pAttValue );
//
//                continue;
//            }
//
//            HELIUM_TRACE(
//                TRACE_WARNING,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Unknown " )
//                TXT( "\"property\" tag attribute \"%s\" encountered.\n" ) ),
//                line,
//                column,
//                pAttName );
//        }
//
//        // Begin parsing the property if we have valid starting data.
//        if( propertyName.IsEmpty() )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Incomplete " )
//                TXT( "\"property\" tag encountered.\n" ) ),
//                line,
//                column );
//
//            pContext->illegalDepth = pContext->depth;
//
//            return;
//        }
//
//        rPropertyTop.name = propertyName;
//        rPropertyTop.bParsing = true;
//
//        return;
//    }
//
//    // Handle "struct" and "array" tags.
//    bool bArrayTag = ( StringCompare( pName, TXT( "array" ) ) == 0 );
//    if( bArrayTag || StringCompare( pName, TXT( "struct" ) ) == 0 )
//    {
//        // Make sure we are actively parsing a property.
//        DynArray< XmlPackageParseProperty >& rPropertyStack = pContext->propertyStack;
//        HELIUM_ASSERT( !rPropertyStack.IsEmpty() );
//
//        XmlPackageParseProperty& rPropertyTop = rPropertyStack.GetLast();
//
//        if( rPropertyTop.name.IsEmpty() )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Encountered " )
//                TXT( "\"%s\" tag outside the scope of a \"property\" tag; ignoring.\n" ) ),
//                line,
//                column,
//                ( bArrayTag ? TXT( "array" ) : TXT( "struct" ) ) );
//
//            pContext->illegalDepth = pContext->depth;
//
//            return;
//        }
//
//        if( rPropertyTop.bClosed )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Encountered " )
//                TXT( "\"%s\" tag within a property that has already been parsed as an array or struct; " )
//                TXT( "ignoring.\n" ) ),
//                line,
//                column,
//                ( bArrayTag ? TXT( "array" ) : TXT( "struct " ) ) );
//
//            pContext->illegalDepth = pContext->depth;
//
//            return;
//        }
//
//        if( !rPropertyTop.bParsing )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Encountered " )
//                TXT( "\"%s\" tag within an \"array\" or \"struct\" tag; ignoring.\n" ) ),
//                line,
//                column,
//                ( bArrayTag ? TXT( "array" ) : TXT( "struct" ) ) );
//
//            pContext->illegalDepth = pContext->depth;
//
//            return;
//        }
//
//        if( bArrayTag && IsValid( rPropertyTop.index ) )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Encountered " )
//                TXT( "nested \"array\" tag, which is not supported (arrays can only contain singular " )
//                TXT( "properties or structs); ignoring.\n" ) ),
//                line,
//                column );
//
//            pContext->illegalDepth = pContext->depth;
//
//            return;
//        }
//
//        // Stop parsing the property text; text should be provided by sub-properties.
//        pContext->propertyTextBuffer.Resize( 0 );
//        rPropertyTop.bParsing = false;
//
//        // If we're parsing an array, initialize the array index, otherwise push a new property to scope.
//        if( bArrayTag )
//        {
//            rPropertyTop.index = 0;
//        }
//        else
//        {
//            XmlPackageParseProperty* pPropertyNew = rPropertyStack.New();
//            HELIUM_ASSERT( pPropertyNew );
//            pPropertyNew->name.Clear();
//            SetInvalid( pPropertyNew->index );
//            pPropertyNew->bParsing = false;
//            pPropertyNew->bClosed = false;
//        }
//
//        return;
//    }
//
//    // Ignore unsupported tags.
//    HELIUM_TRACE(
//        TRACE_WARNING,
//        ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Unknown tag \"%s\" " )
//        TXT( "encountered; ignoring.\n" ) ),
//        line,
//        column,
//        pName );
//
//    pContext->illegalDepth = pContext->depth;
//}
//
///// Parsing handler for end tags.
/////
///// @param[in] pUserData  User data pointer.
///// @param[in] pName      Tag name.
/////
///// @see XmlPackageStartElementHandler(), XmlPackageCharacterDataHandler()
//static void XMLCALL XmlPackageEndElementHandler( void* pUserData, const XML_Char* pName )
//{
//    HELIUM_ASSERT( pUserData );
//    HELIUM_ASSERT( pName );
//    HELIUM_UNREF( pName );
//
//    XmlPackageParseContext* pContext = static_cast< XmlPackageParseContext* >( pUserData );
//
//    uint64_t line = XML_GetCurrentLineNumber( pContext->pParser );
//    uint64_t column = XML_GetCurrentColumnNumber( pContext->pParser );
//    HELIUM_UNREF( line );
//    HELIUM_UNREF( column );
//
//    // Decrease the depth and update as appropriate.
//    HELIUM_ASSERT( pContext->depth != 0 );
//    --pContext->depth;
//
//    if( IsValid( pContext->illegalDepth ) )
//    {
//        if( pContext->depth < pContext->illegalDepth )
//        {
//            // Just broke out of an illegal tag, so clear out the depth.
//            SetInvalid( pContext->illegalDepth );
//        }
//
//        // Ignore this update (even if we just broke out of an illegal tag, there's nothing else for us to do yet).
//        return;
//    }
//
//    DynArray< XmlPackageParseProperty >& rPropertyStack = pContext->propertyStack;
//    size_t propertyStackSize = rPropertyStack.GetSize();
//    HELIUM_ASSERT( propertyStackSize != 0 );
//
//    XmlPackageParseProperty& rPropertyTop = rPropertyStack[ propertyStackSize - 1 ];
//    if( propertyStackSize == 1 && rPropertyTop.name.IsEmpty() )
//    {
//        // Commit "object" tags.
//        if( IsValid( pContext->currentObjectIndex ) )
//        {
//            HELIUM_ASSERT( StringCompare( pName, TXT( "object" ) ) == 0 );
//
//            SetInvalid( pContext->currentObjectIndex );
//        }
//
//        return;
//    }
//
//    // Commit "property" tags for parsed text.
//    if( rPropertyTop.bParsing )
//    {
//        HELIUM_ASSERT( !rPropertyTop.name.IsEmpty() );
//        HELIUM_ASSERT( !rPropertyTop.bClosed );
//
//        HELIUM_ASSERT( StringCompare( pName, TXT( "property" ) ) == 0 );
//
//        // Null-terminate the property text.
//        pContext->propertyTextBuffer.Add( static_cast< XML_Char >( '\0' ) );
//
//        // Set up the property map entry.
//        Pair< String, String > newProperty;
//        newProperty.Second() = pContext->propertyTextBuffer.GetData();
//
//        pContext->propertyTextBuffer.Resize( 0 );
//
//        // Build the property name identifier.
//        String arrayIndexFormat;
//        for( size_t levelIndex = 0; levelIndex < propertyStackSize; ++levelIndex )
//        {
//            if( levelIndex != 0 )
//            {
//                newProperty.First() += TXT( '.' );
//            }
//
//            XmlPackageParseProperty& rCurrentProperty = rPropertyStack[ levelIndex ];
//            newProperty.First() += *rCurrentProperty.name;
//            if( IsValid( rCurrentProperty.index ) )
//            {
//                arrayIndexFormat.Format( TXT( "[%" ) TPRIu32 TXT( "]" ), rCurrentProperty.index );
//                newProperty.First() += arrayIndexFormat;
//            }
//        }
//
//        // Add the property.
//        HELIUM_ASSERT( IsValid( pContext->currentObjectIndex ) );
//        XmlPackageLoader::SerializedObjectData& rObjectData = pContext->pObjects->GetElement(
//            pContext->currentObjectIndex );
//
//        ConcurrentHashMap< String, String >& rProperties = rObjectData.properties;
//        ConcurrentHashMap< String, String >::Accessor propertyAccessor;
//        if( !rProperties.Insert( propertyAccessor, newProperty ) )
//        {
//            HELIUM_TRACE(
//                TRACE_WARNING,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Duplicate " )
//                TXT( "\"%s\" property encountered in object; overwriting existing property.\n" ) ),
//                line,
//                column,
//                *newProperty.First() );
//
//            propertyAccessor->Second() = newProperty.Second();
//        }
//
//        // If this was an array property, update the array index, otherwise clear the property name data for the
//        // next property encountered.
//        if( IsValid( rPropertyTop.index ) )
//        {
//            ++rPropertyTop.index;
//        }
//        else
//        {
//            rPropertyTop.name.Clear();
//        }
//
//        rPropertyTop.bParsing = false;
//
//        return;
//    }
//
//    // Commit "property" tags enclosing structs/arrays.
//    if( rPropertyTop.bClosed )
//    {
//        HELIUM_ASSERT( StringCompare( pName, TXT( "property" ) ) == 0 );
//
//        // Arrays are terminated with the name intact, while structs have the name cleared.
//        if( rPropertyTop.name.IsEmpty() )
//        {
//            // Ending a struct, so pop the struct property level off the property stack.
//            rPropertyStack.Pop();
//
//            // If the struct was contained in an array property, update the array index, otherwise clear the
//            // property name data for the next property encountered.
//            XmlPackageParseProperty& rPropertyTopNew = rPropertyStack.GetLast();
//            if( IsValid( rPropertyTopNew.index ) )
//            {
//                ++rPropertyTopNew.index;
//            }
//            else
//            {
//                rPropertyTopNew.name.Clear();
//            }
//        }
//        else
//        {
//            // Ending an array, so simply clear out the property name for the next property (note that nested arrays
//            // aren't allowed, so we don't have to worry about that case).
//            rPropertyTop.name.Clear();
//        }
//
//        rPropertyTop.bClosed = false;
//
//        return;
//    }
//
//    // Commit "array" tags.
//    if( IsValid( rPropertyTop.index ) )
//    {
//        HELIUM_ASSERT( StringCompare( pName, TXT( "array" ) ) == 0 );
//
//        // Add an array size entry.
//        Pair< String, uint32_t > arraySizeEntry;
//        arraySizeEntry.Second() = rPropertyTop.index;
//        SetInvalid( rPropertyTop.index );
//
//        String arrayIndexFormat;
//        for( size_t levelIndex = 0; levelIndex < propertyStackSize; ++levelIndex )
//        {
//            if( levelIndex != 0 )
//            {
//                arraySizeEntry.First() += TXT( '.' );
//            }
//
//            XmlPackageParseProperty& rCurrentProperty = rPropertyStack[ levelIndex ];
//            arraySizeEntry.First() += *rCurrentProperty.name;
//            if( IsValid( rCurrentProperty.index ) )
//            {
//                arrayIndexFormat.Format( TXT( "[%" ) TPRIu32 TXT( "]" ), rCurrentProperty.index );
//                arraySizeEntry.First() += arrayIndexFormat;
//            }
//        }
//
//        HELIUM_ASSERT( IsValid( pContext->currentObjectIndex ) );
//        XmlPackageLoader::SerializedObjectData& objectData = pContext->pObjects->GetElement(
//            pContext->currentObjectIndex );
//
//        ConcurrentHashMap< String, uint32_t >& rArraySizes = objectData.arraySizes;
//        ConcurrentHashMap< String, uint32_t >::Accessor arraySizeMapAccessor;
//        if( !rArraySizes.Insert( arraySizeMapAccessor, arraySizeEntry ) )
//        {
//            HELIUM_TRACE(
//                TRACE_WARNING,
//                ( TXT( "XmlPackageLoader: (Line %" ) TPRIu64 TXT( ", column %" ) TPRIu64 TXT( ") Duplicate " )
//                TXT( "\"%s\" array property encountered in object; overwriting existing array size.\n" ) ),
//                line,
//                column,
//                *arraySizeEntry.First() );
//
//            arraySizeMapAccessor->Second() = arraySizeEntry.Second();
//        }
//
//        // Tag is now closed.
//        HELIUM_ASSERT( !rPropertyTop.name.IsEmpty() );
//        rPropertyTop.bClosed = true;
//
//        return;
//    }
//
//    // Commit "struct" tags.
//    HELIUM_ASSERT( StringCompare( pName, TXT( "struct" ) ) == 0 );
//
//    HELIUM_ASSERT( rPropertyTop.name.IsEmpty() );
//    HELIUM_ASSERT( IsInvalid( rPropertyTop.index ) );
//
//    rPropertyTop.bClosed = true;
//}
//
///// Parsing handler for character data.
/////
///// @param[in] pUserData  User data pointer.
///// @param[in] pString    String of character data (not null-terminated).
///// @param[in] len        Number of characters parsed.
/////
///// @see XmlPackageStartElementHandler(), XmlPackageEndElementHandler()
//static void XMLCALL XmlPackageCharacterDataHandler( void* pUserData, const XML_Char* pString, int len )
//{
//    HELIUM_ASSERT( pUserData );
//
//    XmlPackageParseContext* pContext = static_cast< XmlPackageParseContext* >( pUserData );
//
//    // Skip if we are still parsing an invalid tag.
//    if( IsValid( pContext->illegalDepth ) )
//    {
//        return;
//    }
//
//    // Skip if we are not parsing property text.
//    DynArray< XmlPackageParseProperty >& rPropertyStack = pContext->propertyStack;
//    if( rPropertyStack.IsEmpty() )
//    {
//        return;
//    }
//
//    XmlPackageParseProperty& rPropertyTop = rPropertyStack.GetLast();
//    if( !rPropertyTop.bParsing )
//    {
//        return;
//    }
//
//    // Push the character data.
//    HELIUM_ASSERT( len >= 0 );
//    pContext->propertyTextBuffer.AddArray( pString, len );
//}
//
///// Constructor.
//XmlPackageLoader::XmlPackageLoader()
//: m_startPreloadCounter( 0 )
//, m_preloadedCounter( 0 )
//, m_loadRequestPool( LOAD_REQUEST_POOL_BLOCK_SIZE )
//, m_packageFileSize( 0 )
//, m_pLoadBuffer( NULL )
//, m_asyncLoadId( Invalid< size_t >() )
//, m_parentPackageLoadId( Invalid< size_t >() )
//{
//}
//
///// Destructor.
//XmlPackageLoader::~XmlPackageLoader()
//{
//    Shutdown();
//}
//
///// Initialize this package loader.
/////
///// @param[in] packagePath  GameObject path of the package to load.
/////
///// @return  True if this loader was initialized successfully, false if not.
/////
///// @see Shutdown()
//bool XmlPackageLoader::Initialize( GameObjectPath packagePath )
//{
//    Shutdown();
//
//    // Make sure the path represents a package.
//    if( packagePath.IsEmpty() )
//    {
//        HELIUM_TRACE( TRACE_ERROR, TXT( "XmlPackageLoader::Initialize(): Empty package path specified.\n" ) );
//
//        return false;
//    }
//
//    HELIUM_TRACE(
//        TRACE_DEBUG,
//        TXT( "XmlPackageLoader::Initialize(): Initializing loader for package \"%s\".\n" ),
//        *packagePath.ToString() );
//
//    if( !packagePath.IsPackage() )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            TXT( "XmlPackageLoader::Initialize(): \"%s\" does not represent a package path.\n" ),
//            *packagePath.ToString() );
//
//        return false;
//    }
//
//    // Store the package path.
//    m_packagePath = packagePath;
//
//    // Attempt to locate the specified package if it already happens to exist.
//    m_spPackage = GameObject::Find< Package >( packagePath );
//    Package* pPackage = m_spPackage;
//    if( pPackage )
//    {
//        if( pPackage->GetLoader() )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                TXT( "XmlPackageLoader::Initialize(): Package \"%s\" already has a loader.\n" ),
//                *packagePath.ToString() );
//
//            m_spPackage.Release();
//
//            return false;
//        }
//
//        pPackage->SetLoader( this );
//    }
//    else
//    {
//        // Make sure we don't have a name clash with a non-package object.
//        GameObjectPtr spObject( GameObject::FindObject( packagePath ) );
//        if( spObject )
//        {
//            HELIUM_ASSERT( !spObject->IsPackage() );
//
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "PackageLoader::Initialize(): Package loader cannot be initialized for \"%s\", as an " )
//                TXT( "object with the same name exists that is not a package.\n" ) ),
//                *packagePath.ToString() );
//
//            return false;
//        }
//    }
//
//    // Build the package file path.  If the package is a user configuration package, use the user data directory,
//    // otherwise use the global data directory.
//    Config& rConfig = Config::GetStaticInstance();
//    Path dataDirectory;
//
//    if ( packagePath == rConfig.GetUserConfigPackagePath() )
//    {
//        if ( !File::GetUserDataDirectory( dataDirectory ) )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                TXT( "PackageLoader::Initialize(): Could not obtain user data directory." ) );
//
//            return false;
//        }
//    }
//    else
//    {
//        if ( !File::GetDataDirectory( dataDirectory ) )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                TXT( "PackageLoader::Initialize(): Could not obtain user data directory." ) );
//
//            return false;
//        }
//    }
//
//    Path basePackageFilePath( dataDirectory + packagePath.ToFilePathString().GetData() );
//
//    m_packageFilePath.Set( basePackageFilePath.Get() + TXT( "/" ) + HELIUM_XML_PACKAGE_FILE_NAME );
//
//    if( !m_packageFilePath.Exists() )
//    {
//        // Fall back onto the non-directory based package file.
//        m_packageFilePath = basePackageFilePath;
//        m_packageFilePath += HELIUM_XML_PACKAGE_FILE_EXTENSION;
//    }
//
//    // Retrieve the size of the package file.  Note that we still keep around the loader even if the package file
//    // isn't found so as to allow for memory-only packages or the creation of new packages at editor runtime.
//    SetInvalid( m_packageFileSize );
//
//    int64_t packageFileSize = m_packageFilePath.Size();
//    if( packageFileSize == -1 )
//    {
//        HELIUM_TRACE(
//            TRACE_WARNING,
//            TXT( "XmlPackageLoader::Initialize(): Failed to locate a valid package file for package \"%s\".\n" ),
//            *m_packageFilePath,
//            *packagePath.ToString() );
//    }
//    else if( packageFileSize == 0 )
//    {
//        HELIUM_TRACE(
//            TRACE_WARNING,
//            TXT( "XmlPackageLoader::Initialize(): Package file \"%s\" for package \"%s\" is empty.\n" ),
//            *m_packageFilePath,
//            *packagePath.ToString() );
//    }
//    else if( static_cast< uint64_t >( packageFileSize ) > static_cast< uint64_t >( ~static_cast< size_t >( 0 ) ) )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            ( TXT( "XmlPackageLoader::Initialize(): Package file \"%s\" exceeds the maximum size supported by " )
//            TXT( "the current platform (package: %" ) TPRIu64 TXT( " bytes; max supported: %" ) TPRIuSZ
//            TXT( " bytes).\n" ) ),
//            *packagePath.ToString(),
//            static_cast< uint64_t >( packageFileSize ),
//            ~static_cast< size_t >( 0 ) );
//
//        if( pPackage )
//        {
//            pPackage->SetLoader( NULL );
//        }
//
//        m_spPackage.Release();
//
//        return false;
//    }
//    else
//    {
//        m_packageFileSize = static_cast< size_t >( packageFileSize );
//    }
//
//    return true;
//}
//
///// @copydoc PackageLoader::Shutdown()
//void XmlPackageLoader::Shutdown()
//{
//    // Sync with any in-flight async load requests.
//    if( m_startPreloadCounter )
//    {
//        while( !TryFinishPreload() )
//        {
//            Tick();
//        }
//    }
//
//    HELIUM_ASSERT( !m_pLoadBuffer );
//    HELIUM_ASSERT( IsInvalid( m_asyncLoadId ) );
//    HELIUM_ASSERT( IsInvalid( m_parentPackageLoadId ) );
//
//    // Unset the reference back to this loader in the package.
//    Package* pPackage = m_spPackage;
//    if( pPackage )
//    {
//        pPackage->SetLoader( NULL );
//    }
//
//    m_spPackage.Release();
//    m_packagePath.Clear();
//
//    AtomicExchangeRelease( m_startPreloadCounter, 0 );
//    AtomicExchangeRelease( m_preloadedCounter, 0 );
//
//    m_objects.Clear();
//
//    size_t loadRequestCount = m_loadRequests.GetSize();
//    for( size_t requestIndex = 0; requestIndex < loadRequestCount; ++requestIndex )
//    {
//        if( m_loadRequests.IsElementValid( requestIndex ) )
//        {
//            LoadRequest* pRequest = m_loadRequests[ requestIndex ];
//            HELIUM_ASSERT( pRequest );
//            m_loadRequestPool.Release( pRequest );
//        }
//    }
//
//    m_loadRequests.Clear();
//
//    m_packageFilePath.Clear();
//    m_packageFileSize = 0;
//}
//
///// Begin asynchronous pre-loading of package information.
/////
///// @see TryFinishPreload()
//bool XmlPackageLoader::BeginPreload()
//{
//    MutexScopeLock scopeLock( m_accessLock );
//
//    HELIUM_ASSERT( !m_startPreloadCounter );
//    HELIUM_ASSERT( !m_preloadedCounter );
//    HELIUM_ASSERT( IsInvalid( m_asyncLoadId ) );
//    HELIUM_ASSERT( IsInvalid( m_parentPackageLoadId ) );
//
//    // Load the parent package if we need to create the current package.
//    if( !m_spPackage )
//    {
//        GameObjectPath parentPackagePath = m_packagePath.GetParent();
//        if( !parentPackagePath.IsEmpty() )
//        {
//            GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
//            HELIUM_ASSERT( pObjectLoader );
//
//            m_parentPackageLoadId = pObjectLoader->BeginLoadObject( parentPackagePath );
//        }
//    }
//
//    // Allocate memory and issue an async load request for the package XML data if a package was found.
//    if( IsValid( m_packageFileSize ) )
//    {
//        HELIUM_ASSERT( !m_pLoadBuffer );
//        m_pLoadBuffer = DefaultAllocator().Allocate( m_packageFileSize );
//        HELIUM_ASSERT( m_pLoadBuffer );
//
//        AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();
//        m_asyncLoadId = rAsyncLoader.QueueRequest( m_pLoadBuffer, String( m_packageFilePath.c_str() ), 0, m_packageFileSize );
//        HELIUM_ASSERT( IsValid( m_asyncLoadId ) );
//    }
//
//    AtomicExchangeRelease( m_startPreloadCounter, 1 );
//
//    return true;
//}
//
///// @copydoc PackageLoader::TryFinishPreload()
//bool XmlPackageLoader::TryFinishPreload()
//{
//    return ( m_preloadedCounter != 0 );
//}
//
///// @copydoc PackageLoader::BeginLoadObject()
//size_t XmlPackageLoader::BeginLoadObject( GameObjectPath path )
//{
//    MutexScopeLock scopeLock( m_accessLock );
//
//    // Make sure preloading has completed.
//    HELIUM_ASSERT( m_preloadedCounter != 0 );
//    if( !m_preloadedCounter )
//    {
//        return Invalid< size_t >();
//    }
//
//    // If this package is requested, simply provide the (already loaded) package instance.
//    if( path == m_packagePath )
//    {
//        LoadRequest* pRequest = m_loadRequestPool.Allocate();
//        HELIUM_ASSERT( pRequest );
//
//        HELIUM_ASSERT( m_spPackage );
//        pRequest->spObject = m_spPackage;
//
//        SetInvalid( pRequest->index );
//        HELIUM_ASSERT( pRequest->linkTable.IsEmpty() );
//        HELIUM_ASSERT( !pRequest->spType );
//        HELIUM_ASSERT( !pRequest->spTemplate );
//        HELIUM_ASSERT( !pRequest->spOwner );
//        SetInvalid( pRequest->templateLoadId );
//        SetInvalid( pRequest->ownerLoadId );
//        SetInvalid( pRequest->persistentResourceDataLoadId );
//        pRequest->pCachedObjectDataBuffer = NULL;
//        pRequest->cachedObjectDataBufferSize = 0;
//
//        pRequest->flags = LOAD_FLAG_PRELOADED;
//
//        size_t requestId = m_loadRequests.Add( pRequest );
//
//        return requestId;
//    }
//
//    // Locate the object within this package.
//    size_t objectCount = m_objects.GetSize();
//    size_t objectIndex;
//    for( objectIndex = 0; objectIndex < objectCount; ++objectIndex )
//    {
//        SerializedObjectData& rObjectData = m_objects[ objectIndex ];
//        if( rObjectData.objectPath == path )
//        {
//            break;
//        }
//    }
//
//    if( objectIndex >= objectCount )
//    {
//        HELIUM_TRACE(
//            TRACE_WARNING,
//            TXT( "XmlPackageLoader::BeginLoadObject(): Failed to locate \"%s\" for loading.\n" ),
//            *path.ToString() );
//
//        return Invalid< size_t >();
//    }
//
//    SerializedObjectData& rObjectData = m_objects[ objectIndex ];
//
//    // Locate the type object.
//    HELIUM_ASSERT( !rObjectData.typeName.IsEmpty() );
//    GameObjectType* pType = GameObjectType::Find( rObjectData.typeName );
//    if( !pType )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            TXT( "XmlPackageLoader::BeginLoadObject(): Failed to locate type \"%s\" for loading object \"%s\".\n" ),
//            *rObjectData.typeName,
//            *path.ToString() );
//
//        return Invalid< size_t >();
//    }
//
//#ifndef NDEBUG
//    size_t loadRequestSize = m_loadRequests.GetSize();
//    for( size_t loadRequestIndex = 0; loadRequestIndex < loadRequestSize; ++loadRequestIndex )
//    {
//        if( !m_loadRequests.IsElementValid( loadRequestIndex ) )
//        {
//            continue;
//        }
//
//        LoadRequest* pRequest = m_loadRequests[ loadRequestIndex ];
//        HELIUM_ASSERT( pRequest );
//        HELIUM_ASSERT( pRequest->index != objectIndex );
//        if( pRequest->index == objectIndex )
//        {
//            return Invalid< size_t >();
//        }
//    }
//#endif
//
//    LoadRequest* pRequest = m_loadRequestPool.Allocate();
//    HELIUM_ASSERT( pRequest );
//    HELIUM_ASSERT( !pRequest->spObject );
//    pRequest->index = objectIndex;
//    HELIUM_ASSERT( pRequest->linkTable.IsEmpty() );
//    pRequest->spType = pType;
//    HELIUM_ASSERT( !pRequest->spTemplate );
//    HELIUM_ASSERT( !pRequest->spOwner );
//    SetInvalid( pRequest->templateLoadId );
//    SetInvalid( pRequest->ownerLoadId );
//    SetInvalid( pRequest->persistentResourceDataLoadId );
//    pRequest->pCachedObjectDataBuffer = NULL;
//    pRequest->cachedObjectDataBufferSize = 0;
//
//    pRequest->flags = 0;
//
//    // If a fully-loaded object already exists with the same name, do not attempt to re-load the object (just mark
//    // the request as complete).
//    pRequest->spObject = GameObject::FindObject( path );
//
//    GameObject* pObject = pRequest->spObject;
//    if( pObject && pObject->IsFullyLoaded() )
//    {
//        pRequest->flags = LOAD_FLAG_PRELOADED;
//    }
//    else
//    {
//        HELIUM_ASSERT( !pObject || !pObject->GetAnyFlagSet( GameObject::FLAG_LOADED | GameObject::FLAG_LINKED ) );
//
//        // Begin loading the template and owner objects.  Note that there isn't much reason to check for failure
//        // until we tick this request, as we need to make sure any other load requests for the template/owner that
//        // did succeed are properly synced anyway.
//        GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
//        HELIUM_ASSERT( pObjectLoader );
//
//        if( rObjectData.templatePath.IsEmpty() )
//        {
//            // Make sure the template is fully loaded.
//            GameObject* pTemplate = pType->GetTemplate();
//            rObjectData.templatePath = pTemplate->GetPath();
//            if( pTemplate->IsFullyLoaded() )
//            {
//                pRequest->spTemplate = pTemplate;
//            }
//            else
//            {
//                pRequest->templateLoadId = pObjectLoader->BeginLoadObject( rObjectData.templatePath );
//            }
//        }
//        else
//        {
//            pRequest->templateLoadId = pObjectLoader->BeginLoadObject( rObjectData.templatePath );
//        }
//
//        GameObjectPath ownerPath = path.GetParent();
//        if( ownerPath == m_packagePath )
//        {
//            // Easy check: if the owner is this package (which is likely), we don't need to load it.
//            pRequest->spOwner = m_spPackage;
//        }
//        else if( !ownerPath.IsEmpty() )
//        {
//            pRequest->ownerLoadId = pObjectLoader->BeginLoadObject( ownerPath );
//        }
//    }
//
//    size_t requestId = m_loadRequests.Add( pRequest );
//
//    return requestId;
//}
//
///// @copydoc PackageLoader::TryFinishLoadObject()
//bool XmlPackageLoader::TryFinishLoadObject(
//    size_t requestId,
//    GameObjectPtr& rspObject,
//    DynArray< GameObjectLoader::LinkEntry >& rLinkTable )
//{
//    HELIUM_ASSERT( requestId < m_loadRequests.GetSize() );
//    HELIUM_ASSERT( m_loadRequests.IsElementValid( requestId ) );
//
//    LoadRequest* pRequest = m_loadRequests[ requestId ];
//    HELIUM_ASSERT( pRequest );
//    if( ( pRequest->flags & LOAD_FLAG_PRELOADED ) != LOAD_FLAG_PRELOADED )
//    {
//        return false;
//    }
//
//    // Sync on template and owner dependencies.
//    GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
//    HELIUM_ASSERT( pObjectLoader );
//
//    if( IsValid( pRequest->templateLoadId ) )
//    {
//        if( !pObjectLoader->TryFinishLoad( pRequest->templateLoadId, pRequest->spTemplate ) )
//        {
//            return false;
//        }
//
//        SetInvalid( pRequest->templateLoadId );
//    }
//
//    if( IsValid( pRequest->ownerLoadId ) )
//    {
//        if( !pObjectLoader->TryFinishLoad( pRequest->ownerLoadId, pRequest->spOwner ) )
//        {
//            return false;
//        }
//
//        SetInvalid( pRequest->ownerLoadId );
//    }
//
//    // Sync on any in-flight async load requests for the cached object data.
//    if( IsValid( pRequest->persistentResourceDataLoadId ) )
//    {
//        AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();
//        size_t bytesRead;
//        if( !rAsyncLoader.TrySyncRequest( pRequest->persistentResourceDataLoadId, bytesRead ) )
//        {
//            return false;
//        }
//
//        SetInvalid( pRequest->persistentResourceDataLoadId );
//    }
//
//    DefaultAllocator().Free( pRequest->pCachedObjectDataBuffer );
//    pRequest->pCachedObjectDataBuffer = NULL;
//    pRequest->cachedObjectDataBufferSize = 0;
//
//    rspObject = pRequest->spObject;
//    GameObject* pObject = rspObject;
//    if( pObject && ( pRequest->flags & LOAD_FLAG_ERROR ) )
//    {
//        pObject->SetFlags( GameObject::FLAG_BROKEN );
//    }
//
//    pRequest->spObject.Release();
//
//    DynArray< LinkEntry >& rInternalLinkTable = pRequest->linkTable;
//    size_t linkTableSize = rInternalLinkTable.GetSize();
//    rLinkTable.Resize( 0 );
//    rLinkTable.Reserve( linkTableSize );
//    for( size_t linkIndex = 0; linkIndex < linkTableSize; ++linkIndex )
//    {
//        GameObjectLoader::LinkEntry* pEntry = rLinkTable.New();
//        HELIUM_ASSERT( pEntry );
//        pEntry->loadId = rInternalLinkTable[ linkIndex ].loadRequestId;
//        pEntry->spObject.Release();
//    }
//
//    rInternalLinkTable.Resize( 0 );
//
//    pRequest->spType.Release();
//    pRequest->spTemplate.Release();
//    pRequest->spOwner.Release();
//
//    m_loadRequests.Remove( requestId );
//    m_loadRequestPool.Release( pRequest );
//
//    return true;
//}
//
///// @copydoc PackageLoader::Tick()
//void XmlPackageLoader::Tick()
//{
//    MutexScopeLock scopeLock( m_accessLock );
//
//    // Do nothing until pre-loading has been started.
//    if( !m_startPreloadCounter )
//    {
//        return;
//    }
//
//    if( !m_preloadedCounter )
//    {
//        // Update package preloading.
//        TickPreload();
//    }
//    else
//    {
//        // Process pending dependencies.
//        TickLoadRequests();
//    }
//}
//
///// @copydoc PackageLoader::GetObjectCount()
//size_t XmlPackageLoader::GetObjectCount() const
//{
//    return m_objects.GetSize();
//}
//
///// @copydoc PackageLoader::GetObjectPath()
//GameObjectPath XmlPackageLoader::GetObjectPath( size_t index ) const
//{
//    HELIUM_ASSERT( index < m_objects.GetSize() );
//
//    return m_objects[ index ].objectPath;
//}
//
///// Get the package managed by this loader.
/////
///// @return  Associated package.
/////
///// @see GetPackagePath()
//Package* XmlPackageLoader::GetPackage() const
//{
//    return m_spPackage;
//}
//
///// Get the object path for the package managed by this loader.
/////
///// @return  Path of the associated package.
/////
///// @see GetPackage()
//GameObjectPath XmlPackageLoader::GetPackagePath() const
//{
//    return m_packagePath;
//}
//
///// @copydoc PackageLoader::IsSourcePackageFile()
//bool XmlPackageLoader::IsSourcePackageFile() const
//{
//    return true;
//}
//
///// @copydoc PackageLoader::GetFileTimestamp()
//int64_t XmlPackageLoader::GetFileTimestamp() const
//{
//    int64_t timestamp =  m_packageFilePath.ModifiedTime();
//
//    return timestamp;
//}
//
///// Update during the package preload process.
//void XmlPackageLoader::TickPreload()
//{
//    HELIUM_ASSERT( m_startPreloadCounter != 0 );
//    HELIUM_ASSERT( m_preloadedCounter == 0 );
//
//    // Test whether loading has completed.
//    if( IsValid( m_asyncLoadId ) )
//    {
//        AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();
//
//        size_t bytesRead = 0;
//        if( !rAsyncLoader.TrySyncRequest( m_asyncLoadId, bytesRead ) )
//        {
//            // Async loading is still in progress.
//            return;
//        }
//
//        HELIUM_ASSERT( bytesRead == m_packageFileSize );
//        if( IsInvalid( bytesRead ) )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                TXT( "XmlPackageLoader: Failed to read the contents of package file \"%s\".\n" ),
//                *m_packageFilePath );
//
//            bytesRead = 0;
//        }
//        else if( bytesRead != m_packageFileSize )
//        {
//            HELIUM_TRACE(
//                TRACE_WARNING,
//                ( TXT( "XmlPackageLoader: Attempted to read %" ) TPRIuSZ TXT( " bytes from package file \"%s\", " )
//                TXT( "but only %" ) TPRIuSZ TXT( " bytes were read.\n" ) ),
//                m_packageFileSize,
//                bytesRead );
//        }
//
//        SetInvalid( m_asyncLoadId );
//
//        // Parse the XML data.
//        XML_Parser pParser = XML_ParserCreate( NULL );
//        HELIUM_ASSERT( pParser );
//        XML_SetElementHandler( pParser, XmlPackageStartElementHandler, XmlPackageEndElementHandler );
//        XML_SetCharacterDataHandler( pParser, XmlPackageCharacterDataHandler );
//
//        HELIUM_ASSERT( !m_packagePath.IsEmpty() );
//
//        XmlPackageParseContext parseContext;
//        parseContext.pParser = pParser;
//        parseContext.packagePath = m_packagePath;
//        parseContext.pObjects = &m_objects;
//        parseContext.depth = 0;
//        SetInvalid( parseContext.illegalDepth );
//        SetInvalid( parseContext.currentObjectIndex );
//
//        XmlPackageParseProperty* pPropertyTop = parseContext.propertyStack.New();
//        HELIUM_ASSERT( pPropertyTop );
//        pPropertyTop->name.Clear();
//        SetInvalid( pPropertyTop->index );
//        pPropertyTop->bParsing = false;
//        pPropertyTop->bClosed = false;
//
//        XML_SetUserData( pParser, &parseContext );
//
//        const char* pData = static_cast< char* >( m_pLoadBuffer );
//        size_t parseBytes;
//        do
//        {
//            parseBytes = Min( bytesRead, PARSE_CHUNK_SIZE );
//            XML_Status parseResult = XML_Parse(
//                pParser,
//                pData,
//                static_cast< int >( parseBytes ),
//                ( parseBytes == 0 ) );
//            if( parseResult != XML_STATUS_OK )
//            {
//                HELIUM_TRACE( TRACE_ERROR, TXT( "XmlPackageLoader::Initialize(): XML parse error.\n" ) );
//                break;
//            }
//
//            pData += parseBytes;
//            bytesRead -= parseBytes;
//        } while( parseBytes != 0 );
//
//        // Free the parser and load buffer.
//        XML_ParserFree( pParser );
//
//        DefaultAllocator().Free( m_pLoadBuffer );
//        m_pLoadBuffer = NULL;
//    }
//
//    // Wait for the parent package to finish loading.
//    GameObjectPtr spParentPackage;
//    if( IsValid( m_parentPackageLoadId ) )
//    {
//        GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
//        HELIUM_ASSERT( pObjectLoader );
//        if( !pObjectLoader->TryFinishLoad( m_parentPackageLoadId, spParentPackage ) )
//        {
//            return;
//        }
//
//        SetInvalid( m_parentPackageLoadId );
//
//        // Package loading should not fail.  If it does, this is a sign of a potentially serious issue.
//        HELIUM_ASSERT( spParentPackage );
//    }
//
//    // Create the package object if it does not yet exist.
//    Package* pPackage = m_spPackage;
//    if( !pPackage )
//    {
//        HELIUM_ASSERT( spParentPackage ? !m_packagePath.GetParent().IsEmpty() : m_packagePath.GetParent().IsEmpty() );
//        HELIUM_VERIFY( GameObject::Create< Package >( m_spPackage, m_packagePath.GetName(), spParentPackage ) );
//        pPackage = m_spPackage;
//        HELIUM_ASSERT( pPackage );
//        pPackage->SetLoader( this );
//    }
//
//    HELIUM_ASSERT( pPackage->GetLoader() == this );
//
//#if HELIUM_TOOLS
//    // Add all resource objects that exist in the package directory.
//    DynArray< ResourceHandler* > resourceHandlers;
//    ResourceHandler::GetAllResourceHandlers( resourceHandlers );
//    size_t resourceHandlerCount = resourceHandlers.GetSize();
//
//    Path packageDirectoryPath;
//
//    if ( !File::GetDataDirectory( packageDirectoryPath ) )
//    {
//        HELIUM_TRACE( TRACE_ERROR, TXT( "XmlPackageLoader::TickPreload(): Could not get data directory.\n" ) );
//        return;
//    }
//
//    packageDirectoryPath += m_packagePath.ToFilePathString().GetData();
//    packageDirectoryPath += TXT("/");
//
//    Directory packageDirectory( packageDirectoryPath );
//
//    for( ; !packageDirectory.IsDone(); packageDirectory.Next() )
//    {
//        const DirectoryItem& item = packageDirectory.GetItem();
//
//        if ( !item.m_Path.IsFile() )
//        {
//            continue;
//        }
//
//        // Make sure an object entry doesn't already exist for the file.
//        String objectNameString( *item.m_Path );
//
//        size_t pathSeparatorLocation = objectNameString.FindReverse( TXT( '/' ) );
//        if( IsValid( pathSeparatorLocation ) )
//        {
//            objectNameString.Substring( objectNameString, pathSeparatorLocation + 1 );
//        }
//
//        Name objectName( objectNameString );
//        size_t objectCount = m_objects.GetSize();
//        size_t objectIndex;
//        for( objectIndex = 0; objectIndex < objectCount; ++objectIndex )
//        {
//            SerializedObjectData& rObjectData = m_objects[ objectIndex ];
//            if( rObjectData.objectPath.GetName() == objectName &&
//                rObjectData.objectPath.GetParent() == m_packagePath )
//            {
//                break;
//            }
//        }
//
//        if( objectIndex < objectCount )
//        {
//            continue;
//        }
//
//        // Check the extension to see if the file is supported by one of the resource handlers.
//        ResourceHandler* pBestHandler = NULL;
//        size_t bestHandlerExtensionLength = 0;
//
//        for( size_t handlerIndex = 0; handlerIndex < resourceHandlerCount; ++handlerIndex )
//        {
//            ResourceHandler* pHandler = resourceHandlers[ handlerIndex ];
//            HELIUM_ASSERT( pHandler );
//
//            const tchar_t* const* ppExtensions;
//            size_t extensionCount;
//            pHandler->GetSourceExtensions( ppExtensions, extensionCount );
//            HELIUM_ASSERT( ppExtensions || extensionCount == 0 );
//
//            for( size_t extensionIndex = 0; extensionIndex < extensionCount; ++extensionIndex )
//            {
//                const tchar_t* pExtension = ppExtensions[ extensionIndex ];
//                HELIUM_ASSERT( pExtension );
//
//                size_t extensionLength = StringLength( pExtension );
//                if( extensionLength > bestHandlerExtensionLength && objectNameString.EndsWith( pExtension ) )
//                {
//                    pBestHandler = pHandler;
//                    bestHandlerExtensionLength = extensionLength;
//
//                    break;
//                }
//            }
//        }
//
//        if( pBestHandler )
//        {
//            // File extension matches a supported source asset type, so add it to the object list.
//            const GameObjectType* pResourceType = pBestHandler->GetResourceType();
//            HELIUM_ASSERT( pResourceType );
//
//            HELIUM_TRACE(
//                TRACE_DEBUG,
//                ( TXT( "XmlPackageLoader: Registered source asset file \"%s\" as as instance of resource " )
//                TXT( "type \"%s\" in package \"%s\".\n" ) ),
//                *objectNameString,
//                *pResourceType->GetName(),
//                *m_packagePath.ToString() );
//
//            SerializedObjectData* pObjectData = m_objects.New();
//            HELIUM_ASSERT( pObjectData );
//            HELIUM_VERIFY( pObjectData->objectPath.Set( objectName, false, m_packagePath ) );
//            pObjectData->typeName = pResourceType->GetName();
//            pObjectData->templatePath.Clear();
//        }
//    }
//
//#endif  // HELIUM_TOOLS
//
//    // Package preloading is now complete.
//    pPackage->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED );
//    pPackage->ConditionalFinalizeLoad();
//
//    AtomicExchangeRelease( m_preloadedCounter, 1 );
//}
//
///// Update load processing of object load requests.
//void XmlPackageLoader::TickLoadRequests()
//{
//    size_t loadRequestCount = m_loadRequests.GetSize();
//    for( size_t loadRequestIndex = 0; loadRequestIndex < loadRequestCount; ++loadRequestIndex )
//    {
//        if( !m_loadRequests.IsElementValid( loadRequestIndex ) )
//        {
//            continue;
//        }
//
//        LoadRequest* pRequest = m_loadRequests[ loadRequestIndex ];
//        HELIUM_ASSERT( pRequest );
//
//        if( !( pRequest->flags & LOAD_FLAG_PROPERTY_PRELOADED ) )
//        {
//            if( !TickDeserialize( pRequest ) )
//            {
//                continue;
//            }
//        }
//
//        if( !( pRequest->flags & LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED ) )
//        {
//            if( !TickPersistentResourcePreload( pRequest ) )
//            {
//                continue;
//            }
//        }
//    }
//}
//
///// Update processing of object property preloading for a given load request.
/////
///// @param[in] pRequest  Load request to process.
/////
///// @return  True if object property preloading for the given load request has completed, false if not.
//bool XmlPackageLoader::TickDeserialize( LoadRequest* pRequest )
//{
//    HELIUM_ASSERT( pRequest );
//    HELIUM_ASSERT( !( pRequest->flags & LOAD_FLAG_PROPERTY_PRELOADED ) );
//
//    GameObject* pObject = pRequest->spObject;
//
//    HELIUM_ASSERT( pRequest->index < m_objects.GetSize() );
//    SerializedObjectData& rObjectData = m_objects[ pRequest->index ];
//
//    // Wait for the template and owner objects to load.
//    GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
//    HELIUM_ASSERT( pObjectLoader );
//
//    if( !rObjectData.templatePath.IsEmpty() )
//    {
//        if( IsValid( pRequest->templateLoadId ) )
//        {
//            if( !pObjectLoader->TryFinishLoad( pRequest->templateLoadId, pRequest->spTemplate ) )
//            {
//                return false;
//            }
//
//            SetInvalid( pRequest->templateLoadId );
//        }
//
//        if( !pRequest->spTemplate )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                TXT( "XmlPackageLoader: Failed to load template object for \"%s\".\n" ),
//                *rObjectData.objectPath.ToString() );
//
//            if( pObject )
//            {
//                pObject->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED );
//                pObject->ConditionalFinalizeLoad();
//            }
//
//            pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;
//
//            return true;
//        }
//    }
//
//    HELIUM_ASSERT( IsInvalid( pRequest->templateLoadId ) );
//    GameObject* pTemplate = pRequest->spTemplate;
//
//    GameObjectPath ownerPath = rObjectData.objectPath.GetParent();
//    if( !ownerPath.IsEmpty() )
//    {
//        if( IsValid( pRequest->ownerLoadId ) )
//        {
//            if( !pObjectLoader->TryFinishLoad( pRequest->ownerLoadId, pRequest->spOwner ) )
//            {
//                return false;
//            }
//
//            SetInvalid( pRequest->ownerLoadId );
//        }
//
//        if( !pRequest->spOwner )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                TXT( "XmlPackageLoader: Failed to load owner object for \"%s\".\n" ),
//                *rObjectData.objectPath.ToString() );
//
//            if( pObject )
//            {
//                pObject->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED );
//                pObject->ConditionalFinalizeLoad();
//            }
//
//            pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;
//
//            return true;
//        }
//    }
//
//    HELIUM_ASSERT( IsInvalid( pRequest->ownerLoadId ) );
//    GameObject* pOwner = pRequest->spOwner;
//
//    GameObjectType* pType = pRequest->spType;
//    HELIUM_ASSERT( pType );
//
//    HELIUM_ASSERT( !pOwner || pOwner->IsFullyLoaded() );
//    HELIUM_ASSERT( !pTemplate || pTemplate->IsFullyLoaded() );
//
//    // If we already had an existing object, make sure the type and template match.
//    if( pObject )
//    {
//        const GameObjectType* pExistingType = pObject->GetGameObjectType();
//        HELIUM_ASSERT( pExistingType );
//        if( pExistingType != pType )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: Cannot load \"%s\" using the existing object as the types do not match " )
//                TXT( "(existing type: \"%s\"; serialized type: \"%s\".\n" ) ),
//                *rObjectData.objectPath.ToString(),
//                *pExistingType->GetName(),
//                *pType->GetName() );
//
//            pObject->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED );
//            pObject->ConditionalFinalizeLoad();
//
//            pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;
//
//            return true;
//        }
//    }
//    else
//    {
//        // Create the object.
//        bool bCreateResult = GameObject::CreateObject(
//            pRequest->spObject,
//            pType,
//            rObjectData.objectPath.GetName(),
//            pOwner,
//            pTemplate );
//        if( !bCreateResult )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                TXT( "XmlPackageLoader: Failed to create \"%s\" during loading.\n" ),
//                *rObjectData.objectPath.ToString() );
//
//            pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;
//
//            return true;
//        }
//
//        pObject = pRequest->spObject;
//        HELIUM_ASSERT( pObject );
//    }
//
//    // Load the object properties.
//    Deserializer deserializer;
//    deserializer.Prepare( &rObjectData, &pRequest->linkTable );
//    bool bResult = deserializer.Serialize( pObject );
//
//    pRequest->flags |= LOAD_FLAG_PROPERTY_PRELOADED;
//
//    if( !bResult )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            TXT( "XmlPackageLoader: Deserialization of object \"%s\" failed.\n" ),
//            *rObjectData.objectPath.ToString() );
//
//        // Clear out object references (object can now be considered fully loaded as well).
//        NullLinker().Serialize( pObject );
//        pObject->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED );
//        pObject->ConditionalFinalizeLoad();
//
//        pRequest->flags |= LOAD_FLAG_ERROR;
//
//        return true;
//    }
//
//    // If the object is a resource (not including the default template object for resource types), attempt to begin
//    // loading any existing persistent resource data stored in the object cache.
//    if( !pObject->IsDefaultTemplate() )
//    {
//        Resource* pResource = Reflect::SafeCast< Resource >( pObject );
//        if( pResource )
//        {
//            Name objectCacheName = pObjectLoader->GetCacheName();
//            CacheManager& rCacheManager = CacheManager::GetStaticInstance();
//
//            Cache* pCache = rCacheManager.GetCache( objectCacheName );
//            HELIUM_ASSERT( pCache );
//            pCache->EnforceTocLoad();
//
//            const Cache::Entry* pEntry = pCache->FindEntry( rObjectData.objectPath, 0 );
//            if( pEntry && pEntry->size != 0 )
//            {
//                HELIUM_ASSERT( IsInvalid( pRequest->persistentResourceDataLoadId ) );
//                HELIUM_ASSERT( !pRequest->pCachedObjectDataBuffer );
//
//                pRequest->pCachedObjectDataBuffer =
//                    static_cast< uint8_t* >( DefaultAllocator().Allocate( pEntry->size ) );
//                HELIUM_ASSERT( pRequest->pCachedObjectDataBuffer );
//                pRequest->cachedObjectDataBufferSize = pEntry->size;
//
//                AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();
//                pRequest->persistentResourceDataLoadId = rAsyncLoader.QueueRequest(
//                    pRequest->pCachedObjectDataBuffer,
//                    pCache->GetCacheFileName(),
//                    pEntry->offset,
//                    pEntry->size );
//                HELIUM_ASSERT( IsValid( pRequest->persistentResourceDataLoadId ) );
//            }
//        }
//    }
//
//    if( IsInvalid( pRequest->persistentResourceDataLoadId ) )
//    {
//        // No persistent resource data needs to be loaded.
//        pObject->SetFlags( GameObject::FLAG_PRELOADED );
//        pRequest->flags |= LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED;
//    }
//
//    // GameObject is now preloaded.
//    return true;
//}
//
///// Update processing of persistent resource data loading for a given load request.
/////
///// @param[in] pRequest  Load request to process.
/////
///// @return  True if persistent resource data loading for the given load request has completed, false if not.
//bool XmlPackageLoader::TickPersistentResourcePreload( LoadRequest* pRequest )
//{
//    HELIUM_ASSERT( pRequest );
//    HELIUM_ASSERT( !( pRequest->flags & LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED ) );
//
//    Resource* pResource = Reflect::AssertCast< Resource >( pRequest->spObject.Get() );
//    HELIUM_ASSERT( pResource );
//
//    // Wait for the cached data load to complete.
//    AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();
//
//    size_t bytesRead = 0;
//    HELIUM_ASSERT( IsValid( pRequest->persistentResourceDataLoadId ) );
//    if( !rAsyncLoader.TrySyncRequest( pRequest->persistentResourceDataLoadId, bytesRead ) )
//    {
//        return false;
//    }
//
//    SetInvalid( pRequest->persistentResourceDataLoadId );
//
//    if( bytesRead != pRequest->cachedObjectDataBufferSize )
//    {
//        HELIUM_TRACE(
//            TRACE_WARNING,
//            ( TXT( "XmlPackageLoader: Requested load of %" ) TPRIu32 TXT( " bytes from cached object data for " )
//            TXT( "\"%s\", but only %" ) TPRIuSZ TXT( " bytes were read.\n" ) ),
//            pRequest->cachedObjectDataBufferSize,
//            *pResource->GetPath().ToString(),
//            bytesRead );
//    }
//
//    // Make sure we read enough bytes to cover the object property data size.
//    if( bytesRead < sizeof( uint32_t ) )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            ( TXT( "XmlPackageLoader: Not enough bytes read of cached object data \"%s\" from which to parse the " )
//            TXT( "property stream size.\n" ) ),
//            *pResource->GetPath().ToString() );
//
//        pRequest->flags |= LOAD_FLAG_ERROR;
//    }
//    else
//    {
//        // Skip over the object property data.
//        uint8_t* pCachedObjectData = pRequest->pCachedObjectDataBuffer;
//        HELIUM_ASSERT( pCachedObjectData );
//
//        uint32_t propertyDataSize = *reinterpret_cast< uint32_t* >( pCachedObjectData );
//
//        size_t byteSkipCount = sizeof( propertyDataSize ) + propertyDataSize;
//        if( byteSkipCount > bytesRead )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader: Cached persistent resource data for \"%s\" extends past the end of the " )
//                TXT( "cached data stream.\n" ) ),
//                *pResource->GetPath().ToString() );
//
//            pRequest->flags |= LOAD_FLAG_ERROR;
//        }
//        else
//        {
//            pCachedObjectData += byteSkipCount;
//            size_t bytesRemaining = bytesRead - byteSkipCount;
//
//            // Make sure we have enough bytes at the end for the resource sub-data count.
//            if( bytesRemaining < sizeof( uint32_t ) )
//            {
//                HELIUM_TRACE(
//                    TRACE_ERROR,
//                    ( TXT( "XmlPackageLoader: Not enough space is reserved in the cached persistent resource " )
//                    TXT( "data stream for \"%s\" for the resource sub-data count.\n" ) ),
//                    *pResource->GetPath().ToString() );
//
//                pRequest->flags |= LOAD_FLAG_ERROR;
//            }
//            else
//            {
//                bytesRemaining -= sizeof( uint32_t );
//
//                // Deserialize the persistent resource data.
//                BinaryDeserializer deserializer;
//                deserializer.Prepare( pCachedObjectData, bytesRemaining );
//
//                deserializer.BeginSerialize();
//                pResource->SerializePersistentResourceData( deserializer );
//                if( !deserializer.EndSerialize() )
//                {
//                    HELIUM_TRACE(
//                        TRACE_ERROR,
//                        ( TXT( "XmlPackageLoader: Attempted to read past the end of the cached data stream when " )
//                        TXT( "deserializing persistent resource data for \"%s\".\n" ) ),
//                        *pResource->GetPath().ToString() );
//
//                    pRequest->flags |= LOAD_FLAG_ERROR;
//                }
//            }
//        }
//    }
//
//    DefaultAllocator().Free( pRequest->pCachedObjectDataBuffer );
//    pRequest->pCachedObjectDataBuffer = NULL;
//    pRequest->cachedObjectDataBufferSize = 0;
//
//    pResource->SetFlags( GameObject::FLAG_PRELOADED );
//
//    pRequest->flags |= LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED;
//
//    return true;
//}
//
///// Constructor.
//XmlPackageLoader::Deserializer::Deserializer()
//: m_pObjectData( NULL )
//, m_pLinkTable( NULL )
//{
//}
//
///// Destructor.
//XmlPackageLoader::Deserializer::~Deserializer()
//{
//}
//
///// Prepare for deserialization of object data.
/////
///// @param[in] pObjectData  Pointer to the data containing data to be deserialized.
///// @param[in[ pLinkTable   Pointer to the object link table.
//void XmlPackageLoader::Deserializer::Prepare( SerializedObjectData* pObjectData, DynArray< LinkEntry >* pLinkTable )
//{
//    m_pObjectData = pObjectData;
//    m_pLinkTable = pLinkTable;
//}
//
///// @copydoc Serializer::Serialize()
//bool XmlPackageLoader::Deserializer::Serialize( GameObject* pObject )
//{
//    HELIUM_ASSERT( pObject );
//
//    // Make sure this deserializer has been properly prepared.
//    HELIUM_ASSERT( m_pObjectData );
//    HELIUM_ASSERT( m_pLinkTable );
//    if( !m_pObjectData || !m_pLinkTable )
//    {
//        return false;
//    }
//
//    // De-serialize the object.
//    m_tagStack.Resize( 0 );
//    HELIUM_VERIFY( m_tagStack.New() );
//
//    pObject->Serialize( *this );
//
//    m_tagStack.Resize( 0 );
//
//    return true;
//}
//
///// @copydoc Serializer::GetMode()
//Serializer::EMode XmlPackageLoader::Deserializer::GetMode() const
//{
//    return MODE_LOAD;
//}
//
///// @copydoc Serializer::SerializeTag()
//void XmlPackageLoader::Deserializer::SerializeTag( const Tag& rTag )
//{
//    HELIUM_ASSERT( !m_tagStack.IsEmpty() );
//
//    // Don't track tags while deserializing an array.
//    TagStackElement& rTagElement = m_tagStack.GetLast();
//    if( IsValid( rTagElement.index ) )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            TXT( "XmlPackageLoader::Deserializer: Tag encountered while deserializing an array." ) );
//
//        return;
//    }
//
//    rTagElement.tag = rTag;
//}
//
///// @copydoc Serializer::CanResolveTags()
//bool XmlPackageLoader::Deserializer::CanResolveTags() const
//{
//    return true;
//}
//
///// @copydoc Serializer::GetPropertyTagNames()
//void XmlPackageLoader::Deserializer::GetPropertyTagNames( DynArray< String >& rTagNames ) const
//{
//    rTagNames.Resize( 0 );
//
//    HELIUM_ASSERT( m_pObjectData );
//    ConcurrentHashMap< String, String >& rPropertyMap = m_pObjectData->properties;
//    ConcurrentHashMap< String, String >::ConstAccessor propertyAccessor;
//    if( rPropertyMap.First( propertyAccessor ) )
//    {
//        do
//        {
//            rTagNames.Add( propertyAccessor->First() );
//            ++propertyAccessor;
//        } while( propertyAccessor.IsValid() );
//    }
//}
//
///// @copydoc Serializer::SerializeBool()
//void XmlPackageLoader::Deserializer::SerializeBool( bool& rValue )
//{
//    uint64_t readValue = rValue;
//    ReadValue(
//        readValue,
//        TXT( "bool" ),
//        ScanfParser< uint64_t >( TXT( "%" ) TSCNu64 ),
//        NullDefaultHandler< uint64_t >() );
//
//    rValue = ( readValue != 0 );
//}
//
///// @copydoc Serializer::SerializeInt8()
//void XmlPackageLoader::Deserializer::SerializeInt8( int8_t& rValue )
//{
//    ReadValue(
//        rValue,
//        TXT( "int8_t" ),
//        ScanfParser< int8_t >( TXT( "%" ) TSCNd8 ),
//        NullDefaultHandler< int8_t >() );
//}
//
///// @copydoc Serializer::SerializeUint8()
//void XmlPackageLoader::Deserializer::SerializeUint8( uint8_t& rValue )
//{
//    ReadValue(
//        rValue,
//        TXT( "uint8_t" ),
//        ScanfParser< uint8_t >( TXT( "%" ) TSCNu8 ),
//        NullDefaultHandler< uint8_t >() );
//}
//
///// @copydoc Serializer::SerializeInt16()
//void XmlPackageLoader::Deserializer::SerializeInt16( int16_t& rValue )
//{
//    ReadValue(
//        rValue,
//        TXT( "int16_t" ),
//        ScanfParser< int16_t >( TXT( "%" ) TSCNd16 ),
//        NullDefaultHandler< int16_t >() );
//}
//
///// @copydoc Serializer::SerializeUint16()
//void XmlPackageLoader::Deserializer::SerializeUint16( uint16_t& rValue )
//{
//    ReadValue(
//        rValue,
//        TXT( "uint16_t" ),
//        ScanfParser< uint16_t >( TXT( "%" ) TSCNu16 ),
//        NullDefaultHandler< uint16_t >() );
//}
//
///// @copydoc Serializer::SerializeInt32()
//void XmlPackageLoader::Deserializer::SerializeInt32( int32_t& rValue )
//{
//    ReadValue(
//        rValue,
//        TXT( "int32_t" ),
//        ScanfParser< int32_t >( TXT( "%" ) TSCNd32 ),
//        NullDefaultHandler< int32_t >() );
//}
//
///// @copydoc Serializer::SerializeUint32()
//void XmlPackageLoader::Deserializer::SerializeUint32( uint32_t& rValue )
//{
//    // Load the result value with the array size if we just started a dynamic array.
//    HELIUM_ASSERT( !m_tagStack.IsEmpty() );
//    TagStackElement& rTagElement = m_tagStack.GetLast();
//    if( IsInvalid( rTagElement.index + 1 ) )
//    {
//        // Set the current tag element's index to an invalid value so that we can get the identifier for the array
//        // property (having a valid index value will append the index to the name, which we don't want for looking
//        // up the array size).
//        SetInvalid( rTagElement.index );
//
//        String propertyName;
//        bool bBuildResult = BuildCurrentPropertyName( propertyName );
//
//        rTagElement.index = 0;
//
//        if( bBuildResult )
//        {
//            ConcurrentHashMap< String, uint32_t >::ConstAccessor arraySizeAccessor;
//            if( m_pObjectData->arraySizes.Find( arraySizeAccessor, propertyName ) )
//            {
//                rValue = arraySizeAccessor->Second();
//            }
//        }
//
//        return;
//    }
//
//    ReadValue(
//        rValue,
//        TXT( "uint32_t" ),
//        ScanfParser< uint32_t >( TXT( "%" ) TSCNu32 ),
//        NullDefaultHandler< uint32_t >() );
//}
//
///// @copydoc Serializer::SerializeInt64()
//void XmlPackageLoader::Deserializer::SerializeInt64( int64_t& rValue )
//{
//    ReadValue(
//        rValue,
//        TXT( "int64_t" ),
//        ScanfParser< int64_t >( TXT( "%" ) TSCNd64 ),
//        NullDefaultHandler< int64_t >() );
//}
//
///// @copydoc Serializer::SerializeUint64()
//void XmlPackageLoader::Deserializer::SerializeUint64( uint64_t& rValue )
//{
//    ReadValue(
//        rValue,
//        TXT( "uint64_t" ),
//        ScanfParser< uint64_t >( TXT( "%" ) TSCNu64 ),
//        NullDefaultHandler< uint64_t >() );
//}
//
///// @copydoc Serializer::SerializeFloat32()
//void XmlPackageLoader::Deserializer::SerializeFloat32( float32_t& rValue )
//{
//    ReadValue(
//        rValue,
//        TXT( "float32_t" ),
//        ScanfFloatParser< float32_t >( TXT( "%f" ) ),
//        NullDefaultHandler< float32_t >() );
//}
//
///// @copydoc Serializer::SerializeFloat64()
//void XmlPackageLoader::Deserializer::SerializeFloat64( float64_t& rValue )
//{
//    ReadValue(
//        rValue,
//        TXT( "float64_t" ),
//        ScanfFloatParser< float64_t >( TXT( "%lf" ) ),
//        NullDefaultHandler< float64_t >() );
//}
//
///// @copydoc Serializer::SerializeBuffer()
//void XmlPackageLoader::Deserializer::SerializeBuffer( void* /*pBuffer*/, size_t /*elementSize*/, size_t /*count*/ )
//{
//    HELIUM_ASSERT_MSG_FALSE( TXT( "SerializeBuffer() is not supported when deserializing from XML data." ) );
//}
//
///// @copydoc Serializer::SerializeEnum()
//void XmlPackageLoader::Deserializer::SerializeEnum(
//    int32_t& rValue,
//    uint32_t nameCount,
//    const tchar_t* const* ppNames )
//{
//    ReadValue( rValue, TXT( "enum" ), EnumParser( nameCount, ppNames ), NullDefaultHandler< int32_t >() );
//}
//
///// @copydoc Serializer::SerializeEnum()
//void XmlPackageLoader::Deserializer::SerializeEnum(
//    int32_t& /*rValue*/,
//    const Helium::Reflect::Enumeration* /*pEnumeration*/ )
//{
//#pragma TODO("Reflect enumeration support")
//}
//
///// @copydoc Serializer::SerializeCharName()
//void XmlPackageLoader::Deserializer::SerializeCharName( CharName& rValue )
//{
//    ReadValue( rValue, TXT( "CharName" ), CharNameParser(), NullDefaultHandler< CharName >() );
//}
//
///// @copydoc Serializer::SerializeWideName()
//void XmlPackageLoader::Deserializer::SerializeWideName( WideName& rValue )
//{
//    ReadValue( rValue, TXT( "WideName" ), WideNameParser(), NullDefaultHandler< WideName >() );
//}
//
///// @copydoc Serializer::SerializeCharString()
//void XmlPackageLoader::Deserializer::SerializeCharString( CharString& rValue )
//{
//    ReadValue( rValue, TXT( "CharString" ), CharStringParser(), NullDefaultHandler< CharString >() );
//}
//
///// @copydoc Serializer::SerializeWideString()
//void XmlPackageLoader::Deserializer::SerializeWideString( WideString& rValue )
//{
//    ReadValue( rValue, TXT( "WideString" ), WideStringParser(), NullDefaultHandler< WideString >() );
//}
//
///// @copydoc Serializer::SerializeObjectReference()
//void XmlPackageLoader::Deserializer::SerializeObjectReference(
//    const GameObjectType* /*pType*/,
//    GameObjectPtr& rspObject )
//{
//    ReadValue(
//        rspObject,
//        TXT( "GameObject reference" ),
//        ObjectParser( m_pLinkTable ),
//        ObjectDefaultHandler( m_pLinkTable ) );
//}
//
///// @copydoc Serializer::BeginStruct()
//void XmlPackageLoader::Deserializer::BeginStruct( EStructTag /*tag*/ )
//{
//    // Push another level onto the tag stack.  Note that validation of tag names is only performed when actually
//    // deserializing values.
//    HELIUM_VERIFY( m_tagStack.New() );
//}
//
///// @copydoc Serializer::EndStruct()
//void XmlPackageLoader::Deserializer::EndStruct()
//{
//    // Pop the top level off the tag stack.
//    if( m_tagStack.GetSize() == 1 )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            TXT( "XmlPackageLoader::Deserializer: EndStruct() called without a corresponding BeginStruct().\n" ) );
//
//        return;
//    }
//
//    m_tagStack.Pop();
//
//    // If the top-level tag is an array index, increment it.
//    TagStackElement& rTag = m_tagStack.GetLast();
//    if( IsValid( rTag.index ) )
//    {
//        ++rTag.index;
//    }
//}
//
///// @copydoc Serializer::BeginArray()
//void XmlPackageLoader::Deserializer::BeginArray( uint32_t /*size*/ )
//{
//    // Set the index in the top-most tag to signify that we are starting an array.
//    HELIUM_ASSERT( !m_tagStack.IsEmpty() );
//    TagStackElement& rTagElement = m_tagStack.GetLast();
//    if( !rTagElement.tag.Get() )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            ( TXT( "XmlPackageLoader::Deserializer: BeginArray() called without having properly serialized a " )
//            TXT( "property tag.\n" ) ) );
//
//        return;
//    }
//
//    if( IsValid( rTagElement.index ) )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            TXT( "XmlPackageLoader::Deserializer: BeginArray() called while already serializing an array.\n" ) );
//
//        return;
//    }
//
//    rTagElement.index = 0;
//}
//
///// @copydoc Serializer::EndArray()
//void XmlPackageLoader::Deserializer::EndArray()
//{
//    // Clear out the top-most tag.
//    HELIUM_ASSERT( !m_tagStack.IsEmpty() );
//    TagStackElement& rTagElement = m_tagStack.GetLast();
//    if( !rTagElement.tag.Get() || IsInvalid( rTagElement.index ) )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            TXT( "XmlPackageLoader::Deserializer: EndArray() called outside of array serialization.\n" ) );
//
//        return;
//    }
//
//    rTagElement.tag = Tag( NULL );
//    SetInvalid( rTagElement.index );
//}
//
///// @copydoc Serializer::BeginDynArray()
//void XmlPackageLoader::Deserializer::BeginDynArray()
//{
//    // Set the index in the top-most tag to signify that we are starting a dynamic array.  The index is set to a
//    // special value (invalid index minus 1) to signify that we are waiting for the dynamic array size.
//    HELIUM_ASSERT( !m_tagStack.IsEmpty() );
//    TagStackElement& rTagElement = m_tagStack.GetLast();
//    if( !rTagElement.tag.Get() )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            ( TXT( "XmlPackageLoader::Deserializer: BeginDynArray() called without having properly serialized a " )
//            TXT( "property tag.\n" ) ) );
//
//        return;
//    }
//
//    if( IsValid( rTagElement.index ) )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            TXT( "XmlPackageLoader::Deserializer: BeginDynArray() called while already serializing an array.\n" ) );
//
//        return;
//    }
//
//    SetInvalid( rTagElement.index );
//    --rTagElement.index;
//}
//
///// @copydoc Serializer::EndDynArray()
//void XmlPackageLoader::Deserializer::EndDynArray()
//{
//    // Clear out the top-most tag.
//    HELIUM_ASSERT( !m_tagStack.IsEmpty() );
//    TagStackElement& rTagElement = m_tagStack.GetLast();
//    if( !rTagElement.tag.Get() || IsInvalid( rTagElement.index ) )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            TXT( "XmlPackageLoader::Deserializer: EndDynArray() called outside of array serialization.\n" ) );
//
//        return;
//    }
//
//    rTagElement.tag = Tag( NULL );
//    SetInvalid( rTagElement.index );
//}
//
///// De-serialize the given value.
/////
///// @param[in] rValue           Value in which to deserialize.
///// @param[in] pTypeString      String used to specify the type being serialized (for logging).
///// @param[in] rParser          Value parsing functor.
///// @param[in] rDefaultHandler  Functor for performing any necessary processing of the serialized property when the
/////                             value is not found in the XML property map.
//template< typename T, typename Parser, typename DefaultHandler >
//void XmlPackageLoader::Deserializer::ReadValue(
//    T& rValue,
//    const tchar_t* pTypeString,
//    const Parser& rParser,
//    const DefaultHandler& rDefaultHandler )
//{
//    HELIUM_UNREF( pTypeString );  // Logging only.
//
//    HELIUM_ASSERT( pTypeString );
//
//    // Make sure we're not waiting for a dynamic array size.
//    HELIUM_ASSERT( !m_tagStack.IsEmpty() );
//    TagStackElement& rTagElement = m_tagStack.GetLast();
//    if( IsInvalid( rTagElement.index + 1 ) )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            ( TXT( "XmlPackageLoader::Deserializer: Attempted to tag a value other than a uint32_t when " )
//            TXT( "deserializing a dynamic array size.\n" ) ) );
//
//        rTagElement.index = 0;
//
//        return;
//    }
//
//    // Build the string identifying the current property.
//    String propertyName;
//    if( !BuildCurrentPropertyName( propertyName ) )
//    {
//        return;
//    }
//
//    // If the current tag can be found in the active object's property map, use its value.
//    const ConcurrentHashMap< String, String >& rProperties = m_pObjectData->properties;
//    ConcurrentHashMap< String, String >::ConstAccessor propertyAccessor;
//    if( rProperties.Find( propertyAccessor, propertyName ) )
//    {
//        if( !rParser( propertyAccessor->Second(), rValue ) )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader::Deserializer: Failed to parse a %s from text for property \"%s\" in " )
//                TXT( "object \"%s\".  Using template value.\n" ) ),
//                pTypeString,
//                *propertyName,
//                *m_pObjectData->objectPath.ToString() );
//        }
//    }
//    else
//    {
//        rDefaultHandler( rValue );
//    }
//
//    // If an array element was just deserialized, update the current array index, otherwise clear the tag so we know
//    // not to accidentally use it for consecutive properties.
//    if( IsValid( rTagElement.index ) )
//    {
//        ++rTagElement.index;
//    }
//    else
//    {
//        rTagElement.tag = Tag( NULL );
//    }
//}
//
///// Build the string identifying the current property.
/////
///// @param[out] rPropertyName  Property name string.  This is modified regardless of whether the full name string
/////                            was built successfully.
/////
///// @return  True if the name string was built successfully, false if invalid tagging was used up to the current
/////          property.
//bool XmlPackageLoader::Deserializer::BuildCurrentPropertyName( String& rPropertyName ) const
//{
//    rPropertyName.Remove( 0, rPropertyName.GetSize() );
//
//    String arrayIndexFormat;
//    size_t tagStackSize = m_tagStack.GetSize();
//    for( size_t tagIndex = 0; tagIndex < tagStackSize; ++tagIndex )
//    {
//        if( tagIndex != 0 )
//        {
//            rPropertyName += TXT( '.' );
//        }
//
//        const TagStackElement& rTagElement = m_tagStack[ tagIndex ];
//
//        const tchar_t* pTagString = rTagElement.tag.Get();
//        // Note that an invalid index minus 1 means that we are or were waiting for a dynamic array size to be
//        // deserialized, so a tag stack element with that value means we have an invalid property.
//        if( !pTagString || IsInvalid( rTagElement.index + 1 ) )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "XmlPackageLoader::Deserializer: Attempted to deserialize a property without having " )
//                TXT( "properly serialized a tag.\n" ) ) );
//
//            return false;
//        }
//
//        rPropertyName += pTagString;
//
//        if( IsValid( rTagElement.index ) )
//        {
//            arrayIndexFormat.Format( TXT( "[%" ) TPRIu32 TXT( "]" ), rTagElement.index );
//            rPropertyName += arrayIndexFormat;
//        }
//    }
//
//    return true;
//}
//
///// Constructor.
/////
///// @param[in] pFormat  "scanf"-style format string for parsing the value.
//template< typename T >
//XmlPackageLoader::Deserializer::ScanfParser< T >::ScanfParser( const tchar_t* pFormat )
//: m_pFormat( pFormat )
//{
//    HELIUM_ASSERT( pFormat );
//}
//
///// Parse a value from the given property text.
/////
///// @param[in]  rText   Property text to parse.
///// @param[out] rValue  Parsed value.
/////
///// @return  True if a value was parsed successfully, false if not.
//template< typename T >
//bool XmlPackageLoader::Deserializer::ScanfParser< T >::operator()( const String& rText, T& rValue ) const
//{
//    if( rText.GetSize() == 0 )
//    {
//        return false;
//    }
//
//    const tchar_t* pTextData = rText.GetData();
//    HELIUM_ASSERT( pTextData );
//
//    int parseResult;
//#if HELIUM_UNICODE
//#ifdef _MSC_VER
//    parseResult = swscanf_s( pTextData, m_pFormat, &rValue );
//#else
//    parseResult = swscanf( pTextData, m_pFormat, &rValue );
//#endif
//#else
//#ifdef _MSC_VER
//    parseResult = sscanf_s( pTextData, m_pFormat, &rValue );
//#else
//    parseResult = sscanf( pTextData, m_pFormat, &rValue );
//#endif
//#endif
//
//    return ( parseResult == 1 );
//}
//
///// Constructor.
/////
///// @param[in] pDecFormat  "scanf"-style format string for parsing the value as a decimal (base-10) string.
//template< typename T >
//XmlPackageLoader::Deserializer::ScanfFloatParser< T >::ScanfFloatParser( const tchar_t* pDecFormat )
//: m_pDecFormat( pDecFormat )
//{
//    HELIUM_ASSERT( m_pDecFormat );
//}
//
///// Parse a value from the given property text.
/////
///// @param[in]  rText   Property text to parse.
///// @param[out] rValue  Parsed value.
/////
///// @return  True if a value was parsed successfully, false if not.
//template< typename T >
//bool XmlPackageLoader::Deserializer::ScanfFloatParser< T >::operator()( const String& rText, T& rValue ) const
//{
//    size_t textSize = rText.GetSize();
//    if( textSize == 0 )
//    {
//        return false;
//    }
//
//    const tchar_t* pTextData = rText.GetData();
//    HELIUM_ASSERT( pTextData );
//
//    // Attempt to parse as a hexadecimal value first.
//    if( ParseHex( rText, rValue ) )
//    {
//        return true;
//    }
//
//    // Fall back to decimal format parsing.
//    int parseResult;
//#if HELIUM_UNICODE
//#ifdef _MSC_VER
//    parseResult = swscanf_s( pTextData, m_pDecFormat, &rValue );
//#else
//    parseResult = swscanf( pTextData, m_pDecFormat, &rValue );
//#endif
//#else
//#ifdef _MSC_VER
//    parseResult = sscanf_s( pTextData, m_pDecFormat, &rValue );
//#else
//    parseResult = sscanf( pTextData, m_pDecFormat, &rValue );
//#endif
//#endif
//
//    return ( parseResult == 1 );
//}
//
///// Parse a hexadecimal floating point value.
/////
///// This is implemented to make up for the lack of support for the "a" type identifier (used for "printf"-style
///// outputting of floating point values in hexadecimal format) in "scanf"-style format strings.
/////
///// @param[in]  rText   Property text to parse.
///// @param[out] rValue  Parsed value.
/////
///// @return  True if a value was parsed successfully, false if not.
//template< typename T >
//bool XmlPackageLoader::Deserializer::ScanfFloatParser< T >::ParseHex( const String& rText, T& rValue )
//{
//    tchar_t character;
//
//    size_t textSize = rText.GetSize();
//    HELIUM_ASSERT( textSize != 0 );
//
//    size_t characterIndex = 0;
//    do
//    {
//        character = rText[ characterIndex ];
//#if HELIUM_UNICODE
//        if( !iswspace( character ) )
//#else
//        if( !isspace( character ) )
//#endif
//        {
//            break;
//        }
//
//        ++characterIndex;
//    } while( characterIndex < textSize );
//
//    if( characterIndex >= textSize )
//    {
//        return false;
//    }
//
//    bool bNegative = ( rText[ characterIndex ] == TXT( '-' ) );
//    if( bNegative )
//    {
//        ++characterIndex;
//        if( characterIndex >= textSize )
//        {
//            return false;
//        }
//    }
//
//    if( textSize - characterIndex < 2 ||
//        rText[ characterIndex ] != TXT( '0' ) ||
//        ( rText[ characterIndex + 1 ] != TXT( 'x' ) && rText[ characterIndex + 1 ] != TXT( 'X' ) ) )
//    {
//        return false;
//    }
//
//    characterIndex += 2;
//
//    uint64_t integerComponent = 0;
//    uint64_t numeratorComponent = 0;
//    uint64_t denominatorComponent = 1;
//    uint64_t exponentComponent = 0;
//    bool bNegativeExponent = false;
//
//    do
//    {
//        character = rText[ characterIndex ];
//        if( character >= TXT( '0' ) && character <= TXT( '9' ) )
//        {
//            integerComponent = integerComponent * 0x10 + character - TXT( '0' );
//        }
//        else if( character >= TXT( 'a' ) && character <= TXT( 'f' ) )
//        {
//            integerComponent = integerComponent * 0x10 + character - TXT( 'a' ) + 0xa;
//        }
//        else if( character >= TXT( 'A' ) && character <= TXT( 'F' ) )
//        {
//            integerComponent = integerComponent * 0x10 + character - TXT( 'A' ) + 0xa;
//        }
//        else
//        {
//            break;
//        }
//
//        ++characterIndex;
//    } while( characterIndex < textSize );
//
//    if( characterIndex < textSize )
//    {
//        character = rText[ characterIndex ];
//        if( character == TXT( '.' ) )
//        {
//            ++characterIndex;
//            while( characterIndex < textSize )
//            {
//                character = rText[ characterIndex ];
//                if( character >= TXT( '0' ) && character <= TXT( '9' ) )
//                {
//                    numeratorComponent = numeratorComponent * 0x10 + character - TXT( '0' );
//                }
//                else if( character >= TXT( 'a' ) && character <= TXT( 'f' ) )
//                {
//                    numeratorComponent = numeratorComponent * 0x10 + character - TXT( 'a' ) + 0xa;
//                }
//                else if( character >= TXT( 'A' ) && character <= TXT( 'F' ) )
//                {
//                    numeratorComponent = numeratorComponent * 0x10 + character - TXT( 'A' ) + 0xa;
//                }
//                else
//                {
//                    break;
//                }
//
//                denominatorComponent *= 0x10;
//                ++characterIndex;
//            }
//        }
//    }
//
//    if( characterIndex < textSize )
//    {
//        character = rText[ characterIndex ];
//        if( character == TXT( 'p' ) || character == TXT( 'P' ) )
//        {
//            ++characterIndex;
//            if( characterIndex < textSize )
//            {
//                bNegativeExponent = ( rText[ characterIndex ] == TXT( '-' ) );
//                if( bNegativeExponent )
//                {
//                    ++characterIndex;
//                }
//
//                while( characterIndex < textSize )
//                {
//                    character = rText[ characterIndex ];
//                    if( character >= TXT( '0' ) && character <= TXT( '9' ) )
//                    {
//                        exponentComponent = exponentComponent * 0x10 + character - TXT( '0' );
//                    }
//                    else if( character >= TXT( 'a' ) && character <= TXT( 'f' ) )
//                    {
//                        exponentComponent = exponentComponent * 0x10 + character - TXT( 'a' ) + 0xa;
//                    }
//                    else if( character >= TXT( 'A' ) && character <= TXT( 'F' ) )
//                    {
//                        exponentComponent = exponentComponent * 0x10 + character - TXT( 'A' ) + 0xa;
//                    }
//                    else
//                    {
//                        break;
//                    }
//
//                    ++characterIndex;
//                }
//            }
//        }
//    }
//
//    // Check for possible division and exponent calculation issues due to integer wrapping.
//    if( denominatorComponent == 0 || exponentComponent > sizeof( uint64_t ) * 8 - 2 )
//    {
//        return false;
//    }
//
//    T exponentScale = static_cast< T >( static_cast< uint64_t >( 1 ) << exponentComponent );
//    if( bNegativeExponent )
//    {
//        exponentScale = static_cast< T >( 1 ) / exponentScale;
//    }
//
//    rValue =
//        ( static_cast< T >( integerComponent ) +
//        static_cast< T >( numeratorComponent ) / static_cast< T >( denominatorComponent ) ) *
//        exponentScale;
//    if( bNegative )
//    {
//        rValue = -rValue;
//    }
//
//    return true;
//}
//
///// Constructor.
/////
///// @param[in] nameCount  Number of enumeration value names.
///// @param[in] ppNames    Array of enumeration name strings.
//XmlPackageLoader::Deserializer::EnumParser::EnumParser( uint32_t nameCount, const tchar_t* const* ppNames )
//: m_nameCount( nameCount )
//, m_ppNames( ppNames )
//{
//    HELIUM_ASSERT( nameCount != 0 );
//    HELIUM_ASSERT( ppNames );
//}
//
///// Parse a value from the given property text.
/////
///// @param[in]  rText   Property text to parse.
///// @param[out] rValue  Parsed value.
/////
///// @return  True if a value was parsed successfully, false if not.
//bool XmlPackageLoader::Deserializer::EnumParser::operator()( const String& rText, int32_t& rValue ) const
//{
//    for( uint32_t nameIndex = 0; nameIndex < m_nameCount; ++nameIndex )
//    {
//        if( rText == m_ppNames[ nameIndex ] )
//        {
//            rValue = static_cast< int32_t >( nameIndex );
//
//            return true;
//        }
//    }
//
//    rValue = -1;
//
//    return true;
//}
//
///// Parse a value from the given property text.
/////
///// @param[in]  rText   Property text to parse.
///// @param[out] rValue  Parsed value.
/////
///// @return  True if a value was parsed successfully, false if not.
//bool XmlPackageLoader::Deserializer::CharStringParser::operator()( const String& rText, CharString& rValue ) const
//{
//#if HELIUM_UNICODE
//
//    if( rText.IsEmpty() )
//    {
//        rValue.Clear();
//
//        return true;
//    }
//
//    const wchar_t* pTextString = rText.GetData();
//    HELIUM_ASSERT( pTextString );
//
//#ifdef _MSC_VER
//    size_t charCount = static_cast< size_t >( -1 );
//    wcstombs_s( &charCount, NULL, 0, pTextString, _TRUNCATE );
//#else
//    size_t charCount = wcstombs( NULL, pTextString, rText.GetSize() );
//#endif
//    if( charCount == static_cast< size_t >( -1 ) )
//    {
//        return false;
//    }
//
//    if( charCount == 0 )
//    {
//        rValue.Clear();
//
//        return true;
//    }
//
//    rValue.Remove( 0, rValue.GetSize() );
//    rValue.Add( ' ', charCount );
//
//    char* pValueString = &rValue[ 0 ];
//    HELIUM_ASSERT( pValueString );
//#ifdef _MSC_VER
//    wcstombs_s( NULL, pValueString, charCount + 1, pTextString, charCount );
//#else
//    wcstombs( pValueString, pTextString, rText.GetSize() );
//#endif
//    pValueString[ charCount ] = '\0';
//
//    return true;
//
//#else  // HELIUM_UNICODE
//
//    rValue = rText;
//
//    return true;
//
//#endif  // HELIUM_UNICODE
//}
//
///// Parse a value from the given property text.
/////
///// @param[in]  rText   Property text to parse.
///// @param[out] rValue  Parsed value.
/////
///// @return  True if a value was parsed successfully, false if not.
//bool XmlPackageLoader::Deserializer::WideStringParser::operator()( const String& rText, WideString& rValue ) const
//{
//#if HELIUM_UNICODE
//
//    rValue = rText;
//
//    return true;
//
//#else  // HELIUM_UNICODE
//
//    if( rText.IsEmpty() )
//    {
//        rValue.Clear();
//
//        return true;
//    }
//
//    const char* pTextString = rText.GetData();
//    HELIUM_ASSERT( pTextString );
//
//#ifdef _MSC_VER
//    size_t wideCharCount = static_cast< size_t >( -1 );
//    mbstowcs_s( &wideCharCount, NULL, 0, pTextString, _TRUNCATE );
//#else
//    size_t wideCharCount = mbstowcs( NULL, pTextString, rText.GetSize() );
//#endif
//    if( wideCharCount == static_cast< size_t >( -1 ) )
//    {
//        return false;
//    }
//
//    if( wideCharCount == 0 )
//    {
//        rValue.Clear();
//
//        return true;
//    }
//
//    rValue.Remove( 0, rValue.GetSize() );
//    rValue.Add( ' ', wideCharCount );
//
//    wchar_t* pValueString = &rValue[ 0 ];
//    HELIUM_ASSERT( pValueString );
//#ifdef _MSC_VER
//    mbstowcs_s( NULL, pValueString, wideCharCount + 1, pTextString, wideCharCount );
//#else
//    mbstowcs( pValueString, pTextString, rText.GetSize() );
//#endif
//    pValueString[ wideCharCount ] = L'\0';
//
//    return true;
//
//#endif  // HELIUM_UNICODE
//}
//
///// Parse a value from the given property text.
/////
///// @param[in]  rText   Property text to parse.
///// @param[out] rValue  Parsed value.
/////
///// @return  True if a value was parsed successfully, false if not.
//bool XmlPackageLoader::Deserializer::CharNameParser::operator()( const String& rText, CharName& rValue ) const
//{
//#if HELIUM_UNICODE
//    CharString tempString;
//    m_stringParser( rText, tempString );
//    rValue.Set( tempString );
//#else
//    rValue.Set( rText );
//#endif
//
//    return true;
//}
//
///// Parse a value from the given property text.
/////
///// @param[in]  rText   Property text to parse.
///// @param[out] rValue  Parsed value.
/////
///// @return  True if a value was parsed successfully, false if not.
//bool XmlPackageLoader::Deserializer::WideNameParser::operator()( const String& rText, WideName& rValue ) const
//{
//#if HELIUM_UNICODE
//    rValue.Set( rText );
//#else
//    WideString tempString;
//    m_stringParser( rText, tempString );
//    rValue.Set( tempString );
//#endif
//
//    return true;
//}
//
///// Constructor.
/////
///// @param[in] pLinkTable  GameObject link table.
//XmlPackageLoader::Deserializer::ObjectParser::ObjectParser( DynArray< LinkEntry >* pLinkTable )
//: m_pLinkTable( pLinkTable )
//{
//    HELIUM_ASSERT( pLinkTable );
//}
//
///// Parse a value from the given property text.
/////
///// @param[in]  rText     Property text to parse.
///// @param[out] rspValue  Parsed value.
/////
///// @return  True if a value was parsed successfully, false if not.
//bool XmlPackageLoader::Deserializer::ObjectParser::operator()( const String& rText, GameObjectPtr& rspValue ) const
//{
//    if( rText.IsEmpty() )
//    {
//        rspValue.Release();
//        rspValue.SetLinkIndex( Invalid< uint32_t >() );
//
//        return true;
//    }
//
//    GameObjectPath path;
//    if( !path.Set( rText ) )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            TXT( "XmlPackageLoader::Deserializer: \"%s\" does not represent a valid object path.\n" ),
//            *rText );
//
//        return false;
//    }
//
//    HELIUM_ASSERT( m_pLinkTable );
//    size_t linkTableSize = m_pLinkTable->GetSize();
//    size_t linkIndex;
//    for( linkIndex = 0; linkIndex < linkTableSize; ++linkIndex )
//    {
//        LinkEntry& rLinkEntry = m_pLinkTable->GetElement( linkIndex );
//        if( rLinkEntry.path == path )
//        {
//            break;
//        }
//    }
//
//    if( linkIndex >= linkTableSize )
//    {
//        HELIUM_ASSERT( linkIndex == linkTableSize );
//        HELIUM_ASSERT( linkIndex < UINT32_MAX );
//
//        LinkEntry* pLinkEntry = m_pLinkTable->New();
//        HELIUM_ASSERT( pLinkEntry );
//        pLinkEntry->path = path;
//
//        GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
//        HELIUM_ASSERT( pObjectLoader );
//        pLinkEntry->loadRequestId = pObjectLoader->BeginLoadObject( path );
//        if( IsInvalid( pLinkEntry->loadRequestId ) )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                TXT( "XmlPackageLoader: Failed to begin loading \"%s\" as a link dependency.\n" ),
//                *rText );
//        }
//    }
//
//    HELIUM_ASSERT( linkIndex < UINT32_MAX );
//
//    rspValue.Release();
//    rspValue.SetLinkIndex( static_cast< uint32_t >( linkIndex ) );
//
//    return true;
//}
//
///// Process a value that was not found in the XML property map.
/////
///// @param[in] rValue  Value to process.
//template< typename T >
//void XmlPackageLoader::Deserializer::NullDefaultHandler< T >::operator()( T& /*rValue*/ ) const
//{
//}
//
///// Constructor.
//XmlPackageLoader::Deserializer::ObjectDefaultHandler::ObjectDefaultHandler( DynArray< LinkEntry >* pLinkTable )
//: m_pLinkTable( pLinkTable )
//{
//    HELIUM_ASSERT( pLinkTable );
//}
//
///// Process a value that was not found in the XML property map.
/////
///// @param[in] rspValue  Value to process.
//void XmlPackageLoader::Deserializer::ObjectDefaultHandler::operator()( GameObjectPtr& rspValue ) const
//{
//    // GameObject references need to be converted to link table indices, so if there is a reference, make sure it exists
//    // in the link table.
//    GameObject* pObject = rspValue;
//    if( !pObject )
//    {
//        rspValue.Release();
//        rspValue.SetLinkIndex( Invalid< uint32_t >() );
//
//        return;
//    }
//
//    GameObjectPath objectPath = pObject->GetPath();
//
//    size_t linkTableSize = m_pLinkTable->GetSize();
//    for( size_t linkTableIndex = 0; linkTableIndex < linkTableSize; ++linkTableIndex )
//    {
//        if( m_pLinkTable->GetElement( linkTableIndex ).path == objectPath )
//        {
//            rspValue.Release();
//            rspValue.SetLinkIndex( static_cast< uint32_t >( linkTableIndex ) );
//
//            return;
//        }
//    }
//
//    LinkEntry* pNewEntry = m_pLinkTable->New();
//    HELIUM_ASSERT( pNewEntry );
//    pNewEntry->path = objectPath;
//
//    GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
//    HELIUM_ASSERT( pObjectLoader );
//    pNewEntry->loadRequestId = pObjectLoader->BeginLoadObject( objectPath );
//    if( IsInvalid( pNewEntry->loadRequestId ) )
//    {
//        HELIUM_TRACE(
//            TRACE_ERROR,
//            TXT( "XmlPackageLoader: Failed to begin reloading \"%s\" as a link dependency.\n" ),
//            *objectPath.ToString() );
//    }
//
//    rspValue.Release();
//    rspValue.SetLinkIndex( static_cast< uint32_t >( linkTableSize ) );
//}
//
///// Constructor.
//XmlPackageLoader::Deserializer::TagStackElement::TagStackElement()
//: tag( NULL )
//, index( Invalid< uint32_t >() )
//{
//}
