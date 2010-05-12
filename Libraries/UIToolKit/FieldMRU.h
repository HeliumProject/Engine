#pragma once

#include "API.h"
#include "MRU.h"

#include "Common/Assert.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Container/OrderedSet.h"

//
// Forwards
//
class wxControlWithItems;

namespace UIToolKit
{
  /////////////////////////////////////////////////////////////////////////////
  // System for managing the global MRU FieldMap 
  //
  class UITOOLKIT_API FieldMRU : public Nocturnal::RefCountBase<FieldMRU>
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

  typedef UITOOLKIT_API Nocturnal::SmartPtr< FieldMRU > FieldMRUPtr;

}
