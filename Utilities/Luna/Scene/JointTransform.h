#pragma once

#include "Luna/API.h"
#include "Transform.h"

// Forwards
namespace Content
{
  class JointTransform;
}

namespace Luna
{
  // Forwards
  class PrimitiveRings;

  class JointTransform : public Luna::Transform
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::JointTransform, Luna::Transform );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

  public:
    JointTransform( Luna::Scene* scene, Content::JointTransform* joint );
    virtual ~JointTransform();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;
  };
}
