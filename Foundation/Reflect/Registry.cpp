#include "Registry.h"
#include "Version.h"
#include "Serializers.h"
#include "DOM.h"

#ifdef REFLECT_OBJECT_TRACKING
# include "Platform/Mutex.h"
# include "Platform/Windows/Debug.h"
#endif

#include "Platform/Atomic.h"
#include "Platform/Windows/Windows.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/Log.h"

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
        i32         g_InitCount = 0;
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
        HELIUM_ASSERT(g_Registry != NULL);

        //
        // Serializers
        //

        g_Registry->RegisterType(Class::Create<Object>("", TXT( "Object" ) ));
        g_Registry->RegisterType(Element::CreateClass( TXT( "Element" ) ));
        g_Registry->RegisterType(Serializer::CreateClass( TXT( "Serializer" ) ));
        g_Registry->RegisterType(ContainerSerializer::CreateClass( TXT( "Container" ) ));
        g_Registry->RegisterType(ElementContainerSerializer::CreateClass( TXT( "ElementContainer" ) ));
        g_Registry->RegisterType(TypeIDSerializer::CreateClass( TXT( "TypeID" ) ));
        g_Registry->RegisterType(PointerSerializer::CreateClass( TXT( "Pointer" ) ));
        g_Registry->RegisterType(EnumerationSerializer::CreateClass( TXT( "Enumeration" ) ));
        g_Registry->RegisterType(BitfieldSerializer::CreateClass( TXT( "Bitfield" ) ));
        g_Registry->RegisterType(PathSerializer::CreateClass( TXT( "Path" ) ));

        // SimpleSerializer
        g_Registry->RegisterType(StringSerializer::CreateClass( TXT( "String" ) ));
        g_Registry->RegisterType(BoolSerializer::CreateClass( TXT( "Bool" ) ));
        g_Registry->RegisterType(U8Serializer::CreateClass( TXT( "U8" ) ));
        g_Registry->RegisterType(I8Serializer::CreateClass( TXT( "I8" ) ));
        g_Registry->RegisterType(U16Serializer::CreateClass( TXT( "U16" ) ));
        g_Registry->RegisterType(I16Serializer::CreateClass( TXT( "I16" ) ));
        g_Registry->RegisterType(U32Serializer::CreateClass( TXT( "U32" ) ));
        g_Registry->RegisterType(I32Serializer::CreateClass( TXT( "I32" ) ));
        g_Registry->RegisterType(U64Serializer::CreateClass( TXT( "U64" ) ));
        g_Registry->RegisterType(I64Serializer::CreateClass( TXT( "I64" ) ));
        g_Registry->RegisterType(F32Serializer::CreateClass( TXT( "F32" ) ));
        g_Registry->RegisterType(F64Serializer::CreateClass( TXT( "F64" ) ));
        g_Registry->RegisterType(GUIDSerializer::CreateClass( TXT( "GUID" ) ));
        g_Registry->RegisterType(TUIDSerializer::CreateClass( TXT( "TUID" ) ));
        g_Registry->RegisterType(Vector2Serializer::CreateClass( TXT( "Vector2" ) ));
        g_Registry->RegisterType(Vector3Serializer::CreateClass( TXT( "Vector3" ) ));
        g_Registry->RegisterType(Vector4Serializer::CreateClass( TXT( "Vector4" ) ));
        g_Registry->RegisterType(Matrix3Serializer::CreateClass( TXT( "Matrix3" ) ));
        g_Registry->RegisterType(Matrix4Serializer::CreateClass( TXT( "Matrix4" ) ));
        g_Registry->RegisterType(QuaternionSerializer::CreateClass( TXT( "Quaternion" ) ));
        g_Registry->RegisterType(Color3Serializer::CreateClass( TXT( "Color3" ) ));
        g_Registry->RegisterType(Color4Serializer::CreateClass( TXT( "Color4" ) ));
        g_Registry->RegisterType(HDRColor3Serializer::CreateClass( TXT( "HDRColor3" ) ));
        g_Registry->RegisterType(HDRColor4Serializer::CreateClass( TXT( "HDRColor4" ) ));

        // ArraySerializer
        g_Registry->RegisterType(ArraySerializer::CreateClass( TXT( "Array" ) ));
        g_Registry->RegisterType(StringArraySerializer::CreateClass( TXT( "StringArray" ) ));
        g_Registry->RegisterType(BoolArraySerializer::CreateClass( TXT( "BoolArray" ) ));
        g_Registry->RegisterType(U8ArraySerializer::CreateClass( TXT( "U8Array" ) ));
        g_Registry->RegisterType(I8ArraySerializer::CreateClass( TXT( "I8Array" ) ));
        g_Registry->RegisterType(U16ArraySerializer::CreateClass( TXT( "U16Array" ) ));
        g_Registry->RegisterType(I16ArraySerializer::CreateClass( TXT( "I16Array" ) ));
        g_Registry->RegisterType(U32ArraySerializer::CreateClass( TXT( "U32Array" ) ));
        g_Registry->RegisterType(I32ArraySerializer::CreateClass( TXT( "I32Array" ) ));
        g_Registry->RegisterType(U64ArraySerializer::CreateClass( TXT( "U64Array" ) ));
        g_Registry->RegisterType(I64ArraySerializer::CreateClass( TXT( "I64Array" ) ));
        g_Registry->RegisterType(F32ArraySerializer::CreateClass( TXT( "F32Array" ) ));
        g_Registry->RegisterType(F64ArraySerializer::CreateClass( TXT( "F64Array" ) ));
        g_Registry->RegisterType(GUIDArraySerializer::CreateClass( TXT( "GUIDArray" ) ));
        g_Registry->RegisterType(TUIDArraySerializer::CreateClass( TXT( "TUIDArray" ) ));
        g_Registry->RegisterType(PathArraySerializer::CreateClass( TXT( "PathArray" ) ));
        g_Registry->RegisterType(Vector2ArraySerializer::CreateClass( TXT( "Vector2Array" ) ));
        g_Registry->RegisterType(Vector3ArraySerializer::CreateClass( TXT( "Vector3Array" ) ));
        g_Registry->RegisterType(Vector4ArraySerializer::CreateClass( TXT( "Vector4Array" ) ));
        g_Registry->RegisterType(Matrix3ArraySerializer::CreateClass( TXT( "Matrix3Array" ) ));
        g_Registry->RegisterType(Matrix4ArraySerializer::CreateClass( TXT( "Matrix4Array" ) ));
        g_Registry->RegisterType(QuaternionArraySerializer::CreateClass( TXT( "QuaternionArray" ) ));
        g_Registry->RegisterType(Color3ArraySerializer::CreateClass( TXT( "Color3Array" ) ));
        g_Registry->RegisterType(Color4ArraySerializer::CreateClass( TXT( "Color4Array" ) ));
        g_Registry->RegisterType(HDRColor3ArraySerializer::CreateClass( TXT( "HDRColor3Array" ) ));
        g_Registry->RegisterType(HDRColor4ArraySerializer::CreateClass( TXT( "HDRColor4Array" ) ));

        // SetSerializer
        g_Registry->RegisterType(SetSerializer::CreateClass( TXT( "Set" ) ));
        g_Registry->RegisterType(StringSetSerializer::CreateClass( TXT( "StrSet" ) ));
        g_Registry->RegisterType(U32SetSerializer::CreateClass( TXT( "U32Set" ) ));
        g_Registry->RegisterType(U64SetSerializer::CreateClass( TXT( "U64Set" ) ));
        g_Registry->RegisterType(F32SetSerializer::CreateClass( TXT( "F32Set" ) ));
        g_Registry->RegisterType(GUIDSetSerializer::CreateClass( TXT( "GUIDSet" ) ));
        g_Registry->RegisterType(TUIDSetSerializer::CreateClass( TXT( "TUIDSet" ) ));
        g_Registry->RegisterType( PathSetSerializer::CreateClass( TXT( "PathSet" ) ));

        // MapSerializer
        g_Registry->RegisterType(MapSerializer::CreateClass( TXT( "Map" ) ));
        g_Registry->RegisterType(StringStringMapSerializer::CreateClass( TXT( "StrStrMap" ) ));
        g_Registry->RegisterType(StringBoolMapSerializer::CreateClass( TXT( "StrBoolMap" ) ));
        g_Registry->RegisterType(StringU32MapSerializer::CreateClass( TXT( "StrU32Map" ) ));
        g_Registry->RegisterType(StringI32MapSerializer::CreateClass( TXT( "StrI32Map" ) ));
        g_Registry->RegisterType(U32StringMapSerializer::CreateClass( TXT( "U32StrMap" ) ));
        g_Registry->RegisterType(U32U32MapSerializer::CreateClass( TXT( "U32U32Map" ) ));
        g_Registry->RegisterType(U32I32MapSerializer::CreateClass( TXT( "U32I32Map" ) ));
        g_Registry->RegisterType(U32U64MapSerializer::CreateClass( TXT( "U32U64Map" ) ));
        g_Registry->RegisterType(I32StringMapSerializer::CreateClass( TXT( "I32StrMap" ) ));
        g_Registry->RegisterType(I32U32MapSerializer::CreateClass( TXT( "I32U32Map" ) ));
        g_Registry->RegisterType(I32I32MapSerializer::CreateClass( TXT( "I32I32Map" ) ));
        g_Registry->RegisterType(I32U64MapSerializer::CreateClass( TXT( "I32U64Map" ) ));
        g_Registry->RegisterType(U64StringMapSerializer::CreateClass( TXT( "U64StrMap" ) ));
        g_Registry->RegisterType(U64U32MapSerializer::CreateClass( TXT( "U64U32Map" ) ));
        g_Registry->RegisterType(U64U64MapSerializer::CreateClass( TXT( "U64U64Map" ) ));
        g_Registry->RegisterType(U64Matrix4MapSerializer::CreateClass( TXT( "U64Matrix4Map" ) ));
        g_Registry->RegisterType(GUIDU32MapSerializer::CreateClass( TXT( "GUIDU32Map" ) ));
        g_Registry->RegisterType(GUIDMatrix4MapSerializer::CreateClass( TXT( "GUIDMatrix4Map" ) ));
        g_Registry->RegisterType(TUIDU32MapSerializer::CreateClass( TXT( "TUIDU32Map" ) ));
        g_Registry->RegisterType(TUIDMatrix4MapSerializer::CreateClass( TXT( "TUIDMatrix4Map" ) ));

        // ElementArraySerializer
        g_Registry->RegisterType(ElementArraySerializer::CreateClass( TXT( "ElementArray" ) ));

        // ElementSetSerializer
        g_Registry->RegisterType(ElementSetSerializer::CreateClass( TXT( "ElementSet" ) ));

        // ElementMapSerializer
        g_Registry->RegisterType(ElementMapSerializer::CreateClass( TXT( "ElementMap" ) ));
        g_Registry->RegisterType(TypeIDElementMapSerializer::CreateClass( TXT( "TypeIDElementMap" ) ));
        g_Registry->RegisterType(StringElementMapSerializer::CreateClass( TXT( "StringElementMap" ) ));
        g_Registry->RegisterType(U32ElementMapSerializer::CreateClass( TXT( "U32ElementMap" ) ));
        g_Registry->RegisterType(I32ElementMapSerializer::CreateClass( TXT( "I32ElementMap" ) ));
        g_Registry->RegisterType(U64ElementMapSerializer::CreateClass( TXT( "U64ElementMap" ) ));
        g_Registry->RegisterType(I64ElementMapSerializer::CreateClass( TXT( "I64ElementMap" ) ));
        g_Registry->RegisterType(GUIDElementMapSerializer::CreateClass( TXT( "GUIDElementMap" ) ));
        g_Registry->RegisterType(TUIDElementMapSerializer::CreateClass( TXT( "TUIDElementMap" ) ));

        //
        // Build Casting Table
        //

        Serializer::Initialize();

        //
        // Register Elements
        //

        g_Registry->RegisterType(Version::CreateClass( TXT( "Version" ) ));
        g_Registry->RegisterType(DocumentNode::CreateClass( TXT("DocumentNode") ));
        g_Registry->RegisterType(DocumentAttribute::CreateClass( TXT("DocumentAttribute") ));
        g_Registry->RegisterType(DocumentElement::CreateClass( TXT("DocumentElement") ));
        g_Registry->RegisterType(Document::CreateClass( TXT("Document") ));
    }

