#include "API.h"
#include "MayaCore.h"
#include "Application.h"

#include <maya/MFnPlugin.h>
#include <maya/MMessage.h>
#include <maya/MSceneMessage.h>

MCallbackId g_MayaExitingCallbackID;

void MayaExitingCallback( void *clientData )
{
  MSceneMessage::removeCallback( g_MayaExitingCallbackID );
}

MStatus initializePlugin( MObject obj )
{
  g_MayaExitingCallbackID = MSceneMessage::addCallback( MSceneMessage::kMayaExiting, MayaExitingCallback );

	return MS::kSuccess;
}
 
MStatus uninitializePlugin( MObject obj )
{
  if ( MayaCore::g_InitCount )
  {
    return MS::kFailure; // we are IN USE!
  }

	MFnPlugin plugin( obj );

  MSceneMessage::removeCallback( g_MayaExitingCallbackID );

  return MS::kSuccess;
}
