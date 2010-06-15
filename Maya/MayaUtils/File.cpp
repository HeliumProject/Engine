#include "File.h"

#include <maya/MFileIO.h>
#include <maya/MGlobal.h>

#include "Foundation/Log.h"
#include "Foundation/File/Path.h"
#include "RCS/RCS.h"

#include "Export.h"

bool g_MayaFileOpen = false;
std::string g_MayaFile = "";

bool Maya::OpenFile(const std::string& filename, MFileIO::ReferenceMode referenceMode )
{
  MStatus stat;

  if (g_MayaFileOpen)
  {
    if( g_MayaFile == filename )
      return true;
  }

  Nocturnal::Path path( filename );
  if ( !path.Exists() )
  {
    return false;
  }

  stat = MFileIO::newFile(true);

  if ( !stat )
  {
    throw Nocturnal::Exception( "(Maya::newFile) could not reset file." );
    return false;
  }

  // this should already be the case by this point...but just in case
  g_MayaFileOpen = false;

  g_MayaFile = filename;

  stat = MFileIO::open( g_MayaFile.c_str(), NULL, true, referenceMode );

  if ( !stat )
  {
    MFileIO::getErrorStatus( &stat );
    throw Nocturnal::Exception( "(Maya::OpenFile) could not load Maya file '%s' (%s)\n", g_MayaFile.c_str(), stat.errorString() );
    return false;
  }

  g_MayaFileOpen = true;

  std::vector<std::string> selection;
  std::vector<std::string> groupNode;
  InitExportInfo( true, selection, groupNode );

  return true;
}

bool Maya::IsFileOpen(const std::string& filename)
{
  if ( g_MayaFileOpen && ( g_MayaFile == filename ) )
    return true;

  return false;
}

bool Maya::CloseFile()
{
  if (!g_MayaFileOpen)
    return true;

  if (MFileIO::newFile(true) != MS::kSuccess)
  {
    throw Nocturnal::Exception("(Maya::CloseFile) unable to close file");
  }

  g_MayaFile = "";
  g_MayaFileOpen = false;

  return true;
}

bool Maya::SaveFile()
{
  MStatus stat;

  if (!g_MayaFileOpen)
    return false;

  RCS::File rcsFile( g_MayaFile );
  rcsFile.Open();

  stat = MFileIO::save(true);
  if ( !stat )
  {
    throw Nocturnal::Exception("(Maya::SaveFile) could not save file");
  }

  return true;
}