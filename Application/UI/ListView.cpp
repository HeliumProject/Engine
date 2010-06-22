#include "ListView.h"
#include "Foundation/Log.h"

using namespace Nocturnal;

IMPLEMENT_DYNAMIC_CLASS( ListView, wxListView )

// needed for some crap below, but must come after the macro called above... shitty
#include "Platform/Windows/Windows.h"

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ListView::ListView()
: m_SelectionDirty( false )
{
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ListView::ListView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name )
: wxListView( parent, id, pos, size, style, validator, name )
, m_SelectionDirty( false )
{
  Connect( GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ListView::OnItemSelected ), NULL, this );
  Connect( GetId(), wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( ListView::OnItemDeselected ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ListView::~ListView()
{
  Disconnect( GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ListView::OnItemSelected ), NULL, this );
  Disconnect( GetId(), wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( ListView::OnItemDeselected ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden so that mouse down events can be detected, and batches of selection
// changes can be accumulated into one event.
// 
WXLRESULT ListView::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
  bool batchSelection = ( nMsg == WM_LBUTTONDOWN );

  if ( batchSelection )
  {
    m_SelectionDirty = false;
  }

  WXLRESULT result = __super::MSWWindowProc( nMsg, wParam, lParam );

  // The base class implementation handles the mouse down event and updates
  // the selection.  If the selection has changed, notify interested listeners.
  if ( batchSelection && m_SelectionDirty )
  {
    m_SelectionChanged.Raise( ListChangeArgs( this ) );
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Deselect all the selected items in this list control.  "Selection changing"
// events will be fired, but "selection changed" events will not.
// 
void ListView::DeselectAll()
{
  long id = GetFirstSelected();
  while ( id >= 0 )
  {
    Select( id, false );
    id = GetNextSelected( id );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an item is selected.  Fires a selection changing event to
// interested listeners, and reverts the change if listeners decide to veto the
// event.
// 
void ListView::OnItemSelected( wxListEvent& args )
{
  m_SelectionDirty = true;

  ListSelectionChangeArgs changing( this, true, args.GetIndex(), args.GetData() );
  m_SelectionChanging.Raise( changing );
  if ( changing.m_Veto )
  {
    Disconnect( GetId(), wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( ListView::OnItemDeselected ), NULL, this );
    Select( args.GetIndex(), false );
    Connect( GetId(), wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( ListView::OnItemDeselected ), NULL, this );
  }

  args.Skip();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an item is deselected.  Fires a selection changing event to
// interested listeners, and reverts the change if listeners decide to veto the
// event.
// 
void ListView::OnItemDeselected( wxListEvent& args )
{
  m_SelectionDirty = true;

  ListSelectionChangeArgs changing( this, false, args.GetIndex(), args.GetData() );
  m_SelectionChanging.Raise( changing );
  if ( changing.m_Veto )
  {
    Disconnect( GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ListView::OnItemSelected ), NULL, this );
    Select( args.GetIndex(), true );
    Connect( GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ListView::OnItemSelected ), NULL, this );
  }

  args.Skip();
}

///////////////////////////////////////////////////////////////////////////////
// Add a listener for when the selection is changing.
// 
void ListView::AddSelectionChangingListener( const ListSelectionChangingSignature::Delegate& listener )
{
  m_SelectionChanging.Add( listener );
}

///////////////////////////////////////////////////////////////////////////////
// Remove a listener for selection changing events.
// 
void ListView::RemoveSelectionChangingListener( const ListSelectionChangingSignature::Delegate& listener )
{
  m_SelectionChanging.Remove( listener );
}

///////////////////////////////////////////////////////////////////////////////
// Add a listener for selection changed events.  One selection change event may
// be fired to indicate a whole group of selection changes.  When notified,
// consumers can iterate over the list and figure out which items are selected.
// 
void ListView::AddSelectionChangedListener( const ListSelectionChangedSignature::Delegate& listener )
{
  m_SelectionChanged.Add( listener );
}

///////////////////////////////////////////////////////////////////////////////
// Remove a listener for selection changed events.
// 
void ListView::RemoveSelectionChangedListener( const ListSelectionChangedSignature::Delegate& listener )
{
  m_SelectionChanged.Remove( listener );
}
