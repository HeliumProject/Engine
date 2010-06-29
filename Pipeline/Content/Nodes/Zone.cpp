#include "Pipeline/Content/Nodes/Zone.h"

using namespace Content;
using namespace Component; 

REFLECT_DEFINE_CLASS(Zone)

void Zone::EnumerateClass( Reflect::Compositor<Zone>& comp )
{
  Reflect::Field* fieldColor = comp.AddField( &Zone::m_Color, "m_Color" );
  Reflect::Field* fieldActive = comp.AddField( &Zone::m_Active, "m_Active" );
  Reflect::Field* fieldInteractive = comp.AddField( &Zone::m_Interactive, "m_Interactive" );
  Reflect::Field* fieldBackground = comp.AddField( &Zone::m_Background, "m_Background" );
  Reflect::Field* fieldPath = comp.AddField( &Zone::m_Path, "m_Path", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldGameModes = comp.AddField( &Zone::m_Modes, "m_Modes" );
  Reflect::Field* fieldGlobal = comp.AddField( &Zone::m_Global, "m_Global" );
  Reflect::Field* fieldHasNavData = comp.AddField( &Zone::m_HasNavData, "m_HasNavData" );
}
