#pragma once

#include "API.h"
#include "Core/Selectable.h"
#include "Inspect/Panel.h"
#include "InspectSymbol/SymbolInterpreter.h"

namespace Inspect
{
  class Choice;
  class Action;
}

namespace Luna
{
  class RuntimeDataAdapterBase : public Selectable
  {
  public: 
    virtual std::string            GetRuntimeClass() const = 0; 
    virtual void                   SetRuntimeClass(const std::string& name) = 0; 
    virtual void                   GetValidRuntimeClasses(V_string& classes) = 0; 
    virtual bool                   RectifyRuntimeData() = 0; 
    virtual Symbol::UDTInstancePtr GetRuntimeInstance() = 0; 
  }; 

  class RuntimeDataPanel : public Inspect::Panel
  {
  public: 
    RuntimeDataPanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection); 
    virtual void Create() NOC_OVERRIDE; 

  protected: 
    // sets up internal state (booleans like m_AllInstancesHaveRuntimeData...)
    // does not change any UI
    //
    void RectifyRuntimeData(); 

    // evaluate our selection and return the following information
    // does not change any UI
    // 
    void GetSelectableClasses( S_string& classList ); 
    void GetSelectableBaseClasses( S_string& classList ); 
    void GetCurrentSelectedClass( std::string& selectedClass ); 

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

  private: 
    Enumerator*                      m_Enumerator;
    OS_SelectableDumbPtr              m_Selection; 
    
    Inspect::SymbolInterpreterPtr     m_SymbolInterpreter;
    Inspect::Action*                  m_RuntimeClassesButton;
    Inspect::Choice*                  m_RuntimeClassesChoice;
    Inspect::Panel*                   m_RuntimeDataPanel;
    

    bool                              m_AllInstancesHaveRuntimeData; 
    bool                              m_EnableClassPicker; 
    bool                              m_EnableClassBrowser; 
    bool                              m_EnableSymbolInterpreter; 
    bool                              m_RecurseSelectableClasses; 

  };

  template <class T, class P>
  class RuntimeDataAdapter : public RuntimeDataAdapterBase
  {
  public: 
    RuntimeDataAdapter(T* target) 
      : m_Target( target )
    {
      NOC_ASSERT(m_Target); 
    }

    virtual std::string GetRuntimeClass() const
    {
      return m_Target->GetPackage<P>()->GetRuntimeClass(); 
    }

    virtual void SetRuntimeClass(const std::string& name)
    {
      return m_Target->GetPackage<P>()->SetRuntimeClass(name); 
    }

    virtual void GetValidRuntimeClasses(V_string& classes)
    {
      return m_Target->GetPackage<P>()->GetValidRuntimeClasses(classes); 
    }

    virtual bool RectifyRuntimeData()
    {
      return m_Target->GetPackage<P>()->RectifyRuntimeData(); 
    }

    virtual Symbol::UDTInstancePtr GetRuntimeInstance()
    {
      return m_Target->GetPackage<P>()->GetRuntimeInstance(); 
    }

    static void CreateSelection(const OS_SelectableDumbPtr& input, OS_SelectableDumbPtr& output)
    {
      for(OS_SelectableDumbPtr::Iterator itr = input.Begin(); itr != input.End(); ++itr)
      {
        T* object = Reflect::ObjectCast<T>( *itr ); 
        if(object)
        {
          output.Append( new RuntimeDataAdapter<T, P>( object )); 
          output.Back()->SetSelected(true); 
        }
      }
    }

    static void DeleteSelection(OS_SelectableDumbPtr& selection)
    {
      for(OS_SelectableDumbPtr::Iterator itr = selection.Begin(); itr != selection.End(); ++itr)
      {
        delete (*itr); 
        *itr = NULL; 
      }

      selection.Clear(); 
    }

  
  private: 
    T* m_Target; 
  }; 


}