#ifdef REFLECT_DEBUG_INIT_AND_CLEANUP
    std::vector<uintptr> trace;
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
        Serializer::Cleanup();

        // delete registry
        delete g_Registry;
        g_Registry = NULL;
    }

#ifdef REFLECT_DEBUG_INIT_AND_CLEANUP
    std::vector<uintptr> trace;
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
: m_InitThread (0)
, m_Created (NULL)
, m_Destroyed (NULL)
{
    m_InitThread = ::GetCurrentThreadId();

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

bool Registry::IsInitThread()
{
    return m_InitThread == GetCurrentThreadId();
}

bool Registry::RegisterType(Type* type)
{
    HELIUM_ASSERT( IsInitThread() );

    switch (type->GetReflectionType())
    {
    case ReflectionTypes::Class:
        {
            Class* classType = static_cast<Class*>(type);

            Insert<M_IDToType>::Result idResult = m_TypesByID.insert(M_IDToType::value_type (classType->m_TypeID, classType));

            if (idResult.second)
            {
                m_TypesByName.insert(M_StrToType::value_type (classType->m_FullName, classType));

                if ( !classType->m_ShortName.empty() )
                {
                    Insert<M_StrToType>::Result shortNameResult = m_TypesByName.insert(M_StrToType::value_type (classType->m_ShortName, classType));

                    if (!shortNameResult.second && classType != shortNameResult.first->second)
                    {
                        Log::Error( TXT( "Re-registration of short name '%s' was attempted with different classType information\n" ), classType->m_ShortName.c_str());
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
                            static_cast<Class*>(baseClass)->m_Derived.insert( classType->m_FullName );
                        }
                        else
                        {
                            Log::Error( TXT( "Base class of '%s' is not a valid type\n" ), classType->m_ShortName.c_str());
                            HELIUM_BREAK();
                            return false;
                        }
                    }
                }

                classType->Report();
            }
            else if (classType != idResult.first->second)
            {
                Log::Error( TXT( "Re-registration of classType '%s' was attempted with different classType information\n" ), classType->m_FullName.c_str());
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
                Insert<M_StrToType>::Result enumResult = m_TypesByName.insert(M_StrToType::value_type (enumeration->m_ShortName, enumeration));

                if (!enumResult.second && enumeration != enumResult.first->second)
                {
                    Log::Error( TXT( "Re-registration of enumeration '%s' was attempted with different type information\n" ), enumeration->m_ShortName.c_str());
                    HELIUM_BREAK();
                    return false;
                }

                enumResult = m_TypesByName.insert(M_StrToType::value_type (enumeration->m_FullName, enumeration));

                if (!enumResult.second && enumeration != enumResult.first->second)
                {
                    Log::Error( TXT( "Re-registration of enumeration '%s' was attempted with different type information\n" ), enumeration->m_ShortName.c_str());
                    HELIUM_BREAK();
                    return false;
                }
            }
            else if (enumeration != idResult.first->second)
            {
                Log::Error( TXT( "Re-registration of enumeration '%s' was attempted with different type information\n" ), enumeration->m_FullName.c_str());
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
    HELIUM_ASSERT( IsInitThread() );

    switch (type->GetReflectionType())
    {
    case ReflectionTypes::Class:
        {
            const Class* classType = static_cast<const Class*>(type);

            if ( !classType->m_ShortName.empty() )
            {
                m_TypesByName.erase( classType->m_ShortName );
            }

            if ( !classType->m_Base.empty() )
            {
                M_StrToType::const_iterator found = m_TypesByName.find( classType->m_Base );
                if ( found != m_TypesByName.end() )
                {
                    if ( found->second->GetReflectionType() == ReflectionTypes::Class )
                    {
                        static_cast<Class*>(found->second.Ptr())->m_Derived.erase( classType->m_FullName );
                    }
                    else
                    {
                        Log::Error( TXT( "Base class of '%s' is not a valid type\n" ), classType->m_ShortName.c_str());
                        HELIUM_BREAK();
                    }
                }
            }

            m_TypesByName.erase(classType->m_FullName);
            m_TypesByID.erase(classType->m_TypeID);

            break;
        }

    case ReflectionTypes::Enumeration:
        {
            const Enumeration* enumeration = static_cast<const Enumeration*>(type);

            m_TypesByName.erase(enumeration->m_ShortName);
            m_TypesByName.erase(enumeration->m_FullName);
        }
    }
}

void Registry::AliasType(const Type* type, const tstring& alias)
{
    HELIUM_ASSERT( IsInitThread() );

    m_TypesByAlias.insert(M_StrToType::value_type (alias, type));
}

void Registry::UnAliasType(const Type* type, const tstring& alias)
{
    HELIUM_ASSERT( IsInitThread() );

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

void Registry::AtomicGetType(int id, const Type** addr) const
{
    Helium::AtomicExchange( (intptr*)addr, (intptr)GetType(id) );
}

void Registry::AtomicGetType(const tstring& str, const Type** addr) const
{
    Helium::AtomicExchange( (intptr*)addr, (intptr)GetType(str) );
}

ObjectPtr Registry::CreateInstance(int id) const
{
    M_IDToType::const_iterator type = m_TypesByID.find(id);

    if (type != m_TypesByID.end() && type->second->GetReflectionType() == ReflectionTypes::Class)
    {
        return ReflectionCast<const Class>(type->second)->m_Create();
    }
    else
    {
        return NULL;
    }
}

ObjectPtr Registry::CreateInstance(const Class* type) const
{
    if (type && type->m_Create)
    {
        return type->m_Create();
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
    Registry::GetInstance()->TrackCreate((uintptr)object);
#endif

    if (m_Created != NULL)
    {
        m_Created(object);
    }
}

void Registry::Destroyed(Object* object)
{
#ifdef REFLECT_OBJECT_TRACKING
    Registry::GetInstance()->TrackDelete((uintptr)object);
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

void Registry::TrackCreate(uintptr ptr)
{
    m_Tracker.Create( ptr );
}

void Registry::TrackDelete(uintptr ptr)
{
    m_Tracker.Delete( ptr );
}

void Registry::TrackCheck(uintptr ptr)
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

CreationRecord::CreationRecord(uintptr ptr)
: m_Address (ptr)
, m_Type (-1)
{

}

void CreationRecord::Dump(FILE* f)
{
    _ftprintf(f, TXT("\n\n"));
    _ftprintf(f, TXT("Addr: %p\n"), m_Address);
    _ftprintf(f, TXT("Name: %s\n"), m_ShortName.c_str());
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
    Helium::TakeMutex mutex (g_TrackerMutex);

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

void Tracker::Create(uintptr ptr)
{
    Helium::TakeMutex mutex (g_TrackerMutex);

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

void Tracker::Delete(uintptr ptr)
{
    Helium::TakeMutex mutex (g_TrackerMutex);

    M_CreationRecord::iterator iter = m_CreatedObjects.find(ptr);
    if ( iter != m_CreatedObjects.end())
    {
        CreationRecord& cr = (*iter).second;

#ifdef REFLECT_OBJECT_VERBOSE_TRACKING
        fprintf(stderr, "%p: DELETE", ptr);
        if (!cr.m_ShortName.empty())
        {
            fprintf(stderr, ": %s\n", cr.m_ShortName.c_str());
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

void Tracker::Check(uintptr ptr)
{
    Helium::TakeMutex mutex (g_TrackerMutex);

    M_CreationRecord::iterator iter = m_CreatedObjects.find(ptr);
    if ( iter != m_CreatedObjects.end())
    {
        CreationRecord& cr = (*iter).second;

        if (cr.m_Type < 0)
        {
            Element* e = reinterpret_cast<Element*>(ptr);

            cr.m_ShortName = e->GetClass()->m_ShortName;
            cr.m_Type = e->GetType();

#ifdef REFLECT_OBJECT_VERBOSE_TRACKING
            fprintf(stderr, "%p: TRACKING", ptr);
            fprintf(stderr, ": %s \n", cr.m_ShortName.c_str());
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
    Helium::TakeMutex mutex (g_TrackerMutex);

    tchar module[MAX_PATH];
    GetModuleFileName( 0, module, MAX_PATH );

    tchar drive[MAX_PATH];
    tchar dir[MAX_PATH];
    tchar name[MAX_PATH];
    _tsplitpath( module, drive, dir, name, NULL );

    tstring path = drive;
    path += dir;
    path += name;

    FILE* f = _tfopen( (path + TXT("ReflectDump.log")).c_str(), TXT("w") );
    if ( f != NULL )
    {
        typedef std::map< u32, std::pair< tstring, u32 > > ObjectLogger;
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
                    objectLog.insert( ObjectLogger::value_type( (*c_current).second.m_Type, std::pair< tstring, u32 >( (*c_current).second.m_ShortName, 1 ) ) );
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
                    objectLog.insert( ObjectLogger::value_type( (*d_current).second.m_Type, std::pair< tstring, u32 >( (*d_current).second.m_ShortName, 1 ) ) );
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

            tchar format[1024];
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
