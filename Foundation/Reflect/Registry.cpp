#include "Registry.h"
#include "Version.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "DOM.h"

#ifdef REFLECT_OBJECT_TRACKING
# include "Platform/Mutex.h"
# include "Platform/Windows/Debug.h"
#endif

#include "Platform/Atomic.h"
#include "Platform/Thread.h"
#include "Foundation/Log.h"
#include "Foundation/Container/Insert.h"

#include <io.h>

// Prints the callstack for every init and cleanup call
// #define REFLECT_DEBUG_INIT_AND_CLEANUP

using Helium::Insert; 

using namespace Helium;
using namespace Helium::Reflect;

// profile interface
#ifdef PROFILE_ACCUMULATION
Profile::Accumulator Reflect::g_CloneAccum ( "Reflect Clone" );
Profile::Accumulator Reflect::g_ParseAccum ( "Reflect Parse" );
Profile::Accumulator Reflect::g_AuthorAccum ( "Reflect Author" );
Profile::Accumulator Reflect::g_ChecksumAccum ( "Reflect Checksum" );
Profile::Accumulator Reflect::g_PreSerializeAccum ( "Reflect Serialize Pre-Process" );
Profile::Accumulator Reflect::g_PostSerializeAccum ( "Reflect Serialize Post-Process" );
Profile::Accumulator Reflect::g_PreDeserializeAccum ( "Reflect Deserialize Pre-Process" );
Profile::Accumulator Reflect::g_PostDeserializeAccum ( "Reflect Deserialize Post-Process" );
#endif

template <class T>
struct CaseInsensitiveCompare
{
    const tstring& value;

    CaseInsensitiveCompare(const tstring& str)
        : value (str)
    {

    }

    bool operator()(const std::pair<const tstring, T> &rhs)
    {
        return _tcsicmp(rhs.first.c_str(), value.c_str()) == 0;
    }
};

namespace Helium
{
    namespace Reflect
    {
        int32_t         g_InitCount = 0;
        Registry*   g_Registry = NULL;
    }
}

bool Reflect::IsInitialized()
{
    return g_Registry != NULL;
}

