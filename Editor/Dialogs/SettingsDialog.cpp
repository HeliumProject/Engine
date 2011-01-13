#include "Precompile.h"
#include "SettingsDialog.h"

#include <wx/wx.h>
#include <wx/listbook.h>

#include "Pipeline/Settings.h"

#include "Foundation/Inspect/Interpreters/Reflect/ReflectInterpreter.h"

using namespace Helium;
using namespace Helium::Editor;

SettingInfo::SettingInfo( Reflect::ObjectPtr& source, Reflect::ObjectPtr& clone, Editor::TreeCanvasPtr& canvas )
: m_Source( source )
, m_Clone( clone )
, m_Canvas( canvas )
{
}

SettingsDialog::SettingsDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: wxDialog( parent, id, title, pos, size, style )
, m_SettingSizer( NULL )
, m_CurrentSetting( NULL )
{
}

SettingsDialog::~SettingsDialog()
{
}

int SettingsDialog::ShowModal( SettingsManager* settingsManager )
{
    m_SettingSizer = new wxBoxSizer( wxVERTICAL );
    m_CurrentSetting = NULL;
    m_SettingInfo.clear();

    M_Settings settings = settingsManager->GetSettingsMap();

    wxListBox* propertiesListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 130 /* 207 */, -1 ) );
    propertiesListBox->Connect( propertiesListBox->GetId(), wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SettingsDialog::OnSettingsChanged ), NULL, this );

    wxSizer* propertiesSizer = new wxBoxSizer( wxHORIZONTAL );
    propertiesSizer->Add( propertiesListBox, 0, wxEXPAND | wxALL, 6 );
    propertiesSizer->Add( 6, 0, 0 );
    propertiesSizer->Add( m_SettingSizer, 1, wxEXPAND | wxALL, 6 );
    propertiesSizer->Add( 6, 0, 0 );

    Inspect::V_Control canvasControls;
    for ( M_Settings::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr )
    {
        Settings* settings = Reflect::SafeCast< Settings >( (*itr).second );

        // skip settings that we don't want the user to see
        if ( settings && !settings->UserVisible() )
        {
            continue;
        }

        Reflect::ObjectPtr clone = (*itr).second->Clone();
        clone->e_Changed.Add( Reflect::ObjectChangeSignature::Delegate( this, &SettingsDialog::OnRefreshElements ) );

        Helium::TreeWndCtrl* treeWndCtrl = new Helium::TreeWndCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxScrolledWindowStyle | wxALWAYS_SHOW_SB | wxCLIP_CHILDREN | wxNO_BORDER, wxPanelNameStr, wxTR_HIDE_ROOT );
        Editor::TreeCanvasPtr canvas = new Editor::TreeCanvas();
        canvas->SetTreeWndCtrl( treeWndCtrl );
        canvasControls.push_back( canvas );

        m_SettingSizer->Add( treeWndCtrl, 1, wxEXPAND, 0 );
        m_SettingSizer->Show( treeWndCtrl, false );

        Inspect::ReflectInterpreterPtr interpreter = new Inspect::ReflectInterpreter( canvas );
        std::vector< Reflect::Object* > elems;
        elems.push_back( clone );
        interpreter->Interpret( elems );
        m_Interpreters.push_back( interpreter );

        tstring uiName;
        (*itr).second->GetClass()->GetProperty( TXT( "UIName" ), uiName );

        if ( uiName.empty() )
        {
            uiName = *(*itr).second->GetClass()->m_Name;
        }

        int index = propertiesListBox->Append( uiName.c_str() );
        Reflect::ObjectPtr source = Reflect::AssertCast< Reflect::Object >( (*itr).second );
        m_SettingInfo.insert( std::make_pair( index, new SettingInfo( source, clone, canvas ) ) );
    }

    wxButton* restoreDefaults = new wxButton( this, wxID_ANY, wxT( "Restore Defaults" ), wxDefaultPosition, wxDefaultSize, 0 );
    restoreDefaults->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDialog::OnRestoreDefaults ), NULL, this );

    wxBoxSizer* propertiesButtonSizer = new wxBoxSizer( wxHORIZONTAL );
    propertiesButtonSizer->Add( restoreDefaults, 0, 0, 0 );

    m_SettingSizer->Add( 0, 6, 0 );
    m_SettingSizer->Add( propertiesButtonSizer, 0, wxALIGN_RIGHT, 0 );
    m_SettingSizer->Add( new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL ), 0, wxEXPAND | wxTOP, 5 );

    wxButton* okButton = new wxButton( this, wxID_ANY, wxT( "OK" ), wxDefaultPosition, wxDefaultSize, 0 );
    okButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDialog::OnOk ), NULL, this );

    wxButton* cancelButton = new wxButton( this, wxID_ANY, wxT( "Cancel" ), wxDefaultPosition, wxDefaultSize, 0 );
    cancelButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDialog::OnCancel ), NULL, this );

    wxBoxSizer* buttonSizer = new wxBoxSizer( wxHORIZONTAL );
    buttonSizer->Add( okButton, 0, wxALL, 5 );
    buttonSizer->Add( cancelButton, 0, wxUP | wxDOWN | wxRIGHT, 5 );
    buttonSizer->Add( 7, 0, 0 );

    wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );
    mainSizer->Add( 0, 1, 0 );
    mainSizer->Add( propertiesSizer, 1, wxEXPAND, 0 );
    mainSizer->Add( buttonSizer, 0, wxALIGN_RIGHT, 0 );
    mainSizer->Add( 0, 8, 0 );

    if ( propertiesListBox->GetCount() && ( m_SettingInfo.find( 0 ) != m_SettingInfo.end() ) )
    {
        propertiesListBox->SetSelection( 0 );
        SelectCanvas( m_SettingInfo[ 0 ] );
    }

    SetSizer( mainSizer );
    Layout();
    Centre();

    int result = wxDialog::ShowModal();

    if ( result == wxID_OK )
    {
        for ( M_SettingInfo::iterator itr = m_SettingInfo.begin(), end = m_SettingInfo.end(); itr != end; ++itr )
        {
            SettingInfo* info = itr->second;
            if ( !info->m_Clone->Equals( info->m_Source ) )
            {
                info->m_Clone->CopyTo( info->m_Source );
                info->m_Source->RaiseChanged();
            }
        }
    }

    for ( M_SettingInfo::iterator itr = m_SettingInfo.begin(), end = m_SettingInfo.end(); itr != end; ++itr )
    {
        itr->second->m_Clone->e_Changed.Remove( Reflect::ObjectChangeSignature::Delegate( this, &SettingsDialog::OnRefreshElements ) );
    }

    return result;
}

