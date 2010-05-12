#pragma once

#include "Instance.h"

#include "Content/GameCamera.h"

namespace Luna
{
  class GameCameraType;

  /////////////////////////////////////////////////////////////////////////////
  // Luna's wrapper for an volume instance.
  // 
  class GameCamera : public Luna::Instance
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::GameCamera, Luna::Instance );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    GameCamera(Luna::Scene* s);
    GameCamera(Luna::Scene* s, Content::GameCamera* volume);
    virtual ~GameCamera();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;
    virtual SceneNodeTypePtr CreateNodeType( Luna::Scene* scene ) const NOC_OVERRIDE;

    virtual void Evaluate(GraphDirection direction) NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawBounds( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr<Luna::GameCamera> LGameCameraPtr;
}
