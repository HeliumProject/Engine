#include "Precompile.h"
#include <maya/MFnPlugin.h>
#include <maya/MSceneMessage.h>

#include "Export/ExportContentCmd.h"
#include "Foundation/InitializerStack.h"
#include "Core/Content/ContentInit.h"

#include "Nodes/EntityInstanceNode.h"
#include "Nodes/EntityInstanceNodeCmd.h"
#include "Nodes/EntityNode.h"

#include "Foundation/InitializerStack.h"
#include "Core/Content/ContentInit.h"
#include "Maya/NodeTypes.h"

#include <maya/MFnPlugin.h>
#include <maya/MUserEventMessage.h>

using namespace Helium;

void AfterSaveCallback( void *clientData );
void MayaExitingCallback( void *clientData );

MCallbackId g_AfterSaveCallbackID;
MCallbackId g_MayaExitingCallbackID;
static Helium::InitializerStack g_InitializerStack;

MAYA_API MStatus initializePlugin( MObject obj )
{
    g_InitializerStack.Push( &Content::Initialize, &Content::Cleanup );

    MStatus   status;
    MString   registerNodeError( "registerNode " );
    MString   registerCommandError( "registerCommand " );
    MFnPlugin plugin( obj, "Insomniac Games", "1.0", "Any" );

    status = MUserEventMessage::registerUserEvent( MString( kUnselectInstanceData ) );
    if ( !status )
    {
        status.perror( "unable to register user event UnselectInstanceData" );
        return status;
    }

    REGISTER_TRANSFORM( EntityInstanceNode, &MPxTransformationMatrix::creator,  MPxTransformationMatrix::baseTransformationMatrixId );
    status = EntityInstanceNode::AddCallbacks();
    if (!status) 
    {
        status.perror( "unable to add callbacks" );
        return status;
    }

    REGISTER_TRANSFORM( EntityGroupNode, &MPxTransformationMatrix::creator,  MPxTransformationMatrix::baseTransformationMatrixId );
    REGISTER_TRANSFORM( EntityNode, &MPxTransformationMatrix::creator,  MPxTransformationMatrix::baseTransformationMatrixId );
    status = EntityNode::AddCallbacks();
    if (!status) 
    {
        status.perror( "unable to add callbacks" );
        return status;
    }

    REGISTER_COMMAND( EntityInstanceNodeCmd );

    if ( MGlobal::mayaState() != MGlobal::kLibraryApp && MGlobal::mayaState() != MGlobal::kBatch )
    {
        g_AfterSaveCallbackID = MSceneMessage::addCallback( MSceneMessage::kAfterSave, AfterSaveCallback );
    }

    g_MayaExitingCallbackID = MSceneMessage::addCallback( MSceneMessage::kMayaExiting, MayaExitingCallback );

    return MS::kSuccess;
}

MAYA_API MStatus uninitializePlugin( MObject obj )
{
    MStatus   status;
    MString   deregisterNodeError( "deregisterNode " );
    MString   deregisterCommandError( "deregisterCommand " );
    MFnPlugin plugin( obj );

    DEREGISTER_TRANSFORM( EntityInstanceNode );
    status = EntityInstanceNode::RemoveCallbacks();
    if (!status) 
    {
        status.perror("unable to remove callbacks");
        return status;
    }

    DEREGISTER_TRANSFORM( EntityGroupNode );
    DEREGISTER_TRANSFORM( EntityNode );
    status = EntityNode::RemoveCallbacks();
    if (!status) 
    {
        status.perror("unable to remove callbacks");
        return status;
    }
    MUserEventMessage::deregisterUserEvent( MString( kUnselectInstanceData ) );

    DEREGISTER_COMMAND( EntityInstanceNodeCmd );

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
//    in EntityNode having to keep separate node instances up to date with the art class scene
//  * Check and see if Maya2008 has support for transient node (unsaved data), this will help allow editability
//    work because instance nodes would normally be saved when exporting the art class data because of 
//    DG connections and DAG
//
