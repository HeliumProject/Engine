#pragma once

#include "API.h"
#include <string>

#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MFileIO.h>

namespace Maya
{
  MAYA_API bool OpenFile (const tstring& filename, MFileIO::ReferenceMode referenceMode = MFileIO::kLoadDefault );
  MAYA_API bool IsFileOpen (const tstring& filename);
  MAYA_API bool CloseFile ();
  MAYA_API bool SaveFile ();
}
