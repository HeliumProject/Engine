#include "Precompile.h"
#include "RunGameFrame.h"

#include "ApplicationPreferences.h"
#include "EditorGenerated.h"
#include "WindowSettings.h"

#include "Common/Environment.h"
#include "Common/String/utilities.h"
#include "Console/Console.h"
#include "FileSystem/FileSystem.h"
#include "UIToolKit/ImageManager.h"
#include "UIToolKit/RegistryConfig.h"
#include "UIToolKit/SortableListView.h"
#include "Windows/Process.h"

#include <algorithm>
#include <sstream>

using namespace Luna;


///////////////////////////////////////////////////////////////////////////////
// Sorts builds according to timestamp, with newer builds before older ones.
// 
struct BuildComparer
{
private:
  const BuildServer& m_BuildServer;

public:
  BuildComparer( const BuildServer& buildserver )
    : m_BuildServer( buildserver )
  {
  }

  bool operator()( const std::string& first, const std::string& second )
  {
    const BuildInfo* build1 = m_BuildServer.FindBuild( first.c_str() );
    const BuildInfo* build2 = m_BuildServer.FindBuild( second.c_str() );

    return build1->m_Timestamp > build2->m_Timestamp;
  }
};

///////////////////////////////////////////////////////////////////////////////
// Copies contents of a V_string into a wxArrayString.
// 
static inline void StdStringArray2WxStringArray( const V_string& std, wxArrayString& wx )
{
  wx.clear();
  wx.reserve( std.size() );
  V_string::const_iterator itr = std.begin();
  V_string::const_iterator end = std.end();
  for ( ; itr != end; ++itr )
  {
    wx.push_back( (*itr).c_str() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
RunGameFrame::RunGameFrame( wxWindow* parent )
: Frame( parent, wxID_ANY, "Run Game" )
, m_Panel( NULL )
, m_SelectedBuild( NULL )
, m_IconOk( UIToolKit::GlobalImageManager().GetImageIndex( "accept_16.png" ) )
, m_IconError( UIToolKit::GlobalImageManager().GetImageIndex( "cancel_16.png" ) )
{
  wxIconBundle iconBundle;
  wxIcon tempIcon;
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "buildserver_64.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "buildserver_32.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "buildserver_16.png" ) );
  iconBundle.AddIcon( tempIcon );
  SetIcons( iconBundle );

  SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Panel = new RunGamePanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
  mainSizer->Add( m_Panel, 1, wxEXPAND | wxALL, 5 );
	
	SetSizerAndFit( mainSizer );
	Layout();

  m_Panel->m_Levels->InsertColumn( 0, "Level" );
  m_Panel->m_Levels->SetImageList( UIToolKit::GlobalImageManager().GetGuiImageList(), wxIMAGE_LIST_SMALL );

  RefreshBuilds();
  if ( m_BuildServer.GetBuilds().size() > 0 )
  {
    m_Panel->m_BuildChoice->Select( 0 );
    SelectBuild();
  }

  // Listeners
  m_Panel->m_BuildChoice->Connect( m_Panel->m_BuildChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( RunGameFrame::OnBuildSelected ), NULL, this );
  m_Panel->m_Levels->Connect( m_Panel->m_Levels->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( RunGameFrame::OnLevelSelected ), NULL, this );
  m_Panel->m_SelectAll->Connect( m_Panel->m_SelectAll->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunGameFrame::OnSelectAll ), NULL, this );
  m_Panel->m_SelectNone->Connect( m_Panel->m_SelectNone->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunGameFrame::OnSelectNone ), NULL, this );
  m_Panel->m_RunGame->Connect( m_Panel->m_RunGame->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunGameFrame::OnRun ), NULL, this );
  m_Panel->m_Rerun->Connect( m_Panel->m_Rerun->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunGameFrame::OnRerun ), NULL, this );

  // Restore layout
  GetApplicationPreferences()->GetRunGameSettings()->ApplyToWindow( this, &m_FrameManager, true );
}
	
