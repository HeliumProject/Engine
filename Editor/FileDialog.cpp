#include "Precompile.h"
#include "FileDialog.h"

#include "Platform/Types.h"
#include "Foundation/File/Path.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/String/Tokenize.h"

#include <wx/wx.h>

/////////////////////////////////////////////////////////////////////////////

using namespace Helium;
using namespace Helium::Editor;

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( FileDialog, wxFileDialog )
EVT_BUTTON( wxID_OK, FileDialog::OnOkButtonClicked )
END_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////////////
FileDialog::FileDialog
(
 wxWindow *parent,
 const wxString& message,      /* = wxFileSelectorPromptStr, */
 const wxString& defaultDir,   /* = wxEmptyString, */
 const wxString& defaultFile,  /* = wxEmptyString, */
 const wxString& wildCard,     /* = wxFileSelectorDefaultWildcardStr, */
 FileDialogStyle style,        /* = FileDialogStyles::DefaultOpen, */
 const wxPoint& pos,           /* = wxDefaultPosition, */
 const wxSize& sz,             /* = wxDefaultSize, */
 const wxString& name          /* = wxFileDialogNameStr */
 )
 : wxFileDialog( parent )
 , m_Style( style )
{
    long styleWXFD = LOWORD( style );

    Create( parent, message, defaultDir, defaultFile, wildCard, styleWXFD, pos, sz, name );
}

/////////////////////////////////////////////////////////////////////////////
FileDialog::~FileDialog()
{
}

///////////////////////////////////////////////////////////////////////////////

bool FileDialog::Create
(
 wxWindow *parent,
 const wxString& message,      /* = wxFileSelectorPromptStr, */
 const wxString& defaultDir,   /* = wxEmptyString, */
 const wxString& defaultFile,  /* = wxEmptyString, */
 const wxString& wildCard,     /* = wxFileSelectorDefaultWildcardStr, */
 long style,                   /* = wxFD_DEFAULT_STYLE, */
 const wxPoint& pos,           /* = wxDefaultPosition, */
 const wxSize& sz,             /* = wxDefaultSize, */
 const wxString& name          /* = wxFileDialogNameStr */
 )
{
    bool result = wxFileDialog::Create( parent, message, defaultDir, defaultFile, wildCard, style, pos, sz, name );

    const wxChar* str = wildCard.c_str();
    SetFilter( str );

    return result;
}

