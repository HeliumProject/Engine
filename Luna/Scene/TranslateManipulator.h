#pragma once

#include "TransformManipulator.h"

namespace Luna
{
  class PrimitiveAxes;
  class PrimitiveCone;
  class PrimitiveCircle;

  namespace TranslateSnappingModes
  {
    enum TranslateSnappingMode
    {
      None,
      Surface,
      Object,
      Vertex,
      Offset,
      Grid,
    };
    static void TranslateSnappingModeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(None, TXT( "None" ) );
      info->AddElement(Surface, TXT( "Surface" ) );
      info->AddElement(Object, TXT( "Object" ) );
      info->AddElement(Vertex, TXT( "Vertex" ) );
      info->AddElement(Offset, TXT( "Offset" ) );
      info->AddElement(Grid, TXT( "Grid" ) );
    }
  }
  typedef TranslateSnappingModes::TranslateSnappingMode TranslateSnappingMode;

  class TranslateManipulator : public Luna::TransformManipulator
  {
  private:
    // Properties
    ManipulatorSpace m_Space;
    TranslateSnappingMode m_SnappingMode;
    float m_Distance;
    bool m_LiveObjectsOnly;

    // UI
    TranslateSnappingMode m_HotSnappingMode;
    bool m_ShowCones;
    float m_Factor;
    Luna::PrimitiveAxes* m_Axes;
    Luna::PrimitiveCircle* m_Ring;
    Luna::PrimitiveCone* m_XCone;
    Math::Vector3 m_XPosition;
    Luna::PrimitiveCone* m_YCone;
    Math::Vector3 m_YPosition;
    Luna::PrimitiveCone* m_ZCone;
    Math::Vector3 m_ZPosition;

    LUNA_DECLARE_TYPE(Luna::TranslateManipulator, Luna::TransformManipulator);
    static void InitializeType();
    static void CleanupType();

  public:
    TranslateManipulator(const ManipulatorMode mode, Luna::Scene* scene, PropertiesGenerator* generator);
    ~TranslateManipulator();

    TranslateSnappingMode GetSnappingMode() const;

    virtual void ResetSize() NOC_OVERRIDE;

    virtual void ScaleTo(float f) NOC_OVERRIDE;

    virtual void Evaluate() NOC_OVERRIDE;

  protected:
    virtual void SetResult() NOC_OVERRIDE;

    void DrawPoints(Math::AxesFlags axis);

  public:
    virtual void Draw( DrawArgs* args ) NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;

    virtual bool MouseDown(wxMouseEvent& e) NOC_OVERRIDE;
    virtual void MouseMove(wxMouseEvent& e) NOC_OVERRIDE;

    virtual void KeyPress( wxKeyEvent& e ) NOC_OVERRIDE;
    virtual void KeyDown( wxKeyEvent& e ) NOC_OVERRIDE;
    virtual void KeyUp( wxKeyEvent& e ) NOC_OVERRIDE;

    virtual void CreateProperties() NOC_OVERRIDE;

    int GetSpace() const;
    void SetSpace(int space);

    bool GetLiveObjectsOnly() const;
    void SetLiveObjectsOnly(bool liveSnap);

    bool GetSurfaceSnap() const;
    void SetSurfaceSnap(bool polygonSnap);

    bool GetObjectSnap() const;
    void SetObjectSnap(bool polygonSnap);

    bool GetVertexSnap() const;
    void SetVertexSnap(bool vertexSnap);

    bool GetOffsetSnap() const;
    void SetOffsetSnap(bool offsetSnap);

    bool GetGridSnap() const;
    void SetGridSnap(bool gridSnap);

    float GetDistance() const;
    void SetDistance(float distance);
  };
}
