#include "Precompile.h"
#include "DetailsFrame.h"

#include "Asset/AssetFile.h"
#include "FileSystem/FileSystem.h"
#include "UIToolKit/ImageManager.h"

using namespace Luna;

static const i32 s_DisplayTimeSize = 32;

std::string GetPrintTime( u64 time )
{
  std::string result;

  // try to get a printer friendly version of the datetime
  __time64_t timeT  = ( __time64_t ) ( time /*/ 1000*/ );

  char timePrint[s_DisplayTimeSize];
  if ( _ctime64_s( timePrint, s_DisplayTimeSize, &timeT ) == 0 )
  {
    // timeT
    result = timePrint;
    result = result.substr( 0, result.length() - 1 );
  }
  else
  {
    sprintf( timePrint, "%ld", time );
    result = timePrint;
  }    
  return result;
}

std::string GetOperationString( RCS::Operation operation )
{
  switch ( operation )
  {
  case RCS::Operations::None:
    return "None";

  case RCS::Operations::Add:
    return "Add";

  case RCS::Operations::Edit:
    return "Edit";

  case RCS::Operations::Delete:
    return "Delete";

  case RCS::Operations::Branch:
    return "Branch";

  case RCS::Operations::Integrate:
    return "Integrate";

  case RCS::Operations::Unknown:
  default:
    return "Unknown";
  }
}

std::string GetRevisionString( const RCS::Revision* revision )
{
  std::string str;
  str += revision->m_Username + "@" + revision->m_Client + " (" + GetOperationString( revision->m_Operation ) + ")\n";
  str += GetPrintTime( revision->m_Time ) + "\n";
  str += revision->m_Description;
  return str;
}

DetailsFrame::DetailsFrame( wxWindow* parent )
: DetailsFrameGenerated( parent )
{
}

void DetailsFrame::Populate( Asset::AssetFile* file )
{
  std::string name = file->GetShortName() + file->GetExtension();
  SetTitle( name + " - " + GetTitle() );

  m_Name->SetValue( name );

  std::string fileType( file->GetEngineTypeName() );
  if ( fileType == "Unknown" || fileType == "Null" )
  {
    fileType = file->GetFileType();
  }
  m_FileType->SetValue( fileType );

  std::stringstream id;
  id << TUID::HexFormat << file->GetFileID();
  m_FileID->SetValue( id.str() );

  std::string folder( file->GetFilePath() );
  FileSystem::StripLeaf( folder );
  m_Folder->SetValue( folder );

  bool gotRevisionInfo = false;
  RCS::File rcsFile( file->GetFilePath() );
  try
  {
    rcsFile.GetInfo( (RCS::GetInfoFlag)( RCS::GetInfoFlags::GetHistory | RCS::GetInfoFlags::GetIntegrationHistory ) );
    gotRevisionInfo = true;
  }
  catch ( const Nocturnal::Exception& e )
  {
    Console::Error( "%s\n", e.what() );
  }

  if ( gotRevisionInfo && rcsFile.ExistsInDepot() )
  {
    if ( rcsFile.IsCheckedOutByMe() )
    {
      m_RevisionStatusIcon->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "accept_16.png" ) );
      m_RevisionStatus->SetLabel( "Checked out to you" );
    }
    else if ( rcsFile.IsCheckedOutBySomeoneElse() )
    {
      m_RevisionStatusIcon->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "cancel_16.png" ) );
      
      std::string usernames;
      rcsFile.GetOpenedByUsers( usernames );
      
      std::string message( "Checked out to: " );
      message += usernames;
      
      m_RevisionStatus->SetLabel( message );
    }
    else if ( !rcsFile.IsUpToDate() )
    {
      m_RevisionStatusIcon->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "warning_16.png" ) );
      m_RevisionStatus->SetLabel( "Out of date" );
    }
    else
    {
      m_RevisionStatusIcon->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "p4_16.png" ) );
      m_RevisionStatus->SetLabel( "Available for check out" );
    }

    if ( !rcsFile.m_Revisions.empty() )
    {
      const RCS::Revision* firstRevision = *rcsFile.m_Revisions.rbegin();
      const RCS::Revision* lastRevision = *rcsFile.m_Revisions.begin();

      if ( firstRevision == lastRevision )
      {
        m_LastCheckInPanel->Hide();
      }
      else
      {
        std::string lastCheckIn = GetRevisionString( lastRevision );
        m_LastCheckIn->SetValue( lastCheckIn );
        m_LastCheckInPanel->Show();
      }

      std::string firstCheckIn = GetRevisionString( firstRevision );
      m_FirstCheckIn->SetValue( firstCheckIn );
    }
  }
  else
  {
    m_RevisionStatus->SetLabel( "No Perforce information available" );
    m_RevisionStatusIcon->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "cancel_16.png" ) );
    m_LastCheckInPanel->Hide();
    m_FirstCheckInPanel->Hide();
  }

  Layout();
  GetSizer()->Layout();
}
