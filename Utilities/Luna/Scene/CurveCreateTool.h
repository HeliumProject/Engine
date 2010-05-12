#pragma once

#include "API.h"

#include "Content/Curve.h"
#include "Curve.h"
#include "SceneTool.h"

namespace Luna
{
  class CurveCreateTool : public Luna::SceneTool
  {
    //
    //  Members
    //

  private:
    // Created flag
    bool m_Created;

    // The selection of the created objects
    OS_SelectableDumbPtr m_Selection;

    // The instance we are creating
    LCurvePtr m_Instance;

  public:
    // Type of curve interpolation
    static Content::CurveType s_CurveType;

    // Should we snap to surfaces
    static bool s_SurfaceSnap;

    // Should we snap to objects
    static bool s_ObjectSnap;

    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(Luna::CurveCreateTool, Luna::SceneTool);
    static void InitializeType();
    static void CleanupType();

  public:
    CurveCreateTool( Luna::Scene* scene, Enumerator* enumerator );
    virtual ~CurveCreateTool();

    void CreateInstance( const Math::Vector3& position );
    void PickPosition( int x, int y, Math::Vector3& position );

  private:
    void AddToScene();

  public:
    virtual bool AllowSelection() NOC_OVERRIDE;

    virtual bool MouseDown( wxMouseEvent& e ) NOC_OVERRIDE;
    virtual void MouseMove( wxMouseEvent& e ) NOC_OVERRIDE;
    virtual void KeyPress( wxKeyEvent& e ) NOC_OVERRIDE;

    virtual void CreateProperties() NOC_OVERRIDE;

    bool GetSurfaceSnap() const;
    void SetSurfaceSnap( bool snap );

    bool GetObjectSnap() const;
    void SetObjectSnap( bool snap );

    int GetPlaneSnap() const;
    void SetPlaneSnap(int snap);

    int GetCurveType() const;
    void SetCurveType( int selection );

    bool GetClosed() const;
    void SetClosed( bool closed );
  };
}