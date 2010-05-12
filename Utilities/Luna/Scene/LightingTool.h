#pragma once

#include "SceneTool.h"
#include "Entity.h"
#include "LightingEnvironment.h"
#include "Inspect/Button.h"
#include "Content/Scene.h"

namespace Luna
{
  class PickVisitor;
  class TranslateManipulator;

  class LightingTool : public Luna::SceneTool
  {
  private:
   (
     // UI globals
    static bool RenderSelectedOnly;
    static u32  PreviewWidth;
    static u32  PreviewHeight;

  private:
  
    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(Luna::LightingTool, Luna::SceneTool);
    static void InitializeType();
    static void CleanupType();

    //
    // Internal Utility
    //

    void RenderAllVisible( Content::Scene& exportScene, S_tuid& assetClasses );
    void RenderSelected( Content::Scene& exportScene, S_tuid& assetClasses );

  public:
    LightingTool( Luna::Scene* scene, Enumerator* enumerator );
    virtual ~LightingTool();

    virtual bool AllowSelection() NOC_OVERRIDE;
    virtual void CreateProperties() NOC_OVERRIDE;
    virtual bool ValidateSelection( OS_SelectableDumbPtr& items ) NOC_OVERRIDE;

    virtual bool Initialize() NOC_OVERRIDE;

    static void OnButtonPreviewRender( Inspect::Button* button );

    bool GetRenderSelected() const;
    void SetRenderSelected( bool selectedOnly );  

  };

  typedef Nocturnal::SmartPtr< Luna::LightingTool > LightingToolPtr;

}
