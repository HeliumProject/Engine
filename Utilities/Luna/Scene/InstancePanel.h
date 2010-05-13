#pragma once

#include "API.h"
#include "SceneNodePanel.h"
#include "Core/Selectable.h"

namespace Luna
{
  class InstancePanel : public Inspect::Panel
  {
  protected:
    Enumerator* m_Enumerator;
    OS_SelectableDumbPtr m_Selection;
    Inspect::CheckBox* m_Solid;
    Inspect::CheckBox* m_SolidOverride;
    Inspect::CheckBox* m_Transparent;
    Inspect::CheckBox* m_TransparentOverride;

    bool                   m_EnableClassPicker; 
    bool                   m_EnableClassBrowser; 
    bool                   m_EnableSymbolInterpreter; 
    bool                   m_RecurseSelectableClasses; 

  public:
    InstancePanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection);
    virtual void Create() NOC_OVERRIDE;

  protected:
    void CreateApplicationType();
    void CreateAppearanceFlags();
    virtual void CreateRuntimeData();

    // sets up internal state (booleans like m_AllInstancesHaveRuntimeData...)
    // does not change any UI
    //
    virtual void RectifyRuntimeData(); 

    // evaluate our selection and return the following information
    // does not change any UI
    // 
    virtual void GetSelectableClasses( S_string& classList ); 
    virtual void GetSelectableBaseClasses( S_string& classList ); 
    virtual void GetCurrentSelectedClass( std::string& selectedClass ); 

    // event handlers
    void OnRuntimeClassButton( Inspect::Button* button );
    void OnRuntimeClassChanged( const Inspect::ChangeArgs& args );

    // refresh the UI after changes..
    // 
    void RefreshClassPicker(); 
    void RefreshInstances( bool layout );

    void OnSolidOverride( const Inspect::ChangeArgs& args );
    void OnTransparentOverride( const Inspect::ChangeArgs& args );

    // utility functions
    //
    static void Intersect(S_string& intersection, const S_string& classList);
    static void RecurseAddDerivedClasses( const std::string& baseClass, S_string& classList );   
  };
}
