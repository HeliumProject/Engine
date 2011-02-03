#include "Precompile.h"
#include "EditorButton.h"

#include "Platform/Assert.h"
#include "Foundation/Flags.h"

#include "Editor/App.h"
#include "Editor/Settings/EditorSettings.h"

using namespace Helium;
using namespace Helium::Editor;

IMPLEMENT_DYNAMIC_CLASS( EditorButton, PanelButton );

EditorButton::EditorButton()
: PanelButton()
{
}

EditorButton::EditorButton( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: PanelButton( parent, id, pos, size, style, name )
, m_ShowText( true )
, m_ShowIcons( true )
{
    SetExtraStyle( GetExtraStyle() | wxWS_EX_PROCESS_UI_UPDATES );

    Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorButton::OnUpdateUI ), NULL, this );
}

EditorButton::~EditorButton()
{
    Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorButton::OnUpdateUI ), NULL, this );
}

void EditorButton::OnUpdateUI( wxUpdateUIEvent& event )
{
    EditorSettings* settings = wxGetApp().GetSettingsManager()->GetSettings< EditorSettings >();
    if ( !settings )
    {
        return;
    }

    bool needsUpdate = false;

    if ( settings->m_ShowTextOnButtons != m_ShowText )
    {
        m_ShowText = settings->m_ShowTextOnButtons;
        needsUpdate = true;
    }

    if ( settings->m_ShowIconsOnButtons != m_ShowIcons )
    {
        m_ShowIcons = settings->m_ShowIconsOnButtons;
        needsUpdate = true;
    }

    if ( !needsUpdate )
    {
        return;
    }

    wxWindowList children = GetChildren();

    for ( wxWindowList::iterator itr = children.begin(), end = children.end(); itr != end; ++itr )
    {
        wxStaticBitmap* bitmap = dynamic_cast< wxStaticBitmap* >( (*itr) );
        if ( bitmap )
        {
            if ( m_ShowIcons )
            {
                bitmap->Show();
            }
            else
            {
                bitmap->Hide();
            }
        }

        wxStaticText* text = dynamic_cast< wxStaticText* >( (*itr) );
        if ( text )
        {
            if ( m_ShowText )
            {
                text->Show();
            }
            else
            {
                text->Hide();
            }
        }
    }

    Layout();
    Refresh( true );
    GetParent()->Layout();
    GetParent()->Refresh();
    event.Skip();
}