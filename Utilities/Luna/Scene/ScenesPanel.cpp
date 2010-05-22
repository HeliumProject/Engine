#include "Precompile.h"
#include "ScenesPanel.h"

#include "Entity.h"
#include "Scene.h"
#include "SceneEditor.h"
#include "SceneManager.h"
#include "SceneRowPanel.h"
#include "SwitchSceneCommand.h"
#include "Zone.h"

#include "Asset/EntityAsset.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"
#include "Content/ContentVersion.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"
#include "Finder/LunaSpecs.h"
#include "Console/Console.h"
#include "UIToolKit/FileDialog.h"
#include "UIToolKit/ImageManager.h"
#include "Common/Container/Insert.h" 

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ScenesPanel::ScenesPanel( SceneEditor* editor, Luna::SceneManager* sceneManager, wxWindow* parent, int id ) 
: wxPanel( parent, id, wxDefaultPosition, wxSize( 300, 200 ), wxTAB_TRAVERSAL, "ScenesPanel" )
, m_Editor( editor )
, m_SceneManager( sceneManager )
, m_ButtonNewZone( NULL )
, m_ButtonMoveSelToZone( NULL )
, m_ScrollWindow( NULL )
, m_ScrollSizer( NULL )
, m_IsSortingEnabled( false )
, m_IsSceneSwitchEnabled( true )
{
    wxBoxSizer* mainSizer;
    mainSizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* toolbarSizer;
    toolbarSizer = new wxBoxSizer( wxHORIZONTAL );

    m_ButtonNewZone = new wxBitmapButton( this, wxID_ANY, UIToolKit::GlobalImageManager().GetBitmap( "newzone_16.png" ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    toolbarSizer->Add( m_ButtonNewZone, 0, wxALL, 2 );
    m_ButtonNewZone->SetToolTip( wxT("Create a new zone and add it to this world.") );

    m_ButtonAddZone = new wxBitmapButton( this, wxID_ANY, UIToolKit::GlobalImageManager().GetBitmap( "addzone_16.png" ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    toolbarSizer->Add( m_ButtonAddZone, 0, wxALL, 2 );
    m_ButtonAddZone->SetToolTip( wxT("Add an existing zone to this world.") );

    m_ButtonMoveSelToZone = new wxBitmapButton( this, wxID_ANY, UIToolKit::GlobalImageManager().GetBitmap( "selectzone_16.png" ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    toolbarSizer->Add( m_ButtonMoveSelToZone, 0, wxALL, 2 );
    m_ButtonMoveSelToZone->SetToolTip( wxT("Move selection into current zone.") );
    m_ButtonMoveSelToZone->Hide();

    mainSizer->Add( toolbarSizer, 0, wxEXPAND, 5 );

    wxBoxSizer* sceneListSizer;
    sceneListSizer = new wxBoxSizer( wxVERTICAL );

    m_ScrollWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxVSCROLL, "ScenesPanel Scrolled Window" );
    m_ScrollWindow->SetScrollRate( 0, 5 );
    m_ScrollSizer = new wxBoxSizer( wxVERTICAL );

    m_ScrollWindow->SetSizer( m_ScrollSizer );
    m_ScrollWindow->Layout();
    m_ScrollSizer->Fit( m_ScrollWindow );
    sceneListSizer->Add( m_ScrollWindow, 1, wxEXPAND | wxALL, 5 );

    mainSizer->Add( sceneListSizer, 1, wxEXPAND, 5 );

    SetSizer( mainSizer );
    Layout();

    // Hookup GUI callbacks
    m_ButtonNewZone->Connect( m_ButtonNewZone->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenesPanel::OnNewZone ), NULL, this );
    m_ButtonAddZone->Connect( m_ButtonAddZone->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenesPanel::OnAddZone ), NULL, this );
    m_ButtonMoveSelToZone->Connect( m_ButtonMoveSelToZone->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenesPanel::OnMoveSelToZone ), NULL, this );

    // Hookup application event callbacks
    m_SceneManager->AddSceneAddedListener( SceneChangeSignature::Delegate ( this, &ScenesPanel::SceneAdded ) );
    m_SceneManager->AddSceneRemovingListener( SceneChangeSignature::Delegate ( this, &ScenesPanel::SceneRemoved ) );
    m_SceneManager->AddCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &ScenesPanel::CurrentSceneChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ScenesPanel::~ScenesPanel()
{
    // Unhook GUI callbacks
    m_ButtonNewZone->Disconnect( m_ButtonNewZone->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenesPanel::OnNewZone ), NULL, this );
    m_ButtonAddZone->Disconnect( m_ButtonAddZone->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenesPanel::OnAddZone ), NULL, this );
    m_ButtonMoveSelToZone->Disconnect( m_ButtonMoveSelToZone->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenesPanel::OnMoveSelToZone ), NULL, this );

    // Unhook application callbacks
    m_SceneManager->RemoveSceneAddedListener( SceneChangeSignature::Delegate ( this, &ScenesPanel::SceneAdded ) );
    m_SceneManager->RemoveSceneRemovingListener( SceneChangeSignature::Delegate ( this, &ScenesPanel::SceneRemoved ) );
    m_SceneManager->RemoveCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &ScenesPanel::CurrentSceneChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the scroll window that shows all the scene rows.
// 
wxScrolledWindow* ScenesPanel::GetScrollWindow()
{
    return m_ScrollWindow;
}

///////////////////////////////////////////////////////////////////////////////
// Sorts all the rows (except the first one, which is the root and should always
// be at the top) alphabetically by zone path.
// 
void ScenesPanel::Sort()
{
    if ( m_IsSortingEnabled )
    {
        m_ScrollWindow->Freeze();

        // Remove all the items from the scroll window (except the first item,
        // that the root scene).
        while ( m_ScrollSizer->GetChildren().size() > 1 )
        {
            m_ScrollSizer->Detach( 1 );
        }

        // Iterate over all the rows and alphabetize them by their paths.
        typedef std::map< std::string, SceneRowPanel* > M_SortedRows;
        M_SortedRows rows;
        M_ZoneRows::const_iterator zoneItr = m_ZoneRows.begin();
        M_ZoneRows::const_iterator zoneEnd = m_ZoneRows.end();
        for ( ; zoneItr != zoneEnd; ++zoneItr )
        {
            // Leave out the root row since it is staying at the top.
            if ( zoneItr->first )
            {
                rows.insert( M_SortedRows::value_type( zoneItr->second->GetRowLabel(), zoneItr->second ) );
            }
        }

        // Go through our sorted list and add all the rows back to the window in the
        // proper order.
        M_SortedRows::const_iterator rowItr = rows.begin();
        M_SortedRows::const_iterator rowEnd = rows.end();
        for ( i32 count = 1; rowItr != rowEnd; ++rowItr )
        {
            if ( count++ % 2 )
            {
                rowItr->second->SetBackgroundColour( wxColour( "#ffffff" ) );
            }
            else
            {
                rowItr->second->SetBackgroundColour( wxColour( "#dddddd" ) );
            }

            m_ScrollSizer->Add( rowItr->second, 0, wxALL|wxEXPAND, 0 );
            m_ScrollSizer->FitInside( m_ScrollWindow );
        }

        m_ScrollWindow->Thaw();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Enables or disables the radio buttons in the zone UI the control switching
// scenes.
// 
void ScenesPanel::EnableSceneSwitch( bool enable )
{
    if ( m_IsSceneSwitchEnabled != enable )
    {
        m_IsSceneSwitchEnabled = enable;

        M_ZoneRows::const_iterator itr = m_ZoneRows.begin();
        M_ZoneRows::const_iterator end = m_ZoneRows.end();
        for ( ; itr != end; ++itr )
        {
            SceneRowPanel* row = itr->second;
            row->EnableSceneSwitch( enable );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the UI is currently enabled for allowing the user to switch
// the current scene.
// 
bool ScenesPanel::IsSceneSwitchEnabled() const
{
    return m_IsSceneSwitchEnabled;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a new row to this panel.
// 
void ScenesPanel::AddRow( Luna::Scene* scene, Zone* zone )
{
    SceneRowPanel* row = new SceneRowPanel( scene, zone, this, m_Editor );
    Nocturnal::Insert<M_ZoneRows>::Result inserted = m_ZoneRows.insert( M_ZoneRows::value_type( zone, row ) );
    NOC_ASSERT( inserted.second );
    if ( inserted.second )
    {
        m_ScrollSizer->Add( row, 0, wxALL|wxEXPAND, 0 );
        m_ScrollSizer->FitInside( m_ScrollWindow );
        if ( zone == NULL )
        {
            row->SetActiveRow( true );
        }
        else
        {
            Sort();
        }
    }
    else
    {
        delete row;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Deletes the row for the specified zone.
// 
void ScenesPanel::DeleteRow( Zone* zone )
{
    M_ZoneRows::iterator found = m_ZoneRows.find( zone );
    if ( found != m_ZoneRows.end() )
    {
        SceneRowPanel* row = found->second;
        m_ScrollSizer->Detach( row );
        m_ScrollSizer->FitInside( m_ScrollWindow );
        m_ScrollSizer->Layout();
        delete row;

        m_ZoneRows.erase( found );

        Sort(); // Re-color the rows
    }
}

///////////////////////////////////////////////////////////////////////////////
// Removes all the rows from this control.
// 
void ScenesPanel::DeleteAllRows()
{
    m_ScrollWindow->DestroyChildren();
    m_ZoneRows.clear();
}

///////////////////////////////////////////////////////////////////////////////
// Prompts the user to save a new zone.  The path to the zone is returned (or 
// an empty path if the operation is cancelled).
// 
std::string ScenesPanel::PromptNewZone( const std::string& defaultPath )
{
    std::string savePath;

    // Prompt
    UIToolKit::FileDialog fileDialog( this, "Create a new zone", defaultPath.c_str(), wxEmptyString, wxFileSelectorDefaultWildcardStr, UIToolKit::FileDialogStyles::Save );
    fileDialog.SetFilter( FinderSpecs::Asset::ZONE_DECORATION.GetDialogFilter() );

    if ( fileDialog.ShowModal() == wxID_OK )
    {
        // If new file name, save
        savePath = fileDialog.GetPath().c_str();

        if ( !FileSystem::Exists( savePath ) )
        {
            Reflect::Archive::ToFile( Reflect::V_Element(), savePath, new Content::ContentVersion() );
        }
    }

    return savePath;
}

///////////////////////////////////////////////////////////////////////////////
// Prompts the user to add an existing zone.  Returns the path to the zone (or 
// an empty string if the operation was cancelled).
// 
std::string ScenesPanel::PromptAddZone( const std::string& defaultPath )
{
    std::string savePath;

    // Prompt
    UIToolKit::FileDialog fileDialog( this, "Add an existing zone", defaultPath.c_str(), wxEmptyString, wxFileSelectorDefaultWildcardStr, UIToolKit::FileDialogStyles::DefaultOpen );
    fileDialog.SetFilter( FinderSpecs::Asset::ZONE_DECORATION.GetDialogFilter() );

    if ( fileDialog.ShowModal() == wxID_OK )
    {
        // If new file name, save
        savePath = fileDialog.GetPath().c_str();
    }

    return savePath;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to actually add a zone, using the specified Prompt Function.
// 
Zone* ScenesPanel::AddZone( std::string ( ScenesPanel::*PromptFunction )( const std::string& ) )
{
    Luna::Scene* rootScene = m_SceneManager->GetRootScene();
    if ( !rootScene )
    {
        // Error
        std::ostringstream msg;
        msg << "You cannot add a zone because there is no scene loaded.";
        wxMessageBox( msg.str().c_str(), "Error", wxOK | wxCENTER | wxICON_ERROR, this );
        return NULL;
    }

    if ( !rootScene->IsEditable() )
    {
        // Error
        std::ostringstream msg;
        msg << "You cannot add a zone because the scene is not editable.";
        wxMessageBox( msg.str().c_str(), "Error", wxOK | wxCENTER | wxICON_ERROR, this );
        return NULL;
    }

    std::string path( rootScene->GetFullPath() );
    FileSystem::StripLeaf( path );

    std::string newZonePath = ( this->*PromptFunction )( path );

    if ( newZonePath.empty() )
    {
        std::ostringstream msg;
        msg << "You must specify a path for the new zone.";
        wxMessageBox( msg.str().c_str(), "Error", wxOK | wxCENTER | wxICON_ERROR, this );
        return NULL;
    }

    Nocturnal::File zoneFile( newZonePath );
    
    if ( !zoneFile.Create() )
    {
        // Error
        std::ostringstream msg;
        msg << "Error creating zone file at location '" << newZonePath << "'";
        wxMessageBox( msg.str().c_str(), "Error", wxOK | wxCENTER | wxICON_ERROR, this );
        return NULL;
    }

    File::Reference zoneRef( zoneFile.GetPath().Get() );

    if ( ContainsZone( zoneRef ) )
    {
        // Error
        std::ostringstream msg;
        msg << "Unable to create zone '" << newZonePath << "'. This zone is already in the world.";
        wxMessageBox( msg.str().c_str(), "Error", wxOK | wxCENTER | wxICON_ERROR, this );
        return NULL;
    }

    // Create the persistent data for the zone with the new tuid
    Content::Zone* contentZone = new Content::Zone();
    contentZone->m_FileReference = new File::Reference( zoneRef );

    // Create the Luna application object that wraps a Content::Zone
    Zone* sceneZone = new Zone( rootScene, contentZone );
    sceneZone->Rename( zoneFile.GetPath().Get() );

#pragma TODO( "Make adding/removing zones undoable" )
    rootScene->AddObject( sceneZone );
    sceneZone->Initialize();

    return sceneZone;
}

///////////////////////////////////////////////////////////////////////////////
// Prompt to create a zone if there are none already in this world
// 
void ScenesPanel::PromptIfNoZones()
{
    // world file is always displayed, so 1 is "empty"
    if ( m_ZoneRows.size() == 1 )
    {
        if ( FileSystem::HasExtension( m_ZoneRows[0]->GetScene()->GetFullPath(), FinderSpecs::Asset::WORLD_DECORATION.GetDecoration() ) )
        {
            if ( wxMessageBox( "There are no zones associated with this level.\nWould you like to create one?", "Create Zone?", wxYES_NO | wxICON_QUESTION, this ) == wxYES )
            {
                // we need to wait until whatever callback we got here from is finished
                wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, m_ButtonNewZone->GetId() );
                wxPostEvent( m_ButtonNewZone, event );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if there is a zone with the specified TUID already in this
// list.
// 
bool ScenesPanel::ContainsZone( const File::Reference& zoneRef ) const
{
    bool found = false;
    M_ZoneRows::const_iterator zoneItr = m_ZoneRows.begin();
    M_ZoneRows::const_iterator zoneEnd = m_ZoneRows.end();
    for ( ; zoneItr != zoneEnd && !found; ++zoneItr )
    {
        Zone* zone = zoneItr->first;
        if ( zone )
        {
            if ( zone->GetFileReference().ReferencesObject() )
            {
                found = zone->GetFileReference()->GetHash() == zoneRef.GetHash(); // breaks on true
            }
        }
    }

    return found;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene is added.  If it's the root scene, a row is added
// to the UI.
// 
void ScenesPanel::SceneAdded( const SceneChangeArgs& args )
{
    if ( m_SceneManager->IsRoot( args.m_Scene ) )
    {
        AddRow( args.m_Scene, NULL );

        args.m_Scene->AddNodeAddedListener( NodeChangeSignature::Delegate ( this, &ScenesPanel::NodeAdded ) );
        args.m_Scene->AddNodeRemovedListener( NodeChangeSignature::Delegate ( this, &ScenesPanel::NodeRemoved ) );
        args.m_Scene->AddLoadStartedListener( LoadSignature::Delegate ( this, &ScenesPanel::SceneLoadStarted ) );
        args.m_Scene->AddLoadFinishedListener( LoadSignature::Delegate ( this, &ScenesPanel::SceneLoadFinished ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene is removed.  If it's the root scene being removed,
// all rows are cleared out.
// 
void ScenesPanel::SceneRemoved( const SceneChangeArgs& args )
{
    if ( m_SceneManager->IsRoot( args.m_Scene ) )
    {
        args.m_Scene->RemoveNodeAddedListener( NodeChangeSignature::Delegate ( this, &ScenesPanel::NodeAdded ) );
        args.m_Scene->RemoveNodeRemovedListener( NodeChangeSignature::Delegate ( this, &ScenesPanel::NodeRemoved ) );
        args.m_Scene->RemoveLoadStartedListener( LoadSignature::Delegate ( this, &ScenesPanel::SceneLoadStarted ) );
        args.m_Scene->RemoveLoadFinishedListener( LoadSignature::Delegate ( this, &ScenesPanel::SceneLoadFinished ) );

        DeleteAllRows();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the current scene is changed on the scene manager.  Updates
// the UI to show the currently selectd scene.
// 
void ScenesPanel::CurrentSceneChanged( const SceneChangeArgs& args )
{
    if ( args.m_Scene )
    {
        const std::string& path = args.m_Scene ? args.m_Scene->GetFullPath() : "";
        M_ZoneRows::const_iterator itr = m_ZoneRows.begin();
        M_ZoneRows::const_iterator end = m_ZoneRows.end();
        for ( ; itr != end; ++itr )
        {
            Zone* zone = itr->first;
            const std::string& pathToCheck = zone ? zone->GetPath() : m_SceneManager->GetRootScene()->GetFullPath();
            SceneRowPanel* panel = itr->second;
            panel->SetActiveRow( path == pathToCheck );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene node is added.  If the node is a zone, a row is
// added to the UI.
// 
void ScenesPanel::NodeAdded( const NodeChangeArgs& args )
{
    if ( args.m_Node->HasType( Reflect::GetType<Zone>() ) )
    {
        AddRow( args.m_Node->GetScene(), Reflect::DangerousCast< Zone >( args.m_Node ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene node is removed.  Deletes any zone rows as 
// necessary.
// 
void ScenesPanel::NodeRemoved( const NodeChangeArgs& args )
{
    if ( args.m_Node->HasType( Reflect::GetType<Zone>() ) )
    {
        DeleteRow( Reflect::DangerousCast< Zone >( args.m_Node ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene load operation is beginning.  Freezes the UI from
// updating during load.
// 
void ScenesPanel::SceneLoadStarted( const LoadArgs& args )
{
    m_ScrollWindow->Freeze();
    m_IsSortingEnabled = false;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene load is finished.  Thaws the UI so that it can
// refresh.
// 
void ScenesPanel::SceneLoadFinished( const LoadArgs& args )
{
    m_IsSortingEnabled = true;
    Sort();
    m_ScrollWindow->Thaw();

    PromptIfNoZones();
}

void ScenesPanel::AddNewZone()
{
    Zone* zone = AddZone( &ScenesPanel::PromptNewZone );
    if ( zone )
    {
        std::string error;
        Luna::Scene* scene = m_SceneManager->OpenZone( zone->GetPath(), error );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user clicks the new zone button.  Creates a new
// zone on the root scene.
// 
void ScenesPanel::OnNewZone( wxCommandEvent& args )
{
    AddNewZone();
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the user clicks the open existing zone button.  Prompts the
// user for the zone to open and adds it to the root scene.
// 
void ScenesPanel::OnAddZone( wxCommandEvent& args )
{
    AddZone( &ScenesPanel::PromptAddZone );
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void ScenesPanel::OnMoveSelToZone( wxCommandEvent& args )
{
    Console::Debug( "Move Selection to Zone\n" );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the delete button is pressed in one of the rows of the
// zone UI.
// 
void ScenesPanel::OnDeleteZoneButton( wxCommandEvent& args )
{
    if ( args.GetEventObject() )
    {
        wxControl* control = wxDynamicCast( args.GetEventObject(), wxControl );
        if ( control && control->GetClientObject() )
        {
            ZoneClientData* data = static_cast< ZoneClientData* >( control->GetClientObject() );
            if ( data )
            {
                Luna::Scene* rootScene = m_SceneManager->GetRootScene();
                if ( rootScene->IsEditable() )
                {
                    Luna::SceneNode* node = rootScene->FindNode( data->m_Zone->GetID() );
                    if ( node )
                    {
                        if ( wxMessageBox( "Are you sure that you want to remove this zone?\n(No data will be deleted, the zone will just no longer be part of this level)", "Remove Zone?", wxCENTER | wxICON_QUESTION | wxYES_NO, m_Editor ) == wxYES )
                        {
                            if ( rootScene->GetSelection().Contains( node ) )
                            {
                                OS_SelectableDumbPtr selection = rootScene->GetSelection().GetItems();
                                selection.Remove( node );
                                rootScene->GetSelection().SetItems( selection );
                            }

#pragma TODO( "Make this work with undo/redo so we don't have to clear the undo queue." )
                            rootScene->RemoveObject( node );
                            rootScene->GetUndoQueue().Reset();
                        }
                    }
                }
            }
        }
    }
}
