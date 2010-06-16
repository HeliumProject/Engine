#include "List.h"
#include "Application/Inspect/Widgets/Canvas.h"

#include "Foundation/String/Tokenize.h"

using namespace Reflect;
using namespace Inspect;

const char* List::s_MapKeyValDelim = ", "; 


///////////////////////////////////////////////////////////////////////////////
// Local class wrapping the wxListBox.
// 
class ListBox : public wxListBox
{
public:
  List* m_ListBox;

  ListBox (wxWindow* parent, List* listBox, long style )
    : wxListBox (parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, style)
    , m_ListBox (listBox)
  {

  }


  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(ListBox, wxListBox)
END_EVENT_TABLE();

///////////////////////////////////////////////////////////////////////////////
// 
// 
List::List()
: m_Sorted( false )
, m_IsMap( false )
{
  m_FixedHeight = true;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void List::Realize(Container* parent)
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  if (m_Window != NULL)
    return;

  m_Window = new ListBox(parent->GetWindow(), this, ( m_Sorted ? wxLB_SORT : 0 ) | wxLB_SINGLE | wxLB_HSCROLL);
  
  wxSize size( -1, m_Canvas->GetStdSize(Math::SingleAxes::Y) * 5 );
  m_Window->SetSize( size );
  m_Window->SetMinSize( size );
  m_Window->SetMaxSize( size );

  __super::Realize(parent);

  if ( !m_Items.empty() )
  {
    AddItems( m_Items );
  }

  if ( !m_SelectedItems.empty() )
  {
    SetSelectedItems( m_SelectedItems );
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void List::Read()
{
  // from data into ui
  if ( IsRealized() )
  {
    const V_string& items = GetItems();

    UpdateUI( items );
  }

  __super::Read();
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
bool List::Write()
{
  bool result = false;

  if ( IsRealized() )
  {
    std::string delimited;
    ListBox* list = Control::Cast< ListBox >( this );
    const i32 total = list->GetCount();
    m_Items.clear();
    m_Items.resize( total );

    for ( i32 index = 0; index < total; ++index )
    {
      if ( !delimited.empty() )
      {
        delimited += Reflect::s_ContainerItemDelimiter;
      }

      const std::string val = list->GetString( index ).c_str();

      if ( m_IsMap )
      {
        std::string::size_type pos = val.find( s_MapKeyValDelim );

        // This is suppose to be a map, there better be a key-value pair
        NOC_ASSERT( pos != std::string::npos );
        if ( std::string::npos != pos )
        {
          delimited += val.substr( 0, pos ) + Reflect::s_ContainerItemDelimiter;
          delimited += val.substr( pos + strlen( s_MapKeyValDelim ) );
        }
      }
      else
      {
        delimited += val;
      }

      m_Items[index] = val;
    }

    if ( IsBound() )
    {
      if ( WriteData( delimited ) )
      {
        result = __super::Write();
      }
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to specify whether the list will automatically sort or not.  You
// must call this function before the control is realized in order for it to 
// have any effect.
// 
void List::SetSorted( bool sort )
{
  NOC_ASSERT( !IsRealized() );
  m_Sorted = sort;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void List::SetMap( bool isMap )
{
  NOC_ASSERT( !IsRealized() );
  m_IsMap = isMap;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a list of all the items (represented as strings).
// 
const V_string& List::GetItems()
{
  if ( IsBound() )
  {
    m_Items.clear();
    std::string str;
    ReadData( str );
    ::Tokenize( str, m_Items, Reflect::s_ContainerItemDelimiter );

    if ( m_IsMap )
    {
      // This list is a map representation so it better have an even number of elemnts
      NOC_ASSERT( m_Items.size() % 2 == 0 );

      V_string::iterator itr = m_Items.begin();
      V_string::iterator previous = itr;
      for ( ; itr != m_Items.end(); ++itr )
      {
        if ( previous != itr )
        {
          *previous = ( *previous ) + s_MapKeyValDelim + ( *itr );
          itr = m_Items.erase( itr );
          previous = itr;

          if ( itr == m_Items.end() )
          {
            break;
          }
        }
      }
    }
  }
  return m_Items;
}

///////////////////////////////////////////////////////////////////////////////
// Clears all the items out of the list (both the underlying data and the UI)
// and adds the new items to the list.  
// 
void List::AddItems( const V_string& items )
{
  if ( IsBound() )
  {
    std::string str = GetDelimitedList( items, Reflect::s_ContainerItemDelimiter );
    WriteData( str );
  }
  else
  {
    if ( m_Items != items )
    {
     m_Items = items;
    }
  }

  UpdateUI( items );
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void List::AddItem( const std::string& item )
{
  if ( IsBound() )
  {
    std::string str;
    ReadData( str );

    if ( !str.empty() )
    {
      str += Reflect::s_ContainerItemDelimiter;
    }
    str += item;

    WriteData( str );

    m_Items.clear();
    ::Tokenize( str, m_Items, s_ContainerItemDelimiter );
  }
  else
  {
    m_Items.push_back( item );
  }

  UpdateUI( m_Items );
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void List::RemoveItem( const std::string& item )
{
  bool uiNeedsUpdate = false;

  if ( IsBound() )
  {
    std::string delimited;
    ReadData( delimited );

    // Search for item + delimiter
    std::string search = item + Reflect::s_ContainerItemDelimiter;
    std::string::size_type pos = delimited.find( search );
    if ( pos == std::string::npos )
    {
      // Not found, search for delimiter + item
      search = Reflect::s_ContainerItemDelimiter + item;
      pos = delimited.find( search );
      if ( pos == std::string::npos )
      {
        // Not found, search for the item with no delimiter (this would occur if the
        // item we are removing is the only item in the list).
        search = item;
        pos = delimited.find( search );
      }
    }
    
    // If we found the item to remove, take it out of the string
    if ( pos != std::string::npos )
    {
      delimited.replace( pos, search.size(), "" );
      WriteData( delimited );

      m_Items.clear();
      ::Tokenize( delimited, m_Items, Reflect::s_ContainerItemDelimiter );
      uiNeedsUpdate = true;
    }
  }
  else
  {
    V_string::iterator itr = m_Items.begin();
    V_string::iterator end = m_Items.end();
    for ( ; itr != end; ++itr )
    {
      const std::string& current = *itr;
      if ( current == item )
      {
        m_Items.erase( itr );
        uiNeedsUpdate = true;
        break;
      }
    }
  }

  if ( uiNeedsUpdate )
  {
    UpdateUI( m_Items );
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
const V_string& List::GetSelectedItems()
{
  if ( IsRealized() )
  {
    ListBox* list = Control::Cast<ListBox>( this );
    wxArrayInt indices;
    const i32 numIndices = list->GetSelections( indices );

    m_SelectedItems.clear();
    m_SelectedItems.resize( numIndices );

    for ( i32 index = 0; index < numIndices; ++index )
    {
      m_SelectedItems[index] = list->GetString( indices[index] ).c_str();
    }
  }

  return m_SelectedItems;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void List::SetSelectedItems( const V_string& items )
{
  if ( IsRealized() )
  {
    ListBox* list = Control::Cast<ListBox>( this );

    // Deselect everything
    list->SetSelection( wxNOT_FOUND );

    m_Window->Freeze();
    // Select each item in the list
    V_string::const_iterator itr = items.begin();
    V_string::const_iterator end = items.end();
    for ( ; itr < end; ++itr )
    {
      list->SetStringSelection( (*itr).c_str() );
    }
    m_Window->Thaw();
  }
  else
  {
    m_SelectedItems = items;
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
std::string List::GetSelectedItems( const std::string delimiter )
{
  std::string items;

  if ( IsRealized() )
  {
    ListBox* list = Control::Cast<ListBox>( this );
    wxArrayInt indices;
    i32 numIndices = list->GetSelections( indices );
    for ( i32 index = 0; index < numIndices; ++index )
    {
      if ( !items.empty() )
      {
        items += delimiter;
      }
      items += list->GetString( index ).c_str();
    }
  }
  else
  {
    V_string::const_iterator itr = m_SelectedItems.begin();
    V_string::const_iterator end = m_SelectedItems.end();
    for ( ; itr != end; ++itr )
    {
      if ( !items.empty() )
      {
        items += delimiter;
      }
      items += *itr;
    }
  }

  return items;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the selected items.  If the control has been realized, it's selection
// list will change.  Otherwise, the selected items are cached until the control
// is realized.  
// 
// NOTE: This function is slower than calling the other SetSelectedItems function
// above.  It's just provided for convenience.
// 
void List::SetSelectedItems( const std::string& delimitedList, const std::string& delimiter )
{
  V_string items;
  ::Tokenize( delimitedList, items, delimiter );
  SetSelectedItems( items );
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to select one or more items.
// 
static inline void SetSelection( ListBox* list, const V_i32 indices )
{
  V_i32::const_iterator indexItr = indices.begin();
  V_i32::const_iterator indexEnd = indices.end();
  for ( ; indexItr != indexEnd; ++indexItr )
  {
    list->Select( *indexItr );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Moves each of the selected items up in the list if possible.
// 
void List::MoveSelectedItems( MoveDirection direction )
{
  bool isDirty = false;
  const V_string& selectedItems = GetSelectedItems();
  V_i32 selectedItemIndices;

  const size_t numSelectedItems = selectedItems.size();
  if ( numSelectedItems > 0 )
  {
    i32 currentSelection = 0;
    const size_t numItems = m_Items.size();
    for ( size_t itemIndex = 0; itemIndex < numItems; ++itemIndex )
    {
      std::string& current = m_Items[itemIndex];
      if ( current == selectedItems[currentSelection] )
      {
        // Move the item up in the list if possible.
        if ( direction == MoveDirections::Up && itemIndex > 0 )
        {
          std::string temp = m_Items[itemIndex-1];
          m_Items[itemIndex-1] = m_Items[itemIndex];
          m_Items[itemIndex] = temp;
          isDirty = true;
          selectedItemIndices.push_back( static_cast< i32 >( itemIndex ) - 1 );
        }
        else if ( direction == MoveDirections::Down && itemIndex + 1 < numItems )
        {
          std::string temp = m_Items[itemIndex+1];
          m_Items[itemIndex+1] = m_Items[itemIndex];
          m_Items[itemIndex] = temp;
          isDirty = true;
          selectedItemIndices.push_back( static_cast< i32 >( itemIndex ) + 1 );
        }

        // Advance selection and quit if we have no more selected items
        ++currentSelection;
        if ( currentSelection >= static_cast< i32 >( numSelectedItems ) )
        {
          break;
        }
      }
    }
  }

  // TODO: This is brutal, we should probably not rebuild the whole control.

  if ( isDirty && IsRealized() )
  {
    ListBox* list = Control::Cast< ListBox >( this );
    list->Freeze();

    AddItems( m_Items );

    // Restore the selected items
    SetSelection( list, selectedItemIndices );

    list->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
std::string List::GetDelimitedList( const V_string& items, const std::string& delimiter )
{
  std::string delimitedList;

  V_string::const_iterator itr = items.begin();
  V_string::const_iterator end = items.end();
  for ( ; itr != end; ++itr )
  {
    if ( !delimitedList.empty() )
    {
      delimitedList += delimiter;
    }
    if ( m_IsMap )
    {
      // Replace our delimiters with the appropriate ones for the container
      std::string::size_type pos = ( *itr ).find_first_of( s_MapKeyValDelim );
      std::string::size_type lastPos = ( *itr ).find_first_not_of( s_MapKeyValDelim, pos );
      if ( pos != std::string::npos )
      {
        delimitedList += ( *itr ).substr( 0, pos ) + Reflect::s_ContainerItemDelimiter + ( *itr ).substr( lastPos );
      }
    }
    else
    {
      delimitedList += *itr;
    }
  }
  
  return delimitedList;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void List::UpdateUI( const V_string& items )
{
  if ( IsRealized() )
  {
    // Clear list box
    ListBox* list = Control::Cast< ListBox >( this );
    list->Freeze();
    list->Clear();

    V_string::const_iterator itr = items.begin();
    V_string::const_iterator end = items.end();
    for ( ; itr != end; ++itr )
    {
      list->Append( (*itr).c_str() );
    }
    list->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Process script commands defining how this list box should behave.
// 
bool List::Process(const std::string& key, const std::string& value)
{
  if (__super::Process(key, value))
    return true;

  if (key == LIST_ATTR_SORTED)
  {
    if (value == ATTR_VALUE_TRUE)
    {
      m_Sorted = true;
      return true;
    }
    else if (value == ATTR_VALUE_FALSE)
    {
      m_Sorted = false;
      return true;
    }
  }

  return false;
}
