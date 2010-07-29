#pragma once

#include <map>

#include "Platform/Assert.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Container/OrderedSet.h"
#include "Application/API.h"

#include "MRU.h"

//
// Forwards
//
class wxControlWithItems;

namespace Helium
{
  /////////////////////////////////////////////////////////////////////////////
  // System for managing the global MRU FieldMap 
  //
  class APPLICATION_API FieldMRU : public Helium::RefCountBase<FieldMRU>
  {
  public:
    FieldMRU()
    {
      Initialize();
    }

    ~FieldMRU()
    {
      Cleanup();
    }

    void Initialize();
    void Cleanup();

    ManagedStringSetPtr GetFieldItems( const tstring& fieldKey, const tstring& defaultValue = TXT( "" ), const bool autoInit = true );
    bool AddItem( wxControlWithItems* control, const tstring& fieldKey, const tstring& value );
    void PopulateControl( wxControlWithItems* control, const tstring& fieldKey, const tstring& defaultValue = TXT( "" ), const bool autoInit = true );  

  private:
    // Mapping of String field name to MostRecentlyUsed
    typedef std::map< tstring, ManagedStringSetPtr > M_ManagedStringSet;

    // Mapping of the most recently used field values
    M_ManagedStringSet  m_Fields;

  };

  typedef APPLICATION_API Helium::SmartPtr< FieldMRU > FieldMRUPtr;

}
