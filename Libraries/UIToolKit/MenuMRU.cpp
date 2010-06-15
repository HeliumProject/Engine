#include "stdafx.h"
#include "MenuMRU.h"

#include "Foundation/File/Path.h"

using namespace UIToolKit;

typedef std::map< i32, std::string> M_MenuItemIDToString;

class MenuMRUEvtHandler : public wxEvtHandler
{
public:
  wxMenu*               m_Menu;
  M_MenuItemIDToString  m_MenuItemIDToString;
  MRUSignature::Event   m_ItemSelected;

  MenuMRUEvtHandler()
    : m_Menu( NULL )
  {
  }

  virtual ~MenuMRUEvtHandler()
  {
    M_MenuItemIDToString::iterator itr = m_MenuItemIDToString.begin();
    M_MenuItemIDToString::iterator end = m_MenuItemIDToString.end();
    for ( ; itr != end ; ++itr )
    {
      Disconnect( itr->first, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MenuMRUEvtHandler::OnMRUMenuItem ) );
    }
  }

  void SetMenu( wxMenu* menu, const OS_string& items )
  {
    // protect the MRU so that it is only tied to one menu
    if ( m_Menu != NULL )
    {
      NOC_ASSERT( !m_Menu || m_Menu == menu );
    }

    m_Menu = menu;

    m_MenuItemIDToString.clear();

    // Clear out the old menu items
    while ( m_Menu->GetMenuItemCount() > 0 )
    {
      m_Menu->Delete( *( m_Menu->GetMenuItems().begin() ) );
    }

    // Build a new list of menu items from the MRU
    OS_string::ReverseIterator mruItr = items.ReverseBegin();
    OS_string::ReverseIterator mruEnd = items.ReverseEnd();
    for ( ; mruItr != mruEnd; ++mruItr )
    {
      const std::string& item = *mruItr;

      wxMenuItem* menuItem = menu->Append( wxID_ANY, item.c_str() );
      Connect( menuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MenuMRUEvtHandler::OnMRUMenuItem ), NULL, this );
      m_MenuItemIDToString.insert( M_MenuItemIDToString::value_type( menuItem->GetId(), item ) );
    }
  }

  void OnMRUMenuItem( wxCommandEvent& args )
  {
    M_MenuItemIDToString::iterator findMenuItem = m_MenuItemIDToString.find( args.GetId() );
    if ( findMenuItem != m_MenuItemIDToString.end() ) 
    {
      m_ItemSelected.Raise( MRUArgs( findMenuItem->second ) );
    }
  }

};

///////////////////////////////////////////////////////////////////////////////
// Constructor
// maxItems - The total number of items to maintain in the list.
// 
MenuMRU::MenuMRU( i32 maxItems, wxWindow* owner )
: MRU< std::string >( maxItems )
, m_Owner( owner )
{
  m_MenuMRUEvtHandler = new MenuMRUEvtHandler();

  m_Owner->PushEventHandler( m_MenuMRUEvtHandler );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
MenuMRU::~MenuMRU()
{
  m_Owner->PopEventHandler();

  delete m_MenuMRUEvtHandler;
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the MRU items and make sure that all the files exist.  If 
// the tuidRequired flag is specified, each file will also be looked up to 
// ensure that it is in the file resolver.  Any invalid files are removed
// from the MRU.
// 
void MenuMRU::RemoveInvalidItems( bool tuidRequired )
{
  S_string remove; // Lame, we should fix this
  OS_OrderedTypeSet::Iterator mruItr = m_OrderedSet.Begin();
  OS_OrderedTypeSet::Iterator mruEnd = m_OrderedSet.End();

  for ( ; mruItr != mruEnd; ++mruItr )
  {
    const std::string& current = *mruItr;

    // Empty file paths are not allowed
    if ( current.empty() )
    {
      remove.insert( current );
      continue;
    }

    // Check to make sure the file exists on disk
    bool exists = Nocturnal::Path( current ).Exists();

    if ( !exists )
    {
      remove.insert( current );
      continue;
    }

#pragma TODO("UIToolKit doesn't reference File dll")
    // If a TUID is required, make sure that we can find a tuid for this file.
    //if ( tuidRequired )
    //{
    //  tuid fileID = TUID::Null;
    //  try
    //  {
    //    fileID = File::GlobalManager().GetID( current );
    //  }
    //  catch ( const File::Exception& )
    //  {
    //    fileID = TUID::Null;
    //  }

    //  if ( fileID == TUID::Null )
    //  {
    //    remove.insert( current );
    //    continue;
    //  }
    //}
  }

  // Remove all the bad items
  S_string::const_iterator removeItr = remove.begin();
  S_string::const_iterator removeEnd = remove.end();
  for ( ; removeItr != removeEnd; ++removeItr )
  {
    Remove( *removeItr );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds the listener to the event handler
//
void MenuMRU::AddItemSelectedListener( const MRUSignature::Delegate& listener )
{
  m_MenuMRUEvtHandler->m_ItemSelected.Add( listener );
}

///////////////////////////////////////////////////////////////////////////////
// Removes the listener to the event handler
// 
void MenuMRU::RemoveItemSelectedListener( const MRUSignature::Delegate& listener )
{
  m_MenuMRUEvtHandler->m_ItemSelected.Remove( listener );
}

///////////////////////////////////////////////////////////////////////////////
// Populates the specified menu with the MRU items.  The caller can register 
// with the MRU to be notified when one of the items is selected.
// 
void MenuMRU::PopulateMenu( wxMenu* menu )
{
  // Remove any items that are not valid file paths.  Note: An option might
  // need to be passed into this function so we know whether or not to
  // require TUIDs for all items in the MRU.  For now, just allow any valid
  // file paths, and don't require TUIDs.
  RemoveInvalidItems( false );

  m_MenuMRUEvtHandler->SetMenu( menu, GetItems() );
}
