#include "Object.h"
#include "Registry.h"
#include "Class.h"
#include "Serializer.h"

#include "Platform/Atomic.h"

#include <malloc.h>

using namespace Helium;
using namespace Helium::Reflect;

Object::Object()
: m_RefCount (0)
{
    if (Reflect::IsInitialized())
    {
        Reflect::Registry::GetInstance()->Created(this);
    }
}

Object::Object(const Object& rhs)
{
    HELIUM_BREAK();
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
        Profile::Memory::Allocate( Reflect::MemoryPool(), (u32)bytes );
    }

    return ::malloc( bytes );
}

void Object::operator delete(void *ptr, size_t bytes)
{
    if (Reflect::MemoryPool().Valid())
    {
        Profile::Memory::Deallocate( Reflect::MemoryPool(), (u32)bytes );
    }

    ::free(ptr);
}

i32 Object::GetType() const
{
    return -1;
}

bool Object::HasType(i32 type) const
{
    return false;
}

const Reflect::Class* Object::GetClass() const
{
    return NULL;
}

void Object::EnumerateClass( Reflect::Compositor<Element>& comp )
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
        Reflect::Registry::GetInstance()->TrackCheck((uintptr)this);
    }
#endif

    Platform::AtomicIncrement( &m_RefCount );
}

void Object::DecrRefCount() const
{
#ifdef REFLECT_OBJECT_TRACKING
    if (Reflect::IsInitialized())
    {
        Reflect::Registry::GetInstance()->TrackCheck((uintptr)this);
    }
#endif

    Platform::AtomicDecrement( &m_RefCount ); 

    if (m_RefCount == 0)
    {
        delete this; 
    }
}