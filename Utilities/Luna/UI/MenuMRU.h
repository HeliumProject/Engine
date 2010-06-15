#pragma once

#include "Luna/API.h"
#include "MRU.h"

#include "Foundation/Automation/Event.h"
#include "Platform/Types.h"

// 
// Forwards
// 
class MenuMRUEvtHandler;

namespace Luna
{
  // Arguements and events for changes to the MRU.
  struct MRUArgs
  {
    std::string m_Item;
    
    MRUArgs( const std::string& item )
      : m_Item( item )
    {
    }
  };
  typedef Nocturnal::Signature< void, const MRUArgs& > MRUSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Class for managing a list of "most recently used" items.  The items are
  // strings (paths to files).  The MRU can be saved to and restored from
  // the registry.  In order to be useful, a piece of UI would need to display
  // the MRU and allow choosing items.
  // 
  class LUNA_CORE_API MenuMRU : public MRU< std::string >
  {
  public:
    MenuMRU( i32 maxItems, wxWindow* owner );
    virtual ~MenuMRU();

    void RemoveInvalidItems( bool tuidRequired = false );
    
    void AddItemSelectedListener( const MRUSignature::Delegate& listener );
    void RemoveItemSelectedListener( const MRUSignature::Delegate& listener );
    void PopulateMenu( wxMenu* menu );

  private:
    wxWindow* m_Owner;
    MenuMRUEvtHandler* m_MenuMRUEvtHandler;
  };

  typedef Nocturnal::SmartPtr< MenuMRU > MenuMRUPtr;
}
