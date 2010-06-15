#include "Precompile.h"
#include "SortableListView.h"
#include "Platform/Types.h"
#include "Foundation/String/Natural.h"
#include "Foundation/Container/Insert.h" 

using Nocturnal::Insert; 
using namespace Luna;

IMPLEMENT_DYNAMIC_CLASS( SortableListView, ListView )

typedef std::map< i32, std::string > M_i32ToString;

///////////////////////////////////////////////////////////////////////////////
// Structure for passing additional information into the static sort function.
// 
struct SortData
{
  SortableListView* m_List;
  i32 m_Column;
  M_i32ToString m_Cache;

  SortData( SortableListView* list, i32 column )
  : m_List( list )
  , m_Column( column )
  {
  }
};

///////////////////////////////////////////////////////////////////////////////
// Helper function to find a string either in the list or in the cache of the
// specified sort data.  If the string for the specified item is not found
// in the cache, the list view is searched and the string is then cached.
// 
const std::string& StringLookup( long item, SortData* data )
{
  static const std::string empty;
  const std::string* text = &empty;

  // Check the cache for the string
  M_i32ToString::iterator found = data->m_Cache.find( item );
  if ( found != data->m_Cache.end() )
  {
    // The string was in the cache, just return it
    text = &(found->second);
  }
  else
  {
    // Cache miss.  Look for the item in the list control.
    const long itemId = data->m_List->FindItem( -1, item );
    if ( itemId >= 0 )
    {
      // The item was found in the list control
      wxListItem info;
      info.SetMask( wxLIST_MASK_TEXT );
      info.SetId( itemId );
      info.SetColumn( data->m_Column );
      const char* temp = data->m_List->GetItem( info ) ? info.GetText().c_str() : "";

      // Cache the value so that the lookup is faster next time
      Insert<M_i32ToString>::Result inserted = data->m_Cache.insert( M_i32ToString::value_type( item, std::string( temp ) ) );
      text = &( inserted.first->second );
    }
  }
  return *text;
}

///////////////////////////////////////////////////////////////////////////////
// A static sort function for sorting items by their text labels.  Looking up
// the text values for each item in the list is slow, so the values are cached
// as this function runs.
// 
int wxCALLBACK LazyMapCompareFunction( long item1, long item2, long sortData )
{
  SortData* data = static_cast< SortData* >( wxUIntToPtr( sortData ) );
  if ( data )
  {
    const std::string& text1 = StringLookup( item1, data );
    const std::string& text2 = StringLookup( item2, data );

    switch ( data->m_List->GetSortMethod() )
    {
    case ListViewSortMethods::Normal:
      return stricmp( text1.c_str(), text2.c_str() );
      break;

    case ListViewSortMethods::Natural:
      return strinatcmp( text1.c_str(), text2.c_str() );
      break;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Sorting algorithm that does not use a cache for sorting the list items.  Each
// item is searched for in the list when needed, and its text label is compared
// with the other item.  This function is way too slow, and no longer used, so
// it can probably be deleted.
// 
int wxCALLBACK SlowCompareFunction( long item1, long item2, long sortData )
{
  SortData* data = static_cast< SortData* >( wxUIntToPtr( sortData ) );
  if ( data )
  {
    const long itemId1 = data->m_List->FindItem( -1, item1 );
    if ( itemId1 >= 0 )
    {
      const long itemId2 = data->m_List->FindItem( -1, item2 );
      if ( itemId2 >= 0 )
      {
        wxListItem info1;
        info1.SetMask( wxLIST_MASK_TEXT );
        info1.SetId( itemId1 );
        info1.SetColumn( data->m_Column );
        const char* text1 = data->m_List->GetItem( info1 ) ? info1.GetText().c_str() : "";

        wxListItem info2;
        info2.SetMask( wxLIST_MASK_TEXT );
        info2.SetId( itemId2 );
        info2.SetColumn( data->m_Column );
        const char* text2 = data->m_List->GetItem( info2 ) ? info2.GetText().c_str() : "";

        switch ( data->m_List->GetSortMethod() )
        {
        case ListViewSortMethods::Normal:
          return stricmp( text1, text2 );
          break;

        case ListViewSortMethods::Natural:
          return strinatcmp( text1, text2 );
          break;
        }
      }
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SortableListView::SortableListView()
: m_IsSortingEnabled( true )
, m_SortMethod( ListViewSortMethods::Natural )
{
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SortableListView::SortableListView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name )
: ListView( parent, id, pos, size, style, validator, name )
, m_IsSortingEnabled( true )
, m_SortMethod( ListViewSortMethods::Natural )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SortableListView::~SortableListView()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if sorting is currently enabled for the list.
// 
bool SortableListView::IsSortingEnabled() const
{
  return m_IsSortingEnabled;
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to enable or disable sorting.  If sorting is disabled, calls to
// SortItems() will do nothing.
// 
void SortableListView::EnableSorting( bool enable )
{
  m_IsSortingEnabled = enable;
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to set the method that the list will use for sorting.
// 
void SortableListView::SetSortMethod( ListViewSortMethods::ListViewSortMethod method )
{
  m_SortMethod = method;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the sort method currently employed by the list.
// 
ListViewSortMethods::ListViewSortMethod SortableListView::GetSortMethod() const
{
  return m_SortMethod;
}

///////////////////////////////////////////////////////////////////////////////
// If sorting is enabled, sorts the list according to the text values in the
// specified column.
// 
bool SortableListView::SortItems( long whichColumn )
{
  bool wasSorted = false;
  if ( IsSortingEnabled() )
  {
    SortData data( this, whichColumn );
    wasSorted = __super::SortItems( &LazyMapCompareFunction, wxPtrToUInt( &data ) );
  }
  return wasSorted;
}
