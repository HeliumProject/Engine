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

namespace Nocturnal
{
  /////////////////////////////////////////////////////////////////////////////
  // System for managing the global MRU FieldMap 
  //
  class APPLICATION_API FieldMRU : public Nocturnal::RefCountBase<FieldMRU>
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

    ManagedStringSetPtr GetFieldItems( const std::string& fieldKey, const std::string& defaultValue = "", const bool autoInit = true );
    bool AddItem( wxControlWithItems* control, const std::string& fieldKey, const std::string& value );
    void PopulateControl( wxControlWithItems* control, const std::string& fieldKey, const std::string& defaultValue = "", const bool autoInit = true );  

  private:
    // Mapping of String field name to MostRecentlyUsed
    typedef std::map< std::string, ManagedStringSetPtr > M_ManagedStringSet;

    // Mapping of the most recently used field values
    M_ManagedStringSet  m_Fields;

  };

  typedef APPLICATION_API Nocturnal::SmartPtr< FieldMRU > FieldMRUPtr;

}
