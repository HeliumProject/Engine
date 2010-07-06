#pragma once

#include "API.h"

#include <set>
#include <Maya/MObject.h>
#include <Maya/MObjectArray.h>
#include <Maya/MTypeId.h>
#include <Maya/MSelectionList.h>
#include <Maya/MGlobal.h>

#include "Foundation/TUID.h"

namespace Maya
{
  struct MObjectCompare
  {
    bool operator()( const MObject& lhs,  const MObject &rhs ) const
    {
      return ( *reinterpret_cast<long*>(&(MObject)(lhs)) < *reinterpret_cast<long*>(&(MObject)(rhs)) );
    }
  };

  typedef std::set<MObject, MObjectCompare> MObjectSet;

  MAYA_API void     MObjectSetToObjectArray( const MObjectSet &nodes, MObjectArray &array );

  MAYA_API MStatus  findNodesOfType( MObjectSet &nodes,    const MTypeId &typeId, const MFn::Type FnType = MFn::kInvalid, MObject& pathRoot = MObject::kNullObj );
  MAYA_API MStatus  findNodesOfType( MObjectArray &array, const MTypeId &typeId, const MFn::Type FnType = MFn::kInvalid, MObject& pathRoot = MObject::kNullObj );

  MAYA_API MStatus  findNodesOfType( MObjectSet &nodes,    const MString &typeStr, bool isDagNode = true );
  MAYA_API MStatus  findNodesOfType( MObjectArray &array, const MString &typeStr, bool isDagNode = true );

  MAYA_API MStatus  findNodesOfType( MObjectSet &nodes,     const MFn::Type FnType, MObject& pathRoot = MObject::kNullObj );
  MAYA_API MStatus  findNodesOfType( MObjectArray& oArray, const MFn::Type FnType, MObject& pathRoot = MObject::kNullObj );

  // the receiver grows in length by the elements in the source
  MAYA_API void     appendObjectArray( MObjectArray & receiver, const MObjectArray & source );
  MAYA_API void     appendObjectArray( MObjectSet &receiver, const MObjectArray & source );

  MAYA_API bool     Exists( const MString& name ); 

  // functions for getting/setting a tuid attribute on an object
  MAYA_API MStatus  SetTUIDAttribute( MObject &object, const MString& idAttributeName, tuid id, const bool hidden = false );
  MAYA_API tuid     GetTUIDAttribute( const MObject &object, const MString& idAttributeName, MStatus* returnStatus = NULL );

  MAYA_API MStatus  SetStringAttribute( MObject &object, const MString& attributeName, const tstring& stringAtr, const bool hidden = false );
  MAYA_API MStatus  GetStringAttribute( const MObject &object, const MString& attributeName, tstring& stringAtr );

  MAYA_API MStatus  RemoveAttribute( MObject& object, const MString& attributeName );

  MAYA_API MStatus  LockHierarchy(MObject& obj, bool state);
  MAYA_API MStatus  LockAttributes( MObject& obj, bool state);

  MAYA_API MStatus  RemoveHierarchy( MObject& object, MSelectionList& list );
}
