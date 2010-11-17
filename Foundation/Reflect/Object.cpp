#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Class.h"

#include "Platform/Atomic.h"

#include <malloc.h>

using namespace Helium;
using namespace Helium::Reflect;

const Type* Object::s_Type = NULL;
const Class* Object::s_Class = NULL;

Object::Object()
: m_RefCount (0)
{
    if (Reflect::IsInitialized())
    {
        Reflect::Registry::GetInstance()->Created(this);
    }
}

Object::~Object()
{
    if (Reflect::IsInitialized())
    {
        Reflect::Registry::GetInstance()->Destroyed(this);
    }
}

void* Object::operator new(size_t bytes)
{
    if (Reflect::MemoryPool().Valid())
    {
        Profile::Memory::Allocate( Reflect::MemoryPool(), (uint32_t)bytes );
    }

    return ::malloc( bytes );
}

void Object::operator delete(void *ptr, size_t bytes)
{
    if (Reflect::MemoryPool().Valid())
    {
        Profile::Memory::Deallocate( Reflect::MemoryPool(), (uint32_t)bytes );
    }

    ::free(ptr);
}

int32_t Object::GetType() const
{
    return ReservedTypes::First;
}

bool Object::HasType(int32_t type) const
{
    return type != ReservedTypes::Invalid;
}

const Reflect::Class* Object::GetClass() const
{
    return Reflect::GetClass<Object>();
}

Reflect::Class* Object::CreateClass( const tstring& name )
{
    HELIUM_ASSERT( s_Class == NULL );
    Reflect::Class* type = Class::Create<Object>( name, TXT("") );
    s_Type = s_Class = type;
    return type;
}

void Object::EnumerateClass( Reflect::Compositor<Object>& comp )
{

}

int Object::GetRefCount() const
{
    return m_RefCount;
}

void Object::IncrRefCount() const
{
#ifdef REFLECT_OBJECT_TRACKING
    if (Reflect::IsInitialized() && m_RefCount != 0)
    {
        Reflect::Registry::GetInstance()->TrackCheck((uintptr_t)this);
    }
#endif

    Helium::AtomicIncrementAcquire( m_RefCount );
}

void Object::DecrRefCount() const
{
#ifdef REFLECT_OBJECT_TRACKING
    if (Reflect::IsInitialized())
    {
        Reflect::Registry::GetInstance()->TrackCheck((uintptr_t)this);
    }
#endif

    Helium::AtomicDecrementRelease( m_RefCount ); 

    if (m_RefCount == 0)
    {
        delete this; 
    }
}