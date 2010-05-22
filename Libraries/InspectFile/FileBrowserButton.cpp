#include "stdafx.h"
#include "FileBrowserButton.h"

#include "Inspect/Canvas.h"
#include "FileSystem/FileSystem.h"
#include "FileBrowser/FileBrowser.h"
#include "Finder/Finder.h"
#include "Common/Container/Insert.h" 

// Using
using namespace File;
using namespace Inspect;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
FileBrowserButton::FileBrowserButton()
: m_Title( "Open" )
{
  SetIcon( "magnify_16.png" );

  Nocturnal::Insert<S_string>::Result inserted = m_Filters.insert( S_string::value_type( "All files (*.*)|*.*" ) );

  SetPath( Finder::ProjectAssets() );
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileBrowserButton::Realize( Inspect::Container* parent )
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  if ( !IsRealized() )
  {
    __super::Realize( parent );

    if ( IsBound() )
    {
      ReadPathData( m_Path );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileBrowserButton::Read()
{
  std::string path = GetPath();
}

///////////////////////////////////////////////////////////////////////////////
// Wrapper around Control::ReadData that cleans the file path returned
//
void FileBrowserButton::ReadPathData( std::string& path ) const
{
  ReadData( path );
  if ( !path.empty() )
  {
    FileSystem::CleanName( path );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Shows the file browser dialog.  If the user selects a file, the cleaned
// path is written back into the control.
// 
bool FileBrowserButton::Write()
{
  // Let the base class do it's work first.
  bool result = __super::Write();

  // Show the file dialog and write the data back to the control.
  if ( IsBound() )
  {
    std::string filterStr = "All files (*.*)|*.*";
    if ( !m_Filters.empty() )
    {
      filterStr = "";
      S_string::iterator it = m_Filters.begin();
      S_string::iterator itEnd = m_Filters.end();
      for ( ; it != itEnd ; ++it )
      {
        filterStr += (*it);
        filterStr += "|";
      }
      filterStr.erase( filterStr.size() - 1 );
    }

    wxWindow* parent = GetCanvas() ? GetCanvas()->GetControl() : NULL;

    File::FileBrowser fileBrowser( parent, wxID_ANY, m_Title.c_str() );
    fileBrowser.SetFilter( filterStr.c_str() );

    if ( fileBrowser.ShowModal() == wxID_OK )
    {
      std::string path = fileBrowser.GetPath().c_str();
      result = WriteData( path );
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileBrowserButton::SetTitleBar( const std::string& title )
{
  m_Title = title;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileBrowserButton::SetFilter( const std::string& filter )
{
  m_Filters.clear();
  Nocturnal::Insert<S_string>::Result inserted = m_Filters.insert( S_string::value_type( filter ) );
}


///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileBrowserButton::SetFilter( const V_string& filter )
{
  m_Filters.clear();

  V_string::const_iterator it = filter.begin();
  V_string::const_iterator itEnd = filter.end();
  for ( ; it != itEnd ; ++it )
  {
    Nocturnal::Insert<S_string>::Result inserted = m_Filters.insert( S_string::value_type( *it ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileBrowserButton::AddFilter( const std::string& filter )
{
  Nocturnal::Insert<S_string>::Result inserted = m_Filters.insert( S_string::value_type( filter ) );
}


///////////////////////////////////////////////////////////////////////////////
// 
// 
std::string FileBrowserButton::GetPath()
{
  if ( IsBound() )
  {
    ReadPathData( m_Path );
  }

  return m_Path;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the path that the file browser will initally start at.  Can be a full
// path to a file, or just a file.
// 
void FileBrowserButton::SetPath( const std::string& path )
{
  if ( !path.empty() )
  {
    m_Path = path;
    FileSystem::CleanName( m_Path );
    if ( IsBound() )
    {
      WriteData( m_Path );
    }
  }
  else
  {
    WriteData( "" );
    m_Path.clear();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called before the button is realized.  Stores the attributes specific to 
// this button.
// 
bool FileBrowserButton::Process(const std::string& key, const std::string& value)
{
  bool wasHandled = false;

  if ( key == BUTTON_FILEBROWSER_ATTR_FILTER )
  {
    SetFilter( value );
    wasHandled = true;
  }
  else if ( key == BUTTON_FILEBROWSER_ATTR_PATH )
  {
    SetPath( value );
    wasHandled = true;
  }
  else if ( key == BUTTON_FILEBROWSER_ATTR_TITLE )
  {
    SetTitleBar( value );
    wasHandled = true;
  }
  else
  {
    wasHandled = __super::Process( key, value );
  }

  return wasHandled;
}
