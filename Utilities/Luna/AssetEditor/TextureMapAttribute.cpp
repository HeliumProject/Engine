#include "Precompile.h"
#include "TextureMapAttribute.h"
#include "PersistentDataFactory.h"

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::TextureMapAttribute );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::TextureMapAttribute types.
// 
void TextureMapAttribute::InitializeType()
{
  Reflect::RegisterClass<Luna::TextureMapAttribute>( "Luna::TextureMapAttribute" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::TextureMapAttribute >(), &TextureMapAttribute::CreateTextureMapAttribute );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::TextureMapAttribute types.
// 
void TextureMapAttribute::CleanupType()
{
  Reflect::UnregisterClass<Luna::TextureMapAttribute>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function. 
// 
Luna::PersistentDataPtr TextureMapAttribute::CreateTextureMapAttribute( Reflect::Element* attribute, Luna::AssetManager* assetManager )
{
  return new Luna::TextureMapAttribute( Reflect::AssertCast< Asset::TextureMapAttribute >( attribute ), assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
TextureMapAttribute::TextureMapAttribute( Asset::TextureMapAttribute* attribute, Luna::AssetManager* assetManager )
: Luna::FileBackedAttribute( attribute, assetManager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
TextureMapAttribute::~TextureMapAttribute()
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to maintain the file ID of this texture map, while copying all the
// other properties.
// 
Undo::CommandPtr TextureMapAttribute::CopyFrom( Luna::PersistentData* src )
{
  Undo::CommandPtr command;

  if ( src->HasType( Reflect::GetType< Luna::TextureMapAttribute >() ) )
  {
    // Create a new source by cloning the persistent data
    Asset::TextureMapAttributePtr clonedPkg = Reflect::AssertCast< Asset::TextureMapAttribute >( src->GetPackage< Reflect::Element >()->Clone() );
    
    // Change the file path on our clone (this way when we copy the data, it
    // won't clobber our file path).
    clonedPkg->SetPath( GetPackage< Asset::TextureMapAttribute >()->GetPath() );

    // Let the base class handle the rest, using our cloned data.
    Luna::TextureMapAttributePtr clonedSrc = PersistentDataFactory::GetInstance()->CreateTyped< Luna::TextureMapAttribute >( clonedPkg, m_AssetManager );
    command = __super::CopyFrom( clonedSrc );
  }
  else
  {
    command = __super::CopyFrom( src );
  }

  return command;
}
