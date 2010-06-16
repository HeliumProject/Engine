#pragma once

#include "Application/API.h"
#include "Foundation/Automation/Event.h"
#include "Platform/Types.h"
#include <wx/listctrl.h>

namespace Nocturnal
{
  // Forwards
  class ListView;

  /////////////////////////////////////////////////////////////////////////////
  // Event arguments and delegate for events that refer to selection/deselection
  // in the list.  Provides some missing features to the list view, such as the
  // ability to veto when a selection change is about to happen.
  // 
  struct ListSelectionChangeArgs
  {
    ListView* m_List; // List that fired the event
    i32 m_Index;      // Index/id of the item whose selection changed
    i32 m_Data;       // Data attached to the list item whose selection changed
    bool m_Select;    // True if a select event, otherwise a deselect event
    bool m_Veto;      // Callbacks should set this to true if the want to veto a selection changing event

    ListSelectionChangeArgs( ListView* list, bool select, i32 index, i32 data, bool veto = false )
    : m_List( list )
    , m_Index( index )
    , m_Data( data )
    , m_Select( select )
    , m_Veto( veto )
    {
    }
  };

  typedef Nocturnal::Signature< void, ListSelectionChangeArgs& > ListSelectionChangingSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Event arguments and delegate for events that indicate a change in a list
  // control.
  // 
  struct ListChangeArgs
  {
    ListView* m_List;

    ListChangeArgs( ListView* list )
    : m_List( list )
    {
    }
  };

  typedef Nocturnal::Signature< void, const ListChangeArgs& > ListSelectionChangedSignature;


  /////////////////////////////////////////////////////////////////////////////
  // Extends the wxListView to provide additional functionality including:
  // - The ability to veto selection changing events (for selection and 
  //   deselection of objects.
  // - An event to batch together all of the selection updates that happen
  //   due to one mouse-down event.  This way, consumers of this class do not
  //   have to NOC_OVERRIDE the normal wx-selection changed events.  They can
  //   subscribe to the SelectionChanged event provided here and just get one
  //   event at the end of a selection range.
  // 
  class APPLICATION_API ListView : public wxListView
  {
  private:
    bool m_SelectionDirty;
    ListSelectionChangingSignature::Event m_SelectionChanging;
    ListSelectionChangedSignature::Event m_SelectionChanged;

  public:
    ListView();
    ListView( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "ListView" );
    virtual ~ListView();
    virtual WXLRESULT MSWWindowProc( WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam );
    virtual void DeselectAll();

  private:
    void OnItemSelected( wxListEvent& args );
    void OnItemDeselected( wxListEvent& args );

  public:
    void AddSelectionChangingListener( const ListSelectionChangingSignature::Delegate& listener );
    void RemoveSelectionChangingListener( const ListSelectionChangingSignature::Delegate& listener );
    void AddSelectionChangedListener( const ListSelectionChangedSignature::Delegate& listener );
    void RemoveSelectionChangedListener( const ListSelectionChangedSignature::Delegate& listener );

  private:
    DECLARE_DYNAMIC_CLASS( ListView )

  };
}