/////////////////////////////////////////////////////////////////////////////
// Overridden to provide path cleaning.
// 
int FileDialog::ShowModal()
{
    // Clear out the file list, we're about to build it.
    m_Files.clear();
    int result = __super::ShowModal();

    if ( result == wxID_OK )
    {
        // Get the file paths
        wxArrayString paths;
        if ( IsMultipleSelectionEnabled() )
        {
            GetPaths( paths );
        }
        else
        {
            wxString path = GetPath();
            if ( !path.IsEmpty() )
            {
                paths.Add( path );
            }
        }

        // Cache as tstring
        const size_t num = paths.Count();
        for ( size_t index = 0; index < num; ++index )
        {
            const wxChar* str = paths[index].c_str();
            m_Files.insert( str );
        }
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////////
// Returns true if this dialog was created with the FileDialogStyles::Multiple
// flag.
// 
bool FileDialog::IsMultipleSelectionEnabled() const
{
    return ( m_Style & FileDialogStyles::Multiple ) == FileDialogStyles::Multiple;
}

/////////////////////////////////////////////////////////////////////////////
// Overridden to clean the paths before returning them.  Call GetFilePaths to 
// work with tstring instead.
// 
void FileDialog::GetPaths( wxArrayString& paths ) const
{
    __super::GetPaths( paths );

    size_t count = paths.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        tstring file = paths[ n ].c_str();
        Helium::Path::Normalize( file );
        paths[n] = file.c_str();
    }
}

/////////////////////////////////////////////////////////////////////////////
// Returns the cleaned file path that was selected by the user.  Only call
// this function when the dialog is in single-select mode, otherwise use
// GetFilePaths.
// 
// NOTE: Return value is only valid if you have called ShowModal and the 
// result was wxID_OK.
// 
const tstring& FileDialog::GetFilePath() const
{
    // Only call this function when working with a dialog in single-select mode
    HELIUM_ASSERT( !IsMultipleSelectionEnabled() );

    static const tstring empty;
    if ( !m_Files.empty() )
    {
        return *m_Files.begin();
    }
    return empty;
}

/////////////////////////////////////////////////////////////////////////////
// Returns all the files selected by the user.  Only call this function when
// working with a file dialog in single-select mode, otherwise use GetFilePath.
// All file paths have been cleaned.
// 
// NOTE: Return value is only valid if you have called ShowModal and the 
// result was wxID_OK.
// 
const std::set< tstring >& FileDialog::GetFilePaths() const
{
    // Only call this function when working with a dialog in multi-select mode
    HELIUM_ASSERT( IsMultipleSelectionEnabled() );

    return m_Files;
}


/////////////////////////////////////////////////////////////////////////////
void FileDialog::SetFilter( const tstring& filter )
{
    m_Filters.Clear();
    AddFilter( filter );
}


/////////////////////////////////////////////////////////////////////////////
void FileDialog::SetFilterIndex( const tstring& filter )
{
    int32_t index = 0;
    OS_string::Iterator itr = m_Filters.Begin();
    OS_string::Iterator end = m_Filters.End();
    for ( int32_t count = 0; itr != end; ++itr, ++count )
    {
        const tstring& current = *itr;
        if ( current == filter )
        {
            index = count;
            break;
        }
    }

    __super::SetFilterIndex( index );
}


/////////////////////////////////////////////////////////////////////////////
// For example, this input:
//  "BMP and GIF files (*.bmp;*.gif)|*.bmp;*.gif|PNG files (*.png)|*.png"
//
// Becomes this map
//  "BMP and GIF files (*.bmp;*.gif)" -> "*.bmp;*.gif"
//  "PNG files (*.png)" -> "*.png"
//
void FileDialog::AddFilter( const tstring& filter )
{
    std::vector< tstring > splitFilter;
    Tokenize( filter, splitFilter, TXT( "\\|" ) );

    if ( (int)splitFilter.size() % 2 != 0 )
        return; // error

    int numFilters = (int)splitFilter.size() / 2;
    for ( int i = 0; i < (int)splitFilter.size() ; i+=2 )
    {
        tstring display = splitFilter.at( i );
        tstring mask    = splitFilter.at( i+1 );

        display += TXT( "|" ) + mask;

        bool inserted = m_Filters.Append( display ); 
    }

    UpdateFilter();
}

void FileDialog::AddFilters( const std::vector< tstring >& filters )
{
    for ( std::vector< tstring >::const_iterator itr = filters.begin(), end = filters.end(); itr != end; ++itr )
    {
        AddFilter( *itr );
    }
}

/////////////////////////////////////////////////////////////////////////////
void FileDialog::UpdateFilter()
{
    if ( m_Style & FileDialogStyles::ShowAllFilesFilter )
    {
        bool inserted = m_Filters.Append( TXT( "All files (*.*)|*.*" ) );
    }

    tstring filterStr = TXT( "" );
    if ( !m_Filters.Empty() )
    {
        OS_string::Iterator it = m_Filters.Begin();
        OS_string::Iterator itEnd = m_Filters.End();
        for ( ; it != itEnd ; ++it )
        {
            if ( !filterStr.empty() )
            {
                filterStr += TXT( "|" );
            }
            filterStr += (*it);
        }
    }

    // update the wxFileDialog wild card
    SetWildcard( filterStr.c_str() );
    __super::SetFilterIndex( m_filterIndex );
}


/////////////////////////////////////////////////////////////////////////////
void FileDialog::OnOkButtonClicked( wxCommandEvent& evt )
{
    // FIXME: check for errors in the user data and submit the form
    // - the AssetName may contain a full path
    // - try to find the location
    // - ensure that the file exists (if required)
    // - ensure that the proper file extension is present
    // - if the location exists but there is no file, go to that location in the view and reset the full path
}
