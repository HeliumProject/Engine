#include "Precompile.h"
#include "ChoiceWidget.h"

#include <wx/ctrlsub.h>
#include <wx/settings.h>

using namespace Helium;
using namespace Helium::Editor;

BEGIN_EVENT_TABLE(ChoiceWindow, wxComboBox)
EVT_COMBOBOX(wxID_ANY, ChoiceWindow::OnConfirm)
EVT_TEXT_ENTER(wxID_ANY, ChoiceWindow::OnTextEnter)
EVT_SET_FOCUS(ChoiceWindow::OnSetFocus)
EVT_KILL_FOCUS(ChoiceWindow::OnKillFocus)
END_EVENT_TABLE();

ChoiceWindow::ChoiceWindow(wxWindow* parent, ChoiceWidget* choiceWidget, int flags)
: wxComboBox (parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, flags)
, m_ChoiceWidget( choiceWidget )
, m_Override( false )
{

}


void ChoiceWindow::OnConfirm(wxCommandEvent& event)
{
    if (!m_Override)
    {
        m_ChoiceWidget->Write(); 
    }

    event.Skip();
}

void ChoiceWindow::OnTextEnter(wxCommandEvent& event)
{
    if (!m_Override)
    {
        m_ChoiceWidget->Write(); 
    }

    // process the event
}

void ChoiceWindow::OnSetFocus(wxFocusEvent& event)
{
    if ( !(GetWindowStyle() & wxCB_READONLY) )
    {
        SetSelection(0, GetLastPosition());
    }

    event.Skip();
}

void ChoiceWindow::OnKillFocus(wxFocusEvent& event)
{
    if (!m_Override)
    {
        m_ChoiceWidget->Write();
    }

    event.Skip();
}

ChoiceWidget::ChoiceWidget( Inspect::Choice* choice )
: m_ChoiceControl( choice )
, m_ChoiceWindow( NULL )
{
    SetControl( choice );
}

void ChoiceWidget::Create( wxWindow* parent )
{
    HELIUM_ASSERT( !m_ChoiceWindow );

    u32 style = 0;
    
    if ( m_ChoiceControl->a_IsDropDown.Get() )
    {
        style |= wxCB_DROPDOWN | wxCB_READONLY;
    }
    else
    {
        style |= wxTE_PROCESS_ENTER;
    }
    
    if ( m_ChoiceControl->a_IsSorted.Get() )
    {
        style |= wxCB_SORT;
    }

    // allocate window and connect common listeners
    SetWindow( m_ChoiceWindow = new ChoiceWindow( parent, this, style ) );

    // add listeners
    m_ChoiceControl->a_Highlight.Changed().AddMethod( this, &ChoiceWidget::HighlightChanged );
    m_ChoiceControl->a_Items.Changed().AddMethod( this, &ChoiceWidget::ItemsChanged );

    // update state of attributes that are not refreshed during Read()
    m_ChoiceControl->a_Highlight.RaiseChanged();
}

void ChoiceWidget::Destroy()
{
    HELIUM_ASSERT( m_ChoiceWindow );

    SetWindow( NULL );

    // remove listeners
    m_ChoiceControl->a_Highlight.Changed().RemoveMethod( this, &ChoiceWidget::HighlightChanged );
    m_ChoiceControl->a_Items.Changed().RemoveMethod( this, &ChoiceWidget::ItemsChanged );

    // destroy
    m_ChoiceWindow->Destroy();
    m_ChoiceWindow = NULL;
}

void ChoiceWidget::Read()
{
    HELIUM_ASSERT( m_ChoiceControl->IsBound() );

    tstring str;
    m_ChoiceControl->ReadStringData(str);

    m_ChoiceWindow->SetOverride( true );
    SetValue(str);
    m_ChoiceWindow->SetOverride( false );
}

bool ChoiceWidget::Write()
{
    HELIUM_ASSERT( m_ChoiceControl->IsBound() );

    bool result = false; 
    tstring value = GetValue();

    bool shouldWrite = m_ChoiceControl->a_EnableAdds.Get() ? true : m_ChoiceControl->Contains( value ); 
    if ( shouldWrite )
    {
        m_ChoiceWindow->SetOverride( true );
        result = m_ChoiceControl->WriteStringData( value );
        m_ChoiceWindow->SetOverride( false );
    }
    else
    {
        // refresh the old data
        Read(); 
    }

    return result;
}

void ChoiceWidget::HighlightChanged( const Attribute<bool>::ChangeArgs& args )
{
    if ( args.m_NewValue )
    {
        m_ChoiceWindow->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ChoiceWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
    }
    else
    {
        m_ChoiceWindow->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ChoiceWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    }
}

void ChoiceWidget::ItemsChanged( const Attribute< std::vector< Inspect::ChoiceItem > >::ChangeArgs& args )
{
    m_ChoiceWindow->Clear();

    if ( !args.m_NewValue.empty() ) 
    {
        m_ChoiceWindow->Freeze();

        std::vector< Inspect::ChoiceItem >::const_iterator itr = args.m_NewValue.begin();
        std::vector< Inspect::ChoiceItem >::const_iterator end = args.m_NewValue.end();
        for ( ; itr != end; ++itr )
        {
            tstring key = itr->m_Key;

            if ( m_ChoiceControl->GetPrefix().length() && key.find( m_ChoiceControl->GetPrefix() ) != key.npos)
            {
                key = key.substr( m_ChoiceControl->GetPrefix().length() );
            }

            m_ChoiceWindow->Append( key.c_str(), const_cast<tstring*>(&itr->m_Data) );
        }

        m_ChoiceWindow->Thaw();
    }
}

tstring ChoiceWidget::GetValue()
{
    wxControlWithItems* cwi = m_ChoiceWindow;

    int selection = cwi->GetSelection();
    if ( selection != wxNOT_FOUND )
    {
        return *static_cast<const tstring*>( cwi->GetClientData( selection ) );
    }

    return tstring ();
}

void ChoiceWidget::SetValue(const tstring& data)
{
    std::vector< Inspect::ChoiceItem >::const_iterator itr = m_ChoiceControl->a_Items.Get().begin();
    std::vector< Inspect::ChoiceItem >::const_iterator end = m_ChoiceControl->a_Items.Get().end();
    for ( ; itr != end; ++itr )
    {
        if ( itr->m_Data == data )
        {
            if (!m_ChoiceWindow->SetStringSelection(itr->m_Key.c_str()))
            {
                HELIUM_BREAK();
            }
            break;
        }
    }
}
