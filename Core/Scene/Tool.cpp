/*#include "Precompile.h"*/
#include "Core/Scene/Tool.h"
#include "Core/Scene/Scene.h"

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT(Core::Tool);

void Tool::InitializeType()
{
  Reflect::RegisterClassType< Core::Tool >( TXT( "Core::Tool" ) );
}

void Tool::CleanupType()
{
  Reflect::UnregisterClassType< Core::Tool >();
}

Tool::Tool( Core::Scene* scene, PropertiesGenerator* generator )
: m_Generator ( generator )
, m_View ( scene->GetViewport() )
, m_Scene ( scene )
, m_AllowSelection( true )
{

}