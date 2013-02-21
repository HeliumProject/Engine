#include "EnginePch.h"
#include "Engine/Config.h"

#include "Engine/Asset.h"
#include "Engine/AssetLoader.h"
#include "Engine/Package.h"
#include "Engine/PackageLoader.h"

using namespace Helium;

Config* Config::sm_pInstance = NULL;

/// Constructor.
Config::Config()
: m_bLoadingConfigPackage( false )
{
    HELIUM_VERIFY( m_configContainerPackagePath.Set(
        Name( HELIUM_CONFIG_CONTAINER_PACKAGE ),
        true,
        AssetPath( NULL_NAME ) ) );
    HELIUM_VERIFY( m_defaultConfigPackagePath.Set(
        Name( HELIUM_CONFIG_DEFAULT_PACKAGE_BASE HELIUM_CONFIG_PLATFORM_SUFFIX ),
        true,
        m_configContainerPackagePath ) );
    HELIUM_VERIFY( m_userConfigPackagePath.Set(
        Name( HELIUM_CONFIG_USER_PACKAGE_BASE ),
        true,
        m_configContainerPackagePath ) );
}

/// Destructor.
Config::~Config()
{
}

/// Begin async loading of configuration settings.
///
/// @see TryFinishLoad()
void Config::BeginLoad()
{
    // Check if loading is already in progress.
    if( !m_objectLoadIds.IsEmpty() )
    {
        HELIUM_TRACE(
            TraceLevels::Warning,
            TXT( "Config::BeginLoad(): Called while configuration loading is already in progress.\n" ) );

        return;
    }

    // Clear out all existing object references.
    m_spDefaultConfigPackage.Release();
    m_spUserConfigPackage.Release();

    m_defaultConfigObjects.Clear();
    m_configObjects.Clear();

    // Initiate pre-loading of the default and user configuration packages.
    AssetLoader* pLoader = AssetLoader::GetStaticInstance();
    HELIUM_ASSERT( pLoader );

    m_objectLoadIds.Clear();
    size_t loadId;

    loadId = pLoader->BeginLoadObject( m_defaultConfigPackagePath );
    HELIUM_ASSERT( IsValid( loadId ) );
    m_objectLoadIds.Add( loadId );

    loadId = pLoader->BeginLoadObject( m_userConfigPackagePath );
    HELIUM_ASSERT( IsValid( loadId ) );
    m_objectLoadIds.Add( loadId );

    m_bLoadingConfigPackage = true;
}

