#include "Registry.h"
#include "Version.h"
#include "Serializers.h"

#ifdef REFLECT_OBJECT_TRACKING
# include "Platform/Mutex.h"
# include "Platform/Windows/Debug.h"
#endif

#include "Platform/Windows/Windows.h"

#include "Foundation/CommandLine.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/Environment.h"
#include "Foundation/Log.h"

#include <io.h>

// Prints the callstack for every init and cleanup call
// #define REFLECT_DEBUG_INIT_AND_CLEANUP

using Nocturnal::Insert; 
using namespace Reflect;

// globals
static Registry* g_Instance = NULL;

// profile interface
#ifdef PROFILE_ACCUMULATION
Profile::Accumulator Reflect::g_CloneAccum ("Reflect Clone");
Profile::Accumulator Reflect::g_ParseAccum ("Reflect Parse");
Profile::Accumulator Reflect::g_AuthorAccum ("Reflect Author");
Profile::Accumulator Reflect::g_ChecksumAccum ("Reflect Checksum");
Profile::Accumulator Reflect::g_PreSerializeAccum ("Reflect Serialize Pre-Process");
Profile::Accumulator Reflect::g_PostSerializeAccum ("Reflect Serialize Post-Process");
Profile::Accumulator Reflect::g_PreDeserializeAccum ("Reflect Deserialize Pre-Process");
Profile::Accumulator Reflect::g_PostDeserializeAccum ("Reflect Deserialize Post-Process");
#endif

template <class T>
struct CaseInsensitiveCompare
{
    const std::string& value;

    CaseInsensitiveCompare(const std::string& str)
        : value (str)
    {

    }

    bool operator()(const std::pair<const std::string, T> &rhs)
    {
        return stricmp(rhs.first.c_str(), value.c_str()) == 0;
    }
};

namespace Reflect
{
    i32 g_InitCount = 0;
}

//
// global destructor should run after cleanup
//
class CleanupChecker
{
public:
    ~CleanupChecker()
    {
        NOC_ASSERT_MSG( Reflect::g_InitCount == 0, ( "Reflect not cleaned up properly!" ) );
    }

} g_Checker;

bool Reflect::IsInitialized()
{
    return g_Instance != NULL;
}

