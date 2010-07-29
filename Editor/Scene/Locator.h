#pragma once

#include "Instance.h"

#include "Pipeline/Content/Nodes/Locator.h"

namespace Editor
{
  class LocatorType;

  /////////////////////////////////////////////////////////////////////////////
  // Editor's wrapper for an locator instance.
  // 
  class Locator : public Editor::Instance
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Editor::Locator, Editor::Instance );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    Locator(Editor::Scene* s);
    Locator(Editor::Scene* s, Content::Locator* locator);
    virtual ~Locator();

    virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
    virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;
    virtual SceneNodeTypePtr CreateNodeType( Editor::Scene* scene ) const HELIUM_OVERRIDE;

    int GetShape() const;
    void SetShape( int shape );

    virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;

    virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
    static void DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

    virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;
    
    static void CreatePanel( CreatePanelArgs& args );
  };

  typedef Helium::SmartPtr<Editor::Locator> LLocatorPtr;
}
