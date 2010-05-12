#include "Precompile.h"
#include "ZonePanel.h"
#include "Reflect/Element.h"
#include "Attribute/AttributeHandle.h"
#include "Content/CinematicListAttribute.h"
#include "AttributeOverrideGroup.h"

using namespace Luna; 

ZonePanel::ZonePanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection)
: m_Selection(selection)
{
  m_Interpreter = m_Enumerator = enumerator;

  // notice that we are copying the original selection into m_Selection

  OS_SelectableDumbPtr::Iterator itr = selection.Begin();
  OS_SelectableDumbPtr::Iterator end = selection.End(); 

  for( ; itr != end; ++itr)
  {
    Selectable* selectable = (*itr); 
    Zone* zone = Reflect::ObjectCast<Zone>( selectable ); 

    if(zone)
    {
      m_Zones.push_back( zone->GetPackage<Content::Zone>() ); 
    }
  }

  // make another copy of the selection for the runtime data panel. 
  ZoneRuntimeDataAdapter::CreateSelection(m_Selection, m_RuntimeSelection); 

  // set up some parent class configuration
  m_Expanded = true;
  m_Text = "Zone"; 
}

ZonePanel::~ZonePanel()
{
  ZoneRuntimeDataAdapter::DeleteSelection(m_RuntimeSelection); 
}

void ZonePanel::Create()
{
  m_ReflectInterpreter = m_Interpreter->CreateInterpreter<Inspect::ReflectInterpreter>(this);  
  m_ReflectInterpreter->Interpret( m_Zones ); 

  m_RuntimeDataPanel = new RuntimeDataPanel(m_Enumerator, m_RuntimeSelection); 
  m_RuntimeDataPanel->SetExpanded(true); 

  m_Enumerator->Push( m_RuntimeDataPanel );
  {
    m_RuntimeDataPanel->SetCanvas( m_Enumerator->GetContainer()->GetCanvas() );
    m_RuntimeDataPanel->Create();
  }
  m_Enumerator->Pop();

  AttributeOverrideGroup< Content::CinematicListAttribute, Zone >* cineListPanel = 
    new AttributeOverrideGroup< Content::CinematicListAttribute, Zone >( "Cinematics", m_Enumerator, m_Selection, 
                           &Luna::SceneNode::HasAttribute< Content::CinematicListAttribute >, 
                           &Luna::SceneNode::SetAttribute< Content::CinematicListAttribute > );

  m_Enumerator->Push( cineListPanel );
  {
    cineListPanel->SetCanvas( m_Enumerator->GetContainer()->GetCanvas() );
    cineListPanel->Create();
  }
  m_Enumerator->Pop();

}
