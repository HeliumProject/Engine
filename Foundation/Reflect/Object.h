#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Platform/Utility.h"

#include "Foundation/Name.h"
#include "Foundation/Container/ConcurrentHashSet.h"
#include "Foundation/Memory/ReferenceCounting.h"
#include "Foundation/Reflect/API.h"
#include "Foundation/Reflect/Exceptions.h"

namespace Helium
{
    namespace Reflect
    {
        class Object;
        class Type;
        class Class;
        template< class T > class Compositor;

        //
        // Reflect::ObjectRefCountSupport provides the support interface for managing reference counting data for
        // Reflect::Object instances.
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
        // Reflect::Object is a reference counted and type checked abstract base class
        //

        class FOUNDATION_API Object HELIUM_ABSTRACT : NonCopyable
        {
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
            // Type id
            //

            // Returns the type id for this instance
            virtual const Reflect::Type* GetType() const;

            // Deduces type membership for this instance
            virtual bool HasType( const Reflect::Type* type ) const;

            // Retrieves the reflection data for this instance
            virtual const Reflect::Class* GetClass() const;

            // Create class data block for this type
            static Reflect::Class* CreateClass( Name name );

            // Enumerates member data (stub)
            static void EnumerateClass( Reflect::Compositor<Object>& comp );

        public:
            static const Type* s_Type;
            static const Class* s_Class;
        };


        //
        // DangerousCast does not type checking
        //

        template<class DerivedT>
        inline DerivedT* DangerousCast(Reflect::Object* base)
        {
            return static_cast<DerivedT*>(base);
        }

        template<class DerivedT>
        inline const DerivedT* ConstDangerousCast(const Reflect::Object* base)
        {
            return static_cast<const DerivedT*>(base);
        }

        //
        // AssertCast type checks in debug and asserts if failure, does no type checking in release
        //

        template<class DerivedT>
        inline DerivedT* AssertCast( Reflect::Object* base )
        {
            if ( base != NULL )
            {
                HELIUM_ASSERT( base->HasType( GetClass<DerivedT>() ) );
            }

            return DangerousCast<DerivedT>( base );
        }

        template<class DerivedT>
        inline const DerivedT* ConstAssertCast(const Reflect::Object* base)
        {
            if ( base != NULL )
            {
                HELIUM_ASSERT( base->HasType( GetClass<DerivedT>() ) );
            }

            return ConstDangerousCast<DerivedT>( base );
        }

        //
        // TryCast type checks and throws if failure
        //

        template<class DerivedT>
        inline DerivedT* TryCast(Reflect::Object* base)
        {
            if ( base != NULL && !base->HasType( GetClass<DerivedT>() ) )
            {
                throw CastException ( TXT( "Object of type '%s' cannot be cast to type '%s'" ), *base->GetClass()->m_Name, *GetClass<DerivedT>()->m_Name );
            }

            return DangerousCast<DerivedT>( base );
        }

        template<class DerivedT>
        inline const DerivedT* ConstTryCast(const Reflect::Object* base)
        {
            if ( base != NULL && !base->HasType( GetClass<DerivedT>() ) )
            {
                throw CastException ( TXT( "Object of type '%s' cannot be cast to type '%s'" ), *base->GetClass()->m_Name, *GetClass<DerivedT>()->m_Name );
            }

            return ConstDangerousCast<DerivedT>( base );
        }

        //
        // ObjectCast always type checks and returns null if failure
        //

        template<class DerivedT>
        inline DerivedT* ObjectCast(Reflect::Object* base)
        {
            if ( base != NULL && base->HasType( GetClass<DerivedT>() ) )
            {
                return DangerousCast<DerivedT>( base );
            }
            else
            {
                return NULL;
            }
        }

        template<class DerivedT>
        inline const DerivedT* ConstObjectCast(const Reflect::Object* base)
        {
            if ( base != NULL && base->HasType( GetClass<DerivedT>() ) )
            {
                return ConstDangerousCast<DerivedT>( base );
            }
            else
            {
                return NULL;
            }
        }

        /// Perform any pre-destruction work before clearing the last strong reference to an object and destroying the
        /// object.
        ///
        /// @param[in] pObject  Object about to be destroyed.
        ///
        /// @see Destroy()
        void ObjectRefCountSupport::PreDestroy( Object* pObject )
        {
            HELIUM_ASSERT( pObject );

            pObject->PreDestroy();
        }

        /// Destroy an object after the final strong reference to it has been cleared.
        ///
        /// @param[in] pObject  Object to destroy.
        ///
        /// @see PreDestroy()
        void ObjectRefCountSupport::Destroy( Object* pObject )
        {
            HELIUM_ASSERT( pObject );

            pObject->Destroy();
        }
    }
}