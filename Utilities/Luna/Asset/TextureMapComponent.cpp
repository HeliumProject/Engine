#include "Precompile.h"
#include "TextureMapComponent.h"
#include "PersistentDataFactory.h"

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::TextureMapComponent );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::TextureMapComponent types.
// 
void TextureMapComponent::InitializeType()
{
  Reflect::RegisterClass<Luna::TextureMapComponent>( TXT( "Luna::TextureMapComponent" ) );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::TextureMapComponent >(), &TextureMapComponent::CreateTextureMapComponent );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::TextureMapComponent types.
// 
void TextureMapComponent::CleanupType()
{
  Reflect::UnregisterClass<Luna::TextureMapComponent>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function. 
// 
Luna::PersistentDataPtr TextureMapComponent::CreateTextureMapComponent( Reflect::Element* attribute, Luna::AssetManager* assetManager )
{
  return new Luna::TextureMapComponent( Reflect::AssertCast< Asset::TextureMapComponent >( attribute ), assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
TextureMapComponent::TextureMapComponent( Asset::TextureMapComponent* attribute, Luna::AssetManager* assetManager )
: Luna::FileBackedComponent( attribute, assetManager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
TextureMapComponent::~TextureMapComponent()
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to maintain the file ID of this texture map, while copying all the
// other properties.
// 
Undo::CommandPtr TextureMapComponent::CopyFrom( Luna::PersistentData* src )
{
  Undo::CommandPtr command;

  if ( src->HasType( Reflect::GetType< Luna::TextureMapComponent >() ) )
  {
    // Create a new source by cloning the persistent data
    Asset::TextureMapComponentPtr clonedPkg = Reflect::AssertCast< Asset::TextureMapComponent >( src->GetPackage< Reflect::Element >()->Clone() );
    
    // Change the file path on our clone (this way when we copy the data, it
    // won't clobber our file path).
    clonedPkg->SetPath( GetPackage< Asset::TextureMapComponent >()->GetPath() );

    // Let the base class handle the rest, using our cloned data.
    Luna::TextureMapComponentPtr clonedSrc = PersistentDataFactory::GetInstance()->CreateTyped< Luna::TextureMapComponent >( clonedPkg, m_AssetManager );
    command = __super::CopyFrom( clonedSrc );
  }
  else
  {
    command = __super::CopyFrom( src );
  }

  return command;
}
