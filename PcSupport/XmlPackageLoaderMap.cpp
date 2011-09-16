////----------------------------------------------------------------------------------------------------------------------
//// XmlPackageLoaderMap.cpp
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
#include "PcSupportPch.h"
//#include "PcSupport/XmlPackageLoaderMap.h"
//
//#include "PcSupport/XmlPackageLoader.h"
//
//namespace Helium
//{
//    /// Constructor.
//    XmlPackageLoaderMap::XmlPackageLoaderMap()
//    {
//    }
//
//    /// Destructor.
//    XmlPackageLoaderMap::~XmlPackageLoaderMap()
//    {
//        ConcurrentHashMap< GameObjectPath, XmlPackageLoader* >::ConstAccessor loaderAccessor;
//        if( m_packageLoaderMap.First( loaderAccessor ) )
//        {
//            do
//            {
//                XmlPackageLoader* pLoader = loaderAccessor->Second();
//                HELIUM_ASSERT( pLoader );
//
//                delete pLoader;
//
//                ++loaderAccessor;
//            } while( loaderAccessor.IsValid() );
//        }
//    }
//
//    /// Get the package loader to use to load the object with the given path, creating it if necessary.
//    ///
//    /// @param[in] path  GameObject path.
//    ///
//    /// @return  Package loader to use to load the specified object.
//    XmlPackageLoader* XmlPackageLoaderMap::GetPackageLoader( GameObjectPath path )
//    {
//        HELIUM_ASSERT( !path.IsEmpty() );
//
//        // Resolve the object's package.
//        GameObjectPath packagePath = path;
//        while( !packagePath.IsPackage() )
//        {
//            packagePath = packagePath.GetParent();
//            if( packagePath.IsEmpty() )
//            {
//                HELIUM_TRACE(
//                    TRACE_ERROR,
//                    ( TXT( "XmlPackageLoaderMap::GetPackageLoader(): Cannot resolve package loader for \"%s\", as it " )
//                      TXT( "is not located in a package.\n" ) ),
//                    *path.ToString() );
//
//                return NULL;
//            }
//        }
//
//        // Locate an existing package loader.
//        ConcurrentHashMap< GameObjectPath, XmlPackageLoader* >::ConstAccessor constMapAccessor;
//        if( m_packageLoaderMap.Find( constMapAccessor, packagePath ) )
//        {
//            XmlPackageLoader* pLoader = constMapAccessor->Second();
//            HELIUM_ASSERT( pLoader );
//
//            return pLoader;
//        }
//
//        // Add a new package loader entry.
//        ConcurrentHashMap< GameObjectPath, XmlPackageLoader* >::Accessor mapAccessor;
//        bool bInserted = m_packageLoaderMap.Insert(
//            mapAccessor,
//            KeyValue< GameObjectPath, XmlPackageLoader* >( packagePath, NULL ) );
//        if( bInserted )
//        {
//            // Entry added, so create and initialize the package loader.
//            XmlPackageLoader* pLoader = new XmlPackageLoader;
//            HELIUM_ASSERT( pLoader );
//
//            bool bInitResult = pLoader->Initialize( packagePath );
//            HELIUM_ASSERT( bInitResult );
//            if( !bInitResult )
//            {
//                HELIUM_TRACE(
//                    TRACE_ERROR,
//                    TXT( "XmlPackageLoaderMap::GetPackageLoader(): Failed to initialize package loader for \"%s\".\n" ),
//                    *packagePath.ToString() );
//
//                m_packageLoaderMap.Remove( mapAccessor );
//
//                return NULL;
//            }
//
//            HELIUM_VERIFY( pLoader->BeginPreload() );
//
//            mapAccessor->Second() = pLoader;
//        }
//
//        XmlPackageLoader* pLoader = mapAccessor->Second();
//        HELIUM_ASSERT( pLoader );
//
//        return pLoader;
//    }
//
//    /// Tick all package loaders for a given GameObjectLoader tick.
//    void XmlPackageLoaderMap::TickPackageLoaders()
//    {
//        // Build the list of package loaders to update this tick from the loader map (the Tick() for a given package
//        // loader could require modification to the package loader map, which would cause a deadlock if we have the same
//        // part of the hash map locked as which needs to be updated).
//        HELIUM_ASSERT( m_packageLoaderTickArray.IsEmpty() );
//
//        ConcurrentHashMap< GameObjectPath, XmlPackageLoader* >::ConstAccessor loaderAccessor;
//        if( m_packageLoaderMap.First( loaderAccessor ) )
//        {
//            do
//            {
//                XmlPackageLoader* pLoader = loaderAccessor->Second();
//                HELIUM_ASSERT( pLoader );
//                m_packageLoaderTickArray.Push( pLoader );
//
//                ++loaderAccessor;
//            } while( loaderAccessor.IsValid() );
//        }
//
//        // Tick each loader.
//        size_t loaderCount = m_packageLoaderTickArray.GetSize();
//        for( size_t loaderIndex = 0; loaderIndex < loaderCount; ++loaderIndex )
//        {
//            XmlPackageLoader* pLoader = m_packageLoaderTickArray[ loaderIndex ];
//            HELIUM_ASSERT( pLoader );
//            pLoader->Tick();
//        }
//
//        // Clear out the tick array.
//        m_packageLoaderTickArray.Resize( 0 );
//    }
//}
