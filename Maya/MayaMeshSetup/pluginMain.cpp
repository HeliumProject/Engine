#include "stdafx.h"

#include "CollisionModeDisplayNode.h"

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
  MStatus status;
  MFnPlugin	plugin( object, "Insomniac Games - MayaMeshSetup", "1.0" );

  //
  // Nodes
  //

  status = plugin.registerNode( CollisionModeDisplayNode::m_NodeName, CollisionModeDisplayNode::m_NodeId, 
    &CollisionModeDisplayNode::Creator, &CollisionModeDisplayNode::Initialize,
    MPxNode::kLocatorNode );
  MCheckErr( status, "Failed to registerNode CollisionModeDisplayNode" );

  //
  // Commands
  //

  //
  // Callbacks
  //

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

  //
  // Commands
  //

  //
  // Callbacks
  //

  status = MMessage::removeCallbacks( g_CallbackIds );
  MCheckErr( status, "Failed to remove callbacks" );

  return status;
}

/////////////////////////////////////////////////////////////////////////////
// Called when maya exits
void MayaExitingCallback( void *clientData )
{
  MMessage::removeCallbacks( g_CallbackIds );
}