/// Poll for whether asynchronous config loading has completed.
///
/// @return  True if loading has completed, false if not.
///
/// @see BeginLoad()
bool Config::TryFinishLoad()
{
    AssetLoader* pLoader = AssetLoader::GetStaticInstance();
    HELIUM_ASSERT( pLoader );

    if( m_bLoadingConfigPackage )
    {
        HELIUM_ASSERT( m_objectLoadIds.GetSize() == 2 );

        size_t defaultConfigLoadId = m_objectLoadIds[ 0 ];
        if( IsValid( defaultConfigLoadId ) )
        {
            HELIUM_ASSERT( !m_spDefaultConfigPackage );

            AssetPtr spPackage;
            if( !pLoader->TryFinishLoad( defaultConfigLoadId, spPackage ) )
            {
                return false;
            }

            m_spDefaultConfigPackage = Reflect::AssertCast< Package >( spPackage.Get() );
            HELIUM_ASSERT( m_spDefaultConfigPackage );

            SetInvalid( m_objectLoadIds[ 0 ] );
        }

        size_t userConfigLoadId = m_objectLoadIds[ 1 ];
        if( IsValid( userConfigLoadId ) )
        {
            HELIUM_ASSERT( !m_spUserConfigPackage );

            AssetPtr spPackage;
            if( !pLoader->TryFinishLoad( userConfigLoadId, spPackage ) )
            {
                return false;
            }

            m_spUserConfigPackage = Reflect::AssertCast< Package >( spPackage.Get() );
            HELIUM_ASSERT( m_spUserConfigPackage );

            SetInvalid( m_objectLoadIds[ 1 ] );
        }

        m_objectLoadIds.Resize( 0 );
        m_bLoadingConfigPackage = false;

        // Begin loading all objects in each configuration package.
        AssetPath packagePath;
        PackageLoader* pPackageLoader;
        size_t objectCount;

        packagePath = m_spDefaultConfigPackage->GetPath();
        pPackageLoader = m_spDefaultConfigPackage->GetLoader();
        HELIUM_ASSERT( pPackageLoader );
        objectCount = pPackageLoader->GetObjectCount();
        for( size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex )
        {
            AssetPath objectPath = pPackageLoader->GetObjectPath( objectIndex );
            if( !objectPath.IsPackage() && objectPath.GetParent() == packagePath )
            {
                size_t loadId = pLoader->BeginLoadObject( objectPath );
                HELIUM_ASSERT( IsValid( loadId ) );

                m_objectLoadIds.Add( loadId );
            }
        }

        packagePath = m_spUserConfigPackage->GetPath();
        pPackageLoader = m_spUserConfigPackage->GetLoader();
        HELIUM_ASSERT( pPackageLoader );
        objectCount = pPackageLoader->GetObjectCount();
        for( size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex )
        {
            AssetPath objectPath = pPackageLoader->GetObjectPath( objectIndex );
            if( !objectPath.IsPackage() && objectPath.GetParent() == packagePath )
            {
                size_t loadId = pLoader->BeginLoadObject( objectPath );
                HELIUM_ASSERT( IsValid( loadId ) );

                m_objectLoadIds.Add( loadId );
            }
        }
    }

    AssetPath defaultConfigPackagePath = m_spDefaultConfigPackage->GetPath();

    AssetPtr spObject;
    size_t objectIndex = m_objectLoadIds.GetSize();
    while( objectIndex != 0 )
    {
        --objectIndex;

        size_t loadId = m_objectLoadIds[ objectIndex ];
        HELIUM_ASSERT( IsValid( loadId ) );
        if( !pLoader->TryFinishLoad( loadId, spObject ) )
        {
            return false;
        }

        if( spObject )
        {
            HELIUM_TRACE( TraceLevels::Info, TXT( "Loaded configuration object \"%s\".\n" ), *spObject->GetPath().ToString() );
            if( spObject->GetPath().GetParent() == defaultConfigPackagePath )
            {
                m_defaultConfigObjects.Add( spObject );
            }
            else
            {
                m_configObjects.Add( spObject );
            }
        }

        m_objectLoadIds.Remove( objectIndex );
    }

    // List of async object load IDs should be empty, but call Clear() on it to regain allocated memory as well.
    HELIUM_ASSERT( m_objectLoadIds.IsEmpty() );
    m_objectLoadIds.Clear();

    // Now that all configuration objects have now been loaded, make sure that a user configuration object
    // corresponds to each default configuration object.
    size_t defaultConfigObjectCount = m_defaultConfigObjects.GetSize();
    for( size_t defaultObjectIndex = 0; defaultObjectIndex < defaultConfigObjectCount; ++defaultObjectIndex )
    {
        Asset* pDefaultConfigObject = m_defaultConfigObjects[ defaultObjectIndex ];
        HELIUM_ASSERT( pDefaultConfigObject );

        Name objectName = pDefaultConfigObject->GetName();

        size_t userConfigObjectCount = m_configObjects.GetSize();
        size_t userObjectIndex;
        for( userObjectIndex = 0; userObjectIndex < userConfigObjectCount; ++userObjectIndex )
        {
            Asset* pUserConfigObject = m_configObjects[ userObjectIndex ];
            HELIUM_ASSERT( pUserConfigObject );
            if( pUserConfigObject->GetName() == objectName )
            {
                break;
            }
        }

        if( userObjectIndex >= userConfigObjectCount )
        {
            const AssetType* pConfigObjectType = pDefaultConfigObject->GetAssetType();
            HELIUM_ASSERT( pConfigObjectType );

            AssetPtr spUserConfigObject;
            bool bCreateResult = Asset::CreateObject(
                spUserConfigObject,
                pConfigObjectType,
                objectName,
                m_spUserConfigPackage,
                pDefaultConfigObject );
            HELIUM_ASSERT( bCreateResult );
            if( bCreateResult )
            {
                HELIUM_ASSERT( spUserConfigObject );

                HELIUM_TRACE(
                    TraceLevels::Info,
                    TXT( "Config: Created user configuration object \"%s\".\n" ),
                    *spUserConfigObject->GetPath().ToString() );

                m_configObjects.Add( spUserConfigObject );
            }
            else
            {
                HELIUM_TRACE(
                    TraceLevels::Error,
                    TXT( "Config: Failed to create user configuration object \"%s\".\n" ),
                    *objectName );
            }
        }
    }

    m_defaultConfigObjects.Clear();

    return true;
}

/// Get the singleton Config instance, creating it if necessary.
///
/// @return  Reference to the Config instance.
///
/// @see DestroyStaticInstance()
Config& Config::GetStaticInstance()
{
    if( !sm_pInstance )
    {
        sm_pInstance = new Config;
        HELIUM_ASSERT( sm_pInstance );
    }

    return *sm_pInstance;
}

/// Destroy the singleton Config instance.
///
/// @see GetStaticInstance()
void Config::DestroyStaticInstance()
{
    delete sm_pInstance;
    sm_pInstance = NULL;
}
