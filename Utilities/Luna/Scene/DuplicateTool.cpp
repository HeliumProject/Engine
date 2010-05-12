#include "Precompile.h"
#include "DuplicateTool.h"

#include "Mesh.h"
#include "Scene.h"
#include "Volume.h"
#include "VolumeType.h"

#include "Pick.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::DuplicateTool);

void DuplicateTool::InitializeType()
{
  Reflect::RegisterClass< Luna::DuplicateTool >( "Luna::DuplicateTool" );
}

void DuplicateTool::CleanupType()
{
  Reflect::UnregisterClass< Luna::DuplicateTool >();
}

DuplicateTool::DuplicateTool(Luna::Scene* scene, Enumerator* enumerator)
: Luna::CreateTool (scene, enumerator)
, m_Source (NULL)
{
  if (!m_Scene->GetSelection().GetItems().Empty())
  {
    m_Source = Reflect::ObjectCast<Luna::Transform>( *m_Scene->GetSelection().GetItems().Begin() );
  }
}

DuplicateTool::~DuplicateTool()
{

}

Luna::TransformPtr DuplicateTool::CreateNode()
{
  if (m_Source)
  {
    return Reflect::ObjectCast<Luna::Transform>( m_Source->Duplicate() );
  }
  else
  {
    return NULL;
  }
}

void DuplicateTool::CreateProperties()
{
  __super::CreateProperties();
}