void Reflect::Initialize()
{
    if (++g_InitCount == 1)
    {
        g_Instance = new Registry();
        NOC_ASSERT(g_Instance != NULL);

        //
        // Serializers
        //

        g_Instance->RegisterType(Class::Create<Object>("", "Object"));
        g_Instance->RegisterType(Element::CreateClass("Element"));
        g_Instance->RegisterType(Serializer::CreateClass("Serializer"));
        g_Instance->RegisterType(ContainerSerializer::CreateClass("Container"));
        g_Instance->RegisterType(ElementContainerSerializer::CreateClass("ElementContainer"));
        g_Instance->RegisterType(TypeIDSerializer::CreateClass("TypeID"));
        g_Instance->RegisterType(PointerSerializer::CreateClass("Pointer"));
        g_Instance->RegisterType(EnumerationSerializer::CreateClass("Enumeration"));
        g_Instance->RegisterType(BitfieldSerializer::CreateClass("Bitfield"));
        g_Instance->RegisterType( PathSerializer::CreateClass( "PathSerializer" ) );

        // SimpleSerializer
        g_Instance->RegisterType(StringSerializer::CreateClass("String"));
        g_Instance->RegisterType(BoolSerializer::CreateClass("Bool"));
        g_Instance->RegisterType(U8Serializer::CreateClass("U8"));
        g_Instance->RegisterType(I8Serializer::CreateClass("I8"));
        g_Instance->RegisterType(U16Serializer::CreateClass("U16"));
        g_Instance->RegisterType(I16Serializer::CreateClass("I16"));
        g_Instance->RegisterType(U32Serializer::CreateClass("U32"));
        g_Instance->RegisterType(I32Serializer::CreateClass("I32"));
        g_Instance->RegisterType(U64Serializer::CreateClass("U64"));
        g_Instance->RegisterType(I64Serializer::CreateClass("I64"));
        g_Instance->RegisterType(F32Serializer::CreateClass("F32"));
        g_Instance->RegisterType(F64Serializer::CreateClass("F64"));
        g_Instance->RegisterType(GUIDSerializer::CreateClass("GUID"));
        g_Instance->RegisterType(TUIDSerializer::CreateClass("TUID"));
        g_Instance->RegisterType(Vector2Serializer::CreateClass("Vector2"));
        g_Instance->RegisterType(Vector3Serializer::CreateClass("Vector3"));
        g_Instance->RegisterType(Vector4Serializer::CreateClass("Vector4"));
        g_Instance->RegisterType(Matrix3Serializer::CreateClass("Matrix3"));
        g_Instance->RegisterType(Matrix4Serializer::CreateClass("Matrix4"));
        g_Instance->RegisterType(QuaternionSerializer::CreateClass("Quaternion"));
        g_Instance->RegisterType(Color3Serializer::CreateClass("Color3"));
        g_Instance->RegisterType(Color4Serializer::CreateClass("Color4"));
        g_Instance->RegisterType(HDRColor3Serializer::CreateClass("HDRColor3"));
        g_Instance->RegisterType(HDRColor4Serializer::CreateClass("HDRColor4"));

        // ArraySerializer
        g_Instance->RegisterType(ArraySerializer::CreateClass("Array"));
        g_Instance->RegisterType(StringArraySerializer::CreateClass("StringArray"));
        g_Instance->RegisterType(BoolArraySerializer::CreateClass("BoolArray"));
        g_Instance->RegisterType(U8ArraySerializer::CreateClass("U8Array"));
        g_Instance->RegisterType(I8ArraySerializer::CreateClass("I8Array"));
        g_Instance->RegisterType(U16ArraySerializer::CreateClass("U16Array"));
        g_Instance->RegisterType(I16ArraySerializer::CreateClass("I16Array"));
        g_Instance->RegisterType(U32ArraySerializer::CreateClass("U32Array"));
        g_Instance->RegisterType(I32ArraySerializer::CreateClass("I32Array"));
        g_Instance->RegisterType(U64ArraySerializer::CreateClass("U64Array"));
        g_Instance->RegisterType(I64ArraySerializer::CreateClass("I64Array"));
        g_Instance->RegisterType(F32ArraySerializer::CreateClass("F32Array"));
        g_Instance->RegisterType(F64ArraySerializer::CreateClass("F64Array"));
        g_Instance->RegisterType(GUIDArraySerializer::CreateClass("GUIDArray"));
        g_Instance->RegisterType(TUIDArraySerializer::CreateClass("TUIDArray"));
        g_Instance->RegisterType(Vector2ArraySerializer::CreateClass("Vector2Array"));
        g_Instance->RegisterType(Vector3ArraySerializer::CreateClass("Vector3Array"));
        g_Instance->RegisterType(Vector4ArraySerializer::CreateClass("Vector4Array"));
        g_Instance->RegisterType(Matrix3ArraySerializer::CreateClass("Matrix3Array"));
        g_Instance->RegisterType(Matrix4ArraySerializer::CreateClass("Matrix4Array"));
        g_Instance->RegisterType(QuaternionArraySerializer::CreateClass("QuaternionArray"));
        g_Instance->RegisterType(Color3ArraySerializer::CreateClass("Color3Array"));
        g_Instance->RegisterType(Color4ArraySerializer::CreateClass("Color4Array"));
        g_Instance->RegisterType(HDRColor3ArraySerializer::CreateClass("HDRColor3Array"));
        g_Instance->RegisterType(HDRColor4ArraySerializer::CreateClass("HDRColor4Array"));

        // SetSerializer
        g_Instance->RegisterType(SetSerializer::CreateClass("Set"));
        g_Instance->RegisterType(StringSetSerializer::CreateClass("StrSet"));
        g_Instance->RegisterType(U32SetSerializer::CreateClass("U32Set"));
        g_Instance->RegisterType(U64SetSerializer::CreateClass("U64Set"));
        g_Instance->RegisterType(F32SetSerializer::CreateClass("F32Set"));
        g_Instance->RegisterType(GUIDSetSerializer::CreateClass("GUIDSet"));
        g_Instance->RegisterType(TUIDSetSerializer::CreateClass("TUIDSet"));
        g_Instance->RegisterType( PathSetSerializer::CreateClass( "PathSet" ) );

        // MapSerializer
        g_Instance->RegisterType(MapSerializer::CreateClass("Map"));
        g_Instance->RegisterType(StringStringMapSerializer::CreateClass("StrStrMap"));
        g_Instance->RegisterType(StringBoolMapSerializer::CreateClass("StrBoolMap"));
        g_Instance->RegisterType(StringU32MapSerializer::CreateClass("StrU32Map"));
        g_Instance->RegisterType(StringI32MapSerializer::CreateClass("StrI32Map"));
        g_Instance->RegisterType(U32StringMapSerializer::CreateClass("U32StrMap"));
        g_Instance->RegisterType(U32U32MapSerializer::CreateClass("U32U32Map"));
        g_Instance->RegisterType(U32I32MapSerializer::CreateClass("U32I32Map"));
        g_Instance->RegisterType(U32U64MapSerializer::CreateClass("U32U64Map"));
        g_Instance->RegisterType(I32StringMapSerializer::CreateClass("I32StrMap"));
        g_Instance->RegisterType(I32U32MapSerializer::CreateClass("I32U32Map"));
        g_Instance->RegisterType(I32I32MapSerializer::CreateClass("I32I32Map"));
        g_Instance->RegisterType(I32U64MapSerializer::CreateClass("I32U64Map"));
        g_Instance->RegisterType(U64StringMapSerializer::CreateClass("U64StrMap"));
        g_Instance->RegisterType(U64U32MapSerializer::CreateClass("U64U32Map"));
        g_Instance->RegisterType(U64U64MapSerializer::CreateClass("U64U64Map"));
        g_Instance->RegisterType(U64Matrix4MapSerializer::CreateClass("U64Matrix4Map"));
        g_Instance->RegisterType(GUIDU32MapSerializer::CreateClass("GUIDU32Map"));
        g_Instance->RegisterType(GUIDMatrix4MapSerializer::CreateClass("GUIDMatrix4Map"));
        g_Instance->RegisterType(TUIDU32MapSerializer::CreateClass("TUIDU32Map"));
        g_Instance->RegisterType(TUIDMatrix4MapSerializer::CreateClass("TUIDMatrix4Map"));

        // ElementArraySerializer
        g_Instance->RegisterType(ElementArraySerializer::CreateClass("ElementArray"));

        // ElementSetSerializer
        g_Instance->RegisterType(ElementSetSerializer::CreateClass("ElementSet"));

        // ElementMapSerializer
        g_Instance->RegisterType(ElementMapSerializer::CreateClass("ElementMap"));
        g_Instance->RegisterType(TypeIDElementMapSerializer::CreateClass("TypeIDElementMap"));
        g_Instance->RegisterType(StringElementMapSerializer::CreateClass("StringElementMap"));
        g_Instance->RegisterType(U32ElementMapSerializer::CreateClass("U32ElementMap"));
        g_Instance->RegisterType(I32ElementMapSerializer::CreateClass("I32ElementMap"));
        g_Instance->RegisterType(U64ElementMapSerializer::CreateClass("U64ElementMap"));
        g_Instance->RegisterType(I64ElementMapSerializer::CreateClass("I64ElementMap"));
        g_Instance->RegisterType(GUIDElementMapSerializer::CreateClass("GUIDElementMap"));
        g_Instance->RegisterType(TUIDElementMapSerializer::CreateClass("TUIDElementMap"));

        //
        // Register Elements
        //

        g_Instance->RegisterType(Version::CreateClass("Version"));

        //
        // Build Casting Table
        //

        Serializer::Initialize();

        //
        // Legacy support for GUID
        //

        g_Instance->AliasType( Reflect::GetClass<GUIDSerializer>(), "UID" );
        g_Instance->AliasType( Reflect::GetClass<GUIDArraySerializer>(), "UIDArray" );
        g_Instance->AliasType( Reflect::GetClass<GUIDSetSerializer>(), "UIDSet" );
        g_Instance->AliasType( Reflect::GetClass<GUIDU32MapSerializer>(), "UIDU32Map" );
        g_Instance->AliasType( Reflect::GetClass<GUIDMatrix4MapSerializer>(), "UIDMatrix4Map" );
        g_Instance->AliasType( Reflect::GetClass<GUIDElementMapSerializer>(), "UIDElementMap" );
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
        delete g_Instance;
        g_Instance = NULL;
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
        g_MemoryPool = Profile::Memory::CreatePool("Reflect Objects");
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
    NOC_ASSERT(g_Instance != NULL);
    return g_Instance;
}

bool Registry::IsInitThread()
{
    return m_InitThread == GetCurrentThreadId();
}

bool Registry::RegisterType(Type* type)
{
    NOC_ASSERT( IsInitThread() );

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
                        Log::Error("Re-registration of short name '%s' was attempted with different classType information\n", classType->m_ShortName.c_str());
                        NOC_BREAK();
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
                            Log::Error("Base class of '%s' is not a valid type\n", classType->m_ShortName.c_str());
                            NOC_BREAK();
                            return false;
                        }
                    }
                }

                classType->Report();
            }
            else if (classType != idResult.first->second)
            {
                Log::Error("Re-registration of classType '%s' was attempted with different classType information\n", classType->m_FullName.c_str());
                NOC_BREAK();
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
                    Log::Error("Re-registration of enumeration '%s' was attempted with different type information\n", enumeration->m_ShortName.c_str());
                    NOC_BREAK();
                    return false;
                }

                enumResult = m_TypesByName.insert(M_StrToType::value_type (enumeration->m_FullName, enumeration));

                if (!enumResult.second && enumeration != enumResult.first->second)
                {
                    Log::Error("Re-registration of enumeration '%s' was attempted with different type information\n", enumeration->m_ShortName.c_str());
                    NOC_BREAK();
                    return false;
                }
            }
            else if (enumeration != idResult.first->second)
            {
                Log::Error("Re-registration of enumeration '%s' was attempted with different type information\n", enumeration->m_FullName.c_str());
                NOC_BREAK();
                return false;
            }

            break;
        }
    }

    return true;
}

