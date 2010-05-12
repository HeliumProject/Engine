#include "Precompile.h"
#include "PersistentDataFactory.h"
#include "Common/Container/Insert.h" 

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
PersistentDataFactory::PersistentDataFactory()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
PersistentDataFactory::~PersistentDataFactory()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the one and only instance of this class.
// 
PersistentDataFactory* PersistentDataFactory::GetInstance()
{
  static PersistentDataFactory thePersistentDataFactory;
  return &thePersistentDataFactory;
}

///////////////////////////////////////////////////////////////////////////////
// Registers a creation function with the type ID for the persistent data.
// Basically, this maps a creation function for a Luna wrapper object (derived
// from Luna::PersistentData) to the type ID of the underlying Reflect::Element.
// 
void PersistentDataFactory::Register( i32 elementDerivedTypeID, CreatePersistentFunc func )
{
  Nocturnal::Insert< M_CreatorFuncs >::Result inserted = m_Funcs.insert( M_CreatorFuncs::value_type( elementDerivedTypeID, func ) );
  if ( !inserted.second && inserted.first->second != func )
  {
    // Attempt to re-register type ID to a different function
    throw Nocturnal::Exception( "PersistentDataFactory: Attempted to re-register type ID '%d'.", elementDerivedTypeID );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Creates the proper Luna::PersistentData-derived object to wrap the specified package.
// 
Luna::PersistentDataPtr PersistentDataFactory::Create( Reflect::Element* element, Luna::AssetManager* assetManager )
{
  Luna::PersistentDataPtr wrapper;
  CreatePersistentFunc func = GetFunction( element->GetClass() );
  if ( func )
  {
    wrapper = ( *func )( element, assetManager );
    if ( wrapper.ReferencesObject() )
    {
      wrapper->Unpack();
    }
    else
    {
      throw Nocturnal::Exception( "PersistentDataFactory: Failed to create a node to wrap element '%s' (typeID=%d).", element->GetTitle().c_str(), element->GetType() );
    }
  }
  else
  {
    throw Nocturnal::Exception( "PersistentDataFactory: No creator function found for element '%s' (typeID=%d).", element->GetTitle().c_str(), element->GetType() );
  }

  return wrapper;
}

///////////////////////////////////////////////////////////////////////////////
// Recursive function to find the creation function for a particular type.  
// Uses the Reflect type information to search up the inheritance chain for
// the first base class that has a creator function in this factory.  Returns
// the function if found, or NULL if no match could be found.
// 
CreatePersistentFunc PersistentDataFactory::GetFunction( const Reflect::Class* packageClass ) const
{
  CreatePersistentFunc func = NULL;

  M_CreatorFuncs::const_iterator found = m_Funcs.find( packageClass->m_TypeID );
  if ( found != m_Funcs.end() )
  {
    func = found->second;
  }
  else if ( !packageClass->m_Base.empty() )
  {
    const Reflect::Class* parentClass = Reflect::Registry::GetInstance()->GetClass( packageClass->m_Base );
    if ( parentClass )
    {
      func = GetFunction( parentClass );
    }
  }

  return func;
}
