#include "Precompile.h"
#include "Primitive.h"

using namespace Editor;

LUNA_DEFINE_TYPE(Editor::Primitive);

void Primitive::InitializeType()
{
  Reflect::RegisterClass< Editor::Primitive >( TXT( "Editor::Primitive" ) );
}

void Primitive::CleanupType()
{
  Reflect::UnregisterClass< Editor::Primitive >();
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

bool Primitive::IsSolid(Editor::Camera* camera) const
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