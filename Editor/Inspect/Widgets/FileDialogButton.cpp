#include "Precompile.h"
#include "FileDialogButton.h"

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/filedlg.h>

using namespace Helium;
using namespace Helium::Editor;

FileDialogButtonWindow::FileDialogButtonWindow( wxWindow* parent, FileDialogButtonWidget* buttonWidget )
: ButtonWindow( parent, buttonWidget )
{
}

void FileDialogButtonWindow::OnClicked( wxCommandEvent& )
{
    m_ButtonWidget->GetControl()->Write();
}

FileDialogButtonWidget::FileDialogButtonWidget( Inspect::Button* control )
: ButtonWidget( control )
{
    Inspect::FileDialogButton* button = static_cast< Inspect::FileDialogButton* >( m_ButtonControl );
    button->d_Clicked.Set( Inspect::FileDialogButtonClickedSignature::Delegate( this, &FileDialogButtonWidget::OnClicked ) );
}

Path FileDialogButtonWidget::OnClicked( const Inspect::FileDialogButtonClickedArgs& args )
{
    HELIUM_ASSERT( m_ButtonWindow );

    long windowStyle = 0;
    switch ( args.m_Type )
    {
    case Inspect::FileDialogTypes::OpenFile:
        windowStyle = wxFD_OPEN;
        break;
    case Inspect::FileDialogTypes::SaveFile:
        windowStyle = wxFD_SAVE;
        break;
    default:
        HELIUM_BREAK();
        break;
    }

    Path path;
    wxFileDialog fileDialog( m_ButtonWindow->GetParent(), args.m_Caption.c_str(), args.m_StartPath.Directory().c_str(), args.m_StartPath.Filename().c_str(), args.m_Filter.c_str(), windowStyle );
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        path.Set( static_cast<const tchar*>( fileDialog.GetPath().c_str() ) );
    }

    return path;
}