///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
RunGameFrame::~RunGameFrame()
{
  m_Panel->m_BuildChoice->Disconnect( m_Panel->m_BuildChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( RunGameFrame::OnBuildSelected ), NULL, this );
  m_Panel->m_Levels->Disconnect( m_Panel->m_Levels->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( RunGameFrame::OnLevelSelected ), NULL, this );
  m_Panel->m_SelectAll->Disconnect( m_Panel->m_SelectAll->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunGameFrame::OnSelectAll ), NULL, this );
  m_Panel->m_SelectNone->Disconnect( m_Panel->m_SelectNone->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunGameFrame::OnSelectNone ), NULL, this );
  m_Panel->m_RunGame->Disconnect( m_Panel->m_RunGame->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunGameFrame::OnRun ), NULL, this );
  m_Panel->m_Rerun->Disconnect( m_Panel->m_Rerun->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunGameFrame::OnRerun ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Save window position/size/etc. to registry.
// 
void RunGameFrame::SaveWindowState()
{
  GetApplicationPreferences()->GetRunGameSettings()->SetFromWindow( this );
}

///////////////////////////////////////////////////////////////////////////////
// Restores window location from the registry.
// 
const std::string& RunGameFrame::GetPreferencePrefix() const
{
  static const std::string runGamePrefPrefix( "RunGame" );
  return runGamePrefPrefix;
}

///////////////////////////////////////////////////////////////////////////////
// Grabs the build from the drop-down and stores that as the current selection.
// Refreshes the level list.
// 
void RunGameFrame::SelectBuild()
{
  m_SelectedBuild = m_BuildServer.FindBuild( m_Panel->m_BuildChoice->GetStringSelection().c_str() );
  RefreshLevels();
}

///////////////////////////////////////////////////////////////////////////////
// Clears the drop-down of builds and repopulates the control with all the
// successfully completed builds.
// 
void RunGameFrame::RefreshBuilds()
{
  std::string buildTemplate( "SINGLE_PLAYER" );
  Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"BUILDSERVER_DEFAULT_TEMPLATE", buildTemplate );
  toUpper( buildTemplate );

  m_BuildServer.RefreshBuilds();

  V_string builds;
  builds.reserve( m_BuildServer.GetBuilds().size() );
  M_Builds::const_iterator buildItr = m_BuildServer.GetBuilds().begin();
  M_Builds::const_iterator buildEnd = m_BuildServer.GetBuilds().end();
  for ( ; buildItr != buildEnd; ++buildItr )
  {
    const BuildInfoPtr& build = buildItr->second;
    std::string currentTemplate( build->m_Template );
    toUpper( currentTemplate );
    if ( build->Succeeded() && buildTemplate.compare( currentTemplate ) == 0 )
    {
      builds.push_back( build->m_Name.c_str() );
    }
  }
  builds.resize( builds.size() );
  std::sort( builds.begin(), builds.end(), BuildComparer( m_BuildServer ) );
  StdStringArray2WxStringArray( builds, m_Builds );

  m_Panel->m_BuildChoice->Freeze();
  m_Panel->m_BuildChoice->Clear();
  m_Panel->m_BuildChoice->Append( m_Builds );
  m_Panel->m_BuildChoice->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Clears the level list and repopulates it based upon the currently selected
// build.
// 
void RunGameFrame::RefreshLevels()
{
  m_Panel->m_Levels->Freeze();
  m_Panel->m_Levels->ClearAll();

  if ( m_SelectedBuild )
  {
    V_LevelInfo::const_iterator itr = m_SelectedBuild->m_Levels.begin();
    V_LevelInfo::const_iterator end = m_SelectedBuild->m_Levels.end();
    for ( u32 buildIndex = 0; itr != end; ++itr, ++buildIndex )
    {
      AddLevelItem( *itr, buildIndex );
    }
  }
  m_Panel->m_Levels->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Adds a level to the list.
// 
void RunGameFrame::AddLevelItem( const LevelInfoPtr& level, u32 index )
{
  i32 row = m_Panel->m_Levels->GetItemCount();

  i32 iconIndex = level->m_HasServerData ? m_IconOk : m_IconError;

  wxListItem nameItem;
  nameItem.SetMask( wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE);
  nameItem.SetText( level->m_Name.c_str() );
  nameItem.SetImage( iconIndex );
  nameItem.SetId( row );
  nameItem.SetData( index );
  nameItem.SetColumn( 0 );
  m_Panel->m_Levels->InsertItem( nameItem );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the command to run seems to be ok.  If false is returned,
// there will be a message saved in error.
// 
bool RunGameFrame::ValidateRun( std::string& error )
{
  if ( !m_SelectedBuild )
  {
    error = "There is no build selected.";
    return false;
  }

  u32 selectionCount = 0;
  bool allLevelsHaveServerData = true;
  for ( i32 listIndex = m_Panel->m_Levels->GetFirstSelected(); listIndex > -1; listIndex = m_Panel->m_Levels->GetNextSelected( listIndex ) )
  {
    i32 buildIndex = m_Panel->m_Levels->GetItemData( listIndex );
    if ( buildIndex >= 0 && buildIndex < static_cast< i32 >( m_SelectedBuild->m_Levels.size() ) )
    {
      const LevelInfo* level = m_SelectedBuild->m_Levels.at( buildIndex );
      if ( !level->m_HasServerData )
      {
        allLevelsHaveServerData = false;
        break;
      }
      else
      {
        ++selectionCount;
      }
    }
  }

  if ( !allLevelsHaveServerData )
  {
    error = "Not all selected levels have valid server data.";
    return false;
  }

  if ( selectionCount == 0 )
  {
    error = "No valid levels are selected.";
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Maintains build selection state.
// 
void RunGameFrame::OnBuildSelected( wxCommandEvent& args )
{
  args.Skip();
  SelectBuild();
}

///////////////////////////////////////////////////////////////////////////////
// Aborts the selection change if the level does not have server data.
// 
void RunGameFrame::OnLevelSelected( wxListEvent& args )
{
  args.Skip();

  if ( m_SelectedBuild )
  {
    i32 buildIndex = args.GetData();
    if ( buildIndex >= 0 && buildIndex < static_cast< i32 >( m_SelectedBuild->m_Levels.size() ) )
    {
      const LevelInfo* level = m_SelectedBuild->m_Levels.at( buildIndex );
      if ( !level->m_HasServerData )
      {
        m_Panel->m_Levels->Select( args.GetIndex(), false );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Selects all levels which have server data.
// 
void RunGameFrame::OnSelectAll( wxCommandEvent& args )
{
  if ( m_SelectedBuild )
  {
    const i32 numItems = m_Panel->m_Levels->GetItemCount();
    for ( i32 listIndex = 0; listIndex < numItems; ++listIndex )
    {
      i32 buildIndex = m_Panel->m_Levels->GetItemData( listIndex );
      if ( buildIndex >= 0 && buildIndex < static_cast< i32 >( m_SelectedBuild->m_Levels.size() ) )
      {
        const LevelInfo* level = m_SelectedBuild->m_Levels.at( buildIndex );
        if ( level->m_HasServerData )
        {
          m_Panel->m_Levels->Select( listIndex, true );
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Deselects all levels.
// 
void RunGameFrame::OnSelectNone( wxCommandEvent& args )
{
  for ( i32 index = m_Panel->m_Levels->GetFirstSelected(); index > -1; index = m_Panel->m_Levels->GetNextSelected( index ) )
  {
    m_Panel->m_Levels->Select( index, false );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Re-runs the last run game command.
// 
void RunGameFrame::OnRerun( wxCommandEvent& args )
{
  std::string path;
  if ( !Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"PROJECT_BAT", path ) )
  {
    wxMessageBox( "Failed to find environment variable 'IG_PROJECT_BAT'", "Error", wxCENTER | wxICON_ERROR | wxOK, this );
    return;
  }

  FileSystem::AppendPath( path, "rerun.bat" );
  FileSystem::Win32Name( path );

  std::string command( "cmd.exe /k \"" + path + "\"" );

  try
  {
    Windows::Execute( command, true, false );
  }
  catch ( ... )
  {
    Console::Error( "Failed to run command: %s\n", command.c_str() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Runs the game with the specified settings.
// 
void RunGameFrame::OnRun( wxCommandEvent& args )
{
  std::string scriptPath;
  if ( !Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"PROJECT_SCRIPTS", scriptPath ) )
  {
    wxMessageBox( "Failed to find environment variable 'IG_PROJECT_SCRIPTS'", "Error", wxCENTER | wxICON_ERROR | wxOK, this );
    return;
  }

  std::string error;
  if ( !ValidateRun( error ) )
  {
    wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, this );
    return;
  }

  FileSystem::AppendPath( scriptPath, "run_game.pl" );
  FileSystem::Win32Name( scriptPath );

  // Launch perl in a new command window, leave new command window open (/k)
  std::string command( "cmd.exe /k perl.exe \"" + scriptPath + "\"" );
  command += " -build " + m_SelectedBuild->m_Name;
  command += " -server_data";

  for ( i32 listIndex = m_Panel->m_Levels->GetFirstSelected(); listIndex > -1; listIndex = m_Panel->m_Levels->GetNextSelected( listIndex ) )
  {
    i32 buildIndex = m_Panel->m_Levels->GetItemData( listIndex );
    if ( buildIndex >= 0 && buildIndex < static_cast< i32 >( m_SelectedBuild->m_Levels.size() ) )
    {
      const LevelInfo* level = m_SelectedBuild->m_Levels.at( buildIndex );
      if ( level->m_HasServerData )
      {
        command += " -level \"" + level->m_Name + "\"";
      }
    }
  }

  if ( !m_Panel->m_Options->GetValue().empty() )
  {
    command += std::string( " " ) + m_Panel->m_Options->GetValue().c_str();
  }

  try
  {
    Windows::Execute( command, true, false );
  }
  catch ( ... )
  {
    Console::Error( "Failed to run command: %s\n", command.c_str() );
  }
}
