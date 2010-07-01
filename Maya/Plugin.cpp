#include "Precompile.h"
#include <maya/MFnPlugin.h>
#include <maya/MSceneMessage.h>

#include "Export/ExportContentCmd.h"
#include "Foundation/InitializerStack.h"
#include "Pipeline/Content/ContentInit.h"

#include "Nodes/Cuboid.h"
#include "Nodes/Sphere.h"
#include "Nodes/Capsule.h"
#include "Nodes/Cylinder.h"

#include "Nodes/CollisionCuboid.h"
#include "Nodes/CollisionSphere.h"
#include "Nodes/CollisionCapsule.h"
#include "Nodes/CollisionCylinder.h"

#include "Nodes/GameplayCuboid.h"
#include "Nodes/GameplaySphere.h"
#include "Nodes/GameplayCapsule.h"
#include "Nodes/GameplayCylinder.h"

#include "Nodes/ExportNode.h"
#include "Nodes/ExportNodeSet.h"
#include "Nodes/ExportInfoCmd.h"

#include "Nodes/EntityNode.h"
#include "Nodes/EntityNodeCmd.h"
#include "Nodes/EntityAssetNode.h"
#include "Nodes/EntityGroupNode.h"

#include "Foundation/InitializerStack.h"
#include "Pipeline/Content/ContentInit.h"
#include "Maya/NodeTypes.h"

#include "Nodes/CallBacks.h"

#include <maya/MFnPlugin.h>
#include <maya/MUserEventMessage.h>

void AfterSaveCallback( void *clientData );
void MayaExitingCallback( void *clientData );

MCallbackId g_AfterSaveCallbackID;
MCallbackId g_MayaExitingCallbackID;
Nocturnal::InitializerStack g_InitializerStack;

MStatus initializePlugin( MObject obj )
{
    g_InitializerStack.Push( &Content::Initialize, &Content::Cleanup );

    MStatus   status;
    MString   registerNodeError( "registerNode " );
    MString   registerCommandError( "registerCommand " );
    MFnPlugin plugin( obj, "Insomniac Games", "1.0", "Any" );

    // MPxCommand
    REGISTER_COMMAND( ExportContentCmd );

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

    REGISTER_NODE( CollisionCuboid, kLocatorNode );
    REGISTER_NODE( CollisionSphere, kLocatorNode );
    REGISTER_NODE( CollisionCylinder, kLocatorNode );
    REGISTER_NODE( CollisionCylinderChild, kLocatorNode );
    REGISTER_NODE( CollisionCapsule, kLocatorNode );
    REGISTER_NODE( CollisionCapsuleChild, kLocatorNode );

    // MPxCommand
    REGISTER_COMMAND( ExportInfoCmd );

    REGISTER_TRANSFORM( ExportNode, &MPxTransformationMatrix::creator, MPxTransformationMatrix::baseTransformationMatrixId );
    REGISTER_NODE( ExportNodeSet, kObjectSet );

    status = CallbacksCreate();
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

    if ( MGlobal::mayaState() != MGlobal::kLibraryApp && MGlobal::mayaState() != MGlobal::kBatch )
    {
        g_AfterSaveCallbackID = MSceneMessage::addCallback( MSceneMessage::kAfterSave, AfterSaveCallback );
    }

    g_MayaExitingCallbackID = MSceneMessage::addCallback( MSceneMessage::kMayaExiting, MayaExitingCallback );

    return MS::kSuccess;
}

MStatus uninitializePlugin( MObject obj )
{
    MStatus   status;
    MString   deregisterNodeError( "deregisterNode " );
    MString   deregisterCommandError( "deregisterCommand " );
    MFnPlugin plugin( obj );

    DEREGISTER_COMMAND( ExportContentCmd );


    DEREGISTER_NODE( CollisionSphere );
    DEREGISTER_NODE( CollisionCylinder );
    DEREGISTER_NODE( CollisionCylinderChild );
    DEREGISTER_NODE( CollisionCuboid );
    DEREGISTER_NODE( CollisionCapsuleChild );
    DEREGISTER_NODE( CollisionCapsule );

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

    if (MGlobal::mayaState() != MGlobal::kLibraryApp && MGlobal::mayaState() != MGlobal::kBatch)
    {
        MSceneMessage::removeCallback( g_AfterSaveCallbackID );
    }

    MSceneMessage::removeCallback( g_MayaExitingCallbackID );

    g_InitializerStack.Cleanup();

    return MS::kSuccess;
}

/////////////////////////////////////////////////////////////////////////////
// Called when maya exits
void MayaExitingCallback( void *clientData )
{
    MSceneMessage::removeCallback( g_MayaExitingCallbackID );

    g_InitializerStack.Cleanup();
}


//
// TODO for this library:
//
//  * Check and see if Maya2008 allows us to instance hierarchies with one node which will remove overhead
//    in EntityAssetNode having to keep separate node instances up to date with the art class scene
//  * Check and see if Maya2008 has support for transient node (unsaved data), this will help allow editability
//    work because instance nodes would normally be saved when exporting the art class data because of 
//    DG connections and DAG
//
