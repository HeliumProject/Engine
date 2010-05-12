#pragma once

#include "Instance.h"

#include "Content/Locator.h"

namespace Luna
{
  class LocatorType;

  /////////////////////////////////////////////////////////////////////////////
  // Luna's wrapper for an locator instance.
  // 
  class Locator : public Luna::Instance
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::Locator, Luna::Instance );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    Locator(Luna::Scene* s);
    Locator(Luna::Scene* s, Content::Locator* locator);
    virtual ~Locator();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;
    virtual SceneNodeTypePtr CreateNodeType( Luna::Scene* scene ) const NOC_OVERRIDE;

    int GetShape() const;
    void SetShape( int shape );

    virtual void Evaluate(GraphDirection direction) NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;
    
    static void CreatePanel( CreatePanelArgs& args );
  };

  typedef Nocturnal::SmartPtr<Luna::Locator> LLocatorPtr;
}
