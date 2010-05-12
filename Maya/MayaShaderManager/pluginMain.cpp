#include "stdafx.h"
#include "API.h"
#include "MayaShaderManager.h"

#include "MayaCore/MayaCore.h"
#include "Finder/LunaSpecs.h"
#include "MayaUtils/ErrorHelpers.h"
#include "UIToolKit/ImageManager.h"

#include <maya/MFnPlugin.h>
#include <maya/MSceneMessage.h>

using namespace Maya;

MIntArray g_CallbackIds;

void MayaExitingCallback( void *clientData );

MStatus MAYASHADERMANAGER_API initializePlugin( MObject obj )
{ 
  MayaCore::Initialize();

  UIToolKit::ImageManagerInit( FinderSpecs::Luna::DEFAULT_THEME_FOLDER.GetFolder(), FinderSpecs::Luna::GAME_THEME_FOLDER.GetFolder() );

  MStatus status;
  MFnPlugin plugin( obj, "Insomniac Games", "1.0", "Any" ); 

  status = plugin.registerCommand( "shaderManager", &Maya::ShaderManager::creator, &Maya::ShaderManager::commandSyntax  );
  status = g_CallbackIds.append( MSceneMessage::addCallback( MSceneMessage::kMayaExiting, Maya::HideShaderBrowserCallBack ) );
  MCheckErr( status, "Failed to add MSceneMessage::kMayaExiting callback Maya::HideShaderBrowserCallBack" );

  g_CallbackIds.append( MSceneMessage::addCallback( MSceneMessage::kMayaExiting, MayaExitingCallback ) );

	return status;
}
 
MStatus MAYASHADERMANAGER_API uninitializePlugin( MObject obj )
{
	MStatus status;
  MFnPlugin plugin( obj );

  plugin.deregisterCommand( "shaderManager" );

  status = MMessage::removeCallbacks( g_CallbackIds );
  MCheckErr( status, "Failed to remove callbacks" );

  UIToolKit::ImageManagerCleanup();

  MayaCore::Cleanup();

	return MS::kSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// Called when maya exits
void MayaExitingCallback( void *clientData )
{
  MMessage::removeCallbacks( g_CallbackIds );

  UIToolKit::ImageManagerCleanup();

  MayaCore::Cleanup();
}