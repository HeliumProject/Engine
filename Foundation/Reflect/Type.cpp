#include "Type.h"
#include "Registry.h"
#include "Foundation/Reflect/Data/Data.h"

#include "Platform/Mutex.h"

using namespace Helium::Reflect;

Helium::Mutex g_TypeMutex;

Type::Type()
: m_Size( 0 )
{

}

Type::~Type()
{

}

void Type::Report() const
{

}

void Type::Unregister() const
{

}