void Registry::UnregisterType(const Type* type)
{
    NOC_ASSERT( IsInitThread() );

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
                        Log::Error("Base class of '%s' is not a valid type\n", classType->m_ShortName.c_str());
                        NOC_BREAK();
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

void Registry::AliasType(const Type* type, const std::string& alias)
{
    NOC_ASSERT( IsInitThread() );

    m_TypesByAlias.insert(M_StrToType::value_type (alias, type));
}

void Registry::UnAliasType(const Type* type, const std::string& alias)
{
    NOC_ASSERT( IsInitThread() );

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

const Type* Registry::GetType(const std::string& str) const
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
#ifdef _WIN64
    InterlockedExchange64( (volatile LONGLONG*)addr, (LONGLONG)(uintptr)GetType(id) );
#else
    InterlockedExchange( (volatile LONG*)addr, (LONG)(uintptr)GetType(id) );
#endif
}

void Registry::AtomicGetType(const std::string& str, const Type** addr) const
{
#ifdef _WIN64
    InterlockedExchange64( (volatile LONGLONG*)addr, (LONGLONG)(uintptr)GetType(str) );
#else
    InterlockedExchange( (volatile LONG*)addr, (LONG)(uintptr)GetType(str) );
#endif
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

ObjectPtr Registry::CreateInstance(const std::string& str) const
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

const std::string& StackRecord::Convert()
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
    fprintf(f, "\n\n");
    fprintf(f, "Addr: %p\n", m_Address);
    fprintf(f, "Name: %s\n", m_ShortName.c_str());
    fprintf(f, "Type: %i\n", m_Type);

#ifdef REFLECT_OBJECT_STACK_TRACKING
    fprintf(f, "Create Stack:\n%s\n", m_CreateStack.ReferencesObject() ? m_CreateStack->Convert().c_str() : "<none>" );
    fprintf(f, "Delete Stack:\n%s\n", m_DeleteStack.ReferencesObject() ? m_DeleteStack->Convert().c_str() : "<none>" );
#endif
}

Platform::Mutex g_TrackerMutex;

Tracker::Tracker()
{

}

Tracker::~Tracker()
{
    Dump();
}

StackRecordPtr Tracker::GetStack()
{
    Platform::TakeMutex mutex (g_TrackerMutex);

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
    Platform::TakeMutex mutex (g_TrackerMutex);

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
        NOC_ASSERT( create_iter != m_CreatedObjects.end() );

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
        NOC_BREAK();
        (*create_iter).second.Dump( stderr );
    }
}

