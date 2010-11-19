#include "Precompile.h"
#include "Frame.h"

#include <cctype>
#include <algorithm>
#include <wx/config.h>

#include "Foundation/Startup.h"
#include "Foundation/String/Utilities.h"

using namespace Helium;
using namespace Helium::Editor;

enum
{
    FrameTimerID = wxID_HIGHEST
};


// Static UI event table
BEGIN_EVENT_TABLE( Frame, wxFrame )
EVT_CLOSE( Frame::OnExiting )
EVT_TIMER( FrameTimerID, Frame::OnHelpTimer )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////////
// *R2 <[<title> - ]>[ <code>(<build>), assets: <assets> ]<[: <extra>>]
// *R2 Editor Asset Editor - [ tech(debug), assets: devel ]: x:/r2/assets/devel/entities/things/stuff/random.entity.hrb
static const tchar_t* s_ProgrammerTitleBarFormat = TXT( "%s(p) %s[ %s%s, assets: %s ]" ); //project, title, code, build, assets, extra

// *R2 <[<title> - ]>[ assets: <assets>, tech/tools: <code> ]<[: <extra>>]
// R2 Editor Asset Editor - [ assets: devel, tech/tools: devel ]: x:/r2/assets/devel/entities/things/stuff/random.entity.hrb
static const tchar_t* s_UserTitleBarFormat = TXT( "%s %s[ assets: %s, tech/tools: %s ]" ); //project, title, assets, code, extra



///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Frame::Frame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxFrame( parent, id, title, pos, size, style, name )
{
    // tell wxAuiManager to manage this frame
    m_FrameManager.SetManagedWindow( this ); 

    SetTitle( title.c_str() );

    m_HelpLastWindow = NULL;
    m_HelpTimer = new wxTimer( this, FrameTimerID );
    m_HelpTimer->Start( 100 );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Frame::~Frame()
{
    m_HelpTimer->Stop();
    delete m_HelpTimer;

    m_FrameManager.UnInit();

    m_ExcludeFromPanelsMenu.clear();
}

void Frame::SetHelpText( const tchar_t* text )
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if there is a command on the undo stack.
// 
bool Frame::CanUndo()
{
    return m_UndoQueue.CanUndo();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if there is a command on the redo stack.
// 
bool Frame::CanRedo()
{
    return m_UndoQueue.CanRedo();
}

///////////////////////////////////////////////////////////////////////////////
// Undo the previous command.
// 
void Frame::Undo()
{
    m_UndoQueue.Undo();
}

///////////////////////////////////////////////////////////////////////////////
// Redo the previously undone command.
// 
void Frame::Redo()
{
    m_UndoQueue.Redo();
}

void Frame::Push( const Undo::CommandPtr& command )
{
    m_UndoQueue.Push( command );
}

///////////////////////////////////////////////////////////////////////////////
// Creates a menu that enumerates all panes attached to the frame manager, as
// long as the pane has a caption.  The menu items will display checkmarks next
// to the name if the panel is currently visible.  Clicking on a menu item will
// toggle its visibility.  Provided as a convenience function so that the base
// classes can just drop this menu on their menu bar.
// 
uint32_t Frame::CreatePanelsMenu( wxMenu* menu )
{
    uint32_t numMenuOptionsAdded = 0;
    wxAuiPaneInfoArray& panes = m_FrameManager.GetAllPanes();
    const size_t numPanes = panes.Count();
    for ( size_t index = 0; index < numPanes; ++index )
    {
        wxAuiPaneInfo& pane = panes.Item( index );
        if ( !pane.caption.empty()
            && m_ExcludeFromPanelsMenu.find( pane.name ) == m_ExcludeFromPanelsMenu.end() )
        {
            wxMenuItem* item = menu->AppendCheckItem( wxID_ANY, pane.caption );
            Connect( item->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Frame::OnShowPanel ), NULL, this );
            m_Panels.insert( M_MenuIdToPanel::value_type( item->GetId(), pane.window ) );
            numMenuOptionsAdded++;
        }
    }

    return numMenuOptionsAdded;
}

///////////////////////////////////////////////////////////////////////////////
// Convenience function for adding check marks next to menu items of panels that
// are currently shown.
// 
void Frame::UpdatePanelsMenu( wxMenu* menu )
{
    const wxAuiPaneInfoArray& panes = m_FrameManager.GetAllPanes();
    const size_t numPanes = panes.Count();
    for ( size_t index = 0; index < numPanes; ++index )
    {
        const wxAuiPaneInfo& pane = panes.Item( index );
        
        if ( m_ExcludeFromPanelsMenu.find( pane.name ) != m_ExcludeFromPanelsMenu.end() )
        {
            continue;
        }
        
        int32_t itemId = menu->FindItem( pane.caption );
        if ( itemId != wxNOT_FOUND )
        {
            menu->Check( itemId, pane.IsShown() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user clicks on an item in the "panels" menu.  Toggles the 
// visibility of the specified panel.
// 
void Frame::OnShowPanel( wxCommandEvent& args )
{
    M_MenuIdToPanel::const_iterator found = m_Panels.find( args.GetId() );
    if ( found != m_Panels.end() )
    {
        wxWindow* window = found->second;
        wxAuiPaneInfo& pane = m_FrameManager.GetPane( window );
        if ( pane.IsOk() )
        {
            pane.Show( !pane.IsShown() );
            m_FrameManager.Update();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called when existing the frame.  Saves the window state.
// 
void Frame::OnExiting( wxCloseEvent& args )
{
    args.Skip();
}

void Frame::OnHelpTimer( wxTimerEvent& evt )
{
    if ( wxWindow::GetCapture() )
    {
        return;
    }

    wxPoint pos = wxGetMousePosition();
    wxWindow *w = wxGenericFindWindowAtPoint( pos );
    if ( w && w != m_HelpLastWindow )
    {
        m_HelpLastWindow = w;
        wxString help = w->GetHelpText();
        while ( help.empty() && w->GetParent() )
        {
            w = w->GetParent();
            help = w->GetHelpText();
        }

        this->SetHelpText( help );
    }
}