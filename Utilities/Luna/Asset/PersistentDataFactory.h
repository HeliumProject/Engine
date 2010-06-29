#pragma once
#include "PersistentData.h"

namespace Luna
{
  // Forwards
  class AssetManager;

  // Function prototype for creating an Luna::PersistentData-derived object.
  typedef Luna::PersistentDataPtr ( *CreatePersistentFunc )( Reflect::Element*, Luna::AssetManager* );

  /////////////////////////////////////////////////////////////////////////////
  // Factory for creating Luna::PersistentData-derived objects.
  // 
  class PersistentDataFactory
  {
  private:
    typedef std::map< i32, CreatePersistentFunc > M_CreatorFuncs;
    M_CreatorFuncs m_Funcs;

  private:
    PersistentDataFactory();

  public:
    ~PersistentDataFactory();

    // Singleton access
    static PersistentDataFactory* GetInstance();

    // Register a creator function with a specific type ID.
    void Register( i32 elementDerivedTypeID, CreatePersistentFunc func );

    // Create an Luna::PersistentData using a previously registered function.
    Luna::PersistentDataPtr Create( Reflect::Element* element, Luna::AssetManager* assetManager );

    // Helper function to create a derived type of Luna::PersistentData.
    template< class T >
    Nocturnal::SmartPtr< T > CreateTyped( Reflect::Element* element, Luna::AssetManager* assetManager )
    {
      return Reflect::AssertCast< T >( Create( element, assetManager ) );
    }

  private:
    CreatePersistentFunc GetFunction( const Reflect::Class* packageClass ) const;
  };
}