void Tracker::Delete(uintptr ptr)
{
    Platform::TakeMutex mutex (g_TrackerMutex);

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
        NOC_ASSERT( delete_iter != m_DeletedObjects.end() );

        // erase the entry from objects which are currently created
        m_CreatedObjects.erase(iter);
    }
    else
    {
        // we should have a creation record for everything that gets deleted
        NOC_BREAK();
    }
}

void Tracker::Check(uintptr ptr)
{
    Platform::TakeMutex mutex (g_TrackerMutex);

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
        NOC_BREAK();
    }
}

void Tracker::Dump()
{
    Platform::TakeMutex mutex (g_TrackerMutex);

    char module[MAX_PATH];
    GetModuleFileName( 0, module, MAX_PATH );

    char drive[MAX_PATH];
    char dir[MAX_PATH];
    char name[MAX_PATH];
    _splitpath( module, drive, dir, name, NULL );

    std::string path = drive;
    path += dir;
    path += name;

    FILE* f = fopen( (path + "ReflectDump.log").c_str(), "w" );
    if ( f != NULL )
    {
        typedef std::map< u32, std::pair< std::string, u32 > > ObjectLogger;
        ObjectLogger ObjectLog;

        {
            M_CreationRecord::iterator c_current = m_CreatedObjects.begin();
            M_CreationRecord::iterator c_end = m_CreatedObjects.end();

            for ( ; c_current != c_end; ++c_current)
            {
                (*c_current).second.Dump( f );

                ObjectLogger::iterator iter = ObjectLog.find( (*c_current).second.m_Type );
                if ( iter == ObjectLog.end() )
                {
                    ObjectLog.insert( ObjectLogger::value_type( (*c_current).second.m_Type, 
                        std::pair< std::string, u32 >( (*c_current).second.m_ShortName, 1 ) ) );
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
                ObjectLogger::iterator iter = ObjectLog.find( (*d_current).second.m_Type );
                if ( iter == ObjectLog.end() )
                {
                    ObjectLog.insert( ObjectLogger::value_type( (*d_current).second.m_Type, 
                        std::pair< std::string, u32 >( (*d_current).second.m_ShortName, 1 ) ) );
                }
                else
                {
                    (*iter).second.second++;
                }
            }
        }

        {
            size_t max = 0;
            ObjectLogger::iterator iter = ObjectLog.begin();  
            ObjectLogger::iterator end = ObjectLog.end();  

            for ( ; iter != end; ++iter )
            {
                max = std::max( (*iter).second.first.length(), max );
            }

            char format[1024];
            sprintf( format, "\nType: %%%ds, Count: %%d", max );

            iter = ObjectLog.begin();
            for ( ; iter != end; ++iter )
            {
                fprintf( f, format, (*iter).second.first.c_str(), (*iter).second.second );
            }
        }

        fclose(f);
    }
}

#endif
