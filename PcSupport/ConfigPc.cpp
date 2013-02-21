//----------------------------------------------------------------------------------------------------------------------
// ConfigPc.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "PcSupportPch.h"
#include "PcSupport/ConfigPc.h"

#include "Engine/FileLocations.h"
#include "Foundation/FilePath.h"
#include "Engine/Config.h"
#include "PcSupport/XmlPackageLoader.h"
#include "PcSupport/XmlSerializer.h"

using namespace Helium;

/// Save the user configuration settings.
///
/// @return  True if the configuration was saved successfully, false if not.
bool ConfigPc::SaveUserConfig()
{
    HELIUM_TRACE( TraceLevels::Info, TXT( "ConfigPc: Saving user configuration.\n" ) );

    Config& rConfig = Config::GetStaticInstance();

    Package* pConfigPackage = rConfig.GetUserConfigPackage();
    if( !pConfigPackage )
    {
        HELIUM_TRACE( TraceLevels::Warning, TXT( "ConfigPc: No user configuration exists to save.\n" ) );

        return false;
    }

    FilePath userDataDirectory;
    if ( !FileLocations::GetUserDataDirectory( userDataDirectory ) )
    {
        HELIUM_TRACE( TraceLevels::Warning, TXT( "ConfigPc: No user data directory could be determined.\n" ) );
        return false;
    }

    AssetPath configPackagePath = pConfigPackage->GetPath();

    //FilePath packageFilePath( userDataDirectory + configPackagePath.ToFilePathString().GetData() + HELIUM_XML_PACKAGE_FILE_EXTENSION );

    //HELIUM_TRACE( TraceLevels::Info, TXT( "ConfigPc: Saving configuration to \"%s\".\n" ), *packageFilePath );

    //PMDTODO: Fix this
    //XmlSerializer serializer;
    //if( !serializer.Initialize( packageFilePath.c_str() ) )
    //{
    //    HELIUM_TRACE(
    //        TraceLevels::Error,
    //        TXT( "ConfigPc: Failed to initialize package serializer for writing to \"%s\".\n" ),
    //        *packageFilePath );

    //    return false;
    //}

    //for( Asset* pConfigObject = pConfigPackage->GetFirstChild();
    //     pConfigObject != NULL;
    //     pConfigObject = pConfigObject->GetNextSibling() )
    //{
    //    if( !pConfigObject->IsPackage() )
    //    {
    //        RecursiveSerializeObject( serializer, pConfigObject );
    //    }
    //}

    //serializer.Shutdown();

    HELIUM_TRACE( TraceLevels::Info, TXT( "ConfigPc: User configuration saved.\n" ) );

    return true;
}

/// Serialize the given object, followed by all its children.
///
/// @param[in] rSerializer  Serializer to use for serialization.
/// @param[in] pObject      Asset to serialize.
//void ConfigPc::RecursiveSerializeObject( XmlSerializer& rSerializer, Asset* pObject )
//{
//    HELIUM_ASSERT( pObject );
//    HELIUM_ASSERT( !pObject->IsPackage() );
//
//    rSerializer.Serialize( pObject );
//
//    for( Asset* pChildObject = pObject->GetFirstChild();
//         pChildObject != NULL;
//         pChildObject = pChildObject->GetNextSibling() )
//    {
//        if( !pChildObject->IsPackage() )
//        {
//            RecursiveSerializeObject( rSerializer, pChildObject );
//        }
//    }
//}
