////----------------------------------------------------------------------------------------------------------------------
//// PcCacheObjectLoader.cpp
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
#include "PcSupportPch.h"
//#include "PcSupport/PcCacheObjectLoader.h"
//
//#include "Engine/Config.h"
//#include "PcSupport/XmlPackageLoader.h"
//
//using namespace Helium;
//
///// Constructor.
//PcCacheObjectLoader::PcCacheObjectLoader()
//{
//}
//
///// Destructor.
//PcCacheObjectLoader::~PcCacheObjectLoader()
//{
//}
//
///// Initialize the static object loader instance as a PcCacheObjectLoader.
/////
///// @return  True if the loader was initialized successfully, false if not or another object loader instance already
/////          exists.
//bool PcCacheObjectLoader::InitializeStaticInstance()
//{
//    if( sm_pInstance )
//    {
//        return false;
//    }
//
//    sm_pInstance = new PcCacheObjectLoader;
//    HELIUM_ASSERT( sm_pInstance );
//
//    return true;
//}
//
///// @copydoc GameObjectLoader::GetPackageLoader()
//PackageLoader* PcCacheObjectLoader::GetPackageLoader( GameObjectPath path )
//{
//    // Route load requests for config objects to the XML package loader.
//    Config& rConfig = Config::GetStaticInstance();
//    GameObjectPath configPath = rConfig.GetConfigContainerPackagePath();
//
//    for( GameObjectPath testPath = path; !testPath.IsEmpty(); testPath = testPath.GetParent() )
//    {
//        if( testPath == configPath )
//        {
//            XmlPackageLoader* pLoader = m_packageLoaderMap.GetPackageLoader( path );
//
//            return pLoader;
//        }
//    }
//
//    // Default to the cache package loaders.
//    PackageLoader* pLoader = CacheObjectLoader::GetPackageLoader( path );
//
//    return pLoader;
//}
//
///// @copydoc GameObjectLoader::TickPackageLoaders()
//void PcCacheObjectLoader::TickPackageLoaders()
//{
//    m_packageLoaderMap.TickPackageLoaders();
//
//    CacheObjectLoader::TickPackageLoaders();
//}
