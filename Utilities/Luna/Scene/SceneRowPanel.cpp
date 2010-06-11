#include "Precompile.h"
#include "SceneRowPanel.h"

#include "RenameDialog.h"
#include "Scene.h"
#include "SceneDocument.h"
#include "SceneEditor.h"
#include "SceneManager.h"
#include "ScenesPanel.h"
#include "SwitchSceneCommand.h"
#include "Zone.h"
#include "Editor/ContextMenu.h"
#include "Foundation/Boost/Regex.h" 
#include "Foundation/CommandLine.h"
#include "Foundation/Log.h"
#include "FileSystem/FileSystem.h"
#include "RCS/RCS.h"
#include "UIToolKit/ImageManager.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SceneRowPanel::SceneRowPanel( Luna::Scene* scene, Zone* zone, ScenesPanel* panel, SceneEditor* editor )
: wxPanel( panel->GetScrollWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, "SceneRowPanel" )
, m_Editor( editor )
, m_RootScene( scene )
, m_Zone( zone )
, m_ScenesPanel( panel )
, m_RadioButton( NULL )
, m_ButtonCheckOutOrSave( NULL )
, m_ButtonMode( ModeCheckout )
, m_Text( NULL )
, m_ToggleLoad( NULL )
, m_ButtonDelete( NULL )
{
    if ( IsRoot() )
    {
        SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DDKSHADOW ) );
    }

    wxBoxSizer* rowSizer;
    rowSizer = new wxBoxSizer( wxHORIZONTAL );

    m_RadioButton = new wxRadioButton( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    m_RadioButton->Enable( IsRoot() );
    rowSizer->Add( m_RadioButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );

    m_ButtonCheckOutOrSave = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 18,18 ), wxBU_AUTODRAW );
    rowSizer->Add( m_ButtonCheckOutOrSave, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );

    std::string text = GetRowLabel();
    m_Text = new wxStaticText( this, wxID_ANY, wxT( text.c_str() ), wxDefaultPosition, wxSize( -1,-1 ), wxALIGN_LEFT | wxST_NO_AUTORESIZE );
    m_Text->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
    if ( IsRoot() )
    {
        m_Text->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
    }
    m_Text->SetMinSize( wxSize( 20,-1 ) );

    rowSizer->Add( m_Text, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3 );

    if ( IsRoot() )
    {
        rowSizer->Add( 40, 0, 0, wxALL, 2 );
    }
    else
    {
        m_ToggleLoad = new wxCheckBox( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
        std::string toggleToolTip = "Load or unload: " + text;
        m_ToggleLoad->SetToolTip( toggleToolTip );
        rowSizer->Add( m_ToggleLoad, 0, wxALL, 5 );

        m_ButtonDelete = new wxBitmapButton( this, wxID_ANY, wxArtProvider::GetBitmap( wxART_DELETE, wxART_OTHER, wxSize( 16, 16 ) ), wxDefaultPosition, wxSize( 18,18 ), wxBU_AUTODRAW );
        m_ButtonDelete->SetToolTip( wxT("Delete this zone") );
        m_ButtonDelete->SetClientObject( new ZoneClientData( m_Zone ) );
        rowSizer->Add( m_ButtonDelete, 0, wxALL, 1 );

        // The parent of this control must handle the delete button press.  This is because this
        // control can't delete itself safely.  The callback will have client data attached to the
        // event object indicating which zone to delete.
        m_ScenesPanel->Connect( m_ButtonDelete->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenesPanel::OnDeleteZoneButton ), NULL, m_ScenesPanel );
    }

    bool enableCheckOutButton = false;
    if ( m_RootScene )
    {
        enableCheckOutButton = m_RootScene->GetSceneDocument()->IsModified();
    }
    UpdateCheckOutButton( enableCheckOutButton );
    UpdateDeleteButton();

    SetSizer( rowSizer );
    Layout();
    rowSizer->Fit( this );

    // Context Menu
    ContextMenuItemPtr renameItem = new ContextMenuItem( "Rename" );
    renameItem->AddCallback( ContextMenuSignature::Delegate( this, &SceneRowPanel::Rename ) );
    m_ContextMenuItems.AppendItem( renameItem );

    SubMenuPtr revisionControlSubMenu = new SubMenu( "Perforce", "", wxArtProvider::GetBitmap( wxART_INFORMATION, wxART_OTHER, wxSize( 16, 16 ) ) );

    ContextMenuItemPtr checkOutItem = new ContextMenuItem( "Check Out" );
    checkOutItem->AddCallback( ContextMenuSignature::Delegate( this, &SceneRowPanel::CheckOutContext ) );
    revisionControlSubMenu->AppendItem( checkOutItem );

    ContextMenuItemPtr historyItem = new ContextMenuItem( "Revision History" );
    historyItem->AddCallback( ContextMenuSignature::Delegate( this, &SceneRowPanel::RevisionHistory ) );
    revisionControlSubMenu->AppendItem( historyItem );

    m_ContextMenuItems.AppendItem( revisionControlSubMenu );

    // Connect dynamic GUI event handlers
    Connect( GetId(), wxEVT_LEFT_DOWN, wxMouseEventHandler( SceneRowPanel::OnButtonDown ), NULL, this );
    Connect( GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler( SceneRowPanel::OnDoubleClickRow ), NULL, this );
    Connect( GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( SceneRowPanel::OnRightClick ), NULL, this );
    m_Text->Connect( m_Text->GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler( SceneRowPanel::OnDoubleClickRow ), NULL, this );
    m_Text->Connect( m_Text->GetId(), wxEVT_SIZE, wxSizeEventHandler( SceneRowPanel::OnResizeText ), NULL, this );
    m_Text->Connect( m_Text->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( SceneRowPanel::OnRightClick ), NULL, this );
    m_RadioButton->Connect( m_RadioButton->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SceneRowPanel::OnRadioButton ), NULL, this );
    m_ButtonCheckOutOrSave->Connect( m_ButtonCheckOutOrSave->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneRowPanel::OnCheckOutSaveButton ), NULL, this );

    if ( m_ToggleLoad )
    {
        m_ToggleLoad->Connect( m_ToggleLoad->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SceneRowPanel::OnToggleLoad ), NULL, this );
    }

    // Connect application event callbacks
    if ( IsRoot() )
    {
        SceneDocument* file = m_RootScene->GetSceneDocument();
        file->AddDocumentPathChangedListener( DocumentPathChangedSignature::Delegate ( this, &SceneRowPanel::DocumentPathChanged ) );
        file->AddDocumentCheckedOutListener( DocumentChangedSignature::Delegate ( this, &SceneRowPanel::DocumentModified ) );
        file->AddDocumentModifiedListener( DocumentChangedSignature::Delegate ( this, &SceneRowPanel::DocumentModified ) );
    }
    else
    {
        m_Editor->GetSceneManager()->AddSceneAddedListener( SceneChangeSignature::Delegate( this, &SceneRowPanel::SceneAdded ) );
        m_Zone->AddNameChangedListener( SceneNodeChangeSignature::Delegate( this, &SceneRowPanel::NodeRenamed ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SceneRowPanel::~SceneRowPanel()
{
    // Unhook GUI event handlers
    Disconnect( GetId(), wxEVT_LEFT_DOWN, wxMouseEventHandler( SceneRowPanel::OnButtonDown ), NULL, this );
    Disconnect( GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler( SceneRowPanel::OnDoubleClickRow ), NULL, this );
    Disconnect( GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( SceneRowPanel::OnRightClick ), NULL, this );
    m_Text->Disconnect( m_Text->GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler( SceneRowPanel::OnDoubleClickRow ), NULL, this );
    m_Text->Disconnect( m_Text->GetId(), wxEVT_SIZE, wxSizeEventHandler( SceneRowPanel::OnResizeText ), NULL, this );
    m_Text->Disconnect( m_Text->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( SceneRowPanel::OnRightClick ), NULL, this );
    m_RadioButton->Disconnect( m_RadioButton->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SceneRowPanel::OnRadioButton ), NULL, this );
    m_ButtonCheckOutOrSave->Disconnect( m_ButtonCheckOutOrSave->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SceneRowPanel::OnCheckOutSaveButton ), NULL, this );

    if ( m_ToggleLoad )
    {
        m_ToggleLoad->Disconnect( m_ToggleLoad->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SceneRowPanel::OnToggleLoad ), NULL, this );
    }

    // Disconnect application event callbacks

    if ( !IsRoot() )
    {
        m_Editor->GetSceneManager()->RemoveSceneAddedListener( SceneChangeSignature::Delegate ( this, &SceneRowPanel::SceneAdded ) );
        m_Zone->RemoveNameChangedListener( SceneNodeChangeSignature::Delegate( this, &SceneRowPanel::NodeRenamed ) );
    }

    Luna::Scene* scene = m_Editor->GetSceneManager()->GetScene( GetFilePath() );
    if ( scene )
    {
        SceneDocument* file = scene->GetSceneDocument();
        file->RemoveDocumentCheckedOutListener( DocumentChangedSignature::Delegate ( this, &SceneRowPanel::DocumentModified ) );
        file->RemoveDocumentPathChangedListener( DocumentPathChangedSignature::Delegate ( this, &SceneRowPanel::DocumentPathChanged ) );
        file->RemoveDocumentModifiedListener( DocumentChangedSignature::Delegate ( this, &SceneRowPanel::DocumentModified ) );
        file->RemoveDocumentSavedListener( DocumentChangedSignature::Delegate ( this, &SceneRowPanel::DocumentModified ) );
    }

    // Remove listeners from every scene for load events
    M_SceneSmartPtr::const_iterator sceneItr = m_Editor->GetSceneManager()->GetScenes().begin();
    M_SceneSmartPtr::const_iterator sceneEnd = m_Editor->GetSceneManager()->GetScenes().end();
    for ( ; sceneItr != sceneEnd; ++sceneItr )
    {
        const ScenePtr& scene = sceneItr->second;
        scene->RemoveLoadFinishedListener( LoadSignature::Delegate ( this, &SceneRowPanel::SceneLoadFinished ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the root scene for this row.
// 
Luna::Scene* SceneRowPanel::GetScene()
{
    return m_RootScene;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the zone for this row (NULL if this row is the root row).
// 
Zone* SceneRowPanel::GetZone()
{
    return m_Zone;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this is the root row (meaning it has no zone).
// 
bool SceneRowPanel::IsRoot() const
{
    return m_Zone == NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Call this function to set whether or not this row represents the current,
// active scene in the UI.  If set to true, this row will adjust it's UI
// (toggle the radio button on, bold the text) to indicate that it is the 
// active row.
// 
void SceneRowPanel::SetActiveRow( bool isCurrent )
{
    m_RadioButton->SetValue( isCurrent );

    wxFont font = m_Text->GetFont();
    if ( isCurrent )
    {
        font.SetWeight( wxFONTWEIGHT_BOLD );
        m_Text->SetFont( font );
    }
    else
    {
        font.SetWeight( wxFONTWEIGHT_NORMAL );
        m_Text->SetFont( font );
    }

    ResizeText();
}

///////////////////////////////////////////////////////////////////////////////
// Set the text displayed on this row.  The label will be truncated and prefixed
// with "..." to make it fit within the available space.
// 
void SceneRowPanel::SetLabel( const std::string& label )
{
    // Label
    std::string trimmed = label;
    TrimString( trimmed, m_Text->GetSize().x );
    m_Text->SetLabel( trimmed.c_str() );

    // Tooltip
    const std::string tooltip( GetFilePath().c_str() );
    m_Text->SetToolTip( tooltip.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
// Enable or disable the radio button that allows the user to change the 
// current scene.  Note: If the scene that this row represents is not loaded,
// the radio button will not be enabled.
// 
void SceneRowPanel::EnableSceneSwitch( bool enable )
{
    Luna::Scene* scene = m_RootScene;
    if ( !IsRoot() )
    {
        scene = m_RootScene->GetManager()->GetScene( m_Zone->GetPath() );
    }

    enable &= ( scene != NULL );
    m_RadioButton->Enable( enable );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the label text for this row.  The path may be to
// a root scene, or a zone.
// 
std::string SceneRowPanel::GetRowLabel() const
{
    std::string label = GetFilePath();
    label = FileSystem::GetLeaf( label );
    FileSystem::StripExtension( label );

    return label;
}

///////////////////////////////////////////////////////////////////////////////
// Trims the specified string to the width (in pixels) requested.  The string
// will be prefixed with "...".
// 
bool SceneRowPanel::TrimString(std::string& str, int width)
{
    wxClientDC dc (m_Text);

    int x, y;
    wxString wxStr (str.c_str());
    dc.GetTextExtent(wxStr, &x, &y, NULL, NULL, &m_Text->GetFont());

    if ( x <= width )
    {
        return false;
    }

    size_t count = str.size();
    for ( size_t i = count; i>0; i-- )
    {
        wxStr = (str.substr(0, i-1) + "...").c_str();

        dc.GetTextExtent( wxStr, &x, &y, NULL, NULL, &m_Text->GetFont() );

        if ( x < width )
        {
            str = wxStr.c_str();
            return true;
        }
    }

    str = "...";
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Call this function when a change has occurred that may have changed the
// size available for the text displayed in this row.
// 
void SceneRowPanel::ResizeText()
{
    SetLabel( GetRowLabel() );
}

///////////////////////////////////////////////////////////////////////////////
// Call this function to set the scene represented by this row as the current,
// editing scene in the scene manager.
// 
void SceneRowPanel::MakeCurrentScene()
{
    Luna::SceneManager* sceneManager = m_RootScene->GetManager();
    Luna::Scene* scene = m_RootScene;

    if ( !IsRoot() )
    {
        scene = sceneManager->GetScene( m_Zone->GetPath() );
    }

    if ( scene && sceneManager->GetRootScene() )
    {
        sceneManager->GetRootScene()->Push( new LSwitchSceneCommand( sceneManager, scene ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Call this function when the checkout/save button needs to be updated.
// 
void SceneRowPanel::UpdateCheckOutButton( bool enabled )
{
    Luna::SceneManager* sceneManager = m_Editor->GetSceneManager();
    DocumentPtr document = new Document( GetFilePath() );

    if ( RCS::PathIsManaged( document->GetFilePath() ) )
    {
        if ( !sceneManager->IsCheckedOut( document ) )
        {
            SetButtonMode( ModeCheckout, true );
        }
        else
        {
            SetButtonMode( ModeSave, enabled );
        }
    }
    else
    {
        SetButtonMode( ModeSave, enabled );
    }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void SceneRowPanel::UpdateDeleteButton()
{
    if ( m_ButtonDelete )
    {
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the path to the scene that this row represents.  The path may be to
// a root scene, or a zone.
// 
std::string SceneRowPanel::GetFilePath() const
{
    if ( IsRoot() )
    {
        return m_RootScene->GetFullPath();
    }
    else
    {
        return m_Zone->GetPath();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the current mode of the checkout/save button.
// 
SceneRowPanel::ButtonMode SceneRowPanel::GetButtonMode()
{
    return m_ButtonMode;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the mode of the checkout/save button, updating the UI accordingly.
// 
void SceneRowPanel::SetButtonMode( SceneRowPanel::ButtonMode mode, bool enabled )
{
    m_ButtonMode = mode;

    switch ( m_ButtonMode )
    {
    case ModeCheckout:
        m_ButtonCheckOutOrSave->SetBitmapLabel( wxArtProvider::GetBitmap( wxART_MISSING_IMAGE, wxART_OTHER, wxSize( 16, 16 ) ) );
        m_ButtonCheckOutOrSave->SetBitmapDisabled( wxArtProvider::GetBitmap( wxART_MISSING_IMAGE, wxART_OTHER, wxSize( 16, 16 ) ) );
        m_ButtonCheckOutOrSave->SetToolTip( "Check out file" );
        break;

    case ModeSave:
        m_ButtonCheckOutOrSave->SetBitmapLabel( wxArtProvider::GetBitmap( wxART_MISSING_IMAGE, wxART_OTHER, wxSize( 16, 16 ) ) );
        m_ButtonCheckOutOrSave->SetBitmapDisabled( wxArtProvider::GetBitmap( wxART_MISSING_IMAGE, wxART_OTHER, wxSize( 16, 16 ) ) );
        m_ButtonCheckOutOrSave->SetToolTip( "Save file" );
        break;
    }

    m_ButtonCheckOutOrSave->Enable( enabled );
}

///////////////////////////////////////////////////////////////////////////////
// Attempts to checkout the file that this row represents.
// 
void SceneRowPanel::CheckOut()
{
    Luna::SceneManager* sceneManager = m_Editor->GetSceneManager();
    Luna::Scene* scene = sceneManager->GetScene( GetFilePath() );
    DocumentPtr file = NULL;
    if ( scene )
    {
        // If the scene is already open, get the editor file that goes with it.
        file = scene->GetSceneDocument();
        NOC_ASSERT( file.ReferencesObject() );
    }
    else
    {
        // The file is not opened yet, so just create a temporary editor file
        // that we will use to checkout the file.
        file = new Document( GetFilePath() );
    }


    if ( file.ReferencesObject() && RCS::PathIsManaged( file->GetFilePath() ) )
    {
        sceneManager->QueryCheckOut( file );
    }

    UpdateCheckOutButton( file->IsModified() );
}

///////////////////////////////////////////////////////////////////////////////
// Attempts to save the scene that this row represents.
// 
void SceneRowPanel::Save()
{
    Luna::SceneManager* sceneManager = m_RootScene->GetManager();
    Luna::Scene* scene = sceneManager->GetScene( GetFilePath() );
    if ( scene )
    {
        std::string error;
        if ( !sceneManager->Save( scene->GetSceneDocument(), error ) )
        {
            wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxID_OK, m_Editor );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the scene path has changed.  Updates the text label on
// this row and the checkout/save button.
// 
void SceneRowPanel::DocumentPathChanged( const DocumentPathChangedArgs& args )
{
    SetLabel( GetRowLabel() );
    UpdateCheckOutButton( args.m_Document->IsModified() );
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a scene is added to the scene manager.  Hooks in the callback
// for when that scene is loaded.
// 
void SceneRowPanel::SceneAdded( const SceneChangeArgs& args )
{
    if ( !IsRoot() )
    {
        args.m_Scene->AddLoadFinishedListener( LoadSignature::Delegate ( this, &SceneRowPanel::SceneLoadFinished ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene (zone) is loaded.  If the scene is the one represented by
// this row, the UI elements are updated to indicate that the zone is loaded.
// 
void SceneRowPanel::SceneLoadFinished( const LoadArgs& args )
{
    if ( !IsRoot() )
    {
        if ( args.m_Success  )
        {
            if ( args.m_Scene->GetFullPath() == m_Zone->GetPath() )
            {
                m_ToggleLoad->SetValue( true );
                m_RadioButton->Enable( m_ScenesPanel->IsSceneSwitchEnabled() );
                UpdateCheckOutButton( args.m_Scene->GetSceneDocument()->IsModified() );

                SceneDocument* file = args.m_Scene->GetSceneDocument();
                file->AddDocumentPathChangedListener( DocumentPathChangedSignature::Delegate ( this, &SceneRowPanel::DocumentPathChanged ) );
                file->AddDocumentCheckedOutListener( DocumentChangedSignature::Delegate ( this, &SceneRowPanel::DocumentModified ) );
                file->AddDocumentModifiedListener( DocumentChangedSignature::Delegate ( this, &SceneRowPanel::DocumentModified ) );
                file->AddDocumentSavedListener( DocumentChangedSignature::Delegate ( this, &SceneRowPanel::DocumentModified ) );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the editor file that this row represents is checked out.
// Updates the checkout/save button.
// 
void SceneRowPanel::DocumentModified( const DocumentChangedArgs& args )
{
    UpdateCheckOutButton( args.m_Document->IsModified() );
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to make sure the specified name is valid.
// 
static inline bool ValidateName( const std::string& name )
{
    const boost::regex pattern( "^[a-zA-Z0-9\\-_]+[a-zA-Z0-9\\-_ ]*$" );
    boost::smatch results; 
    return boost::regex_match( name, results, pattern );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for the context menu item to rename the zone that belongs to this 
// row.
// 
void SceneRowPanel::Rename( const ContextMenuArgsPtr& args )
{
    // Error checking
    std::string path( GetFilePath() );
    if ( !FileSystem::Exists( path ) )
    {
        std::string error = "You must save '";
        error += path.c_str();
        error += "' to disk before you can rename it!";
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
        return;
    }

    bool gotInfo = false;
    RCS::File rcsFile( path );
    rcsFile.GetInfo();

    if ( rcsFile.IsCheckedOutByMe() )
    {
        std::string error = "You must check in '" + path + "' before you can rename it.";
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
        return;
    }
    else if ( rcsFile.IsCheckedOut() )
    {
        std::string error = "Cannot rename '" + path + "' because it is checked out by " + rcsFile.m_Username + ".";
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
        return;
    }

    Luna::Scene* scene = m_Editor->GetSceneManager()->GetScene( path );
    if ( scene )
    {
        if ( scene->GetSceneDocument()->IsModified() )
        {
            std::string error = "You must save and check in '" + path + "' before you can rename it.";
            wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
            return;
        }
    }

    // Prompt for rename
    RenameDialog prompt( this );
    prompt.m_Text->SetValue( GetRowLabel().c_str() );
    if ( prompt.ShowModal() == wxID_OK )
    {
        std::string newName( prompt.m_Text->GetValue().c_str() );
        if ( !ValidateName( newName ) )
        {
            std::string error = "Invalid name '" + newName + "' specified.";
            wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
            return;
        }

        std::string ext;
        FileSystem::GetExtension( path, ext );
        std::string newPath( path );
        FileSystem::StripLeaf( newPath );
        FileSystem::AppendPath( newPath, newName );
        FileSystem::AppendExtension( newPath, ext );

        std::string error;

        // Try to acutally do the rename
        bool succeeded = false;

        RCS::Changeset changeset;
        changeset.m_Description = "Renaming Zone From: " + path + " To: " + newPath;
        changeset.Create();
        if ( changeset.m_Id < 0 || changeset.m_Id == RCS::DefaultChangesetId )
        {
            wxMessageBox( "Could not create new Perforce changlist.", "Error", wxCENTER | wxICON_ERROR | wxOK );
            return;
        }

        RCS::File sourceFile( path );
        RCS::File targetFile( newPath );

        changeset.Rename( sourceFile, targetFile );

        try
        {
            changeset.Commit();
        }
        catch( const Nocturnal::Exception& ex )
        {    
            std::string error = "Some files could not be submitted; ";
            error += ex.what();
            wxMessageBox( error, "Error", wxCENTER | wxICON_ERROR | wxOK, GetParent() );
        }

        if ( scene )
        {
            scene->GetSceneDocument()->SetFilePath( newPath );
        }
        else if ( !IsRoot() )
        {
            m_Zone->SetName( newName );
        }

    }
}

///////////////////////////////////////////////////////////////////////////////
// This callback is a little misleading.  Basically, we are recalculating the
// label on this row whenever a zone is renamed.  However, the zone name is 
// not what is displayed in this row, it's actually the file name on disc 
// (which could be different).  However, this event is triggered during a zone
// rename (see above) anyway, so we can just piggy-back on this event.
// 
void SceneRowPanel::NodeRenamed( const SceneNodeChangeArgs& args )
{
    SetLabel( GetRowLabel() );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for the context menu item to check this file out of revision control.
// 
void SceneRowPanel::CheckOutContext( const ContextMenuArgsPtr& args )
{
    CheckOut();
}

///////////////////////////////////////////////////////////////////////////////
// Callback to display the revision history for the file represented by this row.
// 
void SceneRowPanel::RevisionHistory( const ContextMenuArgsPtr& args )
{
    std::string path = GetFilePath();
    if ( !path.empty() )
    {
        m_Editor->RevisionHistory( path );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the size of the text control is changed.  Updates the 
// control, trimming the text if necessary.
// 
void SceneRowPanel::OnResizeText( wxSizeEvent& args )
{
    ResizeText();
    args.Skip();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the radio button for this row is clicked on.  Attempts to change
// the scene manager's current scene.
// 
void SceneRowPanel::OnRadioButton( wxCommandEvent& args )
{
    MakeCurrentScene();
}

///////////////////////////////////////////////////////////////////////////////
// Override button clicks on the panel so that they don't get passed on to 
// children (you need to actually click a child if you want a child to handle
// a mouse click).
// 
void SceneRowPanel::OnButtonDown( wxMouseEvent& args )
{
    // Do nothing, thereby preventing the event from propagating.  If we don't 
    // stop this event, the radio button will automatically handle it.
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user double-clicks on this row.  Shortcut for clicking the
// radio button on the row.
// 
void SceneRowPanel::OnDoubleClickRow( wxMouseEvent& args )
{
    if ( m_RadioButton->IsEnabled() )
    {
        MakeCurrentScene();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called when a right-click is performed.  Displays a context menu.
// 
void SceneRowPanel::OnRightClick( wxMouseEvent& args )
{
    ContextMenuArgsPtr menuArgs = new ContextMenuArgs();
    ContextMenuPtr contextMenu = new ContextMenu();

    ContextMenuGenerator::Build( m_ContextMenuItems, contextMenu.Ptr() );
    contextMenu->Popup( this, menuArgs );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user clicks the load button on this row.  Loads or unloads
// the zone accordingly.
// 
void SceneRowPanel::OnToggleLoad( wxCommandEvent& args )
{
    Luna::SceneManager* sceneManager = m_RootScene->GetManager();
    if ( m_ToggleLoad->GetValue() )
    {
        // Load
        std::string error;
        Luna::Scene* zone = sceneManager->OpenZone( m_Zone->GetPath(), error );
        if ( zone != NULL )
        {
            m_RadioButton->Enable( m_ScenesPanel->IsSceneSwitchEnabled() );
            sceneManager->GetRootScene()->Push( new LSwitchSceneCommand( sceneManager, zone ) );
        }
        else
        {
            m_ToggleLoad->SetValue( false );
            wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, m_Editor );
        }
    }
    else
    {
        // Unload
        Luna::Scene* scene = sceneManager->GetScene( m_Zone->GetPath() );

        if ( scene )
        {
            SceneDocument* document = scene->GetSceneDocument();

            if ( sceneManager->CloseDocument( document ) )
            {
                m_RadioButton->Disable();
            }
            else
            {
                m_ToggleLoad->SetValue( true );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the checkout/save button is clicked on.  Carries out the 
// appropriate action based upon the current button mode.
// 
void SceneRowPanel::OnCheckOutSaveButton( wxCommandEvent& args )
{
    switch ( GetButtonMode() )
    {
    case ModeCheckout:
        CheckOut();
        break;

    case ModeSave:
        Save();
        break;
    }
}
