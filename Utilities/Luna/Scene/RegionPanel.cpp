#include "Precompile.h"
#include "RegionPanel.h"
#include "Foundation/Reflect/Element.h"

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

  // set up some parent class configuration
  m_Expanded = true;
  m_Text = TXT( "Region" ); 
}

RegionPanel::~RegionPanel()
{
}

void RegionPanel::Create()
{
  m_ReflectInterpreter = m_Interpreter->CreateInterpreter<Inspect::ReflectInterpreter>(this);  
  m_ReflectInterpreter->Interpret( m_Regions ); 
}
