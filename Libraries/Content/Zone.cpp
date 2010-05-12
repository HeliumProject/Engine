#include "Zone.h"
#include "File/Manager.h"
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
  Reflect::Field* fieldFileID = comp.AddField( &Zone::m_FileID, "m_FileID", Reflect::FieldFlags::FileID | Reflect::FieldFlags::Hide );
  Reflect::Field* fieldGameModes = comp.AddField( &Zone::m_Modes, "m_Modes" );
  Reflect::Field* fieldRegions = comp.AddField( &Zone::m_Regions, "m_Regions", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldGlobal = comp.AddField( &Zone::m_Global, "m_Global" );
  Reflect::Field* fieldHasNavData = comp.AddField( &Zone::m_HasNavData, "m_HasNavData" );
}

std::string Zone::GetFilePath() const
{
  std::string filePath = "";
  if ( !File::GlobalManager().GetPath( m_FileID, filePath ) )
  {
    throw Nocturnal::Exception( "Could not locate a path with id: "TUID_HEX_FORMAT" when attempting to locate a zone's class path.\n", m_FileID );
  }
  return filePath;
}

std::string Zone::GetBuiltDir()
{
  std::string filePath = GetFilePath();

  FileSystem::StripLeaf( filePath );
  FileSystem::StripPrefix( Finder::ProjectAssets(), filePath );
  filePath = Finder::ProjectBuilt() + filePath + "zones/" + GetName();
  FileSystem::GuaranteeSlash( filePath );

  return filePath;
}

std::string Zone::GetName()
{
  std::string zoneName = GetFilePath();
  zoneName = FileSystem::GetLeaf( zoneName );
  
  // strip off extension
  while ( FileSystem::HasExtension( zoneName ) )
  {
    FileSystem::StripExtension( zoneName );
  }

  return zoneName;
}
