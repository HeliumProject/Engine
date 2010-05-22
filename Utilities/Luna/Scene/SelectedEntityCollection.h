#pragma once

#include "Entity.h"
#include "Core/Selection.h"
#include "Browser/AssetCollection.h"

namespace Luna
{
  class SelectedEntityCollection : public AssetCollection
  {
  public:
    SelectedEntityCollection( Selection* selection, const std::string& name );
    virtual ~SelectedEntityCollection();

    void ClearSelection();

  private:
    void RemoveEntityListeners();
    void SetAssetsFromSelection( const OS_SelectableDumbPtr& selection );
    void OnEntityClassChanged( const EntityAssetChangeArgs& args );
    void OnSelectionChanged( const OS_SelectableDumbPtr& args );

  private:
    Selection* m_Selection;
    Luna::V_EntityDumbPtr m_WatchedEntities;
  };
  typedef Nocturnal::SmartPtr< SelectedEntityCollection > SelectedEntityCollectionPtr;
}
