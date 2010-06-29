#include "Precompile.h"
#include "ZonePanel.h"
#include "Foundation/Reflect/Element.h"
#include "Pipeline/Component/ComponentHandle.h"
#include "ComponentOverrideGroup.h"

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

  // set up some parent class configuration
  m_Expanded = true;
  m_Text = TXT( "Zone" ); 
}

ZonePanel::~ZonePanel()
{
}

void ZonePanel::Create()
{
  m_ReflectInterpreter = m_Interpreter->CreateInterpreter<Inspect::ReflectInterpreter>(this);  
  m_ReflectInterpreter->Interpret( m_Zones ); 
}
