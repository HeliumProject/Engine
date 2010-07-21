#include "Precompile.h"
#include "Tool.h"
#include "Scene.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::Tool);

void Tool::InitializeType()
{
  Reflect::RegisterClass< Luna::Tool >( TXT( "Luna::Tool" ) );
}

void Tool::CleanupType()
{
  Reflect::UnregisterClass< Luna::Tool >();
}

Tool::Tool( Luna::Scene* scene, PropertiesGenerator* generator )
: m_Generator ( generator )
, m_View ( scene->GetViewport() )
, m_Scene ( scene )
, m_AllowSelection( true )
{

}