void Reflect::Initialize()
{
    if (++g_InitCount == 1)
    {
        g_Registry = new Registry();

        // Bases
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Object>( TXT( "Object" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Element>( TXT( "Element" ) ) );

        // Datas
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Data>( TXT( "Data" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<ContainerData>( TXT( "Container" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<ElementContainerData>( TXT( "ElementContainer" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TypeIDData>( TXT( "TypeID" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<PointerData>( TXT( "Pointer" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<EnumerationData>( TXT( "Enumeration" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<BitfieldData>( TXT( "Bitfield" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<PathData>( TXT( "Path" ) ) );

        // SimpleData
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StringData>( TXT( "String" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<BoolData>( TXT( "Bool" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U8Data>( TXT( "U8" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I8Data>( TXT( "I8" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U16Data>( TXT( "U16" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I16Data>( TXT( "I16" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U32Data>( TXT( "U32" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I32Data>( TXT( "I32" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U64Data>( TXT( "U64" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I64Data>( TXT( "I64" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<F32Data>( TXT( "F32" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<F64Data>( TXT( "F64" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<GUIDData>( TXT( "GUID" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TUIDData>( TXT( "TUID" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Vector2Data>( TXT( "Vector2" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Vector3Data>( TXT( "Vector3" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Vector4Data>( TXT( "Vector4" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Matrix3Data>( TXT( "Matrix3" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Matrix4Data>( TXT( "Matrix4" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Color3Data>( TXT( "Color3" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Color4Data>( TXT( "Color4" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<HDRColor3Data>( TXT( "HDRColor3" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<HDRColor4Data>( TXT( "HDRColor4" ) ) );

        // StlVectorData
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlVectorData>( TXT( "StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StringStlVectorData>( TXT( "StringStlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<BoolStlVectorData>( TXT( "BoolStlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U8StlVectorData>( TXT( "U8StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I8StlVectorData>( TXT( "I8StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U16StlVectorData>( TXT( "U16StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I16StlVectorData>( TXT( "I16StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U32StlVectorData>( TXT( "U32StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I32StlVectorData>( TXT( "I32StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U64StlVectorData>( TXT( "U64StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I64StlVectorData>( TXT( "I64StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<F32StlVectorData>( TXT( "F32StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<F64StlVectorData>( TXT( "F64StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<GUIDStlVectorData>( TXT( "GUIDStlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TUIDStlVectorData>( TXT( "TUIDStlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<PathStlVectorData>( TXT( "PathStlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Vector2StlVectorData>( TXT( "Vector2StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Vector3StlVectorData>( TXT( "Vector3StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Vector4StlVectorData>( TXT( "Vector4StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Matrix3StlVectorData>( TXT( "Matrix3StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Matrix4StlVectorData>( TXT( "Matrix4StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Color3StlVectorData>( TXT( "Color3StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Color4StlVectorData>( TXT( "Color4StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<HDRColor3StlVectorData>( TXT( "HDRColor3StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<HDRColor4StlVectorData>( TXT( "HDRColor4StlVector" ) ) );

        // StlSetData
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlSetData>( TXT( "StlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StringStlSetData>( TXT( "StrStlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U32StlSetData>( TXT( "U32StlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U64StlSetData>( TXT( "U64StlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<F32StlSetData>( TXT( "F32StlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<GUIDStlSetData>( TXT( "GUIDStlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TUIDStlSetData>( TXT( "TUIDStlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType< PathStlSetData>( TXT( "PathStlSet" ) ) );

        // StlMapData
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlMapData>( TXT( "StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StringStringStlMapData>( TXT( "StrStrStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StringBoolStlMapData>( TXT( "StrBoolStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StringU32StlMapData>( TXT( "StrU32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StringI32StlMapData>( TXT( "StrI32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U32StringStlMapData>( TXT( "U32StrStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U32U32StlMapData>( TXT( "U32U32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U32I32StlMapData>( TXT( "U32I32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U32U64StlMapData>( TXT( "U32U64StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I32StringStlMapData>( TXT( "I32StrStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I32U32StlMapData>( TXT( "I32U32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I32I32StlMapData>( TXT( "I32I32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I32U64StlMapData>( TXT( "I32U64StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U64StringStlMapData>( TXT( "U64StrStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U64U32StlMapData>( TXT( "U64U32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U64U64StlMapData>( TXT( "U64U64StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U64Matrix4StlMapData>( TXT( "U64Matrix4StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<GUIDU32StlMapData>( TXT( "GUIDU32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<GUIDMatrix4StlMapData>( TXT( "GUIDMatrix4StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TUIDU32StlMapData>( TXT( "TUIDU32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TUIDMatrix4StlMapData>( TXT( "TUIDMatrix4StlMap" ) ) );

        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<ElementStlVectorData>( TXT( "ElementStlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<ElementStlSetData>( TXT( "ElementStlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<ElementStlMapData>( TXT( "ElementStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TypeIDElementStlMapData>( TXT( "TypeIDElementStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StringElementStlMapData>( TXT( "StringElementStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U32ElementStlMapData>( TXT( "U32ElementStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I32ElementStlMapData>( TXT( "I32ElementStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<U64ElementStlMapData>( TXT( "U64ElementStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<I64ElementStlMapData>( TXT( "I64ElementStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<GUIDElementStlMapData>( TXT( "GUIDElementStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TUIDElementStlMapData>( TXT( "TUIDElementStlMap" ) ) );

        //
        // Build Casting Table
        //

        Data::Initialize();

        //
        // Register Elements
        //

        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Version>( TXT( "Version" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<DocumentNode>( TXT("DocumentNode") ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<DocumentAttribute>( TXT("DocumentAttribute") ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<DocumentElement>( TXT("DocumentElement") ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Document>( TXT("Document") ) );
    }

#ifdef REFLECT_DEBUG_INIT_AND_CLEANUP
    std::vector<uintptr_t> trace;
    Debug::GetStackTrace( trace );

    std::string str;
    Debug::TranslateStackTrace( trace, str );

    Log::Print( "\n" );
    Log::Print("%d\n\n%s\n", g_InitCount, str.c_str() );
#endif
}

void Reflect::Cleanup()
{
    if ( --g_InitCount == 0 )
    {
        // free our casting memory
        Data::Cleanup();

        // delete registry
        delete g_Registry;
        g_Registry = NULL;
    }

#ifdef REFLECT_DEBUG_INIT_AND_CLEANUP
    std::vector<uintptr_t> trace;
    Debug::GetStackTrace( trace );

    std::string str;
    Debug::TranslateStackTrace( trace, str );

    Log::Print( "\n" );
    Log::Print("%d\n\n%s\n", g_InitCount, str.c_str() );
#endif
}

Profile::MemoryPoolHandle g_MemoryPool;

Profile::MemoryPoolHandle Reflect::MemoryPool()
{
    return g_MemoryPool;
}

// private constructor
Registry::Registry()
: m_Created (NULL)
, m_Destroyed (NULL)
{
    if ( Profile::Settings::MemoryProfilingEnabled() )
    {
        g_MemoryPool = Profile::Memory::CreatePool( TXT( "Reflect Objects" ) );
    }
}

Registry::~Registry()
{
    m_TypesByID.clear();
    m_TypesByName.clear();
    m_TypesByAlias.clear();

    m_Created = NULL;
    m_Destroyed = NULL;
}

Registry* Registry::GetInstance()
{
    HELIUM_ASSERT(g_Registry != NULL);
    return g_Registry;
}

bool Registry::RegisterType(Type* type)
{
    HELIUM_ASSERT( IsMainThread() );

    switch (type->GetReflectionType())
    {
    case ReflectionTypes::Class:
        {
            Class* classType = static_cast<Class*>(type);

            Insert<M_IDToType>::Result idResult = m_TypesByID.insert(M_IDToType::value_type (classType->m_TypeID, classType));

            if (idResult.second)
            {
                m_TypesByName.insert(M_StrToType::value_type (classType->m_Name, classType));

                if ( !classType->m_Name.empty() )
                {
                    Insert<M_StrToType>::Result nameResult = m_TypesByName.insert(M_StrToType::value_type (classType->m_Name, classType));

                    if (!nameResult.second && classType != nameResult.first->second)
                    {
                        Log::Error( TXT( "Re-registration of short name '%s' was attempted with different classType information\n" ), classType->m_Name.c_str());
                        HELIUM_BREAK();
                        return false;
                    }
                }

                if ( !classType->m_Base.empty() )
                {
                    M_StrToType::const_iterator found = m_TypesByName.find( classType->m_Base );
                    if (found != m_TypesByName.end())
                    {
                        Type* baseClass = found->second;
                        if (baseClass->GetReflectionType() == ReflectionTypes::Class)
                        {
                            static_cast<Class*>(baseClass)->m_Derived.insert( classType->m_Name );
                        }
                        else
                        {
                            Log::Error( TXT( "Base class of '%s' is not a valid type\n" ), classType->m_Name.c_str());
                            HELIUM_BREAK();
                            return false;
                        }
                    }
                }

                classType->Report();
            }
            else if (classType != idResult.first->second)
            {
                Log::Error( TXT( "Re-registration of classType '%s' was attempted with different classType information\n" ), classType->m_Name.c_str());
                HELIUM_BREAK();
                return false;
            }
            break;
        }

    case ReflectionTypes::Enumeration:
        {
            Enumeration* enumeration = static_cast<Enumeration*>(type);

            Insert<M_IDToType>::Result idResult = m_TypesByID.insert(M_IDToType::value_type (enumeration->m_TypeID, enumeration));

            if (idResult.second)
            {
                Insert<M_StrToType>::Result enumResult = m_TypesByName.insert(M_StrToType::value_type (enumeration->m_Name, enumeration));

                if (!enumResult.second && enumeration != enumResult.first->second)
                {
                    Log::Error( TXT( "Re-registration of enumeration '%s' was attempted with different type information\n" ), enumeration->m_Name.c_str());
                    HELIUM_BREAK();
                    return false;
                }

                enumResult = m_TypesByName.insert(M_StrToType::value_type (enumeration->m_Name, enumeration));

                if (!enumResult.second && enumeration != enumResult.first->second)
                {
                    Log::Error( TXT( "Re-registration of enumeration '%s' was attempted with different type information\n" ), enumeration->m_Name.c_str());
                    HELIUM_BREAK();
                    return false;
                }
            }
            else if (enumeration != idResult.first->second)
            {
                Log::Error( TXT( "Re-registration of enumeration '%s' was attempted with different type information\n" ), enumeration->m_Name.c_str());
                HELIUM_BREAK();
                return false;
            }

            break;
        }
    }

    return true;
}

void Registry::UnregisterType(const Type* type)
{
    HELIUM_ASSERT( IsMainThread() );

    switch (type->GetReflectionType())
    {
    case ReflectionTypes::Class:
        {
            const Class* classType = static_cast<const Class*>(type);

            if ( !classType->m_Name.empty() )
            {
                m_TypesByName.erase( classType->m_Name );
            }

            if ( !classType->m_Base.empty() )
            {
                M_StrToType::const_iterator found = m_TypesByName.find( classType->m_Base );
                if ( found != m_TypesByName.end() )
                {
                    if ( found->second->GetReflectionType() == ReflectionTypes::Class )
                    {
                        static_cast<Class*>(found->second.Ptr())->m_Derived.erase( classType->m_Name );
                    }
                    else
                    {
                        Log::Error( TXT( "Base class of '%s' is not a valid type\n" ), classType->m_Name.c_str());
                        HELIUM_BREAK();
                    }
                }
            }

            m_TypesByName.erase(classType->m_Name);
            m_TypesByID.erase(classType->m_TypeID);

            break;
        }

    case ReflectionTypes::Enumeration:
        {
            const Enumeration* enumeration = static_cast<const Enumeration*>(type);

            m_TypesByName.erase(enumeration->m_Name);
            m_TypesByName.erase(enumeration->m_Name);
        }
    }
}

void Registry::AliasType(const Type* type, const tstring& alias)
{
    HELIUM_ASSERT( IsMainThread() );

    m_TypesByAlias.insert(M_StrToType::value_type (alias, type));
}

void Registry::UnAliasType(const Type* type, const tstring& alias)
{
    HELIUM_ASSERT( IsMainThread() );

    M_StrToType::iterator found = m_TypesByAlias.find( alias );
    if (found != m_TypesByAlias.end() && found->second == type)
    {
        m_TypesByAlias.erase(found);
    }
}

const Type* Registry::GetType(int id) const
{
    M_IDToType::const_iterator found = m_TypesByID.find (id);

    if (found != m_TypesByID.end())
    {
        return found->second;
    }
    else
    {
        return NULL;
    }
}

const Type* Registry::GetType(const tstring& str) const
{
    M_StrToType::const_iterator found = m_TypesByAlias.find(str);

    if (found != m_TypesByAlias.end())
    {
        return found->second;
    }

    found = m_TypesByName.find(str);

    if (found != m_TypesByName.end())
    {
        return found->second;
    }

    found = std::find_if(m_TypesByName.begin(), m_TypesByName.end(), CaseInsensitiveCompare<Type*>(str));

    if (found != m_TypesByName.end())
    {
        return found->second;
    }

    return NULL;
}

const Class* Registry::GetClass(int32_t id) const
{
    return ReflectionCast<const Class>(GetType( id ));
}

const Class* Registry::GetClass(const tstring& str) const
{
    return ReflectionCast<const Class>(GetType( str ));
}

const Enumeration* Registry::GetEnumeration(int32_t id) const
{
    return ReflectionCast<const Enumeration>(GetType( id ));
}

const Enumeration* Registry::GetEnumeration(const tstring& str) const
{
    return ReflectionCast<const Enumeration>(GetType( str ));
}

ObjectPtr Registry::CreateInstance(int id) const
{
    M_IDToType::const_iterator type = m_TypesByID.find(id);

    if (type != m_TypesByID.end() && type->second->GetReflectionType() == ReflectionTypes::Class)
    {
        const Class* cls = ReflectionCast<const Class>(type->second);
        HELIUM_ASSERT( cls->m_Creator );
        if ( cls->m_Creator )
        {
            return cls->m_Creator();
        }
        else
        {
            throw Reflect::TypeInformationException( TXT( "Class '%s' cannot be instanced, it is abstract" ), cls->m_Name.c_str() );
        }
    }
    else
    {
        return NULL;
    }
}

ObjectPtr Registry::CreateInstance(const Class* type) const
{
    if (type && type->m_Creator)
    {
        return type->m_Creator();
    }
    else
    {
        return NULL;
    }
}

ObjectPtr Registry::CreateInstance(const tstring& str) const
{
    M_StrToType::const_iterator type = m_TypesByName.find(str);

    if (type == m_TypesByName.end())
        return NULL;

    if (type->second->GetReflectionType() != ReflectionTypes::Class)
        return NULL;

    return CreateInstance(static_cast<Class*>(type->second.Ptr()));
}

void Registry::Created(Object* object)
{
#ifdef REFLECT_OBJECT_TRACKING
    Registry::GetInstance()->TrackCreate((uintptr_t)object);
#endif

    if (m_Created != NULL)
    {
        m_Created(object);
    }
}

void Registry::Destroyed(Object* object)
{
#ifdef REFLECT_OBJECT_TRACKING
    Registry::GetInstance()->TrackDelete((uintptr_t)object);
#endif

    if (m_Destroyed != NULL)
    {
        m_Destroyed(object);
    }
}

void Registry::SetCreatedCallback(CreatedFunc created)
{
    m_Created = created;
}

void Registry::SetDestroyedCallback(DestroyedFunc destroyed)
{
    m_Destroyed = destroyed;
}

#ifdef REFLECT_OBJECT_TRACKING

void Registry::TrackCreate(uintptr_t ptr)
{
    m_Tracker.Create( ptr );
}

void Registry::TrackDelete(uintptr_t ptr)
{
    m_Tracker.Delete( ptr );
}

void Registry::TrackCheck(uintptr_t ptr)
{
    m_Tracker.Check( ptr );
}

void Registry::TrackDump()
{
    m_Tracker.Dump();
}

const tstring& StackRecord::Convert()
{
    if ( !m_Converted )
    {
        m_Converted = true;

        Debug::TranslateStackTrace( m_Stack, m_String );
    }

    return m_String;
}

CreationRecord::CreationRecord()
: m_Address (0x0)
, m_Type (-1)
{

}

CreationRecord::CreationRecord(uintptr_t ptr)
: m_Address (ptr)
, m_Type (-1)
{

}

void CreationRecord::Dump(FILE* f)
{
    _ftprintf(f, TXT("\n\n"));
    _ftprintf(f, TXT("Addr: %p\n"), m_Address);
    _ftprintf(f, TXT("Name: %s\n"), m_Name.c_str());
    _ftprintf(f, TXT("Type: %i\n"), m_Type);

#ifdef REFLECT_OBJECT_STACK_TRACKING
    _ftprintf(f, TXT("Create Stack:\n%s\n"), m_CreateStack.ReferencesObject() ? m_CreateStack->Convert().c_str() : TXT("<none>") );
    _ftprintf(f, TXT("Delete Stack:\n%s\n"), m_DeleteStack.ReferencesObject() ? m_DeleteStack->Convert().c_str() : TXT("<none>") );
#endif
}

Helium::Mutex g_TrackerMutex;

Tracker::Tracker()
{

}

Tracker::~Tracker()
{
    Dump();
}

StackRecordPtr Tracker::GetStack()
{
    Helium::MutexScopeLock mutex (g_TrackerMutex);

    StackRecordPtr ptr = new StackRecord();

    Debug::GetStackTrace( ptr->m_Stack );

    M_StackRecord::iterator iter = m_Stacks.find( ptr->m_Stack );
    if ( iter != m_Stacks.end() )
    {
        ptr = (*iter).second;
    }
    else
    {
        m_Stacks.insert( M_StackRecord::value_type( ptr->m_Stack, ptr ) );
    }

    return ptr;
}

void Tracker::Create(uintptr_t ptr)
{
    Helium::MutexScopeLock mutex (g_TrackerMutex);

    M_CreationRecord::iterator create_iter = m_CreatedObjects.find( ptr );
    if ( create_iter == m_CreatedObjects.end() )
    {
#ifdef REFLECT_OBJECT_VERBOSE_TRACKING
        fprintf(stderr, "%p: CREATE\n", ptr);
#endif

        CreationRecord cr (ptr);

#ifdef REFLECT_OBJECT_STACK_TRACKING
        cr.m_CreateStack = GetStack();
#endif

        // insert into the map of objects that are currently created
        create_iter = m_CreatedObjects.insert( M_CreationRecord::value_type( ptr, cr ) ).first;
        HELIUM_ASSERT( create_iter != m_CreatedObjects.end() );

        // potentially cleanup an old deletion for this pointer
        M_CreationRecord::iterator delete_iter = m_DeletedObjects.find( ptr );
        if ( delete_iter != m_DeletedObjects.end() )
        {
            m_DeletedObjects.erase( delete_iter );
        }
    }
    else
    {
        // why are we getting two creates w/o a delete?
        HELIUM_BREAK();
        (*create_iter).second.Dump( stderr );
    }
}

void Tracker::Delete(uintptr_t ptr)
{
    Helium::MutexScopeLock mutex (g_TrackerMutex);

    M_CreationRecord::iterator iter = m_CreatedObjects.find(ptr);
    if ( iter != m_CreatedObjects.end())
    {
        CreationRecord& cr = (*iter).second;

#ifdef REFLECT_OBJECT_VERBOSE_TRACKING
        fprintf(stderr, "%p: DELETE", ptr);
        if (!cr.m_Name.empty())
        {
            fprintf(stderr, ": %s\n", cr.m_Name.c_str());
        }
        else
        {
            fprintf(stderr, "\n");
        }
#endif

#ifdef REFLECT_OBJECT_STACK_TRACKING
        cr.m_DeleteStack = GetStack();
#endif

        // insert into the map of objects which are currently deleted
        M_CreationRecord::iterator delete_iter = 
            m_DeletedObjects.insert( M_CreationRecord::value_type( ptr, cr ) ).first;
        HELIUM_ASSERT( delete_iter != m_DeletedObjects.end() );

        // erase the entry from objects which are currently created
        m_CreatedObjects.erase(iter);
    }
    else
    {
        // we should have a creation record for everything that gets deleted
        HELIUM_BREAK();
    }
}

void Tracker::Check(uintptr_t ptr)
{
    Helium::MutexScopeLock mutex (g_TrackerMutex);

    M_CreationRecord::iterator iter = m_CreatedObjects.find(ptr);
    if ( iter != m_CreatedObjects.end())
    {
        CreationRecord& cr = (*iter).second;

        if (cr.m_Type < 0)
        {
            Element* e = reinterpret_cast<Element*>(ptr);

            cr.m_Name = e->GetClass()->m_Name;
            cr.m_Type = e->GetType();

#ifdef REFLECT_OBJECT_VERBOSE_TRACKING
            fprintf(stderr, "%p: TRACKING", ptr);
            fprintf(stderr, ": %s \n", cr.m_Name.c_str());
#endif
        }
    }
    else 
    {
        // uh oh, no so cool, take dump!
        iter = m_DeletedObjects.find(ptr);
        if ( iter != m_DeletedObjects.end() )
        {
            (*iter).second.Dump( stderr );
        }

        // why are we checking something that isn't created???
        HELIUM_BREAK();
    }
}

void Tracker::Dump()
{
    Helium::MutexScopeLock mutex (g_TrackerMutex);

    tchar_t module[MAX_PATH];
    GetModuleFileName( 0, module, MAX_PATH );

    tchar_t drive[MAX_PATH];
    tchar_t dir[MAX_PATH];
    tchar_t name[MAX_PATH];
    _tsplitpath( module, drive, dir, name, NULL );

    tstring path = drive;
    path += dir;
    path += name;

    FILE* f = _tfopen( (path + TXT("ReflectDump.log")).c_str(), TXT("w") );
    if ( f != NULL )
    {
        typedef std::map< uint32_t, std::pair< tstring, uint32_t > > ObjectLogger;
        ObjectLogger objectLog;

        {
            M_CreationRecord::iterator c_current = m_CreatedObjects.begin();
            M_CreationRecord::iterator c_end = m_CreatedObjects.end();

            for ( ; c_current != c_end; ++c_current)
            {
                (*c_current).second.Dump( f );

                ObjectLogger::iterator iter = objectLog.find( (*c_current).second.m_Type );
                if ( iter == objectLog.end() )
                {
                    objectLog.insert( ObjectLogger::value_type( (*c_current).second.m_Type, std::pair< tstring, uint32_t >( (*c_current).second.m_Name, 1 ) ) );
                }
                else
                {
                    (*iter).second.second++;
                } 
            }
        }

        {
            M_CreationRecord::iterator d_current = m_DeletedObjects.begin();
            M_CreationRecord::iterator d_end = m_DeletedObjects.end();

            for ( ; d_current != d_end; ++d_current)
            {
                ObjectLogger::iterator iter = objectLog.find( (*d_current).second.m_Type );
                if ( iter == objectLog.end() )
                {
                    objectLog.insert( ObjectLogger::value_type( (*d_current).second.m_Type, std::pair< tstring, uint32_t >( (*d_current).second.m_Name, 1 ) ) );
                }
                else
                {
                    (*iter).second.second++;
                }
            }
        }

        {
            size_t max = 0;
            ObjectLogger::iterator iter = objectLog.begin();  
            ObjectLogger::iterator end = objectLog.end();  

            for ( ; iter != end; ++iter )
            {
                max = std::max( (*iter).second.first.length(), max );
            }

            tchar_t format[1024];
            _stprintf( format, TXT("\nType: %%%ds, Count: %%d"), max );

            iter = objectLog.begin();
            for ( ; iter != end; ++iter )
            {
                _ftprintf( f, format, (*iter).second.first.c_str(), (*iter).second.second );
            }
        }

        fclose(f);
    }
}

#endif
