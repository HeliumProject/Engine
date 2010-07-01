#include "Precompile.h"
#include "MayaUtils.h"

#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MDagPath.h>

namespace Maya
{
  void RemoveNamespace( const MString & pathName, tstring & result )
  {
    static const tstring barStr(TXT( "|" ));
    static const tstring colonStr(TXT( ":" ));
    result.clear();
    tstring fullName = pathName.asTChar();
    tstring::iterator begin = fullName.begin();
    tstring::iterator end = fullName.end();
    int priorBarIndex = (int)fullName.find( barStr );
    int nextBarIndex = (int)fullName.find( barStr, priorBarIndex + 1 );
    int nextColonIndex = (int)fullName.find( colonStr );
    while ( priorBarIndex != tstring::npos && nextColonIndex != tstring::npos )
    {
      if ( nextBarIndex == tstring::npos || nextColonIndex < nextBarIndex )
      {
        // remove string after prior bar and before colon
        fullName = fullName.substr( 0, priorBarIndex+1 ) + fullName.substr( nextColonIndex+1, fullName.size() );
        nextBarIndex = (int)fullName.find( barStr, priorBarIndex + 1 );
        nextColonIndex = (int)fullName.find( colonStr, priorBarIndex + 1 );
      }
      else
      {
        // move forward, colon isn't between two current bars
        priorBarIndex = nextBarIndex;
        nextBarIndex = (int)fullName.find( barStr, priorBarIndex + 1 );
      }
    }
    result = fullName;
  }

  MStatus FullPathNameWithoutNamespace( MFnDagNode & dagNodeFn, tstring & result )
  {
    MStatus status;
    MString dagPathMString = dagNodeFn.fullPathName( &status );
    RemoveNamespace( dagPathMString, result );
    return status;
  }

  MStatus FullPathNameWithoutNamespace( MDagPath & dagPath, tstring & result )
  {
    MStatus status;
    MString dagPathMString = dagPath.fullPathName( &status );
    RemoveNamespace( dagPathMString, result );
    return status;
  }

  MStatus FullPathNameWithoutNamespace( MFnDagNode & dagNodeFn, MString & result )
  {
    MStatus status;
    MString dagPathMString = dagNodeFn.fullPathName( &status );
    tstring resultStr;
    RemoveNamespace( dagPathMString, resultStr );
    result = resultStr.c_str();
    return status;
  }

  MStatus FullPathNameWithoutNamespace( MDagPath & dagPath, MString & result )
  {
    MStatus status;
    MString dagPathMString = dagPath.fullPathName( &status );
    tstring resultStr;
    RemoveNamespace( dagPathMString, resultStr );
    result = resultStr.c_str();
    return status;
  }
}
