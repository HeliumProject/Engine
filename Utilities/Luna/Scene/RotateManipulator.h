#pragma once

#include "TransformManipulator.h"

namespace Luna
{
  class PrimitiveCircle;

  namespace RotationTypes
  {
    enum RotationType
    {
      None,
      Normal,
      ArcBall,
      CameraPlane,
    };
  }

  typedef RotationTypes::RotationType RotationType;

  class RotateManipulator : public Luna::TransformManipulator
  {
  private:
    ManipulatorSpace m_Space;
    bool m_AxisSnap;
    float m_SnapDegrees;

    // Rotation drag mode
    RotationType m_Type;

    // UI
    Luna::PrimitiveCircle* m_Ring;

    LUNA_DECLARE_TYPE(Luna::RotateManipulator, Luna::TransformManipulator);
    static void InitializeType();
    static void CleanupType();

  public:
    RotateManipulator(const ManipulatorMode mode, Luna::Scene* scene, Enumerator* enumerator);

    ~RotateManipulator();

    virtual void ResetSize() NOC_OVERRIDE;

    virtual void ScaleTo(float factor) NOC_OVERRIDE;

    virtual void Evaluate() NOC_OVERRIDE;

  protected:
    virtual void SetResult() NOC_OVERRIDE;

    bool ClosestSphericalIntersection(Math::Line line, Math::Vector3 spherePosition, float sphereRadius, Math::Vector3 cameraPosition, Math::Vector3& intersection);

  public:
    virtual void Draw( DrawArgs* args ) NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;
    virtual Math::AxesFlags PickRing(PickVisitor* pick, float err);

    virtual bool MouseDown(wxMouseEvent& e) NOC_OVERRIDE;
    virtual void MouseUp(wxMouseEvent& e) NOC_OVERRIDE;
    virtual void MouseMove(wxMouseEvent& e) NOC_OVERRIDE;

    virtual void CreateProperties() NOC_OVERRIDE;

    int GetSpace() const;
    void SetSpace(int space);
    
    bool GetAxisSnap() const;
    void SetAxisSnap(bool axisSnap);

    f32 GetSnapDegrees() const;
    void SetSnapDegrees(float snapDegrees);
  };
}