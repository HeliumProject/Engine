#pragma once

#include "Application/API.h"
#include "Application/Inspect/Widgets/Button Controls/InspectButton.h"

#include "Foundation/TUID.h"

namespace Inspect
{
  const static tchar BUTTON_FILEBROWSER_ATTR_FILTER[] = TXT( "filter" );
  const static tchar BUTTON_FILEBROWSER_ATTR_PATH[] = TXT( "path" );
  const static tchar BUTTON_FILEBROWSER_ATTR_TITLE[] = TXT( "caption" );

  ///////////////////////////////////////////////////////////////////////////
  // Button control that opens a file browser dialog.
  // 
  class APPLICATION_API FileBrowserButton : public Reflect::ConcreteInheritor<FileBrowserButton, Inspect::Button>
  {
  public:
      FileBrowserButton( const tstring& startPath = TXT( "" ) );

    virtual void  Realize( Inspect::Container* parent ) NOC_OVERRIDE;
    virtual void  Read() NOC_OVERRIDE;
    virtual bool  Write() NOC_OVERRIDE;

    void          SetTitleBar( const tstring& title );
    void          SetPath( const tstring& path );
    void          SetTuidRequired( bool isRequired );
    
    void          SetFilter( const tstring& filter );
    void          SetFilter( const std::vector< tstring >& filter );
    void          AddFilter( const tstring& filter );

    tstring   GetPath();

  protected:
    
    virtual bool  Process( const tstring& key, const tstring& value ) NOC_OVERRIDE;

    void          ReadPathData( tstring& path ) const;

    //
    // Members
    //

    tstring         m_Title;        // Caption on the title bar of the file browser dialog (defaults to "Open")
    tstring         m_Path;         // The path to start the file browser in, can contain file name

    std::set< tstring >            m_Filters;      // The filter for which file types to show in the file browser dialog (defaults to "All Files (*.*)|*.*")
  };

  typedef Nocturnal::SmartPtr<FileBrowserButton> FileBrowserButtonPtr;
}