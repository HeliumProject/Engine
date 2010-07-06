#include "Platform/Windows/Windows.h"

#include "AssetClass.h"

#include "Foundation/Component/Component.h"

#include "Pipeline/Asset/Classes/Entity.h"
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
    comp.AddField( &AssetClass::m_Description, "m_Description" );
    comp.AddField( &AssetClass::m_Tags, "m_Tags" );

    comp.AddField( &AssetClass::m_Path, "m_Path", Reflect::FieldFlags::Hide );
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
    return m_Path.Get();
}

tstring AssetClass::GetShortName() const
{
    return m_Path.Basename();
}

void AssetClass::GetDependencies( std::set< Nocturnal::Path >& dependencies )
{
    return;
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

void AssetClass::CopyTo(const Reflect::ElementPtr& destination) 
{
    // Restore the Asset Class ID after performing the copy
    AssetClass* destinationAsset = Reflect::ObjectCast< AssetClass >( destination );
    __super::CopyTo( destination );
}
