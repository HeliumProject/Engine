#include "Precompile.h"
#include "ShaderUsage.h"

#include "PersistentDataFactory.h"

using namespace Luna;

LUNA_DEFINE_TYPE( Luna::ShaderUsage );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ShaderUsage::InitializeType()
{
  Reflect::RegisterClass<Luna::ShaderUsage>( "Luna::ShaderUsage" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::ShaderUsage >(), &ShaderUsage::CreateShaderUsage );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ShaderUsage::CleanupType()
{
  Reflect::UnregisterClass<Luna::ShaderUsage>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator.
// 
Luna::PersistentDataPtr ShaderUsage::CreateShaderUsage( Reflect::Element* shaderUsage, Luna::AssetManager* assetManager )
{
  return new Luna::ShaderUsage( Reflect::AssertCast< Asset::ShaderUsage >( shaderUsage ), assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ShaderUsage::ShaderUsage( Asset::ShaderUsage* shaderUsage, Luna::AssetManager* assetManager )
: Luna::PersistentData( shaderUsage, assetManager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ShaderUsage::~ShaderUsage()
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to prevent clobbering of the m_ShaderID field.
// 
Undo::CommandPtr ShaderUsage::CopyFrom( Luna::PersistentData* src )
{
  Undo::CommandPtr command;

  if ( src->HasType( Reflect::GetType< Luna::ShaderUsage >() ) )
  {
    // Create a new source by cloning the persistent data
    Asset::ShaderUsagePtr clonedPkg = Reflect::AssertCast< Asset::ShaderUsage >( src->GetPackage< Reflect::Element >()->Clone() );
    
    // Change the file path on our clone (this way when we copy the data, it
    // won't clobber our file path).
    clonedPkg->m_ShaderID = GetPackage< Asset::ShaderUsage >()->m_ShaderID;

    // Let the base class handle the rest, using our cloned data.
    Luna::ShaderUsagePtr clonedSrc = PersistentDataFactory::GetInstance()->CreateTyped< Luna::ShaderUsage >( clonedPkg, m_AssetManager );
    command = __super::CopyFrom( clonedSrc );
  }
  else
  {
    command = __super::CopyFrom( src );
  }

  return command;
}
