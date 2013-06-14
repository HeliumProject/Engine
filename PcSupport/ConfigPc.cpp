#include "PcSupportPch.h"
#include "PcSupport/ConfigPc.h"

#include "Engine/FileLocations.h"
#include "Foundation/FilePath.h"
#include "Engine/Config.h"

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
