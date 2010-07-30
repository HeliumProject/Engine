#include "FileDropTarget.h"

#include "Foundation/String/Tokenize.h"

using namespace Helium;

FileDropTarget::FileDropTarget(const tstring& extensions, const tstring& delims)
{
  Tokenize( extensions, m_FileExtensions, delims );
}

bool FileDropTarget::OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames )
{
  if ( filenames.size() != 1 )
  {
    return false;
  }

  FileDroppedArgs args( (const wxChar*)filenames[ 0 ].c_str() );

  if ( !m_FileExtensions.empty() )
  {
    if ( std::find( m_FileExtensions.begin(), m_FileExtensions.end(), args.m_Path.Extension() ) == m_FileExtensions.end() )
    {
      return false;
    }
  }

  m_DropEvent.Raise( args );

  return true;
}