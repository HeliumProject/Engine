#include "Platform/Windows/Windows.h"

#include "AssetClass.h"
#include "Pipeline/Asset/AssetTypeInfo.h"
#include "Pipeline/Asset/AssetExceptions.h"

#include "Pipeline/Component/Component.h"

#include "Pipeline/Asset/Classes/EntityAsset.h"
#include "Pipeline/Asset/Classes/ShaderAsset.h"
#include "Pipeline/Asset/Classes/SceneAsset.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/Object.h"

#include <memory>

using namespace Reflect;
using namespace Asset;

tstring AssetClass::s_BaseBuiltDirectory = TXT( "" );

REFLECT_DEFINE_ABSTRACT( AssetClass );

void AssetClass::EnumerateClass( Reflect::Compositor<AssetClass>& comp )
{
    Reflect::Field* fieldDescription = comp.AddField( &AssetClass::m_Description, "m_Description" );
    Reflect::Field* fieldPath = comp.AddField( &AssetClass::m_Path, "m_Path", Reflect::FieldFlags::Hide );
}

AssetClass::AssetClass()
{
}

AssetClassPtr AssetClass::LoadAssetClass( const tchar* path )
{
    AssetClassPtr assetClass = NULL;
    try
    {
        Nocturnal::Path filePath( path );

        assetClass = Reflect::Archive::FromFile< AssetClass >( filePath );
        assetClass->SetPath( filePath );
        assetClass->LoadFinished();
    }
    catch ( const Nocturnal::Exception& exception )
    {
        Log::Warning( TXT( "%s\n" ), exception.What() );
    }

    // success
    return assetClass;
}


const Nocturnal::Path& AssetClass::GetFilePath()
{
    return m_Path;
}

Nocturnal::Path AssetClass::GetDataDir()
{
    return Nocturnal::Path( m_Path.Directory() );
}

Nocturnal::Path AssetClass::GetBuiltDirectory()
{
#pragma TODO( "make human-readable built directories" )
    tstringstream str;
    str << TUID_HEX_FORMAT << m_Path.Hash();
    Nocturnal::Path builtDirectory( s_BaseBuiltDirectory + TXT( "/" ) + str.str() );
    return builtDirectory;
}

tstring AssetClass::GetFullName() const
{
#pragma TODO( "break out some settings stuff for the asset root" )
    return m_Path.Filename();
}

tstring AssetClass::GetShortName() const
{
    return m_Path.Basename();
}

#pragma TODO( "implement AssetType info lookup functions" )
tstring AssetClass::GetAssetTypeName( const AssetType assetType )
{
    return TXT( "" );
}

tstring AssetClass::GetAssetTypeBuilder( const AssetType AssetType )
{
    return TXT( "" );
}

tstring AssetClass::GetAssetTypeIcon( const AssetType AssetType )
{
    return TXT( "" );
}


void AssetClass::ComponentChanged( const Component::ComponentBase* attr )
{
    __super::ComponentChanged( attr );
}

void AssetClass::SetComponent( const Component::ComponentPtr& attr, bool validate )
{
    __super::SetComponent( attr, validate );
}

void AssetClass::RemoveComponent( i32 typeID )
{
    __super::RemoveComponent( typeID );
}

void AssetClass::Serialize( const AssetVersionPtr &version )
{
    Reflect::Archive::ToFile( this, m_Path, version );

    m_Modified = false;
}

/////////////////////////////////////////////////////////////////////////////
// Override this function to provide logic that makes sure an asset class is
// valid.  Fill out the error message provided so that it can be reported
// to the user.  This function is intended to be used by the UI to make sure
// that an asset is valid before it is built (validation is done during save).
// 
bool AssetClass::ValidateClass( tstring& error ) const
{
    error.clear();
    return true;
}

bool AssetClass::ValidateCompatible( const Component::ComponentPtr &component, tstring& error ) const
{
    if ( component->GetComponentUsage() == Component::ComponentUsages::Instance )
    {
        error = TXT( "The " ) + component->GetClass()->m_UIName + TXT( " component can only be added to an instance of an asset." );
        return false;
    }

    return __super::ValidateCompatible( component, error );
}

void AssetClass::LoadFinished()
{

}

bool AssetClass::IsBuildable() const
{
    return false;
}

bool AssetClass::IsViewable() const
{
    return false;
}

void AssetClass::CopyTo(const Reflect::ElementPtr& destination) 
{
    // Restore the Asset Class ID after performing the copy
    AssetClass* destinationAsset = Reflect::ObjectCast< AssetClass >( destination );
    __super::CopyTo( destination );
}

AssetType AssetClass::GetAssetType() const
{
    if ( this->HasType( Reflect::GetType<SceneAsset>() ) )
    {
        return AssetTypes::Level;
    }

    if ( this->HasType( Reflect::GetType<ShaderAsset>() ) )
    {
        return AssetTypes::Shader;
    }

#pragma TODO( "classify based on attributes" )

    return AssetTypes::Null;
}

static int ScoreAssetType( const AssetClass* assetClass, const AssetClass* engineClass )
{
    int score = 0;

    Component::M_Component::const_iterator itor = assetClass->GetComponents().begin();
    Component::M_Component::const_iterator end = assetClass->GetComponents().end();
    for( ; itor != end; ++itor )
    {
        if( engineClass->ContainsComponent( itor->second->GetType() ) )
            ++score;


    }

    // not sure if we want to do this since the global engine type "templates" contain the
    // maximum number of attributes that something can have and still be classified as that type
    itor = engineClass->GetComponents().begin();
    end = engineClass->GetComponents().end();
    for( ; itor != end; ++itor )
    {
        if( !assetClass->ContainsComponent( itor->second->GetType() ) )
            --score;
    }

    return score;
}
