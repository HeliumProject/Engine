#include "Precompile.h"
#include "CinematicEventsDocument.h"

#include "CinematicEventsEditor.h"
#include "CinematicEventsManager.h"
#include "CinematicEventsInit.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/AnimationAttribute.h"
#include "Asset/AnimationSetAsset.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AnimationSpecs.h"

#include "Editor/Editor.h"

#include "Console/Console.h"

#include "RCS/RCS.h"

using namespace Luna;
using namespace Asset;
using namespace Attribute;

// 
// RTTI
// 
LUNA_DEFINE_TYPE( CinematicEventsDocument );

void CinematicEventsDocument::InitializeType()
{
  Luna::LunaCinematicEvents::g_RegisteredTypes.Push( Reflect::RegisterClass<CinematicEventsDocument>( "CinematicEventsDocument" ) );
}

void CinematicEventsDocument::CleanupType()
{
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
CinematicEventsDocument::CinematicEventsDocument( CinematicEventsManager* manager, const AssetClassPtr& cinematicAsset )
: Document( cinematicAsset->GetFilePath() )
, m_Manager( manager )
, m_CinematicAsset( cinematicAsset )
{
  Initialize();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
CinematicEventsDocument::~CinematicEventsDocument()
{
}

void CinematicEventsDocument::Initialize()
{
  DocumentPtr cinematicFile = new Document( m_CinematicAsset->GetFilePath() );
  m_Files[ m_CinematicAsset->GetFilePath() ] = cinematicFile;

  std::string eventsPath = GetCinematicEventsFile();

  if ( m_Files.find( eventsPath ) == m_Files.end() )
  {
    m_Files[ eventsPath ] = new Document( eventsPath );

    CinematicEventListPtr eventList;
    if ( FileSystem::Exists( eventsPath ) )
    {
      try
      {
        eventList = Reflect::Archive::FromFile< CinematicEventList >( eventsPath );
      }
      catch( Reflect::StreamException )
      {
        Console::Warning( "The cinematic event file is empty. Did you expect it to have any events? (%s)\n", eventsPath.c_str() );
        eventList = new CinematicEventList;
      }
    }

    if ( !eventList.ReferencesObject() )
    {
      eventList = new CinematicEventList;
    }

    m_EventLists[ eventsPath ] = eventList;
  }
}

std::string CinematicEventsDocument::GetCinematicEventsFile( )
{
  return FinderSpecs::Animation::EVENTS_DECORATION.GetMetaDataFile( this->m_CinematicAsset->GetFilePath() );
}

CinematicEventListPtr CinematicEventsDocument::GetCinematicEventList()
{
  std::string eventsFile = GetCinematicEventsFile();

  return m_EventLists[ eventsFile ];
}

bool CinematicEventsDocument::Save()
{
  // save the cinematic file if necessary
  std::string fileName = m_CinematicAsset->GetFilePath();

  M_StringToDocument::iterator it = m_Files.find( fileName );
  if ( it == m_Files.end() )
  {
    NOC_BREAK();
    Console::Error( "%s is not in the file list!\n", fileName.c_str() );
    return false;
  }

  DocumentPtr cinematicEditorFile = it->second;
  if ( cinematicEditorFile->IsModified() )
  {
    try
    {
      m_CinematicAsset->Serialize();
    }
    catch( Nocturnal::Exception& e )
    {
      std::string errorString = std::string( "Failed to save file: " ) + e.what();
      wxMessageBox( errorString.c_str(), "Error", wxOK|wxCENTRE|wxICON_ERROR, m_Manager->GetEditor() );
      return false;
    }
  }

  std::string eventsFile = GetCinematicEventsFile();

  try
  {
    FileSystem::MakePath( eventsFile, true );
  }
  catch ( Nocturnal::Exception& ex )
  {
    std::ostringstream str;
    str << "Failed to make path for '" << eventsFile << "': " << ex.what();
    wxMessageBox( str.str(), "Error", wxOK|wxCENTRE|wxICON_ERROR );
    return false;
  }

  CinematicEventListPtr eventList = m_EventLists[ eventsFile ];

  RCS::File rcsFile( eventsFile );
  try
  {
    rcsFile.Open();
  }
  catch ( Nocturnal::Exception& ex )
  {
    std::stringstream str;
    str << "Unable to check out'" << eventsFile << "': " << ex.what();
    wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
    return false;
  }

  try
  {
    Reflect::Archive::ToFile( eventList, eventsFile );
  }
  catch ( Nocturnal::Exception& ex )
  {
    std::ostringstream str;
    str << "Failed to write file " << eventsFile << ": " << ex.what();
    wxMessageBox( str.str(), "Error", wxOK|wxCENTRE|wxICON_ERROR );
    return false;
  }

  SetModified( false );
  return true;
}

bool CinematicEventsDocument::EditCinematic()
{
  bool success = false;

  std::string fileName = m_CinematicAsset->GetFilePath();

  M_StringToDocument::iterator it = m_Files.find( fileName );
  if ( it == m_Files.end() )
  {
    NOC_BREAK();
    Console::Error( "%s is not in the file list!\n", fileName.c_str() );

    return false;
  }

  DocumentPtr editorFile = it->second;

  if ( m_Manager->AttemptChanges( editorFile ) )
  {
    SetModified( true );
    editorFile->SetModified( true );

    success = true;
  }

  return success;
}

bool CinematicEventsDocument::Edit()
{
  bool success = false;

  std::string fileName;
  fileName = GetCinematicEventsFile();

  M_StringToDocument::iterator it = m_Files.find( fileName );
  if ( it == m_Files.end() )
  {
    NOC_BREAK();
    Console::Error( "%s is not in the file list!\n", fileName.c_str() );

    return false;
  }

  DocumentPtr editorFile = it->second;
 
  if ( m_Manager->AttemptChanges( editorFile ) )
  {
    SetModified( true );
    editorFile->SetModified( true );

    success = true;
  }

  return success;
}
