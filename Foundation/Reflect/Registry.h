#pragma once

#include <map>
#include <string>

#include "Platform/Types.h"

#include "Foundation/Memory.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/File/Path.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/API.h"

namespace Helium
{
    namespace Reflect
    {
#ifdef REFLECT_OBJECT_TRACKING

        //
        // Stack record captures stack addresses
        //

        class StackRecord : public Helium::RefCountBase
        {
        public:
            std::vector<uintptr_t>  m_Stack;
            tstring                 m_String;
            bool                    m_Converted;

            StackRecord()
                : m_Converted ( false )
            {
                m_Stack.reserve( 30 );
            }

            const tstring& Convert();  
        };

        typedef Helium::SmartPtr< StackRecord > StackRecordPtr;
        typedef std::vector< StackRecordPtr > V_StackRecordPtr;
        typedef std::map< std::vector<uintptr_t>, StackRecordPtr > M_StackRecord;


        //
        // Creation record stores object information
        //

        class CreationRecord
        {
        public:
            uintptr_t         m_Address;
            tstring         m_Name;
            int             m_Type;

            StackRecordPtr m_CreateStack;
            StackRecordPtr m_DeleteStack;

            CreationRecord();
            CreationRecord(uintptr_t ptr);

            void Dump(FILE* f);
        };

        typedef std::map<uintptr_t, CreationRecord> M_CreationRecord;


        //
        // Tracker object
        //

        class Tracker
        {
        public:
            M_CreationRecord m_CreatedObjects;
            M_CreationRecord m_DeletedObjects;
            M_StackRecord    m_Stacks;

            Tracker();
            virtual ~Tracker();

            // make a stack record
            StackRecordPtr GetStack();

            // save debug info during creation
            void Create(uintptr_t ptr);

            // callback on object delete
            void Delete(uintptr_t ptr);

            // validate a pointer
            void Check(uintptr_t ptr);

            // dump all debug info
            void Dump();
        };

#endif

        // Callbacks for external APIs
        typedef void (*CreatedFunc)(Object* object);
        typedef void (*DestroyedFunc)(Object* object);

        // Registry containers
        typedef std::map< int, Helium::SmartPtr<Type> > M_IDToType;
        typedef std::map< tstring, Helium::SmartPtr<Type> > M_StrToType;

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

            M_IDToType m_TypesByID;
            M_StrToType m_TypesByName;
            M_StrToType m_TypesByAlias;
            InitializerStack m_InitializerStack;

            CreatedFunc m_Created; // the callback on creation
            DestroyedFunc m_Destroyed; // the callback on deletion

#ifdef REFLECT_OBJECT_TRACKING
            Tracker m_Tracker;
#endif

            Registry();
            virtual ~Registry();

        public:
            // singleton constructor and accessor
            static Registry* GetInstance();

            // used for asserting on thread usage
            bool IsInitThread();

            // register type with registry with type id only
            bool RegisterType (Type* type);
            void UnregisterType (const Type* type);

            // give a type an alias (for legacy considerations)
            void AliasType (const Type* type, const tstring& alias);
            void UnAliasType (const Type* type, const tstring& alias);

            // retrieves type info
            const Type* GetType(int id) const;
            const Type* GetType(const tstring& str) const;

            // class lookup
            const Class* GetClass(int32_t id) const;
            const Class* GetClass(const tstring& str) const;

            // enumeration lookup
            const Enumeration* GetEnumeration(int32_t id) const;
            const Enumeration* GetEnumeration(const tstring& str) const;

            // create instances of classes
            ObjectPtr CreateInstance(int id) const;
            ObjectPtr CreateInstance(const Class* type) const;
            ObjectPtr CreateInstance(const tstring& str) const;

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

#ifdef REFLECT_OBJECT_TRACKING
            void TrackCreate(uintptr_t ptr);
            void TrackDelete(uintptr_t ptr);
            void TrackCheck(uintptr_t ptr);
            void TrackDump();
#endif
        };

        //
        // Helpers to resolve type information
        //

        template<class T>
        inline int32_t GetType()
        {
            const Type* type = T::s_Type;
            HELIUM_ASSERT(type); // if you hit this then your type is not registered
            return type->m_TypeID;
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

        template<class T>
        inline UnregisterFunc RegisterClassType( const tstring& name )
        {
            // create the type information and register it with the registry
            if ( Reflect::Registry::GetInstance()->RegisterType( T::CreateClass( name ) ) )
            {
                // this function will unregister the type we just registered
                return &UnregisterClassType<T>;
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

        template<class T>
        inline UnregisterFunc RegisterEnumType( const tstring& name )
        {
            Reflect::Enumeration* enumeration = T::CreateEnumeration( name );

            // create the type information and register it with the registry
            if ( Reflect::Registry::GetInstance()->RegisterType( enumeration ) )
            {
                // this function will unregister the type we just registered
                return &UnregisterEnumType<T>;
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