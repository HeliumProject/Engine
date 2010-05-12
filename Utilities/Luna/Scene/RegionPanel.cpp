#include "Precompile.h"
#include "RegionPanel.h"
#include "Reflect/Element.h"

using namespace Luna; 

RegionPanel::RegionPanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection)
: m_Selection(selection)
{
  m_Interpreter = m_Enumerator = enumerator;

  // notice that we are copying the original selection into m_Selection

  OS_SelectableDumbPtr::Iterator itr = selection.Begin();
  OS_SelectableDumbPtr::Iterator end = selection.End(); 

  for( ; itr != end; ++itr)
  {
    Selectable* selectable = (*itr); 
    Luna::Region* region = Reflect::ObjectCast<Luna::Region>( selectable ); 

    if(region)
    {
      m_Regions.push_back( region->GetPackage<Content::Region>() ); 
    }
  }

  // make another copy of the selection for the runtime data panel. 
  LRegionRuntimeDataAdapter::CreateSelection(m_Selection, m_RuntimeSelection); 

  // set up some parent class configuration
  m_Expanded = true;
  m_Text = "Region"; 
}

RegionPanel::~RegionPanel()
{
  LRegionRuntimeDataAdapter::DeleteSelection(m_RuntimeSelection); 
}

void RegionPanel::Create()
{
  m_ReflectInterpreter = m_Interpreter->CreateInterpreter<Inspect::ReflectInterpreter>(this);  
  m_ReflectInterpreter->Interpret( m_Regions ); 

  m_RuntimeDataPanel = new RuntimeDataPanel(m_Enumerator, m_RuntimeSelection); 
  m_RuntimeDataPanel->SetExpanded(true); 

  m_Enumerator->Push( m_RuntimeDataPanel );
  {
    m_RuntimeDataPanel->SetCanvas( m_Enumerator->GetContainer()->GetCanvas() );
    m_RuntimeDataPanel->Create();
  }
  m_Enumerator->Pop();
}
