#pragma once

#include "TransformManipulator.h"

namespace Editor
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

  class RotateManipulator : public Editor::TransformManipulator
  {
  private:
    ManipulatorSpace m_Space;
    bool m_AxisSnap;
    float m_SnapDegrees;

    // Rotation drag mode
    RotationType m_Type;

    // UI
    Editor::PrimitiveCircle* m_Ring;

    LUNA_DECLARE_TYPE(Editor::RotateManipulator, Editor::TransformManipulator);
    static void InitializeType();
    static void CleanupType();

  public:
    RotateManipulator(const ManipulatorMode mode, Editor::Scene* scene, PropertiesGenerator* generator);

    ~RotateManipulator();

    virtual void ResetSize() HELIUM_OVERRIDE;

    virtual void ScaleTo(float factor) HELIUM_OVERRIDE;

    virtual void Evaluate() HELIUM_OVERRIDE;

  protected:
    virtual void SetResult() HELIUM_OVERRIDE;

    bool ClosestSphericalIntersection(Math::Line line, Math::Vector3 spherePosition, float sphereRadius, Math::Vector3 cameraPosition, Math::Vector3& intersection);

  public:
    virtual void Draw( DrawArgs* args ) HELIUM_OVERRIDE;
    virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;
    virtual Math::AxesFlags PickRing(PickVisitor* pick, float err);

    virtual bool MouseDown(wxMouseEvent& e) HELIUM_OVERRIDE;
    virtual void MouseUp(wxMouseEvent& e) HELIUM_OVERRIDE;
    virtual void MouseMove(wxMouseEvent& e) HELIUM_OVERRIDE;

    virtual void CreateProperties() HELIUM_OVERRIDE;

    int GetSpace() const;
    void SetSpace(int space);
    
    bool GetAxisSnap() const;
    void SetAxisSnap(bool axisSnap);

    f32 GetSnapDegrees() const;
    void SetSnapDegrees(float snapDegrees);
  };
}