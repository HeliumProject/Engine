#include "Precompile.h"
#include "ShadowDirection.h"

#include "Scene.h"
#include "SceneManager.h"
#include "InstanceType.h"

#include "Core/Enumerator.h"
#include "UIToolKit/ImageManager.h"

#include "PrimitivePointer.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::ShadowDirection);

void ShadowDirection::InitializeType()
{
  Reflect::RegisterClass< Luna::ShadowDirection >( "Luna::ShadowDirection" );

  Enumerator::InitializePanel( "ShadowDirection", CreatePanelSignature::Delegate( &ShadowDirection::CreatePanel ) );
}

void ShadowDirection::CleanupType()
{
  Reflect::UnregisterClass< Luna::ShadowDirection >();
}

ShadowDirection::ShadowDirection(Luna::Scene* scene)
: Luna::Light (scene, new Content::ShadowDirection() )
{

}

ShadowDirection::ShadowDirection(Luna::Scene* scene, Content::ShadowDirection* light)
: Luna::Light ( scene, light )
{

}

i32 ShadowDirection::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "light_16.png" );
}

std::string ShadowDirection::GetApplicationTypeName() const
{
  return "ShadowDirection";
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by ShadowDirection.
//
bool ShadowDirection::ValidatePanel(const std::string& name)
{
  if (name == "ShadowDirection")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

