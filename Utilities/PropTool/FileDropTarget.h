#pragma once

#include "Foundation/File/Path.h"
#include "Foundation/Automation/Event.h"

#include <wx/dnd.h>

struct FileDroppedArgs
{
  Nocturnal::Path m_Path;

  FileDroppedArgs( const std::string& path = "" )
    : m_Path( path )
  {
  }
};
typedef Nocturnal::Signature< void, const FileDroppedArgs& > FileDroppedSignature;
typedef std::set< FileDroppedSignature::Delegate > S_FileDroppedSignature;

class FileDropTarget : public wxFileDropTarget
{
public:
  FileDropTarget(const std::string& extensions, const std::string& delims = ",");

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
  V_string                    m_FileExtensions;
};