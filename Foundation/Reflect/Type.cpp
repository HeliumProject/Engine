#include "Type.h"
#include "Registry.h"
#include "Foundation/Reflect/Data/Data.h"

#include "Platform/Mutex.h"

using namespace Helium::Reflect;

Helium::Mutex g_TypeMutex;

Type::Type()
: m_TypeID (ReservedTypes::Invalid)
, m_Size (0)
{

}

Type::~Type()
{

}

int32_t Type::AssignTypeID()
{
    static int32_t nextID = ReservedTypes::First;

    return nextID++;
}
