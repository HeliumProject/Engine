#include "Precompile.h" 
#include "CharacterPropertiesPanel.h" 
#include "CharacterEditor.h" 
#include "CharacterManager.h" 

namespace Luna
{
  CharacterPropertiesPanel::CharacterPropertiesPanel(CharacterEditor* editor)
    : PropertiesPanel(editor), 
      m_Editor(editor)
  {

    m_CanvasWindow = new Inspect::CanvasWindow(this, wxID_ANY, wxDefaultPosition, wxSize(350, 300), wxALWAYS_SHOW_SB | wxCLIP_CHILDREN); 
    m_Canvas.SetControl(m_CanvasWindow); 

    wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add( m_CanvasWindow, 1, wxEXPAND | wxALL, 0 );

    this->SetSizer( sizer );
    this->Layout();
    sizer->Fit( this );

    CharacterManager* manager = m_Editor->GetManager(); 
    manager->AttributeSelectedEvent()->Add( AttributeSelectionDelegate(this, &CharacterPropertiesPanel::OnAttributeSelect)); 
    manager->JointSelectedEvent()->Add( JointSelectionDelegate(this, &CharacterPropertiesPanel::OnJointSelect)); 

    manager->OpenedEvent()->Add( GenericDelegate(this, &CharacterPropertiesPanel::OnOpen) ); 
    manager->ClosedEvent()->Add( GenericDelegate(this, &CharacterPropertiesPanel::OnClose) ); 



  }

  CharacterPropertiesPanel::~CharacterPropertiesPanel()
  {
    CharacterManager* manager = m_Editor->GetManager(); 
    manager->AttributeSelectedEvent()->Remove( AttributeSelectionDelegate(this, &CharacterPropertiesPanel::OnAttributeSelect)); 
    manager->JointSelectedEvent()->Remove( JointSelectionDelegate(this, &CharacterPropertiesPanel::OnJointSelect)); 

    manager->OpenedEvent()->Remove( GenericDelegate(this, &CharacterPropertiesPanel::OnOpen) ); 
    manager->ClosedEvent()->Remove( GenericDelegate(this, &CharacterPropertiesPanel::OnClose) ); 


    if(m_SelectedAttr)
    {
      m_SelectedAttr->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate(this, &CharacterPropertiesPanel::OnAttributeChanged)); 
    }
  }

  void CharacterPropertiesPanel::OnJointSelect(JointSelectionArgs& args)
  {
    // don't build the panel for now. we want to in the future, though. 
    // BuildPanel(); 
  }

  void CharacterPropertiesPanel::OnAttributeSelect(AttributeSelectionArgs& args)
  {
    if(m_SelectedAttr)
    {
      m_SelectedAttr->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate(this, &CharacterPropertiesPanel::OnAttributeChanged)); 
    }

    CharacterManager* manager = m_Editor->GetManager(); 
    m_SelectedAttr = manager->GetSelectedAttribute(); 

    if ( m_SelectedAttr )
    {
      m_SelectedAttr->AddChangedListener( Reflect::ElementChangeSignature::Delegate(this, &CharacterPropertiesPanel::OnAttributeChanged)); 
    }

    BuildPanel(); 


  }

  void CharacterPropertiesPanel::OnAttributeChanged(const Reflect::ElementChangeArgs& args)
  {
    if(args.m_Element == m_SelectedAttr)
    {
      m_Canvas.Read(); 
    }
  }

  void CharacterPropertiesPanel::BuildPanel()
  {
    CharacterManager* manager = m_Editor->GetManager(); 

    m_Canvas.Clear(); 
    m_Canvas.Freeze(); 

    m_Interpreter = new Inspect::ReflectInterpreter(&m_Canvas); 

#pragma TODO("make it so that this set of attributes only displays when the asset is selected")
    if( manager->GetSkeletonAsset() )
    {
      std::vector<Reflect::Element*> asset; 
      asset.push_back( manager->GetSkeletonAsset() ); 

      m_Interpreter->Interpret( asset ); 
    }


    if(m_SelectedAttr)
    {
      std::vector<Reflect::Element*> attributes; 
      attributes.push_back(m_SelectedAttr); 

      m_Interpreter->Interpret(attributes); 
    }

    m_Canvas.Layout(); 
    m_Canvas.Read(); 

    m_Canvas.Thaw(); 

  }

  void CharacterPropertiesPanel::OnOpen(EmptyArgs& args)
  {
    BuildPanel(); 
  }

  void CharacterPropertiesPanel::OnClose(EmptyArgs& args)
  {
    m_Interpreter = NULL; 
    m_Canvas.Clear(); 
  }
}
