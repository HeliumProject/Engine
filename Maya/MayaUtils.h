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
  bool MAYA_API Init( const tstring& progname );
  void MAYA_API Cleanup( int errorcode = 0 );

  bool MAYA_API MEL( const tstring& command, tstring& result );
}
