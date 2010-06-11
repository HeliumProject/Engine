#include "stdafx.h"
#include "FilteredDropTarget.h"

#include "Foundation/String/Wildcard.h"
#include "Foundation/String/Tokenize.h"
#include "FileSystem/FileSystem.h"
#include "Finder/FinderSpec.h"
#include "Inspect/ClipboardDataObject.h"
#include "Inspect/ClipboardFileList.h"

using namespace Inspect;

FilteredDropTarget::FilteredDropTarget( const Finder::FinderSpec* finderSpec ) 
: Inspect::DropTarget()
, m_FinderSpec( finderSpec )
{
  SetDragOverCallback( Inspect::DragOverCallback::Delegate( this, &FilteredDropTarget::DragOver ) );
  SetDropCallback( Inspect::DropCallback::Delegate( this, &FilteredDropTarget::Drop ) );
}

FilteredDropTarget::~FilteredDropTarget()
{
}

bool FilteredDropTarget::ValidateDrag( const Inspect::DragArgs& args )
{
  Inspect::ClipboardFileListPtr fileList = Reflect::ObjectCast< Inspect::ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
  if ( !fileList )
  {
    return false;
  }

  if ( !m_FinderSpec )
  {
    return true;
  }

  V_string extensions;
  Tokenize( m_FinderSpec->GetFilter(), extensions, ";" );
  if ( extensions.empty() )
  {
    return true;
  }

  for ( S_string::const_iterator fileItr = fileList->GetFilePaths().begin(), fileEnd = fileList->GetFilePaths().end(); fileItr != fileEnd; ++fileItr )
  {
    const std::string& path = *fileItr;
    if ( path.empty() )
    {
      continue;
    }
    
    if ( !FileSystem::Exists( path ) )
    {
      continue;
    }

    for ( V_string::const_iterator itr = extensions.begin(), end = extensions.end(); itr != end; ++itr )
    {
      const std::string& extension = *itr;
      if ( WildcardMatch( extension.c_str(), path.c_str() ) )
      {
        return true;
      }
    }
  }

  return false;
}

wxDragResult FilteredDropTarget::DragOver( const Inspect::DragArgs& args )
{
  if ( ValidateDrag( args ) )
  {
    return args.m_Default;
  }

  return wxDragNone;
}

wxDragResult FilteredDropTarget::Drop( const Inspect::DragArgs& args )
{
  V_string validPaths;
  
  if ( !ValidateDrag( args ) )
  {
    return args.m_Default;
  }

  Inspect::ClipboardFileListPtr fileList = Reflect::ObjectCast< Inspect::ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
  if ( !fileList )
  {
    return args.m_Default;
  }

  V_string extensions;
  if ( m_FinderSpec )
  {
    Tokenize( m_FinderSpec->GetFilter(), extensions, ";" );
  }

  for ( S_string::const_iterator fileItr = fileList->GetFilePaths().begin(), fileEnd = fileList->GetFilePaths().end(); fileItr != fileEnd; ++fileItr )
  {
    const std::string& path = *fileItr;
    if ( path.empty() )
    {
      continue;
    }
    
    if ( !FileSystem::Exists( path ) )
    {
      continue;
    }

    if ( extensions.empty() )
    {
      validPaths.push_back( path );
    }
    else
    {
      for ( V_string::const_iterator itr = extensions.begin(), end = extensions.end(); itr != end; ++itr )
      {
        const std::string& extension = *itr;
        if ( WildcardMatch( extension.c_str(), path.c_str() ) )
        {
          validPaths.push_back( path );
          break;
        }
      }
    }
  }
  
  if ( validPaths.size() )
  {
    m_Dropped.Raise( FilteredDropTargetArgs( validPaths ) );
  }
  
  return args.m_Default;
}

void FilteredDropTarget::AddDroppedListener( const FilteredDropTargetSignature::Delegate& d )
{
  m_Dropped.Add( d );
}

void FilteredDropTarget::RemoveDroppedListener( const FilteredDropTargetSignature::Delegate& d )
{
  m_Dropped.Remove( d );
}
