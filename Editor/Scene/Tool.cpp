#include "Precompile.h"
#include "Tool.h"
#include "Scene.h"

using namespace Editor;

LUNA_DEFINE_TYPE(Editor::Tool);

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