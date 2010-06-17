#pragma once

#include "Luna/API.h"
#include "SceneNode.h"
#include "Scene.h"
#include "Zone.h"
#include "Pipeline/Content/Nodes/Region.h"

namespace Luna
{
  // Forwards
  class Enumerator;

  class Region : public Luna::SceneNode
  {
  public: 
    LUNA_DECLARE_TYPE(Luna::Region, Luna::SceneNode); 

    static void InitializeType(); 
    static void CleanupType(); 

    Region(Luna::Scene* scene, Content::Region* region); 
    virtual ~Region(); 

    virtual i32 GetImageIndex() const;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;
    virtual void SetName(const std::string& value);

    void RemoveZone( const ZonePtr& zone ); 
    void AddZone( const ZonePtr& zone ); 

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;

  private: 
    void NodeAdded( const NodeChangeArgs& args );
    void NodeRemoved( const NodeChangeArgs& args );

    // backwards compat helpers for updating the zone m_Regions list
    // 
    void RemoveSelfFromZone( const ZonePtr& zone ); 
    void AddSelfToZone( const ZonePtr& zone ); 

    static void CreatePanel( CreatePanelArgs& args );
  }; 

  typedef Nocturnal::SmartPtr<Luna::Region> LRegionPtr; 
}
