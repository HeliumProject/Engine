#include "stdafx.h"

#include "SortTreeCtrl.h"
#include "Common/String/Natural.h"

namespace UIToolKit
{

  // Required so that OnCompareItems will be called
  IMPLEMENT_DYNAMIC_CLASS( SortTreeCtrl, TreeCtrl )

    ///////////////////////////////////////////////////////////////////////////////
    // Default constructor - required by IMPLEMENT_DYNAMIC_CLASS
    // 
    SortTreeCtrl::SortTreeCtrl()
  {
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Constructor
  // 
  SortTreeCtrl::SortTreeCtrl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name )
    : TreeCtrl( parent, id, pos, size, style, validator, name )
    , m_AllowSorting( true )
    , m_SortMethod( TreeSortMethods::Natural )
  {
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Destructor
  // 
  SortTreeCtrl::~SortTreeCtrl()
  {
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Returns true if sorting is turned on, false if sorting is turned off.
  // 
  bool SortTreeCtrl::IsSortingEnabled() const
  {
    return m_AllowSorting;
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Set to true to turn sorting on, false to turn sorting off.  If sorting is
  // turned off, calls to SortChildren will have no effect.
  // 
  void SortTreeCtrl::EnableSorting( bool enable )
  {
    m_AllowSorting = enable;
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Returns the current sorting method used by the tree.
  // 
  TreeSortMethod SortTreeCtrl::GetSortMethod() const
  {
    return m_SortMethod;
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Sets the sorting method used by the tree.  If TreeSortMethod::Normal is
  // specified, regular alphabetical sorting is applied to the tree.  If
  // TreeSortMethod::Natrual is specified, sorting is still alphabetical, but
  // numerical values are also ordered properly.
  // 
  void SortTreeCtrl::SetSortMethod( TreeSortMethod method )
  {
    m_SortMethod = method;
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Sorts the tree, optionally recursive
  //
  void SortTreeCtrl::Sort( const wxTreeItemId& root, bool recursive )
  {
    // Don't bother recursing through the tree unless sorting is enabled.
    if ( !IsSortingEnabled() )
      return;

    wxTreeItemId parent = root;
    if ( !parent.IsOk() )
    {
      parent = GetRootItem();
    }

    // cookie is required by GetFirstChild & GetNextChild, it makes it thread safe :)
    if ( recursive )
    {
      wxTreeItemIdValue cookie; 
      wxTreeItemId child = GetFirstChild( parent, cookie );
      while ( child.IsOk() )
      {
        Sort( child );

        child = GetNextChild( parent, cookie );
      }
    }

    SortChildren( parent );
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Callback to compare two tree items.  If item1 should be sorted before item2,
  // a negative number is returned.  If item1 and item2 are equal according to
  // the sorting algorithm, zero is returned.  Finally, if item1 should be sorted
  // after item2, a positive number is returned.
  // 
  int SortTreeCtrl::OnCompareItems( const wxTreeItemId& item1, const wxTreeItemId& item2 )
  {
    int result = 0;

    if ( m_SortMethod == TreeSortMethods::Natural )
    {
      result = strinatcmp( GetItemText( item1 ).c_str(), GetItemText( item2 ).c_str() );
    }
    else
    {
      result = wxStrcmp( GetItemText( item1 ), GetItemText( item2 ) );
    }

    return result;
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Sorts the children of the specified tree item only.  If sorting is disabled
  // this function does nothing.
  // 
  void SortTreeCtrl::SortChildren( const wxTreeItemId& item )
  {
    if ( IsSortingEnabled() )
    {
      __super::SortChildren( item );
    }
  }


}