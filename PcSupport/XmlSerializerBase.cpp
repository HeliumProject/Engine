////----------------------------------------------------------------------------------------------------------------------
//// XmlSerializerBase.cpp
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
#include "PcSupportPch.h"
//#include "PcSupport/XmlSerializerBase.h"
//
//using namespace Helium;
//
//// Structure tag strings.
//static const char STRUCT_TAG_START[] = "<struct>" HELIUM_XML_NEWLINE;
//static const char STRUCT_TAG_END[] = "</struct>" HELIUM_XML_NEWLINE;
//
//// Array tag strings.
//static const char ARRAY_TAG_START[] = "<array>" HELIUM_XML_NEWLINE;
//static const char ARRAY_TAG_END[] = "</array>" HELIUM_XML_NEWLINE;
//
//// Property tag strings.
//static const char PROPERTY_TAG_START_A[] = "<property";
//static const char PROPERTY_TAG_START_B[] = " name=\"";
//static const char PROPERTY_TAG_START_C[] = "\"";
//static const char PROPERTY_TAG_START_D[] = ">";
//static const char PROPERTY_TAG_END[] = "</property>" HELIUM_XML_NEWLINE;
//
//// General strings.
//static const char TAB_STRING[] = "    ";
//static const char NEWLINE_STRING[] = HELIUM_XML_NEWLINE;
//
///// Constructor.
//XmlSerializerBase::XmlSerializerBase()
//: m_propertyTag( NULL )
//, m_currentTag( NULL )
//, m_bClosePropertyWithTabs( false )
//, m_bIgnoreNextValue( false )
//{
//}
//
///// Destructor.
//XmlSerializerBase::~XmlSerializerBase()
//{
//}
//
///// @copydoc Serializer::Serialize()
//bool XmlSerializerBase::Serialize( GameObject* pObject )
//{
//    HELIUM_ASSERT( pObject );
//
//    // Perform derived class pre-serialization.
//    PreSerialize( pObject );
//
//    // Serialize...
//    m_propertyStreamBuffer.Resize( 0 );
//    m_propertyStream.Open( &m_propertyStreamBuffer );
//
//    m_propertyTag = Tag( NULL );
//
//    m_currentTag = Tag( NULL );
//    m_bClosePropertyWithTabs = false;
//
//    m_bIgnoreNextValue = false;
//
//    pObject->Serialize( *this );
//
//    ConditionalClosePropertyTag();
//
//    m_currentTag = Tag( NULL );
//    m_bClosePropertyWithTabs = false;
//
//    m_bIgnoreNextValue = false;
//
//    m_outputLevelStack.Resize( 0 );
//
//    // Perform derived class post-serialization.
//    PostSerialize( pObject );
//
//    m_propertyStream.Close();
//
//    return true;
//}
//
///// @copydoc Serializer::GetMode()
//Serializer::EMode XmlSerializerBase::GetMode() const
//{
//    return MODE_SAVE;
//}
//
///// @copydoc Serializer::SerializeTag()
//void XmlSerializerBase::SerializeTag( const Tag& rTag )
//{
//    ConditionalClosePropertyTag();
//
//    // Make sure we're only at the top-most level or within a struct.
//    if( !m_outputLevelStack.IsEmpty() && m_outputLevelStack.GetLast() != OUTPUT_LEVEL_STRUCT )
//    {
//        HELIUM_TRACE(
//            TraceLevels::Error,
//            TXT( "XmlSerializerBase::SerializeTag(): Called while within a property or array tag.\n" ) );
//
//        return;
//    }
//
//    m_currentTag = rTag;
//}
//
///// @copydoc Serializer::CanResolveTags()
//bool XmlSerializerBase::CanResolveTags() const
//{
//    return true;
//}
//
///// @copydoc Serializer::SerializeBool()
//void XmlSerializerBase::SerializeBool( bool& rValue )
//{
//    uint8_t outValue = rValue;
//    WriteValue( outValue, TXT( "bool" ), PrintfFormatter< uint8_t >( TXT( "%" ) TPRIu8 ) );
//}
//
///// @copydoc Serializer::SerializeInt8()
//void XmlSerializerBase::SerializeInt8( int8_t& rValue )
//{
//    WriteValue( rValue, TXT( "int8_t" ), PrintfFormatter< int8_t >( TXT( "%" ) TPRId8 ) );
//}
//
///// @copydoc Serializer::SerializeUint8()
//void XmlSerializerBase::SerializeUint8( uint8_t& rValue )
//{
//    WriteValue( rValue, TXT( "uint8_t" ), PrintfFormatter< uint8_t >( TXT( "%" ) TPRIu8 ) );
//}
//
///// @copydoc Serializer::SerializeInt16()
//void XmlSerializerBase::SerializeInt16( int16_t& rValue )
//{
//    WriteValue( rValue, TXT( "int16_t" ), PrintfFormatter< int16_t >( TXT( "%" ) TPRId16 ) );
//}
//
///// @copydoc Serializer::SerializeUint16()
//void XmlSerializerBase::SerializeUint16( uint16_t& rValue )
//{
//    WriteValue( rValue, TXT( "uint16_t" ), PrintfFormatter< uint16_t >( TXT( "%" ) TPRIu16 ) );
//}
//
///// @copydoc Serializer::SerializeInt32()
//void XmlSerializerBase::SerializeInt32( int32_t& rValue )
//{
//    WriteValue( rValue, TXT( "int32_t" ), PrintfFormatter< int32_t >( TXT( "%" ) TPRId32 ) );
//}
//
///// @copydoc Serializer::SerializeUint32()
//void XmlSerializerBase::SerializeUint32( uint32_t& rValue )
//{
//    WriteValue( rValue, TXT( "uint32_t" ), PrintfFormatter< uint32_t >( TXT( "%" ) TPRIu32 ) );
//}
//
///// @copydoc Serializer::SerializeInt64()
//void XmlSerializerBase::SerializeInt64( int64_t& rValue )
//{
//    WriteValue( rValue, TXT( "int64_t" ), PrintfFormatter< int64_t >( TXT( "%" ) TPRId64 ) );
//}
//
///// @copydoc Serializer::SerializeUint64()
//void XmlSerializerBase::SerializeUint64( uint64_t& rValue )
//{
//    WriteValue( rValue, TXT( "uint64_t" ), PrintfFormatter< uint64_t >( TXT( "%" ) TPRIu64 ) );
//}
//
///// @copydoc Serializer::SerializeFloat32()
//void XmlSerializerBase::SerializeFloat32( float32_t& rValue )
//{
//    WriteValue( rValue, TXT( "float32_t" ), PrintfFloatFormatter< float32_t >( TXT( "%.6a<!-- %.6f -->" ) ) );
//}
//
///// @copydoc Serializer::SerializeFloat64()
//void XmlSerializerBase::SerializeFloat64( float64_t& rValue )
//{
//    WriteValue( rValue, TXT( "float64_t" ), PrintfFloatFormatter< float64_t >( TXT( "%.13a<!-- %.13f -->" ) ) );
//}
//
///// @copydoc Serializer::SerializeBuffer()
//void XmlSerializerBase::SerializeBuffer( void* /*pBuffer*/, size_t /*elementSize*/, size_t /*count*/ )
//{
//    HELIUM_ASSERT_MSG_FALSE( TXT( "SerializeBuffer() is not supported when using an XML serializer." ) );
//}
//
///// @copydoc Serializer::SerializeEnum()
//void XmlSerializerBase::SerializeEnum( int32_t& rValue, uint32_t nameCount, const tchar_t* const* ppNames )
//{
//    WriteValue( rValue, TXT( "enum" ), EnumFormatter( nameCount, ppNames ) );
//}
//
///// @copydoc Serializer::SerializeEnum()
//void XmlSerializerBase::SerializeEnum( int32_t& rValue, const Helium::Reflect::Enumeration* pEnumeration )
//{
//    WriteValue( rValue, TXT( "Enumeration" ), ReflectEnumFormatter( pEnumeration ) );
//}
//
///// @copydoc Serializer::SerializeCharName()
//void XmlSerializerBase::SerializeCharName( CharName& rValue )
//{
//    WriteValue( rValue, TXT( "CharName" ), NameFormatter< CharNameTable >() );
//}
//
///// @copydoc Serializer::SerializeWideName()
//void XmlSerializerBase::SerializeWideName( WideName& rValue )
//{
//    WriteValue( rValue, TXT( "WideName" ), NameFormatter< WideNameTable >() );
//}
//
///// @copydoc Serializer::SerializeCharString()
//void XmlSerializerBase::SerializeCharString( CharString& rValue )
//{
//    WriteValue( rValue, TXT( "CharString" ), StringFormatter< char >() );
//}
//
///// @copydoc Serializer::SerializeWideString()
//void XmlSerializerBase::SerializeWideString( WideString& rValue )
//{
//    WriteValue( rValue, TXT( "WideString" ), StringFormatter< wchar_t >() );
//}
//
///// @copydoc Serializer::SerializeObjectReference()
//void XmlSerializerBase::SerializeObjectReference( const GameObjectType* /*pType*/, GameObjectPtr& rspObject )
//{
//    WriteValue( rspObject, TXT( "GameObject reference" ), ObjectFormatter() );
//}
//
///// @copydoc Serializer::BeginStruct()
//void XmlSerializerBase::BeginStruct( EStructTag /*tag*/ )
//{
//    // Opening property tags do not end with newline characters, so add one if we are just starting a property.
//    if( ConditionalOpenPropertyTag() )
//    {
//        m_propertyStream.Write( NEWLINE_STRING, 1, sizeof( NEWLINE_STRING ) - 1 );
//        m_bClosePropertyWithTabs = true;
//    }
//
//    // Make sure we are within a valid property or array tag.
//    if( m_outputLevelStack.IsEmpty() || m_outputLevelStack.GetLast() == OUTPUT_LEVEL_STRUCT )
//    {
//        HELIUM_TRACE(
//            TraceLevels::Error,
//            TXT( "XmlSerializerBase::BeginStruct(): Called while not within a valid property or array tag.\n" ) );
//
//        return;
//    }
//
//    // Push a struct output level.
//    m_outputLevelStack.Push( OUTPUT_LEVEL_STRUCT );
//
//    // Write out the opening struct tag.
//    WriteTabs( &m_propertyStream, m_outputLevelStack.GetSize() + 1 );
//    m_propertyStream.Write( STRUCT_TAG_START, 1, sizeof( STRUCT_TAG_START ) - 1 );
//}
//
///// @copydoc Serializer::EndStruct()
//void XmlSerializerBase::EndStruct()
//{
//    ConditionalClosePropertyTag();
//
//    // Make sure we are within a valid struct tag.
//    if( m_outputLevelStack.IsEmpty() || m_outputLevelStack.GetLast() != OUTPUT_LEVEL_STRUCT )
//    {
//        HELIUM_TRACE( TraceLevels::Error, TXT( "XmlSerializerBase::EndStruct(): Called while not within a valid struct tag.\n" ) );
//
//        return;
//    }
//
//    // Write out the closing struct tag.
//    WriteTabs( &m_propertyStream, m_outputLevelStack.GetSize() + 1 );
//    m_propertyStream.Write( STRUCT_TAG_END, 1, sizeof( STRUCT_TAG_END ) - 1 );
//
//    m_outputLevelStack.Pop();
//
//    m_bClosePropertyWithTabs = true;
//}
//
///// @copydoc Serializer::BeginArray()
//void XmlSerializerBase::BeginArray( uint32_t /*size*/ )
//{
//    // Opening property tags do not end with newline characters, so add one if we are just starting a property.
//    if( ConditionalOpenPropertyTag() )
//    {
//        m_propertyStream.Write( NEWLINE_STRING, 1, sizeof( NEWLINE_STRING ) - 1 );
//        m_bClosePropertyWithTabs = true;
//    }
//
//    // Make sure we are within a valid property or array tag.
//    if( m_outputLevelStack.IsEmpty() || m_outputLevelStack.GetLast() == OUTPUT_LEVEL_STRUCT )
//    {
//        HELIUM_TRACE(
//            TraceLevels::Error,
//            TXT( "XmlSerializerBase::BeginArray(): Called while not within a valid property or array tag.\n" ) );
//
//        return;
//    }
//
//    // Push an array output level.
//    m_outputLevelStack.Push( OUTPUT_LEVEL_ARRAY );
//
//    // Write out the opening array tag.
//    WriteTabs( &m_propertyStream, m_outputLevelStack.GetSize() + 1 );
//    m_propertyStream.Write( ARRAY_TAG_START, 1, sizeof( ARRAY_TAG_START ) - 1 );
//}
//
///// @copydoc Serializer::EndArray()
//void XmlSerializerBase::EndArray()
//{
//    // Arrays and dynamic arrays are handled the same when performing XML serialization.
//    EndDynArray();
//}
//
///// @copydoc Serializer::BeginDynArray()
//void XmlSerializerBase::BeginDynArray()
//{
//    // Opening property tags do not end with newline characters, so add one if we are just starting a property.
//    if( ConditionalOpenPropertyTag() )
//    {
//        m_propertyStream.Write( NEWLINE_STRING, 1, sizeof( NEWLINE_STRING ) - 1 );
//        m_bClosePropertyWithTabs = true;
//    }
//
//    // Make sure we are within a valid property or array tag.
//    if( m_outputLevelStack.IsEmpty() || m_outputLevelStack.GetLast() == OUTPUT_LEVEL_STRUCT )
//    {
//        HELIUM_TRACE(
//            TraceLevels::Error,
//            TXT( "XmlSerializerBase::BeginDynArray(): Called while not within a valid property or array tag.\n" ) );
//
//        return;
//    }
//
//    // Push an array output level.
//    m_outputLevelStack.Push( OUTPUT_LEVEL_ARRAY );
//
//    // Write out the opening array tag.
//    WriteTabs( &m_propertyStream, m_outputLevelStack.GetSize() + 1 );
//    m_propertyStream.Write( ARRAY_TAG_START, 1, sizeof( ARRAY_TAG_START ) - 1 );
//
//    // Skip the array size property to come down the stream next.
//    m_bIgnoreNextValue = true;
//}
//
///// @copydoc Serializer::EndDynArray()
//void XmlSerializerBase::EndDynArray()
//{
//    ConditionalClosePropertyTag();
//
//    // Make sure we are within a valid array tag.
//    if( m_outputLevelStack.IsEmpty() || m_outputLevelStack.GetLast() != OUTPUT_LEVEL_ARRAY )
//    {
//        HELIUM_TRACE(
//            TraceLevels::Error,
//            TXT( "XmlSerializerBase::EndArray/EndDynArray(): Called while not within a valid array tag.\n" ) );
//
//        return;
//    }
//
//    // Write out the closing array tag.
//    WriteTabs( &m_propertyStream, m_outputLevelStack.GetSize() + 1 );
//    m_propertyStream.Write( ARRAY_TAG_END, 1, sizeof( ARRAY_TAG_END ) - 1 );
//
//    m_outputLevelStack.Pop();
//
//    m_bClosePropertyWithTabs = true;
//    m_bIgnoreNextValue = false;
//}
//
///// @fn void XmlSerializerBase::PreSerialize( GameObject* pObject )
///// Perform any work prior to serializing a given object.
/////
///// @param[in] pObject  GameObject about to be serialized.
/////
///// @see PostSerialize()
//
///// @fn void XmlSerializerBase::PostSerialize( GameObject* pObject )
///// Perform any work after a given object has been serialized.
/////
///// @param[in] pObject  Serialized object.
/////
///// @see PreSerialize()
//
///// @fn void XmlSerializerBase::PushPropertyData( Name name, const void* pData, size_t size )
///// Perform any necessary processing on the XML data for a recently processed property.
/////
///// @param[in] name   Property name.
///// @param[in] pData  XML data stream for the property.
///// @param[in] size   Property data size, in bytes.
//
///// Write the given single-byte character string to the given stream as a UTF-8 string.
/////
///// @param[in] pStream         Stream to which the data should be written.
///// @param[in] pString         Null-terminated string to write.
///// @param[in] length          Precomputed number of characters in the string.  Note that this is merely an
/////                            optimization; the given string must still contain a null terminator at
/////                            pString[ length ].
///// @param[in] bCharacterData  True if the string should be written as character data, replacing various special
/////                            characters (i.e. angle brackets, ampersands) with the appropriate entity references.
//void XmlSerializerBase::WriteStringAsUtf8( Stream* pStream, const char* pString, size_t length, bool bCharacterData )
//{
//    HELIUM_UNREF( length );  // Not used with Visual C++.
//
//    HELIUM_ASSERT( pStream );
//    HELIUM_ASSERT( length == 0 || ( pString && pString[ length ] == '\0' ) );
//
//#ifdef _MSC_VER
//    size_t wideCharCount = static_cast< size_t >( -1 );
//    mbstowcs_s( &wideCharCount, NULL, 0, pString, _TRUNCATE );
//#else
//    size_t wideCharCount = mbstowcs( NULL, pString, length );
//#endif
//    HELIUM_ASSERT( wideCharCount != static_cast< size_t >( -1 ) );
//    if( wideCharCount != 0 && wideCharCount != static_cast< size_t >( -1 ) )
//    {
//        ThreadLocalStackAllocator stackAllocator;
//
//        wchar_t* pWideString = static_cast< wchar_t* >( stackAllocator.Allocate(
//            sizeof( wchar_t ) * ( wideCharCount + 1 ) ) );
//        HELIUM_ASSERT( pWideString );
//#ifdef _MSC_VER
//        mbstowcs_s( NULL, pWideString, wideCharCount + 1, pString, wideCharCount );
//#else
//        mbstowcs( pWideString, pString, length );
//#endif
//        pWideString[ wideCharCount ] = L'\0';
//
//        WriteStringAsUtf8( pStream, pWideString, wideCharCount, bCharacterData );
//
//        stackAllocator.Free( pWideString );
//    }
//}
//
///// Write the given wide-character string to the output stream as a UTF-8 string.
/////
///// @param[in] pStream         Stream to which the data should be written.
///// @param[in] pString         Null-terminated string to write.
///// @param[in] length          Precomputed number of characters in the string.  Note that this is merely an
/////                            optimization; the given string must still contain a null terminator at
/////                            pString[ length ].
///// @param[in] bCharacterData  True if the string should be written as character data, replacing various special
/////                            characters (i.e. angle brackets, ampersands) with the appropriate entity references.
//void XmlSerializerBase::WriteStringAsUtf8( Stream* pStream, const wchar_t* pString, size_t length, bool bCharacterData )
//{
//    HELIUM_ASSERT( pStream );
//    HELIUM_ASSERT( length == 0 || ( pString && pString[ length ] == L'\0' ) );
//
//    bool bHaveSurrogate = false;
//    uint32_t surrogateStart = L'\0';
//
//    for( size_t characterIndex = 0; characterIndex < length; ++characterIndex )
//    {
//        uint32_t character = static_cast< wchar_t >( pString[ characterIndex ] );
//
//        // Handle UTF-16 surrogate pairs.
//        if( bHaveSurrogate )
//        {
//            HELIUM_ASSERT( character >= 0xdc00 );
//            HELIUM_ASSERT( character < 0xe000 );
//
//            character = ( surrogateStart | ( character - 0xdc00 ) ) + 0x10000;
//
//            bHaveSurrogate = false;
//        }
//        else
//        {
//            bHaveSurrogate = ( character >= 0xd800 && character < 0xdc00 );
//            if( bHaveSurrogate )
//            {
//                surrogateStart = ( character - 0xd800 ) << 10;
//                continue;
//            }
//        }
//
//        // If writing out character data, replace special characters as appropriate.
//        if( bCharacterData )
//        {
//            if( character == static_cast< wchar_t >( L'"' ) )
//            {
//                const char utf8string[] = "&quot;";
//                pStream->Write( utf8string, 1, sizeof( utf8string ) - 1 );
//
//                continue;
//            }
//
//            if( character == static_cast< wchar_t >( L'&' ) )
//            {
//                const char utf8string[] = "&amp;";
//                pStream->Write( utf8string, 1, sizeof( utf8string ) - 1 );
//
//                continue;
//            }
//
//            if( character == static_cast< wchar_t >( L'\'' ) )
//            {
//                const char utf8string[] = "&apos;";
//                pStream->Write( utf8string, 1, sizeof( utf8string ) - 1 );
//
//                continue;
//            }
//
//            if( character == static_cast< wchar_t >( L'<' ) )
//            {
//                const char utf8string[] = "&lt;";
//                pStream->Write( utf8string, 1, sizeof( utf8string ) - 1 );
//
//                continue;
//            }
//
//            if( character == static_cast< wchar_t >( L'>' ) )
//            {
//                const char utf8string[] = "&gt;";
//                pStream->Write( utf8string, 1, sizeof( utf8string ) - 1 );
//
//                continue;
//            }
//        }
//
//        // Convert the current code point to UTF-8 and write.
//        if( character < 0x80 )
//        {
//            uint8_t utf8chars[] = { static_cast< uint8_t >( character ) };
//            pStream->Write( utf8chars, 1, sizeof( utf8chars ) );
//        }
//        else if( character < 0x800 )
//        {
//            uint8_t utf8chars[] =
//            {
//                0xc0 | static_cast< uint8_t >( character >> 6 ),
//                0x80 | static_cast< uint8_t >( character & 0x3f )
//            };
//
//            pStream->Write( utf8chars, 1, sizeof( utf8chars ) );
//        }
//        else if( character < 0x10000 )
//        {
//            uint8_t utf8chars[] =
//            {
//                0xe0 | static_cast< uint8_t >( character >> 12 ),
//                0x80 | static_cast< uint8_t >( ( character >> 6 ) & 0x3f ),
//                0x80 | static_cast< uint8_t >( ( character & 0x3f ) )
//            };
//
//            pStream->Write( utf8chars, 1, sizeof( utf8chars ) );
//        }
//        else
//        {
//            HELIUM_ASSERT( character >= 0x10000 );
//            HELIUM_ASSERT( character < 0x110000 );
//
//            uint8_t utf8chars[] =
//            {
//                0xf0 | static_cast< uint8_t >( character >> 18 ),
//                0x80 | static_cast< uint8_t >( ( character >> 12 ) & 0x3f ),
//                0x80 | static_cast< uint8_t >( ( character >> 6 ) & 0x3f ),
//                0x80 | static_cast< uint8_t >( character & 0x3f )
//            };
//
//            pStream->Write( utf8chars, 1, sizeof( utf8chars ) );
//        }
//    }
//}
//
///// Begin a property tag if one is pending.
/////
///// @return  True if a property tag was opened.
/////
///// @see ConditionalClosePropertyTag()
//bool XmlSerializerBase::ConditionalOpenPropertyTag()
//{
//    // Validate the current tag if we are not serializing an array value.
//    const tchar_t* pPropertyNameString = NULL;
//
//    bool bInArray = ( !m_outputLevelStack.IsEmpty() && m_outputLevelStack.GetLast() == OUTPUT_LEVEL_ARRAY );
//    if( !bInArray )
//    {
//        pPropertyNameString = m_currentTag.Get();
//        if( !pPropertyNameString )
//        {
//            return false;
//        }
//
//        // If the tag for the current top-level property is not set, set it to the property being serialized.
//        if( !m_propertyTag.Get() )
//        {
//            HELIUM_ASSERT( m_outputLevelStack.IsEmpty() );
//            m_propertyTag = m_currentTag;
//        }
//    }
//
//    m_outputLevelStack.Push( OUTPUT_LEVEL_PROPERTY );
//
//    WriteTabs( &m_propertyStream, m_outputLevelStack.GetSize() + 1 );
//    m_propertyStream.Write( PROPERTY_TAG_START_A, 1, sizeof( PROPERTY_TAG_START_A ) - 1 );
//    if( !bInArray )
//    {
//        m_propertyStream.Write( PROPERTY_TAG_START_B, 1, sizeof( PROPERTY_TAG_START_B ) - 1 );
//        WriteStringAsUtf8( &m_propertyStream, pPropertyNameString, StringLength( pPropertyNameString ), true );
//        m_propertyStream.Write( PROPERTY_TAG_START_C, 1, sizeof( PROPERTY_TAG_START_C ) - 1 );
//
//        m_currentTag = Tag( NULL );
//    }
//
//    m_propertyStream.Write( PROPERTY_TAG_START_D, 1, sizeof( PROPERTY_TAG_START_D ) - 1 );
//
//    m_bClosePropertyWithTabs = false;
//
//    return true;
//}
//
///// Close the top-most property tag if one is set.
/////
///// @see ConditionalOpenPropertyTag()
//void XmlSerializerBase::ConditionalClosePropertyTag()
//{
//    if( !m_outputLevelStack.IsEmpty() && m_outputLevelStack.GetLast() == OUTPUT_LEVEL_PROPERTY )
//    {
//        if( m_bClosePropertyWithTabs )
//        {
//            WriteTabs( &m_propertyStream, m_outputLevelStack.GetSize() + 1 );
//            m_bClosePropertyWithTabs = false;
//        }
//
//        m_propertyStream.Write( PROPERTY_TAG_END, 1, sizeof( PROPERTY_TAG_END ) - 1 );
//
//        m_outputLevelStack.Pop();
//
//        // If we just finished a top-level property, push it on for processing in derived classes.
//        if( m_outputLevelStack.IsEmpty() )
//        {
//            HELIUM_ASSERT( m_propertyTag.Get() );
//            Name propertyName( m_propertyTag.Get() );
//
//            m_propertyStream.Close();
//            const void* pPropertyData = m_propertyStreamBuffer.GetData();
//            size_t propertySize = m_propertyStreamBuffer.GetSize();
//            HELIUM_ASSERT( pPropertyData || propertySize == 0 );
//
//            PushPropertyData( propertyName, pPropertyData, propertySize );
//
//            m_propertyStreamBuffer.Resize( 0 );
//            m_propertyStream.Open( &m_propertyStreamBuffer );
//
//            m_propertyTag = Tag( NULL );
//        }
//    }
//}
//
///// Serialize the given value.
/////
///// @param[in] rValue       Value to serialize.
///// @param[in] pTypeString  String used to specify the type being serialized (for logging).
///// @param[in] rFormatter   Value formatter functor.
//template< typename T, typename Formatter >
//void XmlSerializerBase::WriteValue( const T& rValue, const tchar_t* pTypeString, const Formatter& rFormatter )
//{
//    HELIUM_UNREF( pTypeString );  // Logging only.
//
//    HELIUM_ASSERT( pTypeString );
//
//    // Ignore this value if necessary (used to ignore the size parameter passed when serializing a dynamic array).
//    if( m_bIgnoreNextValue )
//    {
//        m_bIgnoreNextValue = false;
//        return;
//    }
//
//    // Make sure the current property tag is active if we're not in an array.
//    if( !m_currentTag.Get() &&
//        ( m_outputLevelStack.IsEmpty() || m_outputLevelStack.GetLast() != OUTPUT_LEVEL_ARRAY ) )
//    {
//        HELIUM_TRACE(
//            TraceLevels::Error,
//            TXT( "XmlSerializerBase: Attempted to serialize a %s without an active property tag.\n" ),
//            pTypeString );
//
//        return;
//    }
//
//    // Serialize the property.
//    HELIUM_VERIFY( ConditionalOpenPropertyTag() );
//    rFormatter( *this, rValue );
//    ConditionalClosePropertyTag();
//
//    // Clear the tag so we know not to accidentally use it for consecutive properties.
//    m_currentTag = Tag( NULL );
//}
//
///// Write out the specified number of tabs for indenting output.
/////
///// @param[in] pStream  Stream to which the data should be written.
///// @param[in] count    Number of tabs to write.
//void XmlSerializerBase::WriteTabs( Stream* pStream, size_t count )
//{
//    HELIUM_ASSERT( pStream );
//
//    for( size_t index = 0; index < count; ++index )
//    {
//        pStream->Write( TAB_STRING, 1, sizeof( TAB_STRING ) - 1 );
//    }
//}
//
///// Constructor.
/////
///// @param[in] pFormat  "printf"-style format string for writing out the value.
//template< typename T >
//XmlSerializerBase::PrintfFormatter< T >::PrintfFormatter( const tchar_t* pFormat )
//: m_pFormat( pFormat )
//{
//    HELIUM_ASSERT( pFormat );
//}
//
///// Write a value to the output stream.
/////
///// @param[in] rSerializer  Reference to the XmlSerializerBase in use.
///// @param[in] rValue       Value to write.
//template< typename T >
//void XmlSerializerBase::PrintfFormatter< T >::operator()( XmlSerializerBase& rSerializer, const T& rValue ) const
//{
//    String valueString;
//    valueString.Format( m_pFormat, rValue );
//    rSerializer.WriteStringAsUtf8(
//        &rSerializer.m_propertyStream,
//        valueString.GetData(),
//        valueString.GetSize(),
//        true );
//}
//
///// Constructor.
/////
///// @param[in] pFormat  "printf"-style format string for writing out the value.  The value is expected to be written
/////                     out twice: once as a hexadecimal (base-16) string, and again as a commented-out decimal
/////                     (base-10) string.
//template< typename T >
//XmlSerializerBase::PrintfFloatFormatter< T >::PrintfFloatFormatter( const tchar_t* pFormat )
//: m_pFormat( pFormat )
//{
//    HELIUM_ASSERT( pFormat );
//}
//
///// Write a value to the output stream.
/////
///// @param[in] rSerializer  Reference to the XmlSerializerBase in use.
///// @param[in] rValue       Value to write.
//template< typename T >
//void XmlSerializerBase::PrintfFloatFormatter< T >::operator()(
//    XmlSerializerBase& rSerializer,
//    const T& rValue ) const
//{
//    String valueString;
//    valueString.Format( m_pFormat, rValue, rValue );
//
//    // Data should be considered pre-formatted (need to allow for comment tags).
//    rSerializer.WriteStringAsUtf8(
//        &rSerializer.m_propertyStream,
//        valueString.GetData(),
//        valueString.GetSize(),
//        false );
//}
//
///// Constructor.
/////
///// @param[in] nameCount  Number of enumeration value names.
///// @param[in] ppNames    Array of enumeration name strings.
//XmlSerializerBase::EnumFormatter::EnumFormatter( uint32_t nameCount, const tchar_t* const* ppNames )
//: m_nameCount( nameCount )
//, m_ppNames( ppNames )
//{
//    HELIUM_ASSERT( nameCount != 0 );
//    HELIUM_ASSERT( ppNames );
//}
//
///// Write a value to the output stream.
/////
///// @param[in] rSerializer  Reference to the XmlSerializerBase in use.
///// @param[in] value        Value to write.
//void XmlSerializerBase::EnumFormatter::operator()( XmlSerializerBase& rSerializer, int32_t value ) const
//{
//    String valueString;
//    if( static_cast< uint32_t >( value ) < m_nameCount )
//    {
//        valueString = m_ppNames[ value ];
//    }
//
//    rSerializer.WriteStringAsUtf8(
//        &rSerializer.m_propertyStream,
//        valueString.GetData(),
//        valueString.GetSize(),
//        true );
//}
//
///// Constructor.
/////
///// @param[in] nameCount  Number of enumeration value names.
///// @param[in] ppNames    Array of enumeration name strings.
//XmlSerializerBase::ReflectEnumFormatter::ReflectEnumFormatter( const Helium::Reflect::Enumeration* pEnumeration )
//: m_enumeration( pEnumeration )
//{
//    HELIUM_ASSERT( m_enumeration );
//}
//
///// Write a value to the output stream.
/////
///// @param[in] rSerializer  Reference to the XmlSerializerBase in use.
///// @param[in] value        Value to write.
//void XmlSerializerBase::ReflectEnumFormatter::operator()( XmlSerializerBase& rSerializer, int32_t value ) const
//{
//    tstring string;
//    m_enumeration->GetElementName( value, string );
//
//    String valueString ( string.c_str() );
//
//    rSerializer.WriteStringAsUtf8(
//        &rSerializer.m_propertyStream,
//        valueString.GetData(),
//        valueString.GetSize(),
//        true );
//}
//
///// Write a value to the output stream.
/////
///// @param[in] rSerializer  Reference to the XmlSerializerBase in use.
///// @param[in] rValue       Value to write.
//template< typename TableType >
//void XmlSerializerBase::NameFormatter< TableType >::operator()(
//    XmlSerializerBase& rSerializer,
//    const NameBase< TableType >& rValue ) const
//{
//    const typename TableType::CharType* pName = rValue.Get();
//    HELIUM_ASSERT( pName );
//    rSerializer.WriteStringAsUtf8( &rSerializer.m_propertyStream, pName, StringLength( pName ), true );
//}
//
///// Write a value to the output stream.
/////
///// @param[in] rSerializer  Reference to the XmlSerializerBase in use.
///// @param[in] rValue       Value to write.
//template< typename CharType >
//void XmlSerializerBase::StringFormatter< CharType >::operator()(
//    XmlSerializerBase& rSerializer,
//    const StringBase< CharType, DefaultAllocator >& rValue ) const
//{
//    rSerializer.WriteStringAsUtf8( &rSerializer.m_propertyStream, rValue.GetData(), rValue.GetSize(), true );
//}
//
///// Write a value to the output stream.
/////
///// @param[in] rSerializer  Reference to the XmlSerializerBase in use.
///// @param[in] pValue       Value to write.
//void XmlSerializerBase::ObjectFormatter::operator()( XmlSerializerBase& rSerializer, GameObject* pValue ) const
//{
//    if( pValue )
//    {
//        String pathName;
//        pValue->GetPath().ToString( pathName );
//        rSerializer.WriteStringAsUtf8(
//            &rSerializer.m_propertyStream,
//            pathName.GetData(),
//            pathName.GetSize(),
//            true );
//    }
//}
