#include "Precompile.h"
#include "Clue.h"

#include "Scene.h"
#include "SceneManager.h"

#include "VolumeType.h"

#include "Core/Enumerator.h"
#include "UIToolKit/ImageManager.h"

#include "Content/Clue.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::Clue);

void Clue::InitializeType()
{
  Reflect::RegisterClass< Luna::Clue >( "Luna::Clue" );

  Enumerator::InitializePanel( "Clue", CreatePanelSignature::Delegate( &Clue::CreatePanel ) );
}

void Clue::CleanupType()
{
  Reflect::UnregisterClass< Luna::Clue >();
}

Clue::Clue(Luna::Scene* scene)
: Luna::Volume (scene, new Content::Clue ())
{

}

Clue::Clue(Luna::Scene* scene, Content::Clue* volume)
: Luna::Volume ( scene, volume )
{

}

Clue::~Clue()
{

}

i32 Clue::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "clue_16.png" );
}

std::string Clue::GetApplicationTypeName() const
{
  return "Clue";
}