#pragma once

#include <map>
#include <vector>
#include "Common/Automation/Event.h"
#include "Common/Container/OrderedSet.h"
#include "Content/LayerTypes.h"
#include "LayerGrid.h"

namespace Luna
{
  class Object;

  /////////////////////////////////////////////////////////////////////////////
  // Class that manages the UI for changing the selectability/visibility of
  // lighting layers. 
  class LightingLayerGrid : public LayerGrid
  {

  public:
    LightingLayerGrid( wxWindow* parent, Luna::SceneManager* sceneManager);
    virtual ~LightingLayerGrid();
 
    static  bool  IsLightLinkableToVolume(Luna::Object* selectable);

    virtual bool  IsSelectionItemsLinked();
    virtual bool  IsSelectableValid(Selectable* sl) const;
    virtual bool  IsSelectionValid() const;
    virtual void  UnlinkSelectedElements();
    virtual void  SelectLinkedElements()  ;
    virtual void  GenerateLayerName(Layer* layer);
    virtual void  CleanUpLayers()                ;

  private:
    bool          GenerateLinkedSetFromSelection(const  OS_SelectableDumbPtr&  selection, 
                                                        OS_SelectableDumbPtr&  linkedSet,
                                                        bool                   selectRows = false);
  };
}
