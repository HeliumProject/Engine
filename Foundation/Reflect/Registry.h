#pragma once

#include "Platform/Types.h"

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
        HELIUM_FOUNDATION_API bool IsInitialized();
        HELIUM_FOUNDATION_API void Initialize();
        HELIUM_FOUNDATION_API void Cleanup();

        HELIUM_FOUNDATION_API Profile::MemoryPoolHandle MemoryPool();

        class HELIUM_FOUNDATION_API Registry
        {
        private:
            friend void Reflect::Initialize();
            friend bool Reflect::IsInitialized();
            friend void Reflect::Cleanup();

            Registry();
            ~Registry();

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
                return Reflect::AssertCast< T >( CreateInstance( Reflect::GetClass< T >() ) );
            }

        private:
            M_HashToType        m_TypesByHash;
        };

        //
        // Helpers to resolve type information
        //

        template<class T>
        inline const Class* GetClass()
        {
            T::s_Registrar.Register();
            return T::s_Class;
        }

        template<class T>
        inline const Structure* GetStructure()
        {
            T::s_Registrar.Register();
            return T::s_Structure;
        }

        template<class T>
        inline const Enumeration* GetEnumeration()
        {
            T::s_Registrar.Register();
            return T::s_Enumeration;
        }
    }
}

#include "Foundation/Reflect/Registry.inl"