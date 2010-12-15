#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Class.h"
#include "Foundation/Container/ObjectPool.h"

#include <malloc.h>

using namespace Helium;
using namespace Helium::Reflect;

/// Static reference count proxy management data.
struct ObjectRefCountSupport::StaticData
{
    /// Number of proxy objects to allocate per block for the proxy pool.
    static const size_t POOL_BLOCK_SIZE = 1024;

    /// Proxy object pool.
    ObjectPool< RefCountProxy< Object > > proxyPool;
#if HELIUM_ENABLE_MEMORY_TRACKING
    /// Active reference count proxies.
    ConcurrentHashSet< RefCountProxy< Object >* > activeProxySet;
#endif

    /// @name Construction/Destruction
    //@{
    StaticData();
    //@}
};

ObjectRefCountSupport::StaticData* ObjectRefCountSupport::sm_pStaticData = NULL;

const Type* Object::s_Type = NULL;
const Class* Object::s_Class = NULL;

/// Retrieve a reference count proxy from the global pool.
///
/// @return  Pointer to a reference count proxy.
///
/// @see Release()
RefCountProxy< Object >* ObjectRefCountSupport::Allocate()
{
    // Lazy initialization of the proxy management data.  Even though this isn't thread-safe, it should still be fine as
    // the proxy system should be initialized from the main thread before any sub-threads are spawned (i.e. during
    // startup type initialization).
    StaticData* pStaticData = sm_pStaticData;
    if( !pStaticData )
    {
        pStaticData = new StaticData;
        HELIUM_ASSERT( pStaticData );
        sm_pStaticData = pStaticData;
    }

    RefCountProxy< Object >* pProxy = pStaticData->proxyPool.Allocate();
    HELIUM_ASSERT( pProxy );

#if HELIUM_ENABLE_MEMORY_TRACKING
    ConcurrentHashSet< RefCountProxy< Object >* >::Accessor activeProxySetAccessor;
    HELIUM_VERIFY( pStaticData->activeProxySet.Insert( activeProxySetAccessor, pProxy ) );
#endif

    return pProxy;
}

/// Release a reference count proxy back to the global pool.
///
/// @param[in] pProxy  Pointer to the reference count proxy to release.
///
/// @see Allocate()
void ObjectRefCountSupport::Release( RefCountProxy< Object >* pProxy )
{
    HELIUM_ASSERT( pProxy );

    StaticData* pStaticData = sm_pStaticData;
    HELIUM_ASSERT( pStaticData );

#if HELIUM_ENABLE_MEMORY_TRACKING
    HELIUM_VERIFY( pStaticData->activeProxySet.Remove( pProxy ) );
#endif

    pStaticData->proxyPool.Release( pProxy );
}

/// Release the name table and free all allocated memory.
///
/// This should only be called immediately prior to application exit.
void ObjectRefCountSupport::Shutdown()
{
    delete sm_pStaticData;
    sm_pStaticData = NULL;
}

#if HELIUM_ENABLE_MEMORY_TRACKING
/// Get the number of active reference count proxies.
///
/// Be careful when using this function, as the number may change if other threads are actively setting and clearing
/// references to objects.  Unless all other threads have been halted or are otherwise no longer using any smart
/// pointers, you should not expect this value to match the number actually iterated when using functions such as
/// GetFirstActiveProxy().
///
/// @return  Current number of active smart pointer references.
///
/// @see GetFirstActiveProxy()
size_t ObjectRefCountSupport::GetActiveProxyCount()
{
    HELIUM_ASSERT( sm_pStaticData );

    return sm_pStaticData->activeProxySet.GetSize();
}

/// Initialize a constant accessor to the first active reference count proxy.
///
/// @param[in] rAccessor  Accessor to initialize.
///
/// @return  True if there are active reference count proxies and the accessor was successfully set to reference the
///          first one, false if not.
///
/// @see GetActiveProxyCount()
bool ObjectRefCountSupport::GetFirstActiveProxy(
    ConcurrentHashSet< RefCountProxy< Object >* >::ConstAccessor& rAccessor )
{
    HELIUM_ASSERT( sm_pStaticData );

    return sm_pStaticData->activeProxySet.First( rAccessor );
}
#endif

/// Constructor.
ObjectRefCountSupport::StaticData::StaticData()
: proxyPool( POOL_BLOCK_SIZE )
{
}

Object::Object()
{

}

Object::~Object()
{

}

void* Object::operator new( size_t bytes )
{
    if ( Reflect::MemoryPool().Valid() )
    {
        Profile::Memory::Allocate( Reflect::MemoryPool(), (uint32_t)bytes );
    }

    Helium::DefaultAllocator allocator;
    void* memory = Helium::AllocateAlignmentHelper( allocator, bytes );

    return memory;
}

void* Object::operator new( size_t /*bytes*/, void* memory )
{
    return memory;
}

void Object::operator delete( void *ptr, size_t bytes )
{
    if ( Reflect::MemoryPool().Valid() )
    {
        Profile::Memory::Deallocate( Reflect::MemoryPool(), (uint32_t)bytes );
    }

    Helium::DefaultAllocator allocator;
    allocator.Free( ptr );
}

void Object::operator delete( void* /*ptr*/, void* /*memory*/ )
{
}

/// Perform any necessary work immediately prior to destroying this object.
///
/// Note that the parent-class implementation should always be chained last.
void Object::PreDestroy()
{
}

/// Actually destroy this object.
///
/// This should only be called by the reference counting system once the last strong reference to this object has
/// been cleared.  It should never be called manually.
void Object::Destroy()
{
    delete this;
}

const Reflect::Type* Object::GetType() const
{
    return Reflect::GetType<Object>();
}

bool Object::HasType( const Reflect::Type* type ) const
{
    return type == Reflect::GetType<Object>();
}

const Reflect::Class* Object::GetClass() const
{
    return Reflect::GetClass<Object>();
}

Reflect::Class* Object::CreateClass( Name name )
{
    HELIUM_ASSERT( s_Class == NULL );
    Reflect::Class* type = Class::Create<Object>( name, NULL_NAME );
    s_Type = s_Class = type;
    return type;
}

void Object::EnumerateClass( Reflect::Compositor<Object>& comp )
{

}
