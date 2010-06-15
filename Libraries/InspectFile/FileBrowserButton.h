#pragma once

#include "API.h"
#include "Inspect/Button.h"

#include "Finder/FinderSpec.h"
#include "Foundation/TUID.h"

namespace Inspect
{
  const static char BUTTON_FILEBROWSER_ATTR_FILTER[] = "filter";
  const static char BUTTON_FILEBROWSER_ATTR_PATH[] = "path";
  const static char BUTTON_FILEBROWSER_ATTR_TITLE[] = "caption";

  ///////////////////////////////////////////////////////////////////////////
  // Button control that opens a file browser dialog.
  // 
  class INSPECTFILE_API FileBrowserButton : public Reflect::ConcreteInheritor<FileBrowserButton, Inspect::Button>
  {
  public:
      FileBrowserButton( const std::string& startPath = "" );

    virtual void  Realize( Inspect::Container* parent ) NOC_OVERRIDE;
    virtual void  Read() NOC_OVERRIDE;
    virtual bool  Write() NOC_OVERRIDE;

    void          SetTitleBar( const std::string& title );
    void          SetPath( const std::string& path );
    void          SetTuidRequired( bool isRequired );
    
    void          SetFilter( const std::string& filter );
    void          SetFilter( const V_string& filter );
    void          AddFilter( const std::string& filter );

    std::string   GetPath();

  protected:
    
    virtual bool  Process( const std::string& key, const std::string& value ) NOC_OVERRIDE;

    void          ReadPathData( std::string& path ) const;

    //
    // Members
    //

    std::string         m_Title;        // Caption on the title bar of the file browser dialog (defaults to "Open")
    std::string         m_Path;         // The path to start the file browser in, can contain file name

    S_string            m_Filters;      // The filter for which file types to show in the file browser dialog (defaults to "All Files (*.*)|*.*")
  };

  typedef Nocturnal::SmartPtr<FileBrowserButton> FileBrowserButtonPtr;
}