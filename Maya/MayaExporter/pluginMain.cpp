#include <maya/MFnPlugin.h>
#include <maya/MSceneMessage.h>

#include "ExportContentCmd.h"

#include "MayaCore/MayaCore.h"
#include "Common/InitializerStack.h"
#include "Content/ContentInit.h"

void AfterSaveCallback( void *clientData );
void MayaExitingCallback( void *clientData );

MCallbackId g_AfterSaveCallbackID;
MCallbackId g_MayaExitingCallbackID;
Nocturnal::InitializerStack g_InitializerStack;

#define REGISTER_COMMAND( cmd ) \
  status = plugin.registerCommand(  cmd::CommandName, \
  & cmd::creator, \
  & cmd::newSyntax ); \
  if ( !status ) \
{ \
  status.perror( registerCommandError + cmd::CommandName ); \
  return status; \
}

#define DEREGISTER_COMMAND( cmd ) \
  status = plugin.deregisterCommand( cmd::CommandName ); \
  if (!status) \
{ \
  status.perror( deregisterCommandError ); \
  return status; \
}

MStatus initializePlugin( MObject obj )
{   
  g_InitializerStack.Push( &MayaCore::Initialize, &MayaCore::Cleanup );
  g_InitializerStack.Push( &Content::Initialize, &Content::Cleanup );

  MStatus   status;
  MString   registerNodeError( "registerNode " );
  MString   registerCommandError( "registerCommand " );
  MFnPlugin plugin( obj, "Insomniac Games", "1.0", "Any" );

  // MPxCommand
  REGISTER_COMMAND( ExportContentCmd );
 
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