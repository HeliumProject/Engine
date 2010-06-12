#pragma once

#include "API.h"

#include <set>
#include <Maya/MObject.h>
#include <Maya/MObjectArray.h>
#include <Maya/MTypeId.h>
#include <Maya/MSelectionList.h>
#include <Maya/MGlobal.h>

#include "UID/TUID.h"

namespace Maya
{
  struct MObjectCompare
  {
    bool operator()( const MObject& lhs,  const MObject &rhs ) const
    {
      return ( *reinterpret_cast<long*>(&(MObject)(lhs)) < *reinterpret_cast<long*>(&(MObject)(rhs)) );
    }
  };

  typedef std::set<MObject, MObjectCompare> S_MObject;

  MAYAUTILS_API void     S_MObjectToObjectArray( const S_MObject &nodes, MObjectArray &array );

  MAYAUTILS_API MStatus  findNodesOfType( S_MObject &nodes,    const MTypeId &typeId, const MFn::Type FnType = MFn::kInvalid, MObject& pathRoot = MObject::kNullObj );
  MAYAUTILS_API MStatus  findNodesOfType( MObjectArray &array, const MTypeId &typeId, const MFn::Type FnType = MFn::kInvalid, MObject& pathRoot = MObject::kNullObj );

  MAYAUTILS_API MStatus  findNodesOfType( S_MObject &nodes,    const MString &typeStr, bool isDagNode = true );
  MAYAUTILS_API MStatus  findNodesOfType( MObjectArray &array, const MString &typeStr, bool isDagNode = true );

  MAYAUTILS_API MStatus  findNodesOfType( S_MObject &nodes,     const MFn::Type FnType, MObject& pathRoot = MObject::kNullObj );
  MAYAUTILS_API MStatus  findNodesOfType( MObjectArray& oArray, const MFn::Type FnType, MObject& pathRoot = MObject::kNullObj );

  // the receiver grows in length by the elements in the source
  MAYAUTILS_API void     appendObjectArray( MObjectArray & receiver, const MObjectArray & source );
  MAYAUTILS_API void     appendObjectArray( S_MObject &receiver, const MObjectArray & source );

  MAYAUTILS_API bool Exists( const MString& name ); 

  // functions for getting/setting a tuid attribute on an object
  MAYAUTILS_API MStatus SetTUIDAttribute( MObject &object, const MString& idAttributeName, tuid id, const bool hidden = false );
  MAYAUTILS_API tuid GetTUIDAttribute( const MObject &object, const MString& idAttributeName, MStatus* returnStatus = NULL );

  MAYAUTILS_API MStatus SetStringAttribute( MObject &object, const MString& attributeName, const std::string& stringAtr, const bool hidden = false );
  MAYAUTILS_API MStatus GetStringAttribute( const MObject &object, const MString& attributeName, std::string& stringAtr );

  MAYAUTILS_API MStatus RemoveAttribute( MObject& object, const MString& attributeName );

  MAYAUTILS_API MStatus LockHierarchy(MObject& obj, bool state);
  MAYAUTILS_API MStatus LockAttributes( MObject& obj, bool state);

  MAYAUTILS_API MStatus RemoveHierarchy( MObject& object, MSelectionList& list );
}
