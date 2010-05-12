#pragma once

#include "Instance.h"

#include "Content/Controller.h"

namespace Luna
{
  class ControllerType;

  /////////////////////////////////////////////////////////////////////////////
  // Luna's wrapper for an volume instance.
  // 
  class Controller : public Luna::Instance
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::Controller, Luna::Instance );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    Controller(Luna::Scene* s);
    Controller(Luna::Scene* s, Content::Controller* volume);
    virtual ~Controller();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;
    virtual SceneNodeTypePtr CreateNodeType( Luna::Scene* scene ) const NOC_OVERRIDE;

    virtual void Evaluate(GraphDirection direction) NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawBounds( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr<Luna::Controller> LControllerPtr;
}
