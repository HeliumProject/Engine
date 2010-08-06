#include "Precompile.h"
#include "Editor/Scene/Tool.h"
#include "Editor/Scene/Scene.h"

using namespace Helium;
using namespace Helium::Editor;

EDITOR_DEFINE_TYPE(Editor::Tool);

void Tool::InitializeType()
{
  Reflect::RegisterClass< Editor::Tool >( TXT( "Editor::Tool" ) );
}

void Tool::CleanupType()
{
  Reflect::UnregisterClass< Editor::Tool >();
}

Tool::Tool( Editor::Scene* scene, PropertiesGenerator* generator )
: m_Generator ( generator )
, m_View ( scene->GetViewport() )
, m_Scene ( scene )
, m_AllowSelection( true )
{

}