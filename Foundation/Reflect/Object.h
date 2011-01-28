#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Platform/Utility.h"

#include "Foundation/Automation/Event.h"
#include "Foundation/Automation/Attribute.h"
#include "Foundation/Container/ConcurrentHashSet.h"
#include "Foundation/File/Path.h"
#include "Foundation/Memory/ReferenceCounting.h"
#include "Foundation/Reflect/API.h"
#include "Foundation/Reflect/Exceptions.h"

namespace Helium
{
    namespace Reflect
    {
        class Type;
        class Field;
        class Composite;
        class Structure;
        class Class;
        class Object;
        class Data;

        //
        // ObjectRefCountSupport provides the support interface for managing reference counting data
        //

        class FOUNDATION_API ObjectRefCountSupport
        {
        public:
            /// Base type of reference counted object.
            typedef Object BaseType;

            /// @name Object Destruction Support
            //@{
            inline static void PreDestroy( Object* pObject );
            inline static void Destroy( Object* pObject );
            //@}

            /// @name Reference Count Proxy Allocation Interface
            //@{
            static RefCountProxy< Object >* Allocate();
            static void Release( RefCountProxy< Object >* pProxy );

            static void Shutdown();
            //@}

#if HELIUM_ENABLE_MEMORY_TRACKING
            /// @name Active Proxy Iteration
            //@{
            static size_t GetActiveProxyCount();
            static bool GetFirstActiveProxy(
                ConcurrentHashSet< RefCountProxy< Object >* >::ConstAccessor& rAccessor );
            //@}
#endif

        private:
            struct StaticData;

            /// Static proxy management data.
            static StaticData* sm_pStaticData;
        };

        //
        // Event delegate to support getting notified if this object changes
        //

        struct ObjectChangeArgs
        {
            const Object* m_Object;
            const Field* m_Field;

            ObjectChangeArgs( const Object* object, const Field* field = NULL )
                : m_Object( object )
                , m_Field( field )
            {
            }
        };
        typedef Helium::Signature< const ObjectChangeArgs&, Helium::AtomicRefCountBase > ObjectChangeSignature;

        //
        // Object is the abstract base class of a serializable class
        //

        class FOUNDATION_API Object HELIUM_ABSTRACT : NonCopyable
        {
        protected:
            HELIUM_DECLARE_REF_COUNT( Object, ObjectRefCountSupport );

        protected:
            Object();

        public:
            virtual ~Object();

            //
            // Memory
            //

            void* operator new( size_t bytes );
            void* operator new( size_t bytes, void* memory );
            void operator delete( void* ptr, size_t bytes );
            void operator delete( void* ptr, void* memory );

            virtual void PreDestroy();
            virtual void Destroy();  // This should only be called by the reference counting system!

            //
            // Type checking
            //

            // Retrieves the reflection data for this instance
            static const Class* s_Class;
            virtual const Reflect::Class* GetClass() const;

            // Deduces type membership for this instance
            bool IsClass( const Reflect::Class* type ) const;

            // Create class data block for this type
            static Reflect::Class* CreateClass( const tchar_t* name );

            // Enumerates member data (stub)
            static void PopulateComposite( Reflect::Composite& comp );

            //
            // Persistence
            //

            // Specifies if the value is directly between the start and end name
            virtual bool                IsCompact() const;

            // This the process callback for sub and primitive objects to have their data be aggregated into the parent instance
            virtual void                ProcessUnknown( Object* object, uint32_t fieldNameCrc );

            // Serialize to a particular data target, just works on this
            void                        ToXML( tstring& xml );
            void                        ToBinary( std::iostream& stream );
            void                        ToFile( const Path& path );

            // Callbacks are executed at the appropriate time by the archive and cloning APIs
            virtual ObjectPtr           GetTemplate() const;
            virtual StrongPtr< Data >   ShouldSerialize( const Field* field );
            virtual void                PreSerialize( const Field* field );
            virtual void                PostSerialize( const Field* field );
            virtual void                PreDeserialize( const Field* field );
            virtual void                PostDeserialize( const Field* field );

