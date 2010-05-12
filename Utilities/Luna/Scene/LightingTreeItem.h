#pragma once

// Includes
#include "API.h"
#include "Editor/ContextMenuGenerator.h"
#include "Core/Selectable.h"
#include "Undo/Command.h"

namespace Luna
{
  // Forwards
  class LightingJob;
  class Object;
  class SceneNode;

  typedef Nocturnal::OrderedSet< wxTreeItemIdValue > OS_TreeItemIds;

  /////////////////////////////////////////////////////////////////////////////
  // Base class for attaching data and logic to items displayed in the lighting
  // job tree control.
  // 
  class LightingTreeItem NOC_ABSTRACT : public wxTreeItemData
  {
  public:
    // Types of tree items
    enum ItemType
    {
      TypeJob,
      TypeShadowGroup,
      TypeRenderGroup,
      TypeLightGroup,
      TypeProbeGroup,
      TypeVolumeGroup,
      TypeUnloadedZone,
      TypeInstance,
      TypeLight,
      TypeProbe,
      TypeVolume,
      TypeLightingEnvironmentGroup,
      TypeLightingEnvironment,
      TypeZone,
      TypeZoneGroup
    };

  private:
    wxTreeCtrlBase* m_Tree;
    Luna::LightingJob* m_LightingJob;
    ItemType m_Type;

  public:
    LightingTreeItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, ItemType type );
    virtual ~LightingTreeItem();
    wxTreeCtrlBase* GetTree() const;
    Luna::LightingJob* GetLightingJob() const;
    ItemType GetType() const;

    virtual void Load() = 0;
    virtual bool IsSelectable( const OS_TreeItemIds& currentSelection ) const = 0;
    virtual void GetSelectableItems( OS_SelectableDumbPtr& selection ) const;
    virtual void SelectItem( Luna::SceneNode* node );
    virtual ContextMenuItemSet GetContextMenuItems();
    virtual bool CanRename() const = 0;
    virtual void Rename( const std::string& newName ) {}
    virtual bool CanAddChild( Object* object ) const = 0;
    virtual Undo::CommandPtr AddChild( Object* object ) = 0;
    virtual Undo::CommandPtr GetRemoveCommand( const OS_SelectableDumbPtr& selection ) = 0;

    // Templated function to cast from a base tree item data to a derived type
    template < class T >
    T* Cast( wxTreeItemData* base, ItemType type )
    {
      T* item = NULL;

      if ( base )
      {
        LightingTreeItem* lightingTreeItem = static_cast< LightingTreeItem* >( base );
        NOC_ASSERT( lightingTreeItem->GetType() == type );
        if ( lightingTreeItem->GetType() == type )
        {
          item = static_cast< T* >( lightingTreeItem );
        }
      }

      return item;
    }
  };
}