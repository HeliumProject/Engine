#include "Precompile.h"
#include "AnimationAttribute.h"

#include "PersistentDataFactory.h"

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::AnimationAttribute );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::AnimationAttribute types.
// 
void AnimationAttribute::InitializeType()
{
  Reflect::RegisterClass< Luna::AnimationAttribute >( "Luna::AnimationAttribute" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::AnimationAttribute >(), &Luna::AnimationAttribute::CreateAnimationAttribute );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::AnimationAttribute types.
// 
void AnimationAttribute::CleanupType()
{
  Reflect::UnregisterClass< Luna::AnimationAttribute >();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function. 
// 
Luna::PersistentDataPtr AnimationAttribute::CreateAnimationAttribute( Reflect::Element* attribute, Luna::AssetManager* assetManager )
{
  return new AnimationAttribute( Reflect::AssertCast< Asset::AnimationAttribute >( attribute ), assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AnimationAttribute::AnimationAttribute( Asset::AnimationAttribute* attribute, Luna::AssetManager* assetManager )
: Luna::AttributeWrapper( attribute, assetManager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AnimationAttribute::~AnimationAttribute()
{
}


///////////////////////////////////////////////////////////////////////////////
// Returns the file ID of the animation set associated with this animation.
// 
tuid AnimationAttribute::GetAnimationSetID() const
{
  const Asset::AnimationAttribute* pkg = GetPackage< Asset::AnimationAttribute >();
  return pkg->m_AnimationSetId;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the animation set that is associated with this animation (and raises
// a changed event on the package).
// 
void AnimationAttribute::SetAnimationSetID( const tuid& fileID )
{
  Asset::AnimationAttribute* pkg = GetPackage< Asset::AnimationAttribute >();
  if ( fileID != pkg->m_AnimationSetId )
  {
    pkg->m_AnimationSetId = fileID;
    pkg->RaiseChanged( pkg->GetClass()->FindField( &Asset::AnimationAttribute::m_AnimationSetId ) );
  }
}
