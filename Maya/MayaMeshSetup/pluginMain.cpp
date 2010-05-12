#include "stdafx.h"

#include "CollisionModeDisplayNode.h"
#include "MaterialDisplayNode.h"
#include "MaterialEditorCmd.h"

#include "MayaCore/MayaCore.h"
#include "MayaUtils/ErrorHelpers.h"
#include "MayaUtils/NodeTypes.h"

using namespace Maya;

MIntArray g_CallbackIds;

void MayaExitingCallback( void *clientData );

///////////////////////////////////////////////////////////////////////////////
// This plugin used to be just for flagging polygons, but now it is generic
// enough that it could really be used for anything. 
//
// A mel script or another plugin will have to manage which objects are in
// the 'drawn' set.  All this plugin does is take those objects and visualize
// them in maya.
//
MStatus initializePlugin( MObject object )
{
  MayaCore::Initialize();

  MStatus status;
  MFnPlugin	plugin( object, "Insomniac Games - MayaMeshSetup", "1.0" );

  //
  // Nodes
  //

  status = plugin.registerNode( CollisionModeDisplayNode::m_NodeName, CollisionModeDisplayNode::m_NodeId, 
    &CollisionModeDisplayNode::Creator, &CollisionModeDisplayNode::Initialize,
    MPxNode::kLocatorNode );
  MCheckErr( status, "Failed to registerNode CollisionModeDisplayNode" );

  status = plugin.registerNode( MaterialDisplayNode::m_NodeName, MaterialDisplayNode::m_NodeId, 
    &MaterialDisplayNode::Creator, &MaterialDisplayNode::Initialize,
    MPxNode::kLocatorNode );
  MCheckErr( status, "Failed to registerNode MaterialDisplayNode" );
  
  status = MaterialDisplayNode::AddCallbacks();
  MCheckErr( status, "Failed to AddCallbacks MaterialDisplayNode" );

  //
  // Commands
  //

  status = plugin.registerCommand( MaterialEditorCmd::m_CommandName,
    &Maya::MaterialEditorCmd::Creator, &Maya::MaterialEditorCmd::CommandSyntax  );
  MCheckErr( status, "Failed to registerCommand MaterialEditorCmd" );

  //
  // Callbacks
  //

  status = g_CallbackIds.append( 
    MSceneMessage::addCallback( MSceneMessage::kMayaExiting, 
    MaterialEditorCmd::MayaExistingCallBack ) );
  MCheckErr( status, "Failed to add MSceneMessage::kMayaExiting callback MaterialEditorCmd::MayaExistingCallBack" );

  g_CallbackIds.append( MSceneMessage::addCallback( MSceneMessage::kMayaExiting, MayaExitingCallback ) );

  return status;
}


MStatus uninitializePlugin( MObject object )
{
  MFnPlugin	plugin( object );
  MStatus status;

  //
  // Nodes
  //

  status = plugin.deregisterNode( CollisionModeDisplayNode::m_NodeId );
  MCheckErr( status, "Failed to deregisterNode CollisionModeDisplayNode" );

  status = plugin.deregisterNode( MaterialDisplayNode::m_NodeId );
  MCheckErr( status, "Failed to deregisterNode MaterialDisplayNode" );

  status = MaterialDisplayNode::RemoveCallbacks();
  MCheckErr( status, "Failed to remove callbacks MaterialDisplayNode" );
  
  //
  // Commands
  //

  status = plugin.deregisterCommand( MaterialEditorCmd::m_CommandName );
  MCheckErr( status, "Failed to deregisterCommand MaterialEditorCmd" );

  //
  // Callbacks
  //

  status = MMessage::removeCallbacks( g_CallbackIds );
  MCheckErr( status, "Failed to remove callbacks" );

  MayaCore::Cleanup();

  return status;
}

/////////////////////////////////////////////////////////////////////////////
// Called when maya exits
void MayaExitingCallback( void *clientData )
{
  MMessage::removeCallbacks( g_CallbackIds );

  MayaCore::Cleanup();
}