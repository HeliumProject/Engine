#pragma once

#include "TransformManipulator.h"

namespace Editor
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

  class TranslateManipulator : public Editor::TransformManipulator
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
    Editor::PrimitiveAxes* m_Axes;
    Editor::PrimitiveCircle* m_Ring;
    Editor::PrimitiveCone* m_XCone;
    Math::Vector3 m_XPosition;
    Editor::PrimitiveCone* m_YCone;
    Math::Vector3 m_YPosition;
    Editor::PrimitiveCone* m_ZCone;
    Math::Vector3 m_ZPosition;

    LUNA_DECLARE_TYPE(Editor::TranslateManipulator, Editor::TransformManipulator);
    static void InitializeType();
    static void CleanupType();

  public:
    TranslateManipulator(const ManipulatorMode mode, Editor::Scene* scene, PropertiesGenerator* generator);
    ~TranslateManipulator();

    TranslateSnappingMode GetSnappingMode() const;

    virtual void ResetSize() HELIUM_OVERRIDE;

    virtual void ScaleTo(float f) HELIUM_OVERRIDE;

    virtual void Evaluate() HELIUM_OVERRIDE;

  protected:
    virtual void SetResult() HELIUM_OVERRIDE;

    void DrawPoints(Math::AxesFlags axis);

  public:
    virtual void Draw( DrawArgs* args ) HELIUM_OVERRIDE;
    virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

    virtual bool MouseDown(wxMouseEvent& e) HELIUM_OVERRIDE;
    virtual void MouseMove(wxMouseEvent& e) HELIUM_OVERRIDE;

    virtual void KeyPress( wxKeyEvent& e ) HELIUM_OVERRIDE;
    virtual void KeyDown( wxKeyEvent& e ) HELIUM_OVERRIDE;
    virtual void KeyUp( wxKeyEvent& e ) HELIUM_OVERRIDE;

    virtual void CreateProperties() HELIUM_OVERRIDE;

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
