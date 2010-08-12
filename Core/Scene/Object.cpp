/*#include "Precompile.h"*/
#include "Core/Scene/Object.h"

using namespace Helium;
using namespace Helium::Core;

SCENE_DEFINE_TYPE( Object );

void Object::InitializeType()
{
  Reflect::RegisterClassType<Object>( TXT( "Core::Object" ) );
}

void Object::CleanupType()
{
  Reflect::UnregisterClassType<Object>();
}

Object::Object()
{

}

Object::~Object()
{

}