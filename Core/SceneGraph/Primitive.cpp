/*#include "Precompile.h"*/
#include "Primitive.h"

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT(Core::Primitive);

void Primitive::InitializeType()
{
  Reflect::RegisterClassType< Core::Primitive >( TXT( "Core::Primitive" ) );
}

void Primitive::CleanupType()
{
  Reflect::UnregisterClassType< Core::Primitive >();
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

bool Primitive::IsSolid(Core::Camera* camera) const
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