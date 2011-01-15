#include "Foundation/Reflect/Registry.h"

#include "Foundation/Log.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/Reflect/Enumeration.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/DOM.h"

#include "Platform/Atomic.h"
#include "Platform/Thread.h"

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
    const tchar_t* value;

    CaseInsensitiveNameCompare( const tchar_t* name )
        : value( name )
    {

    }

    bool operator()( const KeyValue< const tchar_t*, T >& rhs )
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

        // Base
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Object>( TXT( "Object" ) ) );

        // Data
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Data>( TXT( "Data" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<ContainerData>( TXT( "Container" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TypeIDData>( TXT( "TypeID" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<PointerData>( TXT( "Pointer" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<EnumerationData>( TXT( "Enumeration" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<BitfieldData>( TXT( "Bitfield" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<PathData>( TXT( "Path" ) ) );

        // SimpleData
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlStringData>( TXT( "StlString" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<BoolData>( TXT( "Bool" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt8Data>( TXT( "UInt8" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int8Data>( TXT( "Int8" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt16Data>( TXT( "UInt16" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int16Data>( TXT( "Int16" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt32Data>( TXT( "UInt32" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int32Data>( TXT( "Int32" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt64Data>( TXT( "UInt64" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int64Data>( TXT( "Int64" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Float32Data>( TXT( "Float32" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Float64Data>( TXT( "Float64" ) ) );
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
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlStringStlVectorData>( TXT( "StlStringStlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<BoolStlVectorData>( TXT( "BoolStlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt8StlVectorData>( TXT( "UInt8StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int8StlVectorData>( TXT( "Int8StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt16StlVectorData>( TXT( "UInt16StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int16StlVectorData>( TXT( "Int16StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt32StlVectorData>( TXT( "UInt32StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int32StlVectorData>( TXT( "Int32StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt64StlVectorData>( TXT( "UInt64StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int64StlVectorData>( TXT( "Int64StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Float32StlVectorData>( TXT( "Float32StlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Float64StlVectorData>( TXT( "Float64StlVector" ) ) );
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
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlStringStlSetData>( TXT( "StlStringStlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt32StlSetData>( TXT( "UInt32StlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt64StlSetData>( TXT( "UInt64StlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<GUIDStlSetData>( TXT( "GUIDStlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TUIDStlSetData>( TXT( "TUIDStlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<PathStlSetData>( TXT( "PathStlSet" ) ) );

        // StlMapData
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlMapData>( TXT( "StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlStringStlStringStlMapData>( TXT( "StlStringStringStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlStringBoolStlMapData>( TXT( "StlStringBoolStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlStringUInt32StlMapData>( TXT( "StlStringUInt32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlStringInt32StlMapData>( TXT( "StlStringInt32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt32StringStlMapData>( TXT( "UInt32StringStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt32UInt32StlMapData>( TXT( "UInt32UInt32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt32Int32StlMapData>( TXT( "UInt32Int32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt32UInt64StlMapData>( TXT( "UInt32UInt64StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int32StringStlMapData>( TXT( "Int32StringStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int32UInt32StlMapData>( TXT( "Int32UInt32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int32Int32StlMapData>( TXT( "Int32Int32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int32UInt64StlMapData>( TXT( "Int32UInt64StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt64StringStlMapData>( TXT( "UInt64StringStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt64UInt32StlMapData>( TXT( "UInt64UInt32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt64UInt64StlMapData>( TXT( "UInt64UInt64StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt64Matrix4StlMapData>( TXT( "UInt64Matrix4StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<GUIDUInt32StlMapData>( TXT( "GUIDUInt32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<GUIDMatrix4StlMapData>( TXT( "GUIDMatrix4StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TUIDUInt32StlMapData>( TXT( "TUIDUInt32StlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TUIDMatrix4StlMapData>( TXT( "TUIDMatrix4StlMap" ) ) );

        // DynArrayData
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<DynArrayData>( TXT( "DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlStringDynArrayData>( TXT( "StlStringDynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<BoolDynArrayData>( TXT( "BoolDynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt8DynArrayData>( TXT( "UInt8DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int8DynArrayData>( TXT( "Int8DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt16DynArrayData>( TXT( "UInt16DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int16DynArrayData>( TXT( "Int16DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt32DynArrayData>( TXT( "UInt32DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int32DynArrayData>( TXT( "Int32DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt64DynArrayData>( TXT( "UInt64DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int64DynArrayData>( TXT( "Int64DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Float32DynArrayData>( TXT( "Float32DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Float64DynArrayData>( TXT( "Float64DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<GUIDDynArrayData>( TXT( "GUIDDynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TUIDDynArrayData>( TXT( "TUIDDynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<PathDynArrayData>( TXT( "PathDynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Vector2DynArrayData>( TXT( "Vector2DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Vector3DynArrayData>( TXT( "Vector3DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Vector4DynArrayData>( TXT( "Vector4DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Matrix3DynArrayData>( TXT( "Matrix3DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Matrix4DynArrayData>( TXT( "Matrix4DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Color3DynArrayData>( TXT( "Color3DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Color4DynArrayData>( TXT( "Color4DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<HDRColor3DynArrayData>( TXT( "HDRColor3DynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<HDRColor4DynArrayData>( TXT( "HDRColor4DynArray" ) ) );

        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<ObjectStlVectorData>( TXT( "ObjectStlVector" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<ObjectStlSetData>( TXT( "ObjectStlSet" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<ObjectStlMapData>( TXT( "ObjectStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<ObjectDynArrayData>( TXT( "ObjectDynArray" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TypeIDObjectStlMapData>( TXT( "TypeIDObjectStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<StlStringObjectStlMapData>( TXT( "StlStringObjectStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt32ObjectStlMapData>( TXT( "UInt32ObjectStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int32ObjectStlMapData>( TXT( "Int32ObjectStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<UInt64ObjectStlMapData>( TXT( "UInt64ObjectStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Int64ObjectStlMapData>( TXT( "Int64ObjectStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<GUIDObjectStlMapData>( TXT( "GUIDObjectStlMap" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<TUIDObjectStlMapData>( TXT( "TUIDObjectStlMap" ) ) );

        //
        // Build Casting Table
        //

        Data::Initialize();

        //
        // Register Objects
        //

        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Version>( TXT( "Version" ) ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<DocumentNode>( TXT( "DocumentNode") ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<DocumentAttribute>( TXT( "DocumentAttribute") ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<DocumentObject>( TXT( "DocumentObject") ) );
        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Document>( TXT( "Document") ) );
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

        // free basic types
        g_Registry->m_InitializerStack.Cleanup();

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
{
    if ( Profile::Settings::MemoryProfilingEnabled() )
    {
        g_MemoryPool = Profile::Memory::CreatePool( TXT( "Reflect Objects" ) );
    }
}

Registry::~Registry()
{
    m_TypesByHash.Clear();
}

Registry* Registry::GetInstance()
{
    HELIUM_ASSERT(g_Registry != NULL);
    return g_Registry;
}

bool Registry::RegisterType(Type* type)
{
    HELIUM_ASSERT( IsMainThread() );

    uint32_t crc = Crc32( type->m_Name );
    Insert< M_HashToType >::Result result = m_TypesByHash.Insert( M_HashToType::ValueType( crc, type ) );
    if ( !result.Second() )
    {
        Log::Error( TXT( "Re-registration of type %s, could be ambigouous crc: 0x%08x\n" ), type->m_Name, crc );
        HELIUM_BREAK();
        return false;
    }

    type->Report();
    return true;
}

void Registry::UnregisterType(const Type* type)
{
    HELIUM_ASSERT( IsMainThread() );

    type->Unregister();

    uint32_t crc = Crc32( type->m_Name );
    m_TypesByHash.Remove( crc );
}

void Registry::AliasType( const Type* type, const tchar_t* alias )
{
    HELIUM_ASSERT( IsMainThread() );

    uint32_t crc = Crc32( alias );
    m_TypesByHash.Insert( M_HashToType::ValueType( crc, type ) );
}

void Registry::UnaliasType( const Type* type, const tchar_t* alias )
{
    HELIUM_ASSERT( IsMainThread() );

    uint32_t crc = Crc32( alias );
    M_HashToType::Iterator found = m_TypesByHash.Find( crc );
    if ( found != m_TypesByHash.End() && found->Second() == type )
    {
        m_TypesByHash.Remove( crc );
    }
}

const Type* Registry::GetType( uint32_t crc ) const
{
    M_HashToType::ConstIterator found = m_TypesByHash.Find( crc );

    if ( found != m_TypesByHash.End() )
    {
        return found->Second();
    }

    return NULL;
}

const Class* Registry::GetClass( uint32_t crc ) const
{
    return ReflectionCast< const Class >( GetType( crc ) );
}

const Enumeration* Registry::GetEnumeration( uint32_t crc ) const
{
    return ReflectionCast< const Enumeration >( GetType( crc ) );
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

ObjectPtr Registry::CreateInstance( uint32_t crc ) const
{
    M_HashToType::ConstIterator found = m_TypesByHash.Find( crc );

    if ( found != m_TypesByHash.End() )
    {
        const Class* type = ReflectionCast< const Class >( found->Second() );
        if ( type )
        {
            return CreateInstance( type );
        }
    }

    return NULL;
}
