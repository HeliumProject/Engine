#pragma once

#include "Pipeline/Content/Nodes/Transform/Transform.h"

#include "Core/Manipulator.h"

#include "HierarchyNode.h"
#include "Foundation/Math/EulerAngles.h"

namespace Luna
{
  // forwards
  class Scene;

  class Transform;
  typedef Nocturnal::SmartPtr< Luna::Transform > TransformPtr;

  // this is fabulous
  namespace ManipulatorSpaces
  {
    enum ManipulatorSpace;
  }
  typedef ManipulatorSpaces::ManipulatorSpace;

  class LUNA_SCENE_API Transform NOC_ABSTRACT : public Luna::HierarchyNode
  {
    // 
    // Member variables
    // 

  protected:
    // simple scale component
    Math::Scale m_Scale;

    // simple rotation component
    Math::EulerAngles m_Rotate;

    // simple translation component
    Math::Vector3 m_Translate;

    // local matrix
    Math::Matrix4 m_ObjectTransform;
    Math::Matrix4 m_InverseObjectTransform;

    // global matrix
    Math::Matrix4 m_GlobalTransform;
    Math::Matrix4 m_InverseGlobalTransform;

    // bind matrix (computed once per dirty setting)
    bool m_BindIsDirty;
    Math::Matrix4 m_BindTransform;
    Math::Matrix4 m_InverseBindTransform;
    
    // Scale color info
    D3DCOLORVALUE m_ScaleColor;
    D3DCOLORVALUE m_ScaleColorGradient;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::Transform, Luna::HierarchyNode );
    static void InitializeType();
    static void CleanupType();


    // 
    // Member functions
    // 

  public:
    Transform( Luna::Scene* scene, Content::Transform* transform );
    virtual ~Transform();

    virtual void Pack() NOC_OVERRIDE;
    virtual void Unpack() NOC_OVERRIDE;


    //
    // Returns this
    //

    virtual Luna::Transform* GetTransform() NOC_OVERRIDE;
    virtual const Luna::Transform* GetTransform() const NOC_OVERRIDE;


    //
    // Scale
    //

    virtual Math::Scale GetScale() const;
    virtual void SetScale( const Math::Scale& value );

    virtual Math::Vector3 GetScalePivot() const;
    virtual void SetScalePivot( const Math::Vector3& value );


    //
    // Rotate
    //

    virtual Math::EulerAngles GetRotate() const;
    virtual void SetRotate( const Math::EulerAngles& value );

    virtual Math::Vector3 GetRotatePivot() const;
    virtual void SetRotatePivot( const Math::Vector3& value );
 

    //
    // Translate
    //

    virtual Math::Vector3 GetTranslate() const;
    virtual void SetTranslate( const Math::Vector3& value );

    virtual Math::Vector3 GetTranslatePivot() const;
    virtual void SetTranslatePivot( const Math::Vector3& value );


    //
    // Object Transform (transformation of object coordinates into local space)
    //

    Math::Matrix4 GetObjectTransform() const
    {
      return m_ObjectTransform;
    }

    Math::Matrix4 GetInverseObjectTransform() const
    {
      return m_InverseObjectTransform;
    }

    void SetObjectTransform( const Math::Matrix4& transform );


    //
    // Parent Transform (transformation of the frame this object is within into world space)
    //

    Math::Matrix4 GetParentTransform() const
    {
      return m_InverseObjectTransform * m_GlobalTransform;
    }

    Math::Matrix4 GetInverseParentTransform() const
    {
      return m_InverseGlobalTransform * m_ObjectTransform;
    }


    //
    // Global Transform (transformation of object coordinates into world space)
    //

    Math::Matrix4 GetGlobalTransform() const
    {
      return m_GlobalTransform;
    }

    Math::Matrix4 GetInverseGlobalTransform() const
    {
      return m_InverseGlobalTransform;
    }

    void SetGlobalTransform( const Math::Matrix4& transform );


    //
    // Binding Matrices
    //

    virtual Math::Matrix4 GetBindTransform() const;
    virtual Math::Matrix4 GetInverseBindTransform() const;


    //
    // Inherit Transform
    //

    bool GetInheritTransform() const;
    void SetInheritTransform(bool inherit);
    
    //
    // Color
    //
    
    D3DCOLORVALUE GetScaleColor() const;
    D3DCOLORVALUE GetScaleColorGradient() const;


    //
    // Main routines
    //

  public:
    // compute scaling component
    virtual Math::Matrix4 GetScaleComponent() const;

    // compute rotation component
    virtual Math::Matrix4 GetRotateComponent() const;

    // compute translation component
    virtual Math::Matrix4 GetTranslateComponent() const;

    // resets transform to identity
    virtual Undo::CommandPtr ResetTransform();

    // recomputes local components from the global matrix
    virtual Undo::CommandPtr ComputeObjectComponents();

