#include "Foundation/Reflect/Object.h"

#include "Foundation/Container/ObjectPool.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/ArchiveXML.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

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

const Reflect::Class* Object::GetClass() const
{
    return Reflect::GetClass<Object>();
}

bool Object::IsClass( const Reflect::Class* type ) const
{
    const Class* thisType = GetClass();
    HELIUM_ASSERT( thisType );
    return thisType->IsType( type );
}

Reflect::Class* Object::CreateClass( const tchar_t* name )
{
    HELIUM_ASSERT( s_Class == NULL );
    Reflect::Class* type = Class::Create<Object>( name, NULL );
    s_Class = type;
    return type;
}

void Object::PopulateComposite( Reflect::Composite& comp )
{

}

bool Object::IsCompact() const
{
    return false;
}

bool Object::ProcessComponent(ObjectPtr object, const tchar_t* fieldName)
{
    return false; // incurs data loss
}

void Object::ToXML(tstring& xml)
{
    ArchiveXML::ToString( this, xml );
}

void Object::ToBinary(std::iostream& stream)
{
    ArchiveBinary::ToStream( this, stream );
}

void Object::ToFile( const Path& path )
{
    ArchivePtr archive = GetArchive( path );
    archive->Put( this );
    archive->Close();
}

ObjectPtr Object::GetTemplate() const
{
    return ObjectPtr();
}

DataPtr Object::ShouldSerialize( const Field* field )
{
    return field->ShouldSerialize( this );
}

void Object::PreSerialize( const Reflect::Field* field )
{
}

void Object::PostSerialize( const Reflect::Field* field )
{
}

void Object::PreDeserialize( const Reflect::Field* field )
{
}

void Object::PostDeserialize( const Reflect::Field* field )
{
}

void Object::Accept( Visitor& visitor )
{
    if ( !visitor.VisitObject( this ) )
    {
        return;
    }

    const Class* type = GetClass();

    type->Visit( this, visitor );
}

bool Object::Equals( Object* object )
{
    const Class* type = GetClass();

    return type->Equals( this, object );
}

void Object::CopyTo( Object* object )
{
    if ( this != object )
    {
        // 
        // Find common base class
        //

        // This is the common base class type
        const Class* type = NULL; 
        const Class* thisType = this->GetClass();
        const Class* objectType = object->GetClass();

        // Simplest case: the types are the same
        if ( thisType == objectType )
        {
            type = thisType;
        }
        else
        {
            // Types are not the same, we have to search...
            // Iterate up inheritance of this, and look check to see if object HasType for each one
            Reflect::Registry* registry = Reflect::Registry::GetInstance();
            for ( const Class* base = thisType; base && !type; base = static_cast< const Class* >( base->m_Base ) )
            {
                if ( object->IsClass( base ) )
                {
                    // We found the match (which breaks out of this loop)
                    type = ReflectionCast<const Class>( base );
                }
            }

            if ( !type )
            {
                // This should be impossible... at the very least, Object is a common base class for both pointers.
                // This exeception means there's a bug in this function.
                throw Reflect::TypeInformationException( TXT( "Internal error (could not find common base class for %s and %s)" ), thisType->m_Name, objectType->m_Name );
            }
        }

        type->Copy( this, object );
    }
}

ObjectPtr Object::Clone()
{
    ObjectPtr clone = Registry::GetInstance()->CreateInstance( GetClass() );

    PreSerialize( NULL );
    clone->PreDeserialize( NULL );

    const Class* type = GetClass();
    type->Copy( this, clone );

    clone->PostDeserialize( NULL );
    PostSerialize( NULL );

    return clone;
}

void Object::RaiseChanged( const Field* field ) const
{
    e_Changed.Raise( ObjectChangeArgs( this, field ) );
}