#include "Registry.h"
#include "Version.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "DOM.h"

#include "Platform/Atomic.h"
#include "Platform/Thread.h"
#include "Foundation/Log.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/Reflect/ObjectType.h"

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

template< class T >
struct CaseInsensitiveCompare
{
    const tstring& value;

    CaseInsensitiveCompare( const tstring& str )
        : value( str )
    {

    }

    bool operator()( const std::pair< const tstring, T >& rhs )
    {
        return _tcsicmp( rhs.first.c_str(), value.c_str() ) == 0;
    }
};

template< class T >
struct CaseInsensitiveNameCompare
{
    Name value;

    CaseInsensitiveNameCompare( Name name )
        : value( name )
    {

    }

    bool operator()( const KeyValue< Name, T >& rhs )
    {
        return _tcsicmp( *rhs.First(), *value ) == 0;
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
    m_TypesByName.Clear();
    m_TypesByAlias.Clear();

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

            Insert< M_NameToType >::Result nameResult = m_TypesByName.Insert( M_NameToType::ValueType( classType->m_Name, classType ) );

            if ( !nameResult.Second() )
            {
                Log::Error( TXT( "Re-registration of class '%s'\n" ), *classType->m_Name );
                HELIUM_BREAK();
                return false;
            }

            classType->Report();
            break;
        }

    case ReflectionTypes::ObjectType:
    case ReflectionTypes::GameObjectType:
        {
            ObjectType* objectType = static_cast< ObjectType* >( type );

            Insert< M_NameToType >::Result objectTypeResult = m_TypesByName.Insert( M_NameToType::ValueType( objectType->m_Name, objectType ) );

            if( !objectTypeResult.Second() )
            {
                Log::Error( TXT( "Re-registration of object type '%s'\n" ), *objectType->m_Name );
                HELIUM_BREAK();
                return false;
            }

            break;
        }

    case ReflectionTypes::Enumeration:
        {
            Enumeration* enumeration = static_cast<Enumeration*>(type);

            Insert< M_NameToType >::Result enumResult = m_TypesByName.Insert( M_NameToType::ValueType( enumeration->m_Name, enumeration ) );

            if ( !enumResult.Second() )
            {
                Log::Error( TXT( "Re-registration of enumeration '%s'\n" ), *enumeration->m_Name );
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

    type->Unregister();

    m_TypesByName.Remove( type->m_Name );
}

void Registry::AliasType( const Type* type, Name alias )
{
    HELIUM_ASSERT( IsMainThread() );

    m_TypesByAlias.Insert( M_NameToType::ValueType( alias, type ) );
}

void Registry::UnAliasType( const Type* type, Name alias )
{
    HELIUM_ASSERT( IsMainThread() );

    M_NameToType::Iterator found = m_TypesByAlias.Find( alias );
    if ( found != m_TypesByAlias.End() && found->Second() == type )
    {
        m_TypesByAlias.Remove( found );
    }
}

const Type* Registry::GetType( Name name ) const
{
    M_NameToType::ConstIterator found = m_TypesByAlias.Find( name );

    if ( found != m_TypesByAlias.End() )
    {
        return found->Second();
    }

    found = m_TypesByName.Find( name );

    if ( found != m_TypesByName.End() )
    {
        return found->Second();
    }

    found = std::find_if( m_TypesByName.Begin(), m_TypesByName.End(), CaseInsensitiveNameCompare< SmartPtr< Type > >( name ) );

    if ( found != m_TypesByName.End() )
    {
        return found->Second();
    }

    return NULL;
}

const Class* Registry::GetClass( Name name ) const
{
    return ReflectionCast< const Class >( GetType( name ) );
}

const Enumeration* Registry::GetEnumeration( Name name ) const
{
    return ReflectionCast< const Enumeration >( GetType( name ) );
}

ObjectPtr Registry::CreateInstance( const Class* type ) const
{
    if ( type && type->m_Creator )
    {
        return type->m_Creator();
    }
    else
    {
        return NULL;
    }
}

ObjectPtr Registry::CreateInstance( Name name ) const
{
    M_NameToType::ConstIterator type = m_TypesByName.Find( name );

    if ( type == m_TypesByName.End() )
        return NULL;

    if ( type->Second()->GetReflectionType() != ReflectionTypes::Class )
        return NULL;

    return CreateInstance( static_cast< Class* >( type->Second().Ptr() ) );
}

void Registry::Created(Object* object)
{
    if (m_Created != NULL)
    {
        m_Created(object);
    }
}

void Registry::Destroyed(Object* object)
{
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
