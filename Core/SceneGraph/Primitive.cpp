/*#include "Precompile.h"*/
#include "Primitive.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::Primitive);

void Primitive::InitializeType()
{
  Reflect::RegisterClassType< SceneGraph::Primitive >( TXT( "SceneGraph::Primitive" ) );
}

void Primitive::CleanupType()
{
  Reflect::UnregisterClassType< SceneGraph::Primitive >();
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

bool Primitive::IsSolid(SceneGraph::Camera* camera) const
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