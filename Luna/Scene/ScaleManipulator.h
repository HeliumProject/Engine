#pragma once

#include "TransformManipulator.h"

namespace Luna
{
  class PrimitiveAxes;
  class PrimitiveCube;

  class ScaleManipulator : public Luna::TransformManipulator
  {
  private:
    // UI
    Luna::PrimitiveAxes* m_Axes;
    Luna::PrimitiveCube* m_Cube;
    Luna::PrimitiveCube* m_XCube;
    Math::Vector3 m_XPosition;
    Luna::PrimitiveCube* m_YCube;
    Math::Vector3 m_YPosition;
    Luna::PrimitiveCube* m_ZCube;
    Math::Vector3 m_ZPosition;

    bool m_GridSnap;
    float m_Distance;

    LUNA_DECLARE_TYPE(Luna::ScaleManipulator, Luna::TransformManipulator);
    static void InitializeType();
    static void CleanupType();

  public:
    ScaleManipulator(const ManipulatorMode mode, Luna::Scene* scene, PropertiesGenerator* generator);

    ~ScaleManipulator();

    virtual void ResetSize() NOC_OVERRIDE;

    virtual void ScaleTo(float factor) NOC_OVERRIDE;

    virtual void Evaluate() NOC_OVERRIDE;

  protected:
    virtual void SetResult() NOC_OVERRIDE;

  public:
    virtual void Draw( DrawArgs* args ) NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;

    virtual bool MouseDown(wxMouseEvent& e) NOC_OVERRIDE;
    virtual void MouseMove(wxMouseEvent& e) NOC_OVERRIDE;
    
    virtual void CreateProperties() NOC_OVERRIDE;

    bool GetGridSnap() const;
    void SetGridSnap( bool gridSnap );

    float GetDistance() const;
    void SetDistance( float distance );
  };
}