            //
            // Utilities
            //

            // Visits fields recursively, used to interactively traverse structures
            virtual void                Accept( Visitor& visitor );

            // Do comparison logic against other object, checks type and field data
            virtual bool                Equals( Object* object );

            // Copy this object's data into another object isntance
            virtual void                CopyTo( Object* object );

            // Copy this object's data into a new instance
            virtual ObjectPtr           Clone();

            //
            // Notification
            //

            // Event raised when an object is modified
            mutable ObjectChangeSignature::Event e_Changed;

            // Raise the modification event manually, null field mean ambiguous/multiple changes
            virtual void RaiseChanged( const Field* field = NULL ) const;

            // Notify a particular field was changed
            template< class FieldT >
            void FieldChanged( FieldT* fieldAddress ) const;

            // Modify and notify a field change
            template< class ObjectT, class FieldT >
            void ChangeField( FieldT ObjectT::* field, const FieldT& newValue );
        };

        //
        // AssertCast type checks in debug and asserts if failure, does no type checking in release
        //

        template<class DerivedT>
        inline DerivedT* AssertCast( Reflect::Object* base );

        template<class DerivedT>
        inline const DerivedT* AssertCast(const Reflect::Object* base);

        //
        // ThrowCast type checks and throws if failure
        //

        template<class DerivedT>
        inline DerivedT* ThrowCast(Reflect::Object* base);

        template<class DerivedT>
        inline const DerivedT* ThrowCast(const Reflect::Object* base);

        //
        // SafeCast always type checks and returns null if failure
        //

        template<class DerivedT>
        inline DerivedT* SafeCast(Reflect::Object* base);

        template<class DerivedT>
        inline const DerivedT* SafeCast(const Reflect::Object* base);
    }
}

// declares creator for constructable types
#define _REFLECT_DECLARE_CREATOR( OBJECT ) \
public: \
static Helium::Reflect::Object* CreateObject() { return new OBJECT; }

// declares type checking functions
#define _REFLECT_DECLARE_OBJECT( OBJECT, BASE ) \
public: \
typedef BASE Base; \
typedef OBJECT This; \
virtual const Helium::Reflect::Class* GetClass() const HELIUM_OVERRIDE; \
static Helium::Reflect::Class* CreateClass( const tchar_t* name ); \
static const Helium::Reflect::Class* s_Class;

// defines the static type info vars
#define _REFLECT_DEFINE_OBJECT( OBJECT, CREATOR ) \
const Helium::Reflect::Class* OBJECT::GetClass() const \
{ \
    return s_Class; \
} \
\
Helium::Reflect::Class* OBJECT::CreateClass( const tchar_t* name ) \
{ \
    HELIUM_ASSERT( s_Class == NULL ); \
    HELIUM_ASSERT( OBJECT::Base::s_Class != NULL ); \
    Helium::Reflect::Class* type = Helium::Reflect::Class::Create<OBJECT>(name, OBJECT::Base::s_Class->m_Name, CREATOR); \
    s_Class = type; \
    return type; \
} \
const Helium::Reflect::Class* OBJECT::s_Class = NULL;

// declares an abstract object (an object that either A: cannot be instantiated or B: is never actually serialized)
#define REFLECT_DECLARE_ABSTRACT( OBJECT, BASE ) \
    _REFLECT_DECLARE_OBJECT( OBJECT, BASE )

// defines the abstract object class
#define REFLECT_DEFINE_ABSTRACT( OBJECT ) \
    _REFLECT_DEFINE_OBJECT( OBJECT, NULL )

// declares a concrete object with creator
#define REFLECT_DECLARE_OBJECT( OBJECT, BASE ) \
    _REFLECT_DECLARE_OBJECT( OBJECT, BASE ) \
    _REFLECT_DECLARE_CREATOR( OBJECT)

// defines a concrete object
#define REFLECT_DEFINE_OBJECT( OBJECT ) \
    _REFLECT_DEFINE_OBJECT( OBJECT, &OBJECT::CreateObject )

#include "Foundation/Reflect/Object.inl"