#pragma once

#include "Application/API.h"
#include "ListView.h"

namespace Nocturnal
{
  // Enumeration of different sorting methods useable by the tree control
  namespace ListViewSortMethods
  {
    enum ListViewSortMethod
    {
      Normal, // Regular, alphabetical sorting. Ex: Item1, Item10, Item2
      Natural // Natrual string ordering, keeping numeric values in order. Ex: Item1, Item2, Item10
    };
  };
  typedef ListViewSortMethods::ListViewSortMethod ListViewSortMethod;

  /////////////////////////////////////////////////////////////////////////////
  // Extends the normal list view to provide automatic sorting of items by the
  // text value in various columns (meaningful mainly in report mode of a list).
  // Calling SortItems will sort the entire list, based upon the text values
  // in the specified column.  In order for sorting to work, each item in the
  // list must have a unique identifier located in its item data (see
  // wxListItem::SetData).  This is a limitation of wxWidgets, in that the sort
  // callback only returns the item data of the items to be compared.
  // 
  class APPLICATION_API SortableListView : public ListView
  {
  private:
    bool m_IsSortingEnabled;
    ListViewSortMethod m_SortMethod;

  public:
    SortableListView();
    SortableListView( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT( "SortableListView" ) );
    virtual ~SortableListView();
    bool IsSortingEnabled() const;
    void EnableSorting( bool enable = true );
    void DisableSorting() { EnableSorting( false ); }
    void SetSortMethod( ListViewSortMethod method );
    ListViewSortMethod GetSortMethod() const;
    bool SortItems( long whichColumn = 0 );

  private:
    DECLARE_DYNAMIC_CLASS( SortableListView )
  };
}
