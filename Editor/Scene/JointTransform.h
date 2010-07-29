#pragma once

#include "Editor/API.h"
#include "Transform.h"

// Forwards
namespace Content
{
  class JointTransform;
}

namespace Editor
{
  // Forwards
  class PrimitiveRings;

  class JointTransform : public Editor::Transform
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Editor::JointTransform, Editor::Transform );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

  public:
    JointTransform( Editor::Scene* scene, Content::JointTransform* joint );
    virtual ~JointTransform();

    virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
    virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

    virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
    static void DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;
  };
}
