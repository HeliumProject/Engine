#pragma once

#include "Foundation/File/Path.h"
#include "Foundation/Automation/Event.h"

#include <wx/dnd.h>

struct FileDroppedArgs
{
  Nocturnal::Path m_Path;

  FileDroppedArgs( const tstring& path = TXT( "" ) )
    : m_Path( path )
  {
  }
};
typedef Nocturnal::Signature< void, const FileDroppedArgs& > FileDroppedSignature;
typedef std::set< FileDroppedSignature::Delegate > S_FileDroppedSignature;

class FileDropTarget : public wxFileDropTarget
{
public:
  FileDropTarget(const tstring& extensions, const tstring& delims = TXT( "," ) );

  void AddListener( FileDroppedSignature::Delegate& listener )
  {
    m_DropEvent.Add( listener );
  }

  void RemoveListener( FileDroppedSignature::Delegate& listener )
  {
    m_DropEvent.Remove( listener );
  }

protected:
  virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames );

private:
  FileDroppedSignature::Event m_DropEvent;
  std::vector< tstring >                    m_FileExtensions;
};