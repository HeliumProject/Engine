#pragma once

#include "API.h"
#include <string>

#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MGlobal.h>
#include <maya/MFileIO.h>
#include <maya/MFnDagNode.h>

namespace Maya
{
  bool MAYAUTILS_API IsInitialized();
  bool MAYAUTILS_API Init( const std::string& progname );
  void MAYAUTILS_API Cleanup( int errorcode = 0 );

  bool MAYAUTILS_API MEL( const std::string& command, std::string& result );
}
