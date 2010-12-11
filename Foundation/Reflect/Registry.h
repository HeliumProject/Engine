#pragma once

#include <map>
#include <string>

#include "Platform/Types.h"

#include "Foundation/Memory.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Name.h"
#include "Foundation/Container/SortedMap.h"
#include "Foundation/File/Path.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/API.h"

namespace Helium
{
    namespace Reflect
    {
        // Callbacks for external APIs
        typedef void (*CreatedFunc)(Object* object);
        typedef void (*DestroyedFunc)(Object* object);

        // Registry containers
        typedef SortedMap< Name, Helium::SmartPtr< Type > > M_NameToType;

        // Profile interface
#ifdef PROFILE_ACCUMULATION
        extern Profile::Accumulator g_CloneAccum;
        extern Profile::Accumulator g_ParseAccum;
        extern Profile::Accumulator g_AuthorAccum;
        extern Profile::Accumulator g_ChecksumAccum;
        extern Profile::Accumulator g_PreSerializeAccum;
        extern Profile::Accumulator g_PostSerializeAccum;
        extern Profile::Accumulator g_PreDeserializeAccum;
        extern Profile::Accumulator g_PostDeserializeAccum;
#endif

        // Init/Cleanup
        FOUNDATION_API bool IsInitialized();
        FOUNDATION_API void Initialize();
        FOUNDATION_API void Cleanup();

        FOUNDATION_API Profile::MemoryPoolHandle MemoryPool();

        class FOUNDATION_API Registry
        {
        private:
            friend void Reflect::Initialize();
            friend bool Reflect::IsInitialized();
            friend void Reflect::Cleanup();

            M_NameToType m_TypesByName;
            M_NameToType m_TypesByAlias;
            InitializerStack m_InitializerStack;

            CreatedFunc m_Created; // the callback on creation
            DestroyedFunc m_Destroyed; // the callback on deletion

            Registry();
            virtual ~Registry();

        public:
            // singleton constructor and accessor
            static Registry* GetInstance();

            // used for asserting on thread usage
            bool IsInitThread();

            // register type with registry with type id only
            bool RegisterType( Type* type );
            void UnregisterType( const Type* type );

            // give a type an alias (for legacy considerations)
            void AliasType( const Type* type, Name alias );
            void UnAliasType( const Type* type, Name alias );

            // retrieves type info
            const Type* GetType( Name name ) const;

            // class lookup
            const Class* GetClass( Name name ) const;

            // enumeration lookup
            const Enumeration* GetEnumeration( Name name ) const;

            // create instances of classes
            ObjectPtr CreateInstance( const Class* type ) const;
            ObjectPtr CreateInstance( Name name ) const;

            template<class T>
            Helium::SmartPtr< T > CreateInstance()
            {
                return Reflect::AssertCast< T >( CreateInstance( Reflect::GetType< T >() ) );
            }

            // callbacks
            void Created(Object* object);
            void Destroyed(Object* object);

            // callback setup
            void SetCreatedCallback(CreatedFunc created);
            void SetDestroyedCallback(DestroyedFunc destroyed);
        };

        //
        // Helpers to resolve type information
        //

        template<class T>
        inline const Type* GetType()
        {
            const Type* type = T::s_Type;
            HELIUM_ASSERT(type); // if you hit this then your type is not registered
            return type;
        }

        template<class T>
        inline const Class* GetClass()
        {
            const Class* type = T::s_Class;
            HELIUM_ASSERT(type); // if you hit this then your type is not registered
            return type;
        }

        template<class T>
        inline const Enumeration* GetEnumeration()
        {
            const Enumeration* type = T::s_Enumeration;
            HELIUM_ASSERT(type); // if you hit this then your type is not registered
            return type;
        }

        //
        // Registration templates, these help with creating and registering classes with the registry
        //

        typedef void (*UnregisterFunc)();

        template< class T >
        inline UnregisterFunc RegisterClassType( Name name )
        {
            // create the type information and register it with the registry
            if ( Reflect::Registry::GetInstance()->RegisterType( T::CreateClass( name ) ) )
            {
                // this function will unregister the type we just registered
                return &UnregisterClassType< T >;
            }

            // there was a problem
            return NULL;
        }

        template< class T >
        inline UnregisterFunc RegisterClassType( const tchar_t* name )
        {
            return RegisterClassType< T >( Name( name ) );
        }

        template<class T>
        inline void UnregisterClassType()
        {
            // retrieve the class information and unregister it from the registry
            Reflect::Registry::GetInstance()->UnregisterType( Reflect::GetClass<T>() );
        }

        typedef void EnumerateEnumFunc( Reflect::Enumeration& info );

        template< class T >
        inline UnregisterFunc RegisterEnumType( Name name )
        {
            Reflect::Enumeration* enumeration = T::CreateEnumeration( name );

            // create the type information and register it with the registry
            if ( Reflect::Registry::GetInstance()->RegisterType( enumeration ) )
            {
                // this function will unregister the type we just registered
                return &UnregisterEnumType< T >;
            }

            // there was a problem
            return NULL;
        }

        template< class T >
        inline UnregisterFunc RegisterEnumType( const tchar_t* name )
        {
            return RegisterEnumType< T >( Name( name ) );
        }

        template<class T>
        inline void UnregisterEnumType()
        {
            // retrieve the class information and unregister it from the registry
            Reflect::Registry::GetInstance()->UnregisterType( Reflect::GetEnumeration<T>() );
        }
    }
}