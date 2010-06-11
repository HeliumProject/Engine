#include "Platform/Windows/Windows.h"

#include "AssetClass.h"
#include "AssetTypeInfo.h"
#include "Exceptions.h"

#include "Attribute/Attribute.h"

#include "EntityAsset.h"
#include "ShaderAsset.h"
#include "SceneAsset.h"

#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Foundation/Log.h"
#include "Reflect/Version.h"
#include "Reflect/Object.h"

#include <memory>

using namespace Reflect;
using namespace Asset;

std::string AssetClass::s_BaseBuiltDirectory = "";

REFLECT_DEFINE_ABSTRACT( AssetClass );

void AssetClass::EnumerateClass( Reflect::Compositor<AssetClass>& comp )
{
    Reflect::Field* fieldDescription = comp.AddField( &AssetClass::m_Description, "m_Description" );
    Reflect::Field* fieldPath = comp.AddField( &AssetClass::m_Path, "m_Path", Reflect::FieldFlags::Hide );
}

AssetClass::AssetClass()
{
}

AssetClassPtr AssetClass::LoadAssetClass( const char* path )
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
        Log::Warning( "%s\n", exception.what() );
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
    std::stringstream str;
    str << TUID_HEX_FORMAT << m_Path.Hash();
    Nocturnal::Path builtDirectory( s_BaseBuiltDirectory + std::string( "/" ) + str.str() );
    return builtDirectory;
}

std::string AssetClass::GetFullName() const
{
#pragma TODO( "break out some settings stuff for the asset root" )
    return m_Path.Filename();
}

std::string AssetClass::GetShortName() const
{
    std::string shortName = m_Path.Filename();

    // strip off extension
    while ( FileSystem::HasExtension( shortName ) )
    {
        FileSystem::StripExtension( shortName );
    }

    return shortName;
}

#pragma TODO( "implement AssetType info lookup functions" )
std::string AssetClass::GetAssetTypeName( const AssetType assetType )
{
    return "";
}

std::string AssetClass::GetAssetTypeBuilder( const AssetType AssetType )
{
    return "";
}

std::string AssetClass::GetAssetTypeIcon( const AssetType AssetType )
{
    return "";
}


void AssetClass::AttributeChanged( const Attribute::AttributeBase* attr )
{
    __super::AttributeChanged( attr );
}

void AssetClass::SetAttribute( const Attribute::AttributePtr& attr, bool validate )
{
    __super::SetAttribute( attr, validate );
}

void AssetClass::RemoveAttribute( i32 typeID )
{
    __super::RemoveAttribute( typeID );
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
bool AssetClass::ValidateClass( std::string& error ) const
{
    error.clear();
    return true;
}

bool AssetClass::ValidateCompatible( const Attribute::AttributePtr &attr, std::string& error ) const
{
    if ( attr->GetAttributeUsage() == Attribute::AttributeUsages::Instance )
    {
        error = "The " + attr->GetClass()->m_UIName + " attribute can only be added to an instance of an asset.";
        return false;
    }

    return __super::ValidateCompatible( attr, error );
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

    Attribute::M_Attribute::const_iterator itor = assetClass->GetAttributes().begin();
    Attribute::M_Attribute::const_iterator end = assetClass->GetAttributes().end();
    for( ; itor != end; ++itor )
    {
        if( engineClass->ContainsAttribute( itor->second->GetType() ) )
            ++score;


    }

    // not sure if we want to do this since the global engine type "templates" contain the
    // maximum number of attributes that something can have and still be classified as that type
    itor = engineClass->GetAttributes().begin();
    end = engineClass->GetAttributes().end();
    for( ; itor != end; ++itor )
    {
        if( !assetClass->ContainsAttribute( itor->second->GetType() ) )
            --score;
    }

    return score;
}
