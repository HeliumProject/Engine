#include "Precompile.h"
#include "ContextMenuGenerator.h"

#include "Foundation/Container/Insert.h" 
#include "Application/Undo/BatchCommand.h"

using Nocturnal::Insert; 
using namespace Luna;


///////////////////////////////////////////////////////////////////////////////
// ContextMenuItemSet
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ContextMenuItemSet::ContextMenuItemSet()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ContextMenuItemSet::~ContextMenuItemSet()
{
}

///////////////////////////////////////////////////////////////////////////////
// Inserts the specified menu item at the front of the list.
// 
void ContextMenuItemSet::PrependItem( const ContextMenuItemPtr& item )
{
  if ( !ContextMenuItem::IsSeparator( item ) )
  {
    Insert<S_string>::Result inserted = m_Labels.insert( item->GetLabel() );
    NOC_ASSERT( inserted.second ); // Menu labels must be unique within a menu
  }

  m_Items.push_front( item );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a menu item to the end of the list.  The item should be uniquely named
// when compared to the other items in the list.
// 
void ContextMenuItemSet::AppendItem( const ContextMenuItemPtr& item )
{
  if ( !ContextMenuItem::IsSeparator( item ) )
  {
    Insert<S_string>::Result inserted = m_Labels.insert( item->GetLabel() );
    NOC_ASSERT( inserted.second ); // Menu labels must be unique within a menu
  }

  m_Items.push_back( item );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a separator to the list.  You cannot add two separtors in a row.  If
// you attempt to, the second separator will not be added.
// 
void ContextMenuItemSet::AppendSeparator()
{
  bool append = true;

  if ( m_Items.size() > 0 )
  {
    // Don't allow the user to put multiple separators in a row.
    append = !ContextMenuItem::IsSeparator( m_Items.back() );
  }

  if ( append )
  {
    AppendItem( ContextMenuItem::Separator() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Appends a separator after the specified item.
// 
void ContextMenuItemSet::AppendSeparatorAfter( const ContextMenuItemPtr& afterItem )
{
  D_ContextMenuItemSmartPtr::iterator insertLoc = m_Items.begin();
  D_ContextMenuItemSmartPtr::iterator itr = m_Items.begin();
  D_ContextMenuItemSmartPtr::iterator end = m_Items.end();
  for ( ; itr != end; ++itr )
  {
    if ( *itr == afterItem )
    {
      insertLoc = ++itr;
      break;
    }
  }

  if ( insertLoc != m_Items.begin() )
  {
    m_Items.insert( insertLoc, ContextMenuItem::Separator() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the item with the specified label.  Additional items (separators)
// may also be removed.  See RemoveAt for more information.
// 
void ContextMenuItemSet::Remove( const std::string& label )
{
  if ( !label.empty() && ContainsLabel( label ) )
  {
    D_ContextMenuItemSmartPtr::const_iterator itr = m_Items.begin();
    D_ContextMenuItemSmartPtr::const_iterator end = m_Items.end();
    for ( u32 index = 0; itr != end; ++itr, ++index )
    {
      const ContextMenuItemPtr& item = *itr;
      if ( item->GetLabel() == label )
      {
        RemoveAt( index );
        break;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the menu item at the specified index into the list of items.  Additional
// separtor items may also be removed according to the following rules:
// 1. The list should not have two separators in a row.
// 2. The list should not end with a separator
// 3. The list should not begin with a separator.
// 
u32 ContextMenuItemSet::RemoveAt( const u32 index )
{
  u32 numItemsRemoved = 0;
  if ( index < m_Items.size() )
  {
    // Is the item before the one we are removing a separator?
    bool isPreviousSeparator = false;
    if ( index > 0 )
    {
      isPreviousSeparator = ContextMenuItem::IsSeparator( m_Items.at( index - 1 ) );
    }

    // Is the item after the one we are removing a separator?
    bool isNextSeparator = false;
    if ( index + 1 < m_Items.size() )
    {
      isNextSeparator = ContextMenuItem::IsSeparator( m_Items.at( index + 1 ) );
    }

    // If the item we are removing is not a separator, update the list of labels.
    bool isSeparator = ContextMenuItem::IsSeparator( m_Items.at( index ) );
    if ( !isSeparator )
    {
      m_Labels.erase( m_Items.at( index )->GetLabel() );
    }

    // Erase the item.
    m_Items.erase( m_Items.begin() + index );
    numItemsRemoved++;

    // If we removed an item between two separators, automatically delete one of the separators.
    if ( isPreviousSeparator && isNextSeparator )
    {
      m_Items.erase( m_Items.begin() + index );
      numItemsRemoved++;
    }

    if ( m_Items.size() > 0 )
    {
      // Context menus cannot start with a separator.
      if ( ContextMenuItem::IsSeparator( *m_Items.begin() ) )
      {
        m_Items.erase( m_Items.begin() );
        numItemsRemoved++;
      }
    }

    if ( m_Items.size() > 0 )
    {
      // Context menus cannot end with a separator.
      if ( ContextMenuItem::IsSeparator( *( m_Items.end() - 1 ) ) )
      {
        m_Items.erase( m_Items.end() - 1 );
        numItemsRemoved++;
      }
    }
  }

  return numItemsRemoved;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of items.
// 
const D_ContextMenuItemSmartPtr& ContextMenuItemSet::GetItems() const
{
  return m_Items;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if there is an item with the specified label somewhere in this
// context menu.
// 
bool ContextMenuItemSet::ContainsLabel( const std::string& label ) const
{
  return m_Labels.find( label ) != m_Labels.end();
}

///////////////////////////////////////////////////////////////////////////////
// Locates the menu item with the specifed label and returns it.
// 
ContextMenuItemPtr ContextMenuItemSet::Find( const std::string& label ) const
{
  ContextMenuItemPtr item;
  if ( ContainsLabel( label ) )
  {
    D_ContextMenuItemSmartPtr::const_iterator itr = m_Items.begin();
    D_ContextMenuItemSmartPtr::const_iterator end = m_Items.end();
    for ( ; itr != end && !item.ReferencesObject(); ++itr )
    {
      if ( (*itr)->GetLabel() == label )
      {
        item = *itr; // breaks out of the loop
      }
    }
  }
  return item;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the set is empty.
// 
bool ContextMenuItemSet::IsEmpty() const
{
  return m_Items.size() == 0;
}

///////////////////////////////////////////////////////////////////////////////
// Removes all items from the set.
// 
void ContextMenuItemSet::Clear()
{
  m_Labels.clear();
  m_Items.clear();
}



///////////////////////////////////////////////////////////////////////////////
// ContextMenuGenerator
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Takes the two specified lists of menu items and returns a new list of menu
// items that only contains the common elements.
// 
ContextMenuItemSet ContextMenuGenerator::Merge( const ContextMenuItemSet& first, const ContextMenuItemSet& second )
{
  ContextMenuItemSet result = first;
  D_ContextMenuItemSmartPtr::const_iterator itr = first.GetItems().begin();
  D_ContextMenuItemSmartPtr::const_iterator end = first.GetItems().end();
  for ( u32 index = 0; itr != end; ++itr )
  {
    const ContextMenuItemPtr& item = *itr;
    if ( !ContextMenuItem::IsSeparator( item ) )
    {
      ContextMenuItemPtr found = second.Find( item->GetLabel() );
      if ( found.ReferencesObject() )
      {
        item->MergeCallbacks( *found.Ptr() );
        item->Enable( item->IsEnabled() && found->IsEnabled() );
      }
      else
      {
        result.Remove( item->GetLabel() );
      }
    }
  }
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Takes the list of context menu items and adds them to the specified context
// menu.
// 
void ContextMenuGenerator::Build( const ContextMenuItemSet& elements, ContextMenu* menu )
{
  NOC_ASSERT( menu );
  if ( menu )
  {
    D_ContextMenuItemSmartPtr::const_iterator itr = elements.GetItems().begin();
    D_ContextMenuItemSmartPtr::const_iterator end = elements.GetItems().end();
    for ( ; itr != end; ++itr )
    {
      menu->AddItem( *itr );
    }
  }
}
