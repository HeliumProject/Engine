#pragma once

#include "Platform/Types.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Memory.h"
#include "Foundation/Checksum/Crc32.h"
#include "Foundation/Container/SortedMap.h"
#include "Foundation/File/Path.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Foundation/Reflect/ReflectionInfo.h"

namespace Helium
{
    namespace Reflect
    {
        // Callbacks for external APIs
        typedef void (*CreatedFunc)(Object* object);
        typedef void (*DestroyedFunc)(Object* object);

        // Registry containers
        typedef SortedMap< uint32_t, Helium::SmartPtr< Type > > M_HashToType;

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
            void AliasType( const Type* type, const tchar_t* alias );
            void UnaliasType( const Type* type, const tchar_t* alias );

            // type lookup
            const Type* GetType( uint32_t crc ) const;
            inline const Type* GetType( const tchar_t* name ) const;

            // class lookup
            const Class* GetClass( uint32_t crc ) const;
            inline const Class* GetClass( const tchar_t* name ) const;

            // enumeration lookup
            inline const Enumeration* GetEnumeration( uint32_t crc ) const;
            inline const Enumeration* GetEnumeration( const tchar_t* name ) const;

            // create instances of classes
            ObjectPtr CreateInstance( const Class* type ) const;
            ObjectPtr CreateInstance( uint32_t crc ) const;
            inline ObjectPtr CreateInstance( const tchar_t* name ) const;

            template<class T>
            Helium::SmartPtr< T > CreateInstance()
            {
                return Reflect::AssertCast< T >( CreateInstance( Reflect::GetType< T >() ) );
            }

        private:
            M_HashToType        m_TypesByHash;
            InitializerStack    m_InitializerStack;
        };

        //
        // Helpers to resolve type information
        //

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
        inline UnregisterFunc RegisterClassType( const tchar_t* name )
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

        template<class T>
        inline void UnregisterClassType()
        {
            // retrieve the class information and unregister it from the registry
            Reflect::Registry::GetInstance()->UnregisterType( Reflect::GetClass<T>() );
        }

        typedef void EnumerateEnumFunc( Reflect::Enumeration& info );

        template< class T >
        inline UnregisterFunc RegisterEnumType( const tchar_t* name )
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

        template<class T>
        inline void UnregisterEnumType()
        {
            // retrieve the class information and unregister it from the registry
            Reflect::Registry::GetInstance()->UnregisterType( Reflect::GetEnumeration<T>() );
        }
    }
}

#include "Foundation/Reflect/Registry.inl"