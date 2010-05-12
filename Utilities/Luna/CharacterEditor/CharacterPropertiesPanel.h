#pragma once

#include "CharacterForms.h" 
#include "CharacterManager.h" 

#include "Inspect/Canvas.h" 
#include "InspectReflect/ReflectInterpreter.h" 

#include "Reflect/Element.h" 

namespace Luna
{
  class CharacterEditor; 

  class CharacterPropertiesPanel : public PropertiesPanel
  {
  public: 
    CharacterPropertiesPanel(CharacterEditor* editor); 
    ~CharacterPropertiesPanel(); 

  private: 
    CharacterEditor*                  m_Editor; 
    Inspect::CanvasWindow*         m_CanvasWindow; 
    Inspect::Canvas                m_Canvas; 
    Inspect::ReflectInterpreterPtr m_Interpreter; 
    Content::JointAttributePtr         m_SelectedAttr; 

    void OnAttributeSelect(AttributeSelectionArgs& args); 
    void OnJointSelect(JointSelectionArgs& args);
    void OnAttributeChanged(const Reflect::ElementChangeArgs& args); 

    void OnOpen(EmptyArgs& args); 
    void OnClose(EmptyArgs& args); 

    void BuildPanel(); 


  }; 
}; 
