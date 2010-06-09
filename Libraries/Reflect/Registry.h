#pragma once

#include <map>
#include <string>

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/File/Path.h"

#include "API.h"
#include "Tracker.h"

#include "Type.h"
#include "Class.h"
#include "Enumeration.h"

namespace Reflect
{
  // Callbacks for external APIs
  typedef void (*CreatedFunc)(Object* object);
  typedef void (*DestroyedFunc)(Object* object);

  // Registry containers
  typedef std::map< int, Nocturnal::SmartPtr<Type> > M_IDToType;
  typedef std::map< std::string, Nocturnal::SmartPtr<Type> > M_StrToType;

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
  REFLECT_API bool IsInitialized();

  REFLECT_API void Initialize();
  REFLECT_API void InitializeModules( const std::string& searchDirectory );

  REFLECT_API void Cleanup();
  REFLECT_API void CleanupModules();
  REFLECT_API void FreeModules();

  REFLECT_API Profile::MemoryPoolHandle MemoryPool();

  class REFLECT_API Registry
  {
  private:
    friend void Reflect::Initialize();
    friend bool Reflect::IsInitialized();
    friend void Reflect::Cleanup();

    M_IDToType m_TypesByID;
    M_StrToType m_TypesByName;
    M_StrToType m_TypesByAlias;

    u32 m_InitThread;
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
    void AliasType (const Type* type, const std::string& alias);
    void UnAliasType (const Type* type, const std::string& alias);

    // retrieves type info
    const Type* GetType(int id) const;
    const Type* GetType(const std::string& str) const;

    // for threading safely
    void AtomicGetType(int id, const Type** addr) const;
    void AtomicGetType(const std::string& str, const Type** addr) const;

    // class lookup
    inline const Class* GetClass(i32 id) const
    {
      return ReflectionCast<const Class>(GetType( id ));
    }
    inline const Class* GetClass(const std::string& str) const
    {
      return ReflectionCast<const Class>(GetType( str ));
    }

    // enumeration lookup
    inline const Enumeration* GetEnumeration(i32 id) const
    {
      return ReflectionCast<const Enumeration>(GetType( id ));
    }
    inline const Enumeration* GetEnumeration(const std::string& str) const
    {
      return ReflectionCast<const Enumeration>(GetType( str ));
    }

    // create instances of classes
    ObjectPtr CreateInstance(int id) const;
    ObjectPtr CreateInstance(const Class* type) const;
    ObjectPtr CreateInstance(const std::string& str) const;

    template<class T>
    Nocturnal::SmartPtr< T > CreateInstance()
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
    void TrackCreate(PointerSizedUInt ptr);
    void TrackDelete(PointerSizedUInt ptr);
    void TrackCheck(PointerSizedUInt ptr);
    void TrackDump();
#endif
  };

  //
  // These inline templates actually cache out their result (per translation unit)
  //  and are generally preferrable to calling into the Registry every time you need something
  //

  template<class T>
  inline i32 GetType()
  {
    static i32 cached = ReservedTypes::Invalid;

    if ( cached != ReservedTypes::Invalid )
    {
      return cached;
    }

    const Type* type = NULL;
    Registry::GetInstance()->AtomicGetType( typeid( T ).name(), &type );
    NOC_ASSERT(type); // if you hit this then your type is not registered

    if ( type )
    {
      static IDTracker tracker; 
      tracker.Set( type, &cached );

      return cached = type->m_TypeID;
    }
    else
    {
      return ReservedTypes::Invalid;
    }
  }

  template<class T>
  inline const Class* GetClass()
  {
    static const Class* cached = NULL;
    if ( cached != NULL )
    {
      return cached;
    }

    const Type* type = NULL;
    Registry::GetInstance()->AtomicGetType( typeid( T ).name(), &type );
    NOC_ASSERT(type); // if you hit this then your type is not registered

    if ( type )
    {
      static TypeTracker<const Class*> tracker; 
      tracker.Set( type, &cached ); 

      return cached = ReflectionCast<const Class>( type );
    }
    else
    {
      return NULL;
    }
  }

  template<class T>
  inline const Enumeration* GetEnumeration()
  {
    static const Enumeration* cached = NULL;
    if ( cached != NULL )
    {
      return cached;
    }

    const Type* type = NULL;
    Registry::GetInstance()->AtomicGetType( typeid( T ).name(), &type );
    NOC_ASSERT(type); // if you hit this then your type is not registered

    if ( type )
    {
      static TypeTracker<const Enumeration*> tracker; 
      tracker.Set( type, &cached );

      return cached = ReflectionCast<const Enumeration>(type);
    }
    else
    {
      return NULL;
    }
  }

  //
  // Registration templates, these help with creating and registering classes with the registry
  //

  typedef void (*UnregisterFunc)();

  template<class T>
  inline UnregisterFunc RegisterClass(const std::string& shortName = "")
  {
    // create the type information and register it with the registry
    if ( Reflect::Registry::GetInstance()->RegisterType( T::CreateClass( shortName ) ) )
    {
      // this function will unregister the type we just registered
      return &UnregisterClass<T>;
    }

    // there was a problem
    return NULL;
  }

  template<class T>
  inline void UnregisterClass()
  {
    // retrieve the class information and unregister it from the registry
    Reflect::Registry::GetInstance()->UnregisterType( Reflect::GetClass<T>() );
  }

  typedef void EnumerateEnumerationFunc( Reflect::Enumeration* info );

  template<class T>
  inline UnregisterFunc RegisterEnumeration(EnumerateEnumerationFunc enumerate, const std::string& shortName = "")
  {
    Reflect::Enumeration* info = Reflect::Enumeration::Create<T>( shortName );

    // defer to this function
    enumerate( info );

    // create the type information and register it with the registry
    if ( Reflect::Registry::GetInstance()->RegisterType( info ) )
    {
      // this function will unregister the type we just registered
      return &UnregisterEnumeration<T>;
    }

    // there was a problem
    return NULL;
  }

  template<class T>
  inline void UnregisterEnumeration()
  {
    // retrieve the class information and unregister it from the registry
    Reflect::Registry::GetInstance()->UnregisterType( Reflect::GetEnumeration<T>() );
  }
}

//
// This defines the entry points for modules that want to expose thier types dynamically
//

#define REFLECT_DEFINE_ENTRY_POINTS(__Initialize, __Cleanup)        \
  extern "C" __declspec(dllexport) void __InitializeReflectModule() \
  {                                                                 \
    __Initialize();                                                 \
  }                                                                 \
                                                                    \
  extern "C" __declspec(dllexport) void __CleanupReflectModule()    \
  {                                                                 \
    __Cleanup();                                                    \
  }
