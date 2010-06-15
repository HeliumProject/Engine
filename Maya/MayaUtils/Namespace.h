#pragma once

#include "API.h"
#include <string>

#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MFnDagNode.h>

namespace Maya
{
  void MAYAUTILS_API RemoveNamespace( const MString & pathName, std::string & result );
  MStatus MAYAUTILS_API FullPathNameWithoutNamespace( MFnDagNode & dagNodeFn, std::string & result );
  MStatus MAYAUTILS_API FullPathNameWithoutNamespace( MDagPath & dagPath, std::string & result );
  MStatus MAYAUTILS_API FullPathNameWithoutNamespace( MFnDagNode & dagNodeFn, MString & result );
  MStatus MAYAUTILS_API FullPathNameWithoutNamespace( MDagPath & dagPath, MString & result );
}
