#pragma once

#include "API.h"
#include <string>

#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MFileIO.h>

namespace Maya
{
  MAYAUTILS_API bool OpenFile (const std::string& filename, MFileIO::ReferenceMode referenceMode = MFileIO::kLoadDefault );
  MAYAUTILS_API bool IsFileOpen (const std::string& filename);
  MAYAUTILS_API bool CloseFile ();
  MAYAUTILS_API bool SaveFile ();
}
