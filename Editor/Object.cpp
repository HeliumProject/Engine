#include "Precompile.h"
#include "Object.h"

using namespace Editor;

LUNA_DEFINE_TYPE( Object );

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