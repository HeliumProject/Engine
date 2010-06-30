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
  bool MAYA_API IsInitialized();
  bool MAYA_API Init( const std::string& progname );
  void MAYA_API Cleanup( int errorcode = 0 );

  bool MAYA_API MEL( const std::string& command, std::string& result );
}
