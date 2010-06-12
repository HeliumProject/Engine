#pragma once

#include "API.h"
#include <maya/MGlobal.h>
#include <maya/MFnDagNode.h>

namespace Maya
{
  ////////////////////////////////////////////////////////////////////////////////
  //
  // duplicate
  //
  /// \param  sourcePath                    the sub tree to be duplicated
  /// \param  destinParent                  the parent where duplication will be placed
  /// \param  resultPath                    return value, a child of parent containing result
  /// \param  instanceGeometry              should meshes/nurbs/etc be instanced (default duplicate)
  /// \param  allowShadersFromReferenceFile can a reference file shader be used (default copy refs or find same insomniac shader)
  /// \param  duplicateUpstreamGraph        duplicates any inputs to nodes (doesn't apply to instances)
  /// \result                               true indicates that resultPath is valid
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API bool duplicate( const MDagPath & sourcePath, const MDagPath & destinParent, MDagPath & resultPath, bool instanceGeometry = false, bool instanceAppropriateTransforms = false, bool allowShadersFromReferenceFile = false, bool duplicateUpstreamGraph = false );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // duplicate
  //
  /// \param  sourceObj                     the dependency node to duplicate
  /// \param  resultObj                     return value, a new dependency node
  /// \param  duplicateUpstreamGraph        duplicates any dependency node inputs to nodes
  /// \param  duplicateDagNodes             whether to follow dag nodes that are upstream
  /// \result                               true indicates that resultPath is valid
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API bool duplicate( const MObject & sourceObj, MObject & resultObj, bool duplicateUpstreamGraph = false, bool duplicateDagNodes = false );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // dagPathFromSingleInstance
  //
  /// \param obj       dag object that represents a non-instance dag node
  /// \return          dag path such that retargetToSingleInstance(obj).node()==obj
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API MDagPath dagPathFromSingleInstance( MObject obj );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // parentOfPath
  //
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API MDagPath parentOfPath( const MDagPath & dagPath );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // retargetToSingleInstance
  //
  /// \param dagNodeFn attach this function set to a dag path rather than just an object
  /// \return          failure is will occur if the dagNode represents more than a single instance
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API bool retargetToSingleInstance( MFnDagNode & dagNodeFn );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // retargetToChildOfParent
  //
  /// /param dagNodeFn the node that is to be retargeted (set to that instance that is a child of parentFn)
  /// /param parentFn  should already be a parent of dagNodeFn, for some instance
  /// /return          true indicates success
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API bool retargetToChildOfParent( MFnDagNode & dagNodeFn, MFnDagNode & parentFn );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // childPath
  //
  /// \param nodeFn       function set for the dag node being queried
  /// \param i            index of the i'th child requested from nodeFn
  /// \param returnStatus error code for any failure, otherwise MS::kSuccess
  /// \return             the path name for the i'th child of nodeFn
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API MDagPath childPath( MFnDagNode & nodeFn, unsigned int i,  MStatus * returnStatus = NULL );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // nameWithoutNamespace
  //
  /// \param nodeFn       function set for the dag node being queried
  /// \return             the name of the node without any prefix: namespace
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API MString nameWithoutNamespace( MFnDependencyNode & nodeFn );
}