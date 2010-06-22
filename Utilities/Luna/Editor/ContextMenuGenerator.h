#pragma once

#include "Luna/API.h"
#include "ContextMenu.h"

namespace Luna
{
  typedef std::deque< ContextMenuItemPtr > D_ContextMenuItemSmartPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Class representing a list of context menu items.  You can add and remove
  // items before converting this list into a real context menu.
  // 
  class LUNA_EDITOR_API ContextMenuItemSet
  {
  private:
    D_ContextMenuItemSmartPtr m_Items;
    std::set< std::string > m_Labels;

  public:
    ContextMenuItemSet();
    virtual ~ContextMenuItemSet();

    void PrependItem( const ContextMenuItemPtr& item );
    void AppendItem( const ContextMenuItemPtr& item );
    void AppendSeparator();
    void AppendSeparatorAfter( const ContextMenuItemPtr& afterItem );
    void Remove( const std::string& label );
    u32 RemoveAt( const u32 index );
    const D_ContextMenuItemSmartPtr& GetItems() const;
    bool ContainsLabel( const std::string& label ) const;
    ContextMenuItemPtr Find( const std::string& label ) const;
    bool IsEmpty() const;
    void Clear();
  };



  /////////////////////////////////////////////////////////////////////////////
  // Helper functions for generating context menus from sets of items.
  // 
  namespace ContextMenuGenerator
  {
    LUNA_EDITOR_API ContextMenuItemSet Merge( const ContextMenuItemSet& first, const ContextMenuItemSet& second );
    LUNA_EDITOR_API void Build( const ContextMenuItemSet& elements, ContextMenu* menu );
  };
}
