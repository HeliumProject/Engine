#include "Precompile.h"
#include "DuplicateTool.h"

#include "Mesh.h"
#include "Scene.h"
#include "Volume.h"
#include "VolumeType.h"

#include "Pick.h"

using namespace Helium;
using namespace Helium::Editor;

EDITOR_DEFINE_TYPE(Editor::DuplicateTool);

void DuplicateTool::InitializeType()
{
  Reflect::RegisterClass< Editor::DuplicateTool >( TXT( "Editor::DuplicateTool" ) );
}

void DuplicateTool::CleanupType()
{
  Reflect::UnregisterClass< Editor::DuplicateTool >();
}

DuplicateTool::DuplicateTool(Editor::Scene* scene, PropertiesGenerator* generator)
: Editor::CreateTool (scene, generator)
, m_Source (NULL)
{
  if (!m_Scene->GetSelection().GetItems().Empty())
  {
    m_Source = Reflect::ObjectCast<Editor::Transform>( *m_Scene->GetSelection().GetItems().Begin() );
  }
}

DuplicateTool::~DuplicateTool()
{

}

Editor::TransformPtr DuplicateTool::CreateNode()
{
  if (m_Source)
  {
    return Reflect::ObjectCast<Editor::Transform>( m_Source->Duplicate() );
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