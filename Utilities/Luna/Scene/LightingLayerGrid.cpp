#include "Precompile.h"
#include "LightingLayerGrid.h"
#include "DependencyCommand.h"
#include "Grid.h"
#include "Layer.h"
#include "Scene.h"
#include "SceneEditor.h"
#include "SceneManager.h"
#include "SceneEditorIDs.h"
#include "Console/Console.h"
#include "UIToolKit/ImageManager.h"
#include "Common/Container/Insert.h" 

#include "DirectionalLight.h"
#include "ShadowDirection.h"
#include "AmbientLight.h"
#include "SunLight.h"

#include "LightingVolume.h"

// Using
using namespace Luna;
 
LightingLayerGrid::LightingLayerGrid( wxWindow* parent, Luna::SceneManager* sceneManager)
: LayerGrid( parent, sceneManager,  Content::LayerTypes::LT_Lighting)
{
}

///////////////////////////////////////////////////////////////////////////////
//
bool LightingLayerGrid::IsLightLinkableToVolume(Luna::Object* selectable)
{
  /*
  if( (Reflect::ConstObjectCast< Luna::DirectionalLight >( selectable )  != NULL) ||
      (Reflect::ConstObjectCast< Luna::ShadowDirection >( selectable )   != NULL) ||
      (Reflect::ConstObjectCast< Luna::AmbientLight >( selectable )      != NULL) ||
      (Reflect::ConstObjectCast< Luna::SunLight >( selectable )          != NULL))
  {
    return true;
  }
  */
  if( (Reflect::ConstObjectCast< Luna::Light >( selectable )  != NULL) )
  {
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// 
bool  IsSelectionValidForLightLinking(const OS_SelectableDumbPtr& selection)
{
  OS_SelectableDumbPtr::Iterator  itr  = selection.Begin();
  OS_SelectableDumbPtr::Iterator  end  = selection.End();

  bool  foundLightingVolume = false;
  bool  foundLight          = false;

  //Loop over the selection
  for ( ; itr != end; ++itr )
  {
    //If we have already found at least a lighting volume and a different light we are good to go
    if(foundLightingVolume && foundLight)
    {
      break;
    }

    //Check for lighting volumes
    if(Reflect::ObjectCast< Luna::LightingVolume >( *itr )  != NULL)
    {
      foundLightingVolume = true;
      continue;
    }

    //Check for linkable lights
    if(LightingLayerGrid::IsLightLinkableToVolume( *itr )  != false)
    {
      foundLight = true;
      continue;
    }
  }

  //Report the findings
  return (foundLightingVolume && foundLight);
}

///////////////////////////////////////////////////////////////////////////////
//
//There are a few rules to meet before we allow for linking of 
//lighting volumes and other types of lights 
//
bool  LightingLayerGrid::IsSelectionValid() const
{
  return IsSelectionValidForLightLinking(m_Scene->GetSelection().GetItems());
}

///////////////////////////////////////////////////////////////////////////////
//
void  LightingLayerGrid::SelectLinkedElements()
{
  OS_SelectableDumbPtr            selection             = m_Scene->GetSelection().GetItems();
  OS_SelectableDumbPtr            linkedSet;
  
  OS_SelectableDumbPtr::Iterator  originalSelectionItr  = selection.Begin();
  OS_SelectableDumbPtr::Iterator  originalSelectionEnd  = selection.End();

  OS_SelectableDumbPtr            lightingVolumes;
  OS_SelectableDumbPtr            lights;

  //Create a list of lighting volumes and a list of lights that might be linked to them
  for ( ; originalSelectionItr != originalSelectionEnd; ++originalSelectionItr )
  {
    Selectable* selectable = (*originalSelectionItr);

    //Check for lighting volumes
    if(Reflect::ObjectCast< Luna::LightingVolume >( selectable )  != NULL)
    {
      lightingVolumes.Append(selectable);
      continue;
    }

    //Check for directional, ambient and sun lights
    if(IsLightLinkableToVolume(selectable))
    {
      lights.Append(selectable);
    }
  }

  //If both the lighting volumes and lights list are empty, bail out, nothing here to do
  if((lightingVolumes.Size() == 0) && (lights.Size() == 0))
  {
    return;
  }

  //We have three cases here: 
#define CASE_A 0  // if we have a set of lighting volumes and a set of lights in the selection, select all of their linked elements
#define CASE_B 1  // if we don't have any lighting volumes in the selection, select all of the lighting volumes linked to the lights
#define CASE_C 2  // if we don't have any lights in the selection, select all of the lights linked to the lighting volumes

  //The case id
  u32 case_id;

  //Generate the cases
  {
    if((lightingVolumes.Size() != 0) && (lights.Size() != 0))
    {
      case_id = CASE_A;
    }

    if((lightingVolumes.Size() == 0) && (lights.Size() != 0))
    {
      case_id = CASE_B;
    }

    if((lightingVolumes.Size() != 0) && (lights.Size() == 0))
    {
      case_id = CASE_C;
    }      
  }

  M_LayerDumbPtr::const_iterator layerItr = m_Layers.begin();
  M_LayerDumbPtr::const_iterator layerEnd = m_Layers.end();

  //Unselect all rows
  m_Grid->SelectRow(-1, false);

  //Loop over the layers
  for ( ; layerItr != layerEnd; ++layerItr )
  {
    const std::string&    layerName     = layerItr->first;
    Luna::Layer*          lunaLayer     = layerItr->second;
    u32                   rowIndex      = m_Grid->GetRowNumber(layerName);

    OS_SelectableDumbPtr  layerMembers  = lunaLayer->GetMembers();

    //Process the cases
    switch(case_id)
    {
      case CASE_A:
      {
        //Collect the lighting volumes
        {
          //For each light, check if there is a layer containing it 
          OS_SelectableDumbPtr::Iterator  lightVolumeItr  = lightingVolumes.Begin();
          OS_SelectableDumbPtr::Iterator  lightVolumeEnd  = lightingVolumes.End();
          for ( ; lightVolumeItr != lightVolumeEnd; ++lightVolumeItr )
          {
            Selectable* lightingVolume = (*lightVolumeItr);

            //Check if this layer contains the lighting  volume
            if(layerMembers.Contains(lightingVolume) != false)
            {    
              //Collect all members
              OS_SelectableDumbPtr::Iterator  memberItr  = layerMembers.Begin();
              OS_SelectableDumbPtr::Iterator  memberEnd  = layerMembers.End();
              for ( ; memberItr != memberEnd; ++memberItr )
              {
                Selectable* member = (*memberItr);

                //Select the row
                m_Grid->SelectRow(rowIndex, (linkedSet.Empty() == false));
                linkedSet.Append(member);
              }
            }
          }//Done with the lighting volumes
        }

        //Collect the lights
        {
          //For each light, check if there is a layer containing it and a lighting
          //volume from the list at the same time
          OS_SelectableDumbPtr::Iterator  lightItr  = lights.Begin();
          OS_SelectableDumbPtr::Iterator  lightEnd  = lights.End();
          for ( ; lightItr != lightEnd; ++lightItr )
          {
            Selectable* light = (*lightItr);

            //Check if this layer contains the light
            if(layerMembers.Contains(light) != false)
            {
              //For each lighting volume, check if it is in the same layer as the light
              OS_SelectableDumbPtr::Iterator  lightVolumeItr  = lightingVolumes.Begin();
              OS_SelectableDumbPtr::Iterator  lightVolumeEnd  = lightingVolumes.End();

              for ( ; lightVolumeItr != lightVolumeEnd; ++lightVolumeItr )
              {
                //Lighting volume
                Selectable* lightingVolume = (*lightVolumeItr);
                if(layerMembers.Contains(lightingVolume))
                {
                  //Select the row
                  m_Grid->SelectRow(rowIndex, (linkedSet.Empty() == false));

                  //We have found a link, keep it around for removal
                  linkedSet.Append(light);
                }
              }//Done with the lighting volumes
            }
          }//Done with the lights
        }
      }
      break;

      case CASE_B:
      {
        //For each light, check if there is a layer containing it 
        OS_SelectableDumbPtr::Iterator  lightItr  = lights.Begin();
        OS_SelectableDumbPtr::Iterator  lightEnd  = lights.End();
        for ( ; lightItr != lightEnd; ++lightItr )
        {
          Selectable* light = (*lightItr);

          //Check if this layer contains the light
          if(layerMembers.Contains(light) != false)
          {              
            //Collect the lighting volumes
            OS_SelectableDumbPtr::Iterator  memberItr  = layerMembers.Begin();
            OS_SelectableDumbPtr::Iterator  memberEnd  = layerMembers.End();
            for ( ; memberItr != memberEnd; ++memberItr )
            {
              Selectable* member = (*memberItr);

              //Check for lighting volumes
              if(Reflect::ObjectCast< Luna::LightingVolume >( member )  != NULL)
              {
                //Select the row
                m_Grid->SelectRow(rowIndex, (linkedSet.Empty() == false));
                linkedSet.Append(member);
              }
            }
          }
        }//Done with the lights
      }
      break;

      case CASE_C:
      {
        //For each light, check if there is a layer containing it 
        OS_SelectableDumbPtr::Iterator  lightVolumeItr  = lightingVolumes.Begin();
        OS_SelectableDumbPtr::Iterator  lightVolumeEnd  = lightingVolumes.End();
        for ( ; lightVolumeItr != lightVolumeEnd; ++lightVolumeItr )
        {
          Selectable* lightingVolume = (*lightVolumeItr);

          //Check if this layer contains the lighting  volume
          if(layerMembers.Contains(lightingVolume) != false)
          {              
            //Collect the lights
            OS_SelectableDumbPtr::Iterator  memberItr  = layerMembers.Begin();
            OS_SelectableDumbPtr::Iterator  memberEnd  = layerMembers.End();
            for ( ; memberItr != memberEnd; ++memberItr )
            {
              Selectable* member = (*memberItr);

              //Check for lighting volumes
              if(IsLightLinkableToVolume(member))
              {
                //Select the row
                m_Grid->SelectRow(rowIndex, (linkedSet.Empty() == false));
                linkedSet.Append(member);
              }
            }
          }
        }//Done with the lighting volumes
      }
      break;
    }//switch
  }//Done with the layers

  //Highlight the new selection
  if(linkedSet.Size() != 0)
  {
    m_Scene->GetSelection().SetItems( linkedSet );
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// 
bool  LightingLayerGrid::GenerateLinkedSetFromSelection(const OS_SelectableDumbPtr& selection, 
                                                              OS_SelectableDumbPtr& linkedSet,
                                                              bool                  selectRows)
{
  OS_SelectableDumbPtr::Iterator  originalSelectionItr  = selection.Begin();
  OS_SelectableDumbPtr::Iterator  originalSelectionEnd  = selection.End();

  OS_SelectableDumbPtr            lightingVolumes;
  OS_SelectableDumbPtr            lights;

  //Clear the new set
  linkedSet.Clear();

  //Create a list of lighting volumes and a list of lights that might be linked to them
  for ( ; originalSelectionItr != originalSelectionEnd; ++originalSelectionItr )
  {
    Selectable* selectable = (*originalSelectionItr);

    //Check for lighting volumes
    if(Reflect::ObjectCast< Luna::LightingVolume >( selectable )  != NULL)
    {
      lightingVolumes.Append(selectable);
      continue;
    }

    //Check for directional, ambient and sun lights
    if(IsLightLinkableToVolume(selectable))
    {
      lights.Append(selectable);
    }
  }

  //If both the lighting volumes and lights list are empty, bail out, nothing here to do
  if((lightingVolumes.Size() == 0) && (lights.Size() == 0))
  {
    return false;
  }

  //We have three cases here: 
#define CASE_A 0  // if we have a set of lighting volumes and a set of lights in the selection, remove any links between the two sets
#define CASE_B 1  // if we don't have any lighting volumes in the selection, remove any links the selected lights may have to any lighting volumes
#define CASE_C 2  // if we don't have any lights in the selection, remove any links the lighting volumes have to any lights

  //The case id
  u32 case_id;

  //Generate the cases
  {
    if((lightingVolumes.Size() != 0) && (lights.Size() != 0))
    {
      case_id = CASE_A;
    }

    if((lightingVolumes.Size() == 0) && (lights.Size() != 0))
    {
      case_id = CASE_B;
    }

    if((lightingVolumes.Size() != 0) && (lights.Size() == 0))
    {
      case_id = CASE_C;
    }      
  }

  M_LayerDumbPtr::const_iterator layerItr = m_Layers.begin();
  M_LayerDumbPtr::const_iterator layerEnd = m_Layers.end();

  //Unselect all rows
  if(selectRows)
  {
    m_Grid->SelectRow(-1, false);
  }

  //Loop over the layers
  for ( ; layerItr != layerEnd; ++layerItr )
  {
    const std::string&    layerName     = layerItr->first;
    Luna::Layer*          lunaLayer     = layerItr->second;
    u32                   rowIndex      = m_Grid->GetRowNumber(layerName);

    OS_SelectableDumbPtr  layerMembers  = lunaLayer->GetMembers();

    //Process the cases
    switch(case_id)
    {
      case CASE_A:
      {
        //For each light, check if there is a layer containing it and a lighting
        //volume from the list at the same time
        OS_SelectableDumbPtr::Iterator  lightItr  = lights.Begin();
        OS_SelectableDumbPtr::Iterator  lightEnd  = lights.End();
        for ( ; lightItr != lightEnd; ++lightItr )
        {
          Selectable* light = (*lightItr);
        
          //Check if this layer contains the light
          if(layerMembers.Contains(light) != false)
          {
            //For each lighting volume, check if it is in the same layer as the light
            OS_SelectableDumbPtr::Iterator  lightVolumeItr  = lightingVolumes.Begin();
            OS_SelectableDumbPtr::Iterator  lightVolumeEnd  = lightingVolumes.End();

            for ( ; lightVolumeItr != lightVolumeEnd; ++lightVolumeItr )
            {
              //Lighting volume
              Selectable* lightingVolume = (*lightVolumeItr);
              if(layerMembers.Contains(lightingVolume))
              {
                if(selectRows)
                {
                  //Select the row
                  m_Grid->SelectRow(rowIndex, (linkedSet.Empty() == false));
                }
                
                //We have found a link, keep it around for removal
                linkedSet.Append(light);
              }
            }//Done with the lighting volumes
          }
        }//Done with the lights
      }
      break;

      case CASE_B:
      {
        //For each light, check if there is a layer containing it 
        OS_SelectableDumbPtr::Iterator  lightItr  = lights.Begin();
        OS_SelectableDumbPtr::Iterator  lightEnd  = lights.End();
        for ( ; lightItr != lightEnd; ++lightItr )
        {
          Selectable* light = (*lightItr);

          //Check if this layer contains the light
          if(layerMembers.Contains(light) != false)
          {              
            if(selectRows)
            {
              //Select the row
              m_Grid->SelectRow(rowIndex, (linkedSet.Empty() == false));
            }

            //We have found a link, keep it around for removal
            linkedSet.Append(light);
          }
        }//Done with the lights
      }
      break;

      case CASE_C:
      {
        //For each light, check if there is a layer containing it 
        OS_SelectableDumbPtr::Iterator  lightVolumeItr  = lightingVolumes.Begin();
        OS_SelectableDumbPtr::Iterator  lightVolumeEnd  = lightingVolumes.End();
        for ( ; lightVolumeItr != lightVolumeEnd; ++lightVolumeItr )
        {
          Selectable* lightingVolume = (*lightVolumeItr);

          //Check if this layer contains the lighting  volume
          if(layerMembers.Contains(lightingVolume) != false)
          {              
            if(selectRows)
            {
              //Select the row
              m_Grid->SelectRow(rowIndex, (linkedSet.Empty() == false));
            }

            //We have found a link, keep it around for removal
            linkedSet.Append(lightingVolume);
          }
        }//Done with the lighting volumes
      }
      break;
    }//switch
  }//Done with the layers

  return (linkedSet.Empty() == false);
}

///////////////////////////////////////////////////////////////////////////////
//
bool  LightingLayerGrid::IsSelectionItemsLinked()
{
  OS_SelectableDumbPtr linkedSet;
  return GenerateLinkedSetFromSelection(m_Scene->GetSelection().GetItems(), linkedSet);
}

///////////////////////////////////////////////////////////////////////////////
//
void  LightingLayerGrid::UnlinkSelectedElements()
{
  OS_SelectableDumbPtr  originalSet  = m_Scene->GetSelection().GetItems();
  OS_SelectableDumbPtr  linkedSet;

  //Check our list to remove
  if(GenerateLinkedSetFromSelection(originalSet, linkedSet, true) == false)
  {
    return;
  }
   
  //Replace our current selection with the elements to remove
  m_Scene->GetSelection().SetItems( linkedSet );

  //Remove the selected elements
  LayerSelectedItems(false);

  //Clean up invalid layers
  CleanUpLayers();

  //Restore our original selection
  m_Scene->GetSelection().SetItems( originalSet );
}

///////////////////////////////////////////////////////////////////////////////
//
void LightingLayerGrid::CleanUpLayers()
{
  M_LayerDumbPtr::const_iterator layerItr = m_Layers.begin();
  M_LayerDumbPtr::const_iterator layerEnd = m_Layers.end();
  bool                           found    = false;

  //Unselect all rows
  m_Grid->SelectRow(-1, false);

  for ( ; layerItr != layerEnd; ++layerItr )
  {
    const std::string&  layerName         = layerItr->first;
    Luna::Layer*        lunaLayer         = layerItr->second;
    size_t              membersCount      = lunaLayer->GetMembers().Size();
    u32                 rowIndex          = m_Grid->GetRowNumber(layerName);
    
    //Check the size
    if(IsSelectionValidForLightLinking(lunaLayer->GetMembers())  == false)
    {
      //Select the row
      m_Grid->SelectRow(rowIndex, found);
      found = true;
    }
  }//for

  //Check if we have found any invalid layers
  if(found == true)
  {
    //Remove all invalid layers
    DeleteSelectedLayers();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
bool LightingLayerGrid::IsSelectableValid(Selectable* selectable) const
{
  if(selectable)
  {
    //Check for lighting volumes
    if(Reflect::ObjectCast< Luna::LightingVolume >( selectable ))     return true;

    //Check for directional
    if(Reflect::ObjectCast< Luna::Light >( selectable ))   return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
//
void  LightingLayerGrid::GenerateLayerName(Layer* layer) 
{
  const OS_SelectableDumbPtr&     selection             = m_Scene->GetSelection().GetItems();

  OS_SelectableDumbPtr::Iterator  originalSelectionItr  = selection.Begin();
  OS_SelectableDumbPtr::Iterator  originalSelectionEnd  = selection.End();

  OS_SelectableDumbPtr            lightingVolumes;
  OS_SelectableDumbPtr            lights;

  //Create a list of lighting volumes and a list of lights that might be linked to them
  for ( ; originalSelectionItr != originalSelectionEnd; ++originalSelectionItr )
  {
    Selectable* selectable = (*originalSelectionItr);

    //Check for lighting volumes
    if(Reflect::ObjectCast< Luna::LightingVolume >( selectable )  != NULL)
    {
      lightingVolumes.Append(selectable);
      continue;
    }

    //Check for directional, ambient and sun lights
    if(IsLightLinkableToVolume(selectable))
    {
      lights.Append(selectable);
    }
  }

  //If both the lighting volumes and lights list are empty, bail out, nothing here to do
  if((lightingVolumes.Size() == 0) && (lights.Size() == 0))
  {
    NOC_ASSERT(!"How the fuck did we get here?")
    return;
  }

  //One lighting volume and multiple lights
  if(lightingVolumes.Size() == 1)
  {
    LightingVolume* lightingVolume  = Reflect::ObjectCast< Luna::LightingVolume >( *lightingVolumes.Begin() );
    std::string     name            = lightingVolume->GetName() + " Link to Lights";
    layer->SetName(name);
    return;
  }

  //One lighting volume and one light lights
  if((lightingVolumes.Size() > 1) && (lights.Size() == 1))
  {
    SceneNode*      node  = Reflect::ObjectCast< SceneNode >( *lights.Begin() );
    std::string     name  = "Lighting Volumes Link to " + node->GetName();
    layer->SetName(name);
    return;
  }

  layer->SetName("Lighting Link Layer");
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingLayerGrid::~LightingLayerGrid()
{
}