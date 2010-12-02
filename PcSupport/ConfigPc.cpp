//----------------------------------------------------------------------------------------------------------------------
// ConfigPc.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "PcSupportPch.h"
#include "PcSupport/ConfigPc.h"

#include "Foundation/File/File.h"
#include "Foundation/File/Path.h"
#include "Engine/Config.h"
#include "PcSupport/XmlPackageLoader.h"
#include "PcSupport/XmlSerializer.h"

using namespace Lunar;

/// Save the user configuration settings.
///
/// @return  True if the configuration was saved successfully, false if not.
bool ConfigPc::SaveUserConfig()
{
    HELIUM_TRACE( TRACE_INFO, TXT( "ConfigPc: Saving user configuration.\n" ) );

    Config& rConfig = Config::GetStaticInstance();

    Package* pConfigPackage = rConfig.GetUserConfigPackage();
    if( !pConfigPackage )
    {
        HELIUM_TRACE( TRACE_WARNING, TXT( "ConfigPc: No user configuration exists to save.\n" ) );

        return false;
    }

    Path userDataDirectory;
    if ( !File::GetUserDataDirectory( userDataDirectory ) )
    {
        HELIUM_TRACE( TRACE_WARNING, TXT( "ConfigPc: No user data directory could be determined.\n" ) );
        return false;
    }

    GameObjectPath configPackagePath = pConfigPackage->GetPath();

    Path packageFilePath( userDataDirectory + configPackagePath.ToFilePathString().GetData() + L_XML_PACKAGE_FILE_EXTENSION );

    HELIUM_TRACE( TRACE_INFO, TXT( "ConfigPc: Saving configuration to \"%s\".\n" ), *packageFilePath );

    XmlSerializer serializer;
    if( !serializer.Initialize( packageFilePath.c_str() ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "ConfigPc: Failed to initialize package serializer for writing to \"%s\".\n" ),
            *packageFilePath );

        return false;
    }

    size_t configObjectCount = pConfigPackage->GetChildCount();
    for( size_t objectIndex = 0; objectIndex < configObjectCount; ++objectIndex )
    {
        GameObject* pObject = pConfigPackage->GetChild( objectIndex );
        if( pObject && !pObject->IsPackage() )
        {
            RecursiveSerializeObject( serializer, pObject );
        }
    }

    serializer.Shutdown();

    HELIUM_TRACE( TRACE_INFO, TXT( "ConfigPc: User configuration saved.\n" ) );

    return true;
}

/// Serialize the given object, followed by all its children.
///
/// @param[in] rSerializer  Serializer to use for serialization.
/// @param[in] pObject      GameObject to serialize.
void ConfigPc::RecursiveSerializeObject( XmlSerializer& rSerializer, GameObject* pObject )
{
    HELIUM_ASSERT( pObject );
    HELIUM_ASSERT( !pObject->IsPackage() );

    rSerializer.Serialize( pObject );

    size_t childCount = pObject->GetChildCount();
    for( size_t childIndex = 0; childIndex < childCount; ++childIndex )
    {
        GameObject* pChildObject = pObject->GetChild( childIndex );
        if( pChildObject && !pChildObject->IsPackage() )
        {
            RecursiveSerializeObject( rSerializer, pChildObject );
        }
    }
}
