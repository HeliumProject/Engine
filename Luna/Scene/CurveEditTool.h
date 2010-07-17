#pragma once

#include "SceneTool.h"

#include "Pipeline/Content/Nodes/Curve.h"
#include "Curve.h"

namespace Luna
{
  class PickVisitor;
  class TranslateManipulator;

  namespace CurveEditModes
  {
    enum CurveEditMode
    {
      None,
      Modify,
      Insert,
      Remove,
    };
  }
  typedef CurveEditModes::CurveEditMode CurveEditMode;

  class CurveEditTool: public Luna::SceneTool
  {
  private:
    static CurveEditMode s_EditMode;
    static bool s_CurrentSelection;

    CurveEditMode m_HotEditMode;
    Luna::TranslateManipulator* m_ControlPointManipulator;
 
    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(Luna::CurveEditTool, Luna::SceneTool);
    static void InitializeType();
    static void CleanupType();

  public:
    CurveEditTool( Luna::Scene* scene, PropertiesGenerator* generator );
    virtual ~CurveEditTool();

    CurveEditMode GetEditMode() const;

    virtual bool MouseDown( wxMouseEvent& e ) NOC_OVERRIDE;
    virtual void MouseUp( wxMouseEvent& e) NOC_OVERRIDE;
    virtual void MouseMove( wxMouseEvent& e ) NOC_OVERRIDE;

    virtual void KeyPress( wxKeyEvent& e ) NOC_OVERRIDE;
    virtual void KeyDown( wxKeyEvent& e ) NOC_OVERRIDE;
    virtual void KeyUp( wxKeyEvent& e ) NOC_OVERRIDE;

    virtual bool ValidateSelection( OS_SelectableDumbPtr& items ) NOC_OVERRIDE;

    virtual void Evaluate() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args ) NOC_OVERRIDE;

    virtual void CreateProperties() NOC_OVERRIDE;

    int GetCurveEditMode() const;
    void SetCurveEditMode( int mode );
    
    bool GetSelectionMode() const;
    void SetSelectionMode( bool mode );
    
    void StoreSelectedCurves();
    
  protected:
    OS_SelectableDumbPtr m_SelectedCurves;
  };
}
