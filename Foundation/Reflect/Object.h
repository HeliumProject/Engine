#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"

#include "Foundation/Memory/SmartPtr.h"

#include "API.h"
#include "Class.h"
#include "Registry.h"
#include "Exceptions.h"

namespace Helium
{
    namespace Reflect
    {
        //
        // Reflect::Object is a reference counted and type checked HELIUM_ABSTRACT base class
        //

        class FOUNDATION_API Object HELIUM_ABSTRACT
        {
        private:
            mutable int32_t m_RefCount;

        protected:
            Object();

        private:
            Object(const Object& rhs);

        public:
            virtual ~Object();


            //
            // Memory
            //

            void* operator new(size_t bytes);
            void operator delete(void *ptr, size_t bytes);


            //
            // Type id
            //

            // Returns the type id for this instance
            virtual int32_t GetType() const;

            // Deduces type membership for this instance
            virtual bool HasType(int32_t type) const;

            // Retrieves the reflection data for this instance
            virtual const Reflect::Class* GetClass() const;

            // Enumerates member data (stub)
            static void EnumerateClass( Reflect::Compositor<Element>& comp );


            //
            // Reference Counting
            //

        public:
            int GetRefCount() const;
            void IncrRefCount() const;
            void DecrRefCount() const;
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
        inline DerivedT* AssertCast(Reflect::Object* base)
        {
            if ( base != NULL )
            {
                HELIUM_ASSERT( base->HasType(GetClass<DerivedT>()->m_TypeID) );
            }

            return DangerousCast<DerivedT>(base);
        }

        template<class DerivedT>
        inline const DerivedT* ConstAssertCast(const Reflect::Object* base)
        {
            if ( base != NULL )
            {
                HELIUM_ASSERT( base->HasType(GetClass<DerivedT>()->m_TypeID) );
            }

            return ConstDangerousCast<DerivedT>(base);
        }

        //
        // TryCast type checks and throws if failure
        //

        template<class DerivedT>
        inline DerivedT* TryCast(Reflect::Object* base)
        {
            if ( base != NULL && !base->HasType(GetClass<DerivedT>()->m_TypeID) )
            {
                throw CastException ( TXT( "Object of type '%s' cannot be cast to type '%s'" ), base->GetClass()->m_ShortName.c_str(), GetClass<DerivedT>()->m_ShortName.c_str() );
            }

            return DangerousCast<DerivedT>(base);
        }

        template<class DerivedT>
        inline const DerivedT* ConstTryCast(const Reflect::Object* base)
        {
            if ( base != NULL && !base->HasType(GetClass<DerivedT>()->m_TypeID) )
            {
                throw CastException ( TXT( "Object of type '%s' cannot be cast to type '%s'" ), base->GetClass()->m_ShortName.c_str(), GetClass<DerivedT>()->m_ShortName.c_str() );
            }

            return ConstDangerousCast<DerivedT>(base);
        }

        //
        // ObjectCast always type checks and returns null if failure
        //

        template<class DerivedT>
        inline DerivedT* ObjectCast(Reflect::Object* base)
        {
            if ( base != NULL && base->HasType(GetClass<DerivedT>()->m_TypeID) )
            {
                return DangerousCast<DerivedT>(base);
            }
            else
            {
                return NULL;
            }
        }

        template<class DerivedT>
        inline const DerivedT* ConstObjectCast(const Reflect::Object* base)
        {
            if ( base != NULL && base->HasType(GetClass<DerivedT>()->m_TypeID) )
            {
                return ConstDangerousCast<DerivedT>(base);
            }
            else
            {
                return NULL;
            }
        }
    }
}