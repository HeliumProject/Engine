#include "Precompile.h"
#include "FileDialogDisplayer.h"

#include "Editor/FileDialog.h"
#include "Editor/Dialogs/YesNoAllDialog.h"

using namespace Helium;
using namespace Helium::Editor;

void FileDialogDisplayer::DisplayFileDialog( const Helium::FileDialogArgs& args )
{
    int32_t style = 0;

    switch ( args.m_Type )
    {
    case FileDialogTypes::OpenFile:
        style = FileDialogStyles::DefaultOpen;
        break;

    case FileDialogTypes::SaveFile:
        style = FileDialogStyles::DefaultSave;
        break;

    default:
        HELIUM_ASSERT( false );
    }

    FileDialog saveDlg( NULL, args.m_Caption.c_str(), args.m_DefaultDirectory.c_str(), args.m_DefaultFile.c_str(), TXT( "" ),  style );

    saveDlg.AddFilter( args.m_Filters );

    Helium::Path path;

    if ( saveDlg.ShowModal() == wxID_OK )
    {
        path.Set( static_cast<const tchar*>( saveDlg.GetPath().c_str() ) );
    }

    args.m_Result = path;
}