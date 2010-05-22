#include "Zone.h"
#include "Finder/Finder.h"
#include "FileSystem/FileSystem.h"
#include "Console/Console.h"

#include "Attribute/AttributeHandle.h"

using namespace Content;
using namespace Attribute; 

REFLECT_DEFINE_CLASS(Zone)

void Zone::EnumerateClass( Reflect::Compositor<Zone>& comp )
{
  Reflect::Field* fieldColor = comp.AddField( &Zone::m_Color, "m_Color" );
  Reflect::Field* fieldActive = comp.AddField( &Zone::m_Active, "m_Active" );
  Reflect::Field* fieldInteractive = comp.AddField( &Zone::m_Interactive, "m_Interactive" );
  Reflect::Field* fieldBackground = comp.AddField( &Zone::m_Background, "m_Background" );
  Reflect::Field* fieldFileReference = comp.AddField( &Zone::m_FileReference, "m_FileReference", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldGameModes = comp.AddField( &Zone::m_Modes, "m_Modes" );
  Reflect::Field* fieldRegions = comp.AddField( &Zone::m_Regions, "m_Regions", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldGlobal = comp.AddField( &Zone::m_Global, "m_Global" );
  Reflect::Field* fieldHasNavData = comp.AddField( &Zone::m_HasNavData, "m_HasNavData" );
}
