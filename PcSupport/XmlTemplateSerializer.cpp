////----------------------------------------------------------------------------------------------------------------------
//// XmlTemplateSerializer.cpp
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
#include "PcSupportPch.h"
//#include "PcSupport/XmlTemplateSerializer.h"
//
//using namespace Helium;
//
///// Constructor.
//XmlTemplateSerializer::XmlTemplateSerializer()
//{
//}
//
///// Destructor.
//XmlTemplateSerializer::~XmlTemplateSerializer()
//{
//}
//
///// Search for a property with the given name.
/////
///// @param[in] propertyName  Property name.
/////
///// @return  Pointer to the property data if found, null pointer if not found.
//const XmlTemplateSerializer::PropertyData* XmlTemplateSerializer::FindProperty( Name propertyName ) const
//{
//    size_t propertyCount = m_properties.GetSize();
//    for( size_t propertyIndex = 0; propertyIndex < propertyCount; ++propertyIndex )
//    {
//        const PropertyData& rPropertyData = m_properties[ propertyIndex ];
//        if( rPropertyData.GetName() == propertyName )
//        {
//            return &rPropertyData;
//        }
//    }
//
//    return NULL;
//}
//
///// Clear out all the property data.
//void XmlTemplateSerializer::ClearProperties()
//{
//    m_properties.Clear();
//}
//
///// @copydoc XmlSerializerBase::PreSerialize()
//void XmlTemplateSerializer::PreSerialize( GameObject* /*pObject*/ )
//{
//    m_properties.Remove( 0, m_properties.GetSize() );
//}
//
///// @copydoc XmlSerializerBase::PostSerialize()
//void XmlTemplateSerializer::PostSerialize( GameObject* /*pObject*/ )
//{
//}
//
///// @copydoc XmlSerializerBase::PushPropertyData()
//void XmlTemplateSerializer::PushPropertyData( Name name, const void* pData, size_t size )
//{
//    HELIUM_ASSERT( pData || size == 0 );
//
//    // Search for an existing property with the given name.
//    size_t propertyCount = m_properties.GetSize();
//    for( size_t propertyIndex = 0; propertyIndex < propertyCount; ++propertyIndex )
//    {
//        if( m_properties[ propertyIndex ].GetName() == name )
//        {
//            HELIUM_TRACE(
//                TRACE_WARNING,
//                ( TXT( "XmlTemplateSerializer: Duplicate property \"%s\" serialized.  The existing property data " )
//                TXT( "will be overwritten.\n" ) ),
//                *name );
//
//            m_properties.RemoveSwap( propertyIndex );
//
//            break;
//        }
//    }
//
//    // Store the property data.
//    HELIUM_VERIFY( m_properties.New( name, pData, size ) );
//}
//
///// Constructor.
/////
///// @param[in] name         Property name.
///// @param[in] pSourceData  Property data.  A copy of this, managed by this object, will be made.
///// @param[in] size         Property data size in bytes.
//XmlTemplateSerializer::PropertyData::PropertyData( Name name, const void* pSourceData, size_t size )
//: m_name( name )
//, m_pData( NULL )
//, m_size( size )
//{
//    if( size != 0 )
//    {
//        HELIUM_ASSERT( pSourceData );
//
//        m_pData = DefaultAllocator().Allocate( size );
//        HELIUM_ASSERT( m_pData );
//        MemoryCopy( m_pData, pSourceData, size );
//    }
//}
//
///// Copy constructor.
/////
///// @param[in] rSource  Source object from which to copy.
//XmlTemplateSerializer::PropertyData::PropertyData( const PropertyData& rSource )
//: m_name( rSource.m_name )
//, m_pData( NULL )
//, m_size( rSource.m_size )
//{
//    if( m_size != 0 )
//    {
//        HELIUM_ASSERT( rSource.m_pData );
//
//        m_pData = DefaultAllocator().Allocate( m_size );
//        HELIUM_ASSERT( m_pData );
//        MemoryCopy( m_pData, rSource.m_pData, m_size );
//    }
//}
//
///// Destructor.
//XmlTemplateSerializer::PropertyData::~PropertyData()
//{
//    DefaultAllocator().Free( m_pData );
//}
//
///// Assignment operator.
/////
///// @param[in] rSource  Source object from which to copy.
/////
///// @return  Reference to this object.
//XmlTemplateSerializer::PropertyData& XmlTemplateSerializer::PropertyData::operator=( const PropertyData& rSource )
//{
//    if( this != &rSource )
//    {
//        DefaultAllocator allocator;
//
//        allocator.Free( m_pData );
//        m_pData = NULL;
//
//        m_name = rSource.m_name;
//        m_size = rSource.m_size;
//        if( m_size != 0 )
//        {
//            HELIUM_ASSERT( rSource.m_pData );
//
//            m_pData = allocator.Allocate( m_size );
//            HELIUM_ASSERT( m_pData );
//            MemoryCopy( m_pData, rSource.m_pData, m_size );
//        }
//    }
//
//    return *this;
//}
