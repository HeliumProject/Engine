#include "Precompile.h"
#include "Primitive.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::Primitive);

void Primitive::InitializeType()
{
  Reflect::RegisterClass< Luna::Primitive >( "Luna::Primitive" );
}

void Primitive::CleanupType()
{
  Reflect::UnregisterClass< Luna::Primitive >();
}

Primitive::Primitive(ResourceTracker* tracker)
: VertexResource (tracker)
, m_IsSolid (false)
, m_IsTransparent (false)
, m_IsUsingCameraShadingMode (false)
{
  SetPopulator( PopulateSignature::Delegate( this, &Primitive::Populate ) );
}

Primitive::~Primitive()
{
  Delete();
}

bool Primitive::IsSolid(Luna::Camera* camera) const
{
  if (m_IsUsingCameraShadingMode)
  {
    switch (camera->GetShadingMode())
    {
    case ShadingModes::Wireframe:
      {
        return false;
      }

    case ShadingModes::Material:
      {
        return true;
      }

    case ShadingModes::Texture:
      {
        return true;
      }
    }

    return m_IsSolid;
  }
  else
  {
    return m_IsSolid;
  }
}