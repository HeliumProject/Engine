////----------------------------------------------------------------------------------------------------------------------
//// XmlSerializer.cpp
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
#include "PcSupportPch.h"
//#include "PcSupport/XmlSerializer.h"
//
//#include "Engine/FileLocations.h"
//#include "Foundation/FilePath.h"
//#include "Foundation/FileStream.h"
//#include "Foundation/BufferedStream.h"
//
//using namespace Helium;
//
//// XML file header (UTF-8 string).
//static const char XML_FILE_HEADER[] =
//"<?xml version=\"1.0\" encoding=\"utf-8\"?>" HELIUM_XML_NEWLINE "<package>" HELIUM_XML_NEWLINE;
//// XML file footer (UTF-8 string).
//static const char XML_FILE_FOOTER[] = "</package>" HELIUM_XML_NEWLINE;
//
//// GameObject tag strings.
//static const char OBJECT_TAG_START_A[] = "    <object name=\"";
//static const char OBJECT_TAG_START_B[] = "\" type=\"";
//static const char OBJECT_TAG_START_C[] = "\" template=\"";
//static const char OBJECT_TAG_START_D[] = "\">" HELIUM_XML_NEWLINE;
//static const char OBJECT_TAG_END[] = "    </object>" HELIUM_XML_NEWLINE;
//
///// Constructor.
//XmlSerializer::XmlSerializer()
//: m_pFileStream( NULL )
//, m_pStream( NULL )
//{
//}
//
///// Destructor.
//XmlSerializer::~XmlSerializer()
//{
//    Shutdown();
//}
//
///// Initialize this serializer.
/////
///// @param[in] pFileName  Name of the file to which the serialized data should be written.
/////
///// @return  True if initialization was successful, false if not.
/////
///// @see Shutdown()
//bool XmlSerializer::Initialize( const tchar_t* pFileName )
//{
//    HELIUM_ASSERT( pFileName );
//
//    Shutdown();
//
//    // Make sure the output directory exists.
//    Path outputFile( pFileName );
//    outputFile.MakePath();
//
//    // Attempt to open the file for writing.
//    m_pFileStream = FileStream::OpenFileStream( pFileName, FileStream::MODE_WRITE, true );
//    if( !m_pFileStream )
//    {
//        HELIUM_TRACE( TRACE_ERROR, TXT( "XmlSerializer::Initialize(): Failed to open \"%s\" for writing.\n" ), pFileName );
//
//        return false;
//    }
//
//    // Wrap the file stream with a buffered stream.
//    m_pStream = new BufferedStream( m_pFileStream );
//    HELIUM_ASSERT( m_pStream );
//
//    // Write the XML header.
//    m_pStream->Write( XML_FILE_HEADER, 1, sizeof( XML_FILE_HEADER ) - 1 );
//
//    return true;
//}
//
///// Shut down this serializer.
/////
///// @see Initialize()
//void XmlSerializer::Shutdown()
//{
//    // If we've been initialized, write the XML footer and close the buffered stream.
//    if( m_pStream )
//    {
//        m_pStream->Write( XML_FILE_FOOTER, 1, sizeof( XML_FILE_FOOTER ) - 1 );
//        delete m_pStream;
//        m_pStream = NULL;
//    }
//
//    // Close the file stream last (buffered stream needs to be closed first so that it gets flushed).
//    delete m_pFileStream;
//    m_pFileStream = NULL;
//
//    // Release allocated memory.
//    m_templateSerializer.ClearProperties();
//}
//
///// @copydoc XmlSerializerBase::PreSerialize()
//void XmlSerializer::PreSerialize( GameObject* pObject )
//{
//    HELIUM_ASSERT( pObject );
//
//    // Serialize the object's template.
//    GameObject* pTemplate = Reflect::AssertCast< GameObject >( pObject->GetTemplate() );
//    HELIUM_ASSERT( pTemplate );
//
//    m_templateSerializer.Serialize( pTemplate );
//
//    // Write the opening tag for the current object.
//    String objectPath;
//
//    HELIUM_ASSERT( m_pStream );
//    m_pStream->Write( OBJECT_TAG_START_A, 1, sizeof( OBJECT_TAG_START_A ) - 1 );
//
//    Name objectName = pObject->GetName();
//    const tchar_t* pNameString = objectName.Get();
//    HELIUM_ASSERT( pNameString );
//    WriteStringAsUtf8( m_pStream, pNameString, StringLength( pNameString ), true );
//
//    m_pStream->Write( OBJECT_TAG_START_B, 1, sizeof( OBJECT_TAG_START_B ) - 1 );
//
//    const GameObjectType* pType = pObject->GetGameObjectType();
//    HELIUM_ASSERT( pType );
//    pNameString = pType->GetName().Get();
//    HELIUM_ASSERT( pNameString );
//    WriteStringAsUtf8( m_pStream, pNameString, StringLength( pNameString ), true );
//
//    m_pStream->Write( OBJECT_TAG_START_C, 1, sizeof( OBJECT_TAG_START_C ) - 1 );
//
//    if( pTemplate != pType->GetTemplate() )
//    {
//        pTemplate->GetPath().ToString( objectPath );
//        WriteStringAsUtf8( m_pStream, objectPath.GetData(), objectPath.GetSize(), true );
//    }
//
//    m_pStream->Write( OBJECT_TAG_START_D, 1, sizeof( OBJECT_TAG_START_D ) - 1 );
//}
//
///// @copydoc XmlSerializerBase::PostSerialize()
//void XmlSerializer::PostSerialize( GameObject* /*pObject*/ )
//{
//    // Write the closing tag.
//    m_pStream->Write( OBJECT_TAG_END, 1, sizeof( OBJECT_TAG_END ) - 1 );
//}
//
///// @copydoc XmlSerializerBase::PushPropertyData()
//void XmlSerializer::PushPropertyData( Name name, const void* pData, size_t size )
//{
//    HELIUM_ASSERT( pData || size == 0 );
//
//    // Compare the given property data with that of the same property in the template object if one exists.  If the
//    // data matches, we don't need to write out this property.
//    const XmlTemplateSerializer::PropertyData* pTemplateProperty = m_templateSerializer.FindProperty( name );
//    if( pTemplateProperty )
//    {
//        size_t templatePropertySize = pTemplateProperty->GetSize();
//        if( size == templatePropertySize )
//        {
//            const void* pTemplatePropertyData = pTemplateProperty->GetData();
//            HELIUM_ASSERT( pTemplatePropertyData );
//
//            if( MemoryCompare( pData, pTemplatePropertyData, size ) == 0 )
//            {
//                return;
//            }
//        }
//    }
//
//    // Property data differs or does not exist in the template, so write it out to the file stream.
//    m_pStream->Write( pData, 1, size );
//}
