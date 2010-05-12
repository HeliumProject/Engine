#pragma once

#include "Instance.h"

#include "Content/Volume.h"

namespace Luna
{
  class VolumeType;

  /////////////////////////////////////////////////////////////////////////////
  // Luna's wrapper for an volume instance.
  // 
  class Volume : public Luna::Instance
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::Volume, Luna::Instance );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    Volume(Luna::Scene* s);
    Volume(Luna::Scene* s, Content::Volume* volume);
    virtual ~Volume();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;
    virtual SceneNodeTypePtr CreateNodeType( Luna::Scene* scene ) const NOC_OVERRIDE;

    int GetShape() const;
    void SetShape( int shape );

    virtual void Evaluate(GraphDirection direction) NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;
    
    static void CreatePanel( CreatePanelArgs& args );

    //
    // Should we show the pointer
    //

    bool IsPointerVisible() const;
    void SetPointerVisible(bool visible);
  };

  typedef Nocturnal::SmartPtr<Luna::Volume> VolumePtr;
  typedef std::vector< Luna::Volume* > V_VolumeDumbPtr;
}
