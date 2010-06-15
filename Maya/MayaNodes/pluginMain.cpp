#include "stdafx.h"

#include "CameraController.h"
#include "CameraControllerUICmd.h"

#include "Cuboid.h"
#include "Sphere.h"
#include "Capsule.h"
#include "Cylinder.h"

#include "CollisionCuboid.h"
#include "CollisionSphere.h"
#include "CollisionCapsule.h"
#include "CollisionCylinder.h"

#include "GameplayCuboid.h"
#include "GameplaySphere.h"
#include "GameplayCapsule.h"
#include "GameplayCylinder.h"

#include "Plane.h"
#include "JointEffector.h"

#include "CenterOfMass.h"
#include "NavEffectorSphere.h"
#include "NavEffectorCylinder.h"
#include "NavEffectorCuboid.h"
#include "NavClueCuboid.h"

#include "DestructionGlue.h"
#include "DestructionPin.h"
#include "DestructionCmd.h"

#include "ExportNode.h"
#include "ExportNodeSet.h"
#include "ExportInfoCmd.h"

#include "EntityNode.h"
#include "EntityNodeCmd.h"
#include "EntityAssetNode.h"
#include "EntityGroupNode.h"

#include "Foundation/InitializerStack.h"
#include "Content/ContentInit.h"
#include "MayaUtils/NodeTypes.h"

#include "CallBacks.h"

#include <maya/MFnPlugin.h>
#include <maya/MUserEventMessage.h>

//
// TODO for this library:
//
//  * Check and see if Maya2008 allows us to instance hierarchies with one node which will remove overhead
//    in EntityAssetNode having to keep separate node instances up to date with the art class scene
//  * Check and see if Maya2008 has support for transient node (unsaved data), this will help allow editability
//    work because instance nodes would normally be saved when exporting the art class data because of 
//    DG connections and DAG
//

MCallbackId g_MayaExitingCallbackID;

Nocturnal::InitializerStack g_InitializerStack;

void MayaExitingCallback( void *clientData )
{
  MSceneMessage::removeCallback( g_MayaExitingCallbackID );

  g_InitializerStack.Cleanup();
}

MStatus initializePlugin( MObject obj )
{ 
  g_InitializerStack.Push( &Content::Initialize, &Content::Cleanup );

  MString registerNodeError( "registerNode " );
  MString registerCommandError( "registerCommand " );
  MFnPlugin plugin( obj, "Insomniac Games", "1.0", "Any" );

  REGISTER_NODE( CameraController, kDependNode );

  REGISTER_NODE( Locator, kLocatorNode );
  REGISTER_NODE( Cuboid, kLocatorNode );
  REGISTER_NODE( Sphere, kLocatorNode );
  REGISTER_NODE( Cylinder, kLocatorNode );
  REGISTER_NODE( CylinderChild, kLocatorNode );
  REGISTER_NODE( Capsule, kLocatorNode );
  REGISTER_NODE( CapsuleChild, kLocatorNode );

  REGISTER_NODE( GameplayCuboid, kLocatorNode );
  REGISTER_NODE( GameplaySphere, kLocatorNode );
  REGISTER_NODE( GameplayCylinder, kLocatorNode );
  REGISTER_NODE( GameplayCapsule, kLocatorNode );

  REGISTER_NODE( CollisionCylinder, kLocatorNode );
  REGISTER_NODE( CollisionCylinderChild, kLocatorNode );
  REGISTER_NODE( NavEffectorCylinder, kLocatorNode );

  REGISTER_NODE( CollisionSphere, kLocatorNode );
  REGISTER_NODE( CollisionCapsule, kLocatorNode );
  REGISTER_NODE( CollisionCapsuleChild, kLocatorNode );
  REGISTER_NODE( JointEffector, kLocatorNode );
  REGISTER_NODE( NavEffectorSphere, kLocatorNode );
  REGISTER_NODE( CenterOfMass, kLocatorNode );
  
  REGISTER_NODE( Plane, kLocatorNode );
 
  REGISTER_NODE( CollisionCuboid, kLocatorNode );
  REGISTER_NODE( NavClueCuboid, kLocatorNode );
  REGISTER_NODE( NavEffectorCuboid, kLocatorNode );

  REGISTER_NODE( DestructionGlue, kLocatorNode );
  REGISTER_NODE( DestructionPin, kLocatorNode );

  // MPxCommand
  REGISTER_COMMAND( ExportInfoCmd );
  REGISTER_COMMAND( DestructionCmd );
  REGISTER_COMMAND( CameraControllerUICmd );

  REGISTER_TRANSFORM( ExportNode, &MPxTransformationMatrix::creator, MPxTransformationMatrix::baseTransformationMatrixId );
  REGISTER_NODE( ExportNodeSet, kObjectSet );

  MStatus status = CallbacksCreate();
	if ( !status )
  {
    status.perror( "register call backs" );
    return status;
  }

  status = MUserEventMessage::registerUserEvent( MString( kUnselectInstanceData ) );
  if ( !status )
  {
    status.perror( "unable to register user event UnselectInstanceData" );
    return status;
  }

  REGISTER_TRANSFORM( EntityNode, &MPxTransformationMatrix::creator,  MPxTransformationMatrix::baseTransformationMatrixId );
  status = EntityNode::AddCallbacks();
	if (!status) 
  {
    status.perror( "unable to add callbacks" );
    return status;
  }
  
  REGISTER_TRANSFORM( EntityAssetNode, &MPxTransformationMatrix::creator,  MPxTransformationMatrix::baseTransformationMatrixId );
  status = EntityAssetNode::AddCallbacks();
	if (!status) 
  {
    status.perror( "unable to add callbacks" );
    return status;
  }

  REGISTER_TRANSFORM( EntityGroupNode, &MPxTransformationMatrix::creator,  MPxTransformationMatrix::baseTransformationMatrixId );
  REGISTER_COMMAND( EntityNodeCmd );

  g_MayaExitingCallbackID = MSceneMessage::addCallback( MSceneMessage::kMayaExiting, MayaExitingCallback );
  
	return MS::kSuccess;
}
 
