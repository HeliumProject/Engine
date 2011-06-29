#include "EditorPch.h"
#include "EditorButton.h"

#include "Platform/Assert.h"
#include "Foundation/Flags.h"

#include "Editor/App.h"
#include "Editor/Settings/EditorSettings.h"

#include <wx/bitmap.h>

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

    if ( !( GetOptions() & PanelButtonOptions::AlwaysShowText ) && ( settings->m_ShowTextOnButtons != m_ShowText ) )
    {
        m_ShowText = settings->m_ShowTextOnButtons;
        needsUpdate = true;
    }

    if ( settings->m_ShowIconsOnButtons != m_ShowIcons )
    {
        m_ShowIcons = settings->m_ShowIconsOnButtons;
        needsUpdate = true;
    }

    if ( settings->m_IconSizeOnButtons != m_IconSize )
    {
        m_IconSize = settings->m_IconSizeOnButtons;
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

            DynamicBitmap* dynamicBitmap = dynamic_cast< DynamicBitmap* >( (*itr) );
            if ( dynamicBitmap )
            {
                switch( m_IconSize )
                {
                case IconSize::Small:
                    dynamicBitmap->SetIconSize( 16 );
                    break;
                case IconSize::Medium:
                    dynamicBitmap->SetIconSize( 24 );
                    break;
                case IconSize::Large:
                    dynamicBitmap->SetIconSize( 32 );
                    break;
                default:
                    HELIUM_BREAK();
                    break;
                }
            }

            continue;
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

            continue;
        }
    }

    Refresh();
    GetSizer()->RecalcSizes();
    Layout();
    GetParent()->Refresh();
    GetParent()->GetSizer()->RecalcSizes();
    GetParent()->Layout();
    event.Skip();
}