    // centers the pivot points
    virtual Undo::CommandPtr CenterTransform();

    // compute all member matrices
    virtual void Evaluate( GraphDirection direction ) NOC_OVERRIDE;

    // render to viewport
    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;

    // issue draw commands
    static void DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    // do intersection testing
    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;

    // manipulator support
    virtual void ConnectManipulator(ManiuplatorAdapterCollection* collection) NOC_OVERRIDE;

    // ui integration
    virtual bool ValidatePanel(const tstring& name) NOC_OVERRIDE;

    // creator
    static void CreatePanel(CreatePanelArgs& args);

    f32 GetScaleX() const;
    void SetScaleX(f32 scale);
    f32 GetScaleY() const;
    void SetScaleY(f32 scale);
    f32 GetScaleZ() const;
    void SetScaleZ(f32 scale);

    f32 GetRotateX() const;
    void SetRotateX(f32 rotate);
    f32 GetRotateY() const;
    void SetRotateY(f32 rotate);
    f32 GetRotateZ() const;
    void SetRotateZ(f32 rotate);

    f32 GetTranslateX() const;
    void SetTranslateX(f32 translate);
    f32 GetTranslateY() const;
    void SetTranslateY(f32 translate);
    f32 GetTranslateZ() const;
    void SetTranslateZ(f32 translate);
  };

  class TransformScaleManipulatorAdapter : public ScaleManipulatorAdapter
  {
  protected:
    Luna::Transform* m_Transform;
    
  public:
    TransformScaleManipulatorAdapter(Luna::Transform* transform)
      : m_Transform (transform)
    {

    }

    virtual Luna::HierarchyNode* GetNode() NOC_OVERRIDE
    {
      return m_Transform;
    }

    virtual Math::Matrix4 GetFrame(ManipulatorSpace space) NOC_OVERRIDE;
    virtual Math::Matrix4 GetObjectMatrix() NOC_OVERRIDE;
    virtual Math::Matrix4 GetParentMatrix() NOC_OVERRIDE;

    virtual Math::Vector3 GetPivot() NOC_OVERRIDE
    {
      return m_Transform->GetScalePivot();
    }

    virtual Math::Scale GetValue() NOC_OVERRIDE
    {
      return m_Transform->GetScale();
    }

    virtual Undo::CommandPtr SetValue(const Math::Scale& v) NOC_OVERRIDE
    {
      return new Undo::PropertyCommand<Math::Scale> ( new Nocturnal::MemberProperty<Luna::Transform, Math::Scale> (m_Transform, &Luna::Transform::GetScale, &Luna::Transform::SetScale), v );
    }
  };

  class TransformScalePivotManipulatorAdapter : public TranslateManipulatorAdapter
  {
  protected:
    Luna::Transform* m_Transform;
    
  public:
    TransformScalePivotManipulatorAdapter(Luna::Transform* transform)
      : m_Transform (transform)
    {

    }

    virtual Luna::HierarchyNode* GetNode() NOC_OVERRIDE
    {
      return m_Transform;
    }

    virtual bool AllowSelfSnap() NOC_OVERRIDE
    {
      return true;
    }

    virtual Math::Matrix4 GetFrame(ManipulatorSpace space) NOC_OVERRIDE;
    virtual Math::Matrix4 GetObjectMatrix() NOC_OVERRIDE;
    virtual Math::Matrix4 GetParentMatrix() NOC_OVERRIDE;

    virtual Math::Vector3 GetPivot() NOC_OVERRIDE
    {
      return Math::Vector3::Zero;
    }

    virtual Math::Vector3 GetValue() NOC_OVERRIDE
    {
      return m_Transform->GetScalePivot();
    }

    virtual Undo::CommandPtr SetValue(const Math::Vector3& v) NOC_OVERRIDE
    {
      return new Undo::PropertyCommand<Math::Vector3> ( new Nocturnal::MemberProperty<Luna::Transform, Math::Vector3> (m_Transform, &Luna::Transform::GetScalePivot, &Luna::Transform::SetScalePivot), v );
    }
  };

  class TransformRotateManipulatorAdapter : public RotateManipulatorAdapter
  {
  protected:
    Luna::Transform* m_Transform;
    
  public:
    TransformRotateManipulatorAdapter(Luna::Transform* transform)
      : m_Transform (transform)
    {

    }

    virtual Luna::HierarchyNode* GetNode() NOC_OVERRIDE
    {
      return m_Transform;
    }

    virtual Math::Matrix4 GetFrame(ManipulatorSpace space) NOC_OVERRIDE;
    virtual Math::Matrix4 GetObjectMatrix() NOC_OVERRIDE;
    virtual Math::Matrix4 GetParentMatrix() NOC_OVERRIDE;

    virtual Math::Vector3 GetPivot() NOC_OVERRIDE
    {
      return m_Transform->GetRotatePivot();
    }

    virtual Math::EulerAngles GetValue() NOC_OVERRIDE
    {
      return m_Transform->GetRotate();
    }

    virtual Undo::CommandPtr SetValue(const Math::EulerAngles& v) NOC_OVERRIDE
    {
      return new Undo::PropertyCommand<Math::EulerAngles> ( new Nocturnal::MemberProperty<Luna::Transform, Math::EulerAngles> (m_Transform, &Luna::Transform::GetRotate, &Luna::Transform::SetRotate), v );
    }
  };

  class TransformRotatePivotManipulatorAdapter : public TranslateManipulatorAdapter
  {
  protected:
    Luna::Transform* m_Transform;
    
  public:
    TransformRotatePivotManipulatorAdapter(Luna::Transform* transform)
      : m_Transform (transform)
    {

    }

    virtual Luna::HierarchyNode* GetNode() NOC_OVERRIDE
    {
      return m_Transform;
    }

    virtual bool AllowSelfSnap() NOC_OVERRIDE
    {
      return true;
    }

    virtual Math::Matrix4 GetFrame(ManipulatorSpace space) NOC_OVERRIDE;
    virtual Math::Matrix4 GetObjectMatrix() NOC_OVERRIDE;
    virtual Math::Matrix4 GetParentMatrix() NOC_OVERRIDE;

    virtual Math::Vector3 GetPivot() NOC_OVERRIDE
    {
      return Math::Vector3::Zero;
    }

    virtual Math::Vector3 GetValue() NOC_OVERRIDE
    {
      return m_Transform->GetRotatePivot();
    }

    virtual Undo::CommandPtr SetValue(const Math::Vector3& v) NOC_OVERRIDE
    {
      return new Undo::PropertyCommand<Math::Vector3> ( new Nocturnal::MemberProperty<Luna::Transform, Math::Vector3> (m_Transform, &Luna::Transform::GetRotatePivot, &Luna::Transform::SetRotatePivot), v );
    }
  };

  class TransformTranslateManipulatorAdapter : public TranslateManipulatorAdapter
  {
  protected:
    Luna::Transform* m_Transform;
    
  public:
    TransformTranslateManipulatorAdapter(Luna::Transform* transform)
      : m_Transform (transform)
    {

    }

    virtual Luna::HierarchyNode* GetNode() NOC_OVERRIDE
    {
      return m_Transform;
    }

    virtual Math::Matrix4 GetFrame(ManipulatorSpace space) NOC_OVERRIDE;
    virtual Math::Matrix4 GetObjectMatrix() NOC_OVERRIDE;
    virtual Math::Matrix4 GetParentMatrix() NOC_OVERRIDE;

    virtual Math::Vector3 GetPivot() NOC_OVERRIDE
    {
      return m_Transform->GetTranslatePivot();
    }

    virtual Math::Vector3 GetValue() NOC_OVERRIDE
    {
      return m_Transform->GetTranslate();
    }

    virtual Undo::CommandPtr SetValue(const Math::Vector3& v) NOC_OVERRIDE
    {
      return new Undo::PropertyCommand<Math::Vector3> ( new Nocturnal::MemberProperty<Luna::Transform, Math::Vector3> (m_Transform, &Luna::Transform::GetTranslate, &Luna::Transform::SetTranslate), v );
    }
  };

  class TransformTranslatePivotManipulatorAdapter : public TranslateManipulatorAdapter
  {
  protected:
    Luna::Transform* m_Transform;
    
  public:
    TransformTranslatePivotManipulatorAdapter(Luna::Transform* transform)
      : m_Transform (transform)
    {

    }

    virtual Luna::HierarchyNode* GetNode() NOC_OVERRIDE
    {
      return m_Transform;
    }

    virtual bool AllowSelfSnap() NOC_OVERRIDE
    {
      return true;
    }

    virtual Math::Matrix4 GetFrame(ManipulatorSpace space) NOC_OVERRIDE;
    virtual Math::Matrix4 GetObjectMatrix() NOC_OVERRIDE;
    virtual Math::Matrix4 GetParentMatrix() NOC_OVERRIDE;

    virtual Math::Vector3 GetPivot() NOC_OVERRIDE
    {
      return Math::Vector3::Zero;
    }

    virtual Math::Vector3 GetValue() NOC_OVERRIDE
    {
      return m_Transform->GetTranslatePivot();
    }

    virtual Undo::CommandPtr SetValue(const Math::Vector3& v) NOC_OVERRIDE
    {
      return new Undo::PropertyCommand<Math::Vector3> ( new Nocturnal::MemberProperty<Luna::Transform, Math::Vector3> (m_Transform, &Luna::Transform::GetTranslatePivot, &Luna::Transform::SetTranslatePivot), v );
    }
  };
}