void SettingsDialog::OnRestoreDefaults( wxCommandEvent& args )
{

    wxMessageDialog dialog( this, wxT( "Are you sure you want to reset these settings to the default values?" ), wxT( "Confirm Default Settings" ), wxYES_NO );
    if ( dialog.ShowModal() == wxID_NO )
    {
        return;
    }

    if ( !m_CurrentSetting )
    {
        return;
    }

    Reflect::ObjectPtr defaultElement = Reflect::SafeCast<Reflect::Object>( Reflect::Registry::GetInstance()->CreateInstance( m_CurrentSetting->m_Clone->GetClass() ) );
    if ( !defaultElement )
    {
        return;
    }

    if ( !defaultElement->Equals( m_CurrentSetting->m_Clone ) )
    {
        defaultElement->CopyTo( m_CurrentSetting->m_Clone );
        m_CurrentSetting->m_Clone->RaiseChanged();
        m_CurrentSetting->m_Canvas->Read();
    }
}

void SettingsDialog::OnOk( wxCommandEvent& args )
{
    if ( !m_CurrentSetting )
    {
        return;
    }

    if ( m_CurrentSetting->m_Clone->Equals( m_CurrentSetting->m_Source ) )
    {
        return;
    }

    m_CurrentSetting->m_Clone->CopyTo( m_CurrentSetting->m_Source );
    m_CurrentSetting->m_Source->RaiseChanged();

    EndModal( wxID_OK );
}

void SettingsDialog::OnCancel( wxCommandEvent& args )
{
    EndModal( wxID_CANCEL );
}

void SettingsDialog::OnSettingsChanged( wxCommandEvent& args )
{
    SettingInfo* newSettingInfo = NULL;

    M_SettingInfo::iterator itr = m_SettingInfo.find( args.GetInt() );
    if ( itr != m_SettingInfo.end() )
    {
        newSettingInfo = itr->second;
    }

    SelectCanvas( newSettingInfo );
}

void SettingsDialog::OnRefreshElements( const Reflect::ObjectChangeArgs& args )
{
    if ( m_CurrentSetting )
    {
        m_CurrentSetting->m_Canvas->Read();
    }
}

void SettingsDialog::SelectCanvas( SettingInfo* settingInfo )
{
    if ( !m_SettingSizer )
    {
        return;
    }

    if ( m_CurrentSetting )
    {
        m_SettingSizer->Show( m_CurrentSetting->m_Canvas->GetTreeWndCtrl(), false );
    }

    m_CurrentSetting = settingInfo;

    if ( m_CurrentSetting )
    {
        m_SettingSizer->Show( m_CurrentSetting->m_Canvas->GetTreeWndCtrl(), true );
        m_CurrentSetting->m_Canvas->Realize( NULL );
    }

    m_SettingSizer->Layout();
}
