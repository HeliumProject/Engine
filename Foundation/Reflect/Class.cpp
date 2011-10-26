#include "FoundationPch.h"
#include "Class.h"
#include "Object.h"
#include "Registry.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "ArchiveBinary.h"

#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Reflect;

Class::Class()
: m_Creator (NULL)
{

}

Class::~Class()
{

}

Class* Class::Create()
{
    return new Class();
}

template<>
Class* Class::Create< Object >( const tchar_t* name, const tchar_t* baseName, CreateObjectFunc creator )
{
    Class* info = Class::Create();

    info->m_Size = sizeof( Object );
    info->m_Name = name;

    // object should have no creator
    HELIUM_ASSERT( creator == NULL );

    // object should have no base class
    HELIUM_ASSERT( baseName == NULL );

    return info;
}
