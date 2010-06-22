#include "ComponentInit.h" 

#include "Component.h"
#include "ComponentCollection.h"

#include "ComponentCategory.h"
#include "ComponentCategories.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Reflect/Registry.h"

using namespace Component;

i32 g_ComponentInitCount = 0; 
Nocturnal::InitializerStack g_ComponentRegisteredTypes;

Nocturnal::InitializerStack g_ComponentInitializerStack;

void Component::Initialize()
{
  if (++g_ComponentInitCount == 1)
  {
    g_ComponentInitializerStack.Push( &Reflect::Initialize, &Reflect::Cleanup );

//    g_ComponentRegisteredTypes.Push( Reflect::RegisterClass<ComponentCollection>( "ComponentCollection" ) );

    g_ComponentRegisteredTypes.Push( Reflect::RegisterClass<ComponentBase>( "ComponentBase" ) );
    g_ComponentRegisteredTypes.Push( Reflect::RegisterClass<ComponentCollection>( "ComponentCollection" ) );
    g_ComponentRegisteredTypes.Push( Reflect::RegisterClass<ComponentCategory>( "ComponentCategory" ) );

    ComponentCategories::Initialize();
  }
}

void Component::Cleanup()
{
  if (--g_ComponentInitCount == 0)
  {
    ComponentCategories::Cleanup();
    g_ComponentRegisteredTypes.Cleanup();
    g_ComponentInitializerStack.Cleanup();
  }
}
