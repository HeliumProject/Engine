#include "Precompile.h"
#include "Object.h"

using namespace Luna;

LUNA_DEFINE_TYPE( Object );

void Object::InitializeType()
{
  Reflect::RegisterClass<Object>( TXT( "Luna::Object" ) );
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