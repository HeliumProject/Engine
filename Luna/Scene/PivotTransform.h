#pragma once

#include "Luna/API.h"
#include "Transform.h"

// Forwards
namespace Content
{
  class PivotTransform;
}

namespace Luna
{
  //  TOTAL TRANSFORM:
  //     -1                      -1
  //  [Sp]x[S]x[Sh]x[Sp]x[St]x[Rp]x[R]x[Rp]x[Rt]x[T]x[Tp]

  class LUNA_SCENE_API PivotTransform : public Luna::Transform
  {
    // 
    // Member variables
    // 

  protected:
    Math::Shear m_Shear;
    Math::Vector3 m_ScalePivot;
    Math::Vector3 m_ScalePivotTranslate;
    Math::Vector3 m_RotatePivot;
    Math::Vector3 m_RotatePivotTranslate;
    Math::Vector3 m_TranslatePivot;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::PivotTransform, Luna::Transform );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

  public:
    PivotTransform( Luna::Scene* scene );
    PivotTransform( Luna::Scene* scene, Content::PivotTransform* pivotTransform );

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual tstring GetApplicationTypeName() const NOC_OVERRIDE;

    virtual void Pack() NOC_OVERRIDE;
    virtual void Unpack() NOC_OVERRIDE;


    //
    // Group
    //

  public:
    virtual bool IsGroup()
    {
      return GetType() == Reflect::GetType<Luna::PivotTransform>();
    }


    //
    // Shear
    //

    Math::Shear GetShear() const;
    void SetShear(const Math::Shear& value);


    //
    // ScalePivot
    //

    virtual Math::Vector3 GetScalePivot() const NOC_OVERRIDE;
    virtual void SetScalePivot(const Math::Vector3& value) NOC_OVERRIDE;
    void SetScalePivot(const Math::Vector3& value, bool snapSiblings);


    //
    // ScalePivotTranslate
    //

    Math::Vector3 GetScalePivotTranslate() const;
    void SetScalePivotTranslate(const Math::Vector3& value);


    //
    // RotatePivot
    //

    virtual Math::Vector3 GetRotatePivot() const NOC_OVERRIDE;
    virtual void SetRotatePivot(const Math::Vector3& value) NOC_OVERRIDE;
    void SetRotatePivot(const Math::Vector3& value, bool snapSiblings);


    //
    // RotatePivotTranslate
    //

    Math::Vector3 GetRotatePivotTranslate() const;
    void SetRotatePivotTranslate(const Math::Vector3& value);


    //
    // TranslatePivot
    //

    virtual Math::Vector3 GetTranslatePivot() const NOC_OVERRIDE;
    virtual void SetTranslatePivot(const Math::Vector3& value) NOC_OVERRIDE;
    void SetTranslatePivot(const Math::Vector3& value, bool snapSiblings);


    //
    // SnapPivots
    //

    bool GetSnapPivots() const;
    void SetSnapPivots(bool value);


    //
    // Main routines
    //

  public:
    virtual Math::Matrix4 GetScaleComponent() const NOC_OVERRIDE;
    virtual Math::Matrix4 GetRotateComponent() const NOC_OVERRIDE;
    virtual Math::Matrix4 GetTranslateComponent() const NOC_OVERRIDE;

    // resets transform to identity
    virtual Undo::CommandPtr ResetTransform() NOC_OVERRIDE;

    // recomputes local components from the global matrix
    virtual Undo::CommandPtr ComputeObjectComponents() NOC_OVERRIDE;

    // repositions group nodes WRT thier children
    virtual Undo::CommandPtr CenterTransform() NOC_OVERRIDE;

    // ui integration
    virtual bool ValidatePanel(const tstring& name) NOC_OVERRIDE;

    // creator
    static void CreatePanel(CreatePanelArgs& args);

    f32 GetShearYZ() const;
    void SetShearYZ(f32 translate);
    f32 GetShearXZ() const;
    void SetShearXZ(f32 translate);
    f32 GetShearXY() const;
    void SetShearXY(f32 translate);

    f32 GetScalePivotX() const;
    void SetScalePivotX(f32 translate);
    f32 GetScalePivotY() const;
    void SetScalePivotY(f32 translate);
    f32 GetScalePivotZ() const;
    void SetScalePivotZ(f32 translate);

    f32 GetScalePivotTranslateX() const;
    void SetScalePivotTranslateX(f32 translate);
    f32 GetScalePivotTranslateY() const;
    void SetScalePivotTranslateY(f32 translate);
    f32 GetScalePivotTranslateZ() const;
    void SetScalePivotTranslateZ(f32 translate);

    f32 GetRotatePivotX() const;
    void SetRotatePivotX(f32 translate);
    f32 GetRotatePivotY() const;
    void SetRotatePivotY(f32 translate);
    f32 GetRotatePivotZ() const;
    void SetRotatePivotZ(f32 translate);

    f32 GetRotatePivotTranslateX() const;
    void SetRotatePivotTranslateX(f32 translate);
    f32 GetRotatePivotTranslateY() const;
    void SetRotatePivotTranslateY(f32 translate);
    f32 GetRotatePivotTranslateZ() const;
    void SetRotatePivotTranslateZ(f32 translate);

    f32 GetTranslatePivotX() const;
    void SetTranslatePivotX(f32 translate);
    f32 GetTranslatePivotY() const;
    void SetTranslatePivotY(f32 translate);
    f32 GetTranslatePivotZ() const;
    void SetTranslatePivotZ(f32 translate);
  };

  typedef Nocturnal::SmartPtr<Luna::PivotTransform> LPivotTransformPtr;
}
