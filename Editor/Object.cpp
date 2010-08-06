#include "Precompile.h"
#include "Editor/Object.h"

using namespace Helium;
using namespace Helium::Editor;

EDITOR_DEFINE_TYPE( Object );

void Object::InitializeType()
{
  Reflect::RegisterClass<Object>( TXT( "Editor::Object" ) );
}

void Object::CleanupType()
{
  Reflect::UnregisterClass<Object>();
}

Object::Object()
{

}

Object::~Object()
{

}