MStatus uninitializePlugin( MObject obj )
{
  MStatus status;
  MString deregisterNodeError( "deregisterNode " );
  MString deregisterCommandError( "deregisterCommand " );
	MFnPlugin plugin( obj );

  DEREGISTER_NODE( CollisionSphere );
  DEREGISTER_NODE( Plane );
  DEREGISTER_NODE( CenterOfMass );
  DEREGISTER_NODE( NavEffectorSphere );
  DEREGISTER_NODE( NavEffectorCylinder );
  DEREGISTER_NODE( NavEffectorCuboid );
  DEREGISTER_NODE( NavClueCuboid );
  DEREGISTER_NODE( JointEffector );
  DEREGISTER_NODE( CollisionCylinder );
  DEREGISTER_NODE( CollisionCylinderChild );
  DEREGISTER_NODE( CollisionCuboid );
  DEREGISTER_NODE( CollisionCapsuleChild );
  DEREGISTER_NODE( CollisionCapsule );
  DEREGISTER_NODE( DestructionGlue );
  DEREGISTER_NODE( DestructionPin );
  DEREGISTER_NODE( CameraController );

  DEREGISTER_NODE( GameplayCapsule );
  DEREGISTER_NODE( GameplayCylinder );
  DEREGISTER_NODE( GameplaySphere );
  DEREGISTER_NODE( GameplayCuboid );

  DEREGISTER_NODE( CapsuleChild );
  DEREGISTER_NODE( Capsule );
  DEREGISTER_NODE( CylinderChild );
  DEREGISTER_NODE( Cylinder );
  DEREGISTER_NODE( Sphere );
  DEREGISTER_NODE( Cuboid );
  DEREGISTER_NODE( Locator );

  DEREGISTER_COMMAND( ExportInfoCmd );
  DEREGISTER_COMMAND( CameraControllerUICmd );

  status = plugin.deregisterNode( ExportNode::s_TypeID );
  if (!status)
  {
    status.perror( deregisterNodeError );
    return status;
  }

  status = plugin.deregisterNode( ExportNodeSet::s_TypeID );
  if (!status)
  {
    status.perror( deregisterNodeError );
    return status;
  }

  status = CallbacksDelete();
	if (!status) 
  {
    status.perror("unable to delete call backs");
    return status;
  }

  DEREGISTER_TRANSFORM( EntityNode );
  status = EntityNode::RemoveCallbacks();
	if (!status) 
  {
    status.perror("unable to remove callbacks");
    return status;
  }

  DEREGISTER_TRANSFORM( EntityAssetNode );
  status = EntityAssetNode::RemoveCallbacks();
	if (!status) 
  {
    status.perror("unable to remove callbacks");
    return status;
  }
  MUserEventMessage::deregisterUserEvent( MString( kUnselectInstanceData ) );

  DEREGISTER_TRANSFORM( EntityGroupNode );
  DEREGISTER_COMMAND( EntityNodeCmd );

  MSceneMessage::removeCallback( g_MayaExitingCallbackID );

  g_InitializerStack.Cleanup();

  return MS::kSuccess;
}
