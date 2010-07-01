#include "Precompile.h"

#include "Callbacks.h"
#include "CameraController.h"
#include "ExportNode.h"
#include "ExportNodeSet.h"
#include "Maya/NodeTypes.h"

MIntArray g_CallbackIDs; 

#pragma TODO( "Remove this legacy support for old prim flags" )
void FixupPrimFlags( void )
{
  MFnDependencyNode nodeFn;
  MObject attr;
  MFnNumericAttribute nAttr;
  
  bool value;
  MStatus status;

  static int numLegacy = 3;
  static char* oldNames[] = { "pu1", "pu2", "pu3" };
  static char* newLongNames[] = { "primPhysics", "primGround", "primEffects" };
  static char* newShortNames[] = { "pph", "pg", "pe" };


  Maya::S_MObject objects;
  Maya::findNodesOfType( objects, MFn::kPluginLocatorNode );

  Maya::S_MObject::iterator itor = objects.begin();
  Maya::S_MObject::iterator end = objects.end();
  for( ; itor != end; ++itor )
  {
    nodeFn.setObject( *itor );
    
    for ( u32 i = 0; i < (u32)numLegacy; ++i )
    {
      attr = nodeFn.attribute( oldNames[ i ], &status );
      if ( status )
      {
        // get the old setting
        MPlug plug( *itor, attr );
        plug.getValue( value );
        
        // unless the old attribute was true, we skip it
        if ( !value )
          continue;

        // see if it has the new attribute
        attr = nodeFn.attribute( newShortNames[ i ], &status );
        if ( status )
        {
          MGlobal::displayInfo( MString( "WARNING: Changing attribute '" ) + oldNames[ i ] + MString("' to '") + newLongNames[ i ] + MString( "'" ) );

          // set all the old attributes to false
          plug.setValue( false );

          // set the new attribute to the old value
          plug.setAttribute( attr );
          plug.setValue( value );
        }
      }
    }
  }
  objects.clear();
}

#pragma TODO( "Remove this legacy support for water plane murkiness" )
void FixupWaterPlaneMurkiness( void )
{
  MObject attr;
  MFnDependencyNode nodeFn;
  MFnNumericAttribute nAttr;
  MStatus status;

  Maya::S_MObject objects;
  Maya::findNodesOfType( objects, IGL_WATER_PLANE );

  Maya::S_MObject::iterator itor = objects.begin();
  Maya::S_MObject::iterator end = objects.end();
  for( ; itor != end; ++itor )
  {
    nodeFn.setObject( *itor );
    attr = nodeFn.attribute( "murkiness", &status );
    if ( !status )
    {
      continue;
    }
    
    // get the old value
    double oldValue;
    MPlug plug( *itor, attr );
    plug.getValue( oldValue );
    if ( oldValue < 0.0 )
    {
      continue;
    }

    // see if it has the new attribute
    attr = nodeFn.attribute( "waterAlpha", &status );
    if ( !status )
    {
      continue;
    }

    // set all the old attributes to -1.0
    plug.setValue( -1.0 );

    // set the alpha to the converted murkiness value
    double newValue = 1.0 - exp( -oldValue );
    plug.setAttribute( attr );
    plug.setValue( newValue );
    
    char message[ 512 ];
    sprintf( message, "WARNING: Converting attribute 'murkiness' (%0.3f) to 'waterAlpha' (%0.3f)", oldValue, newValue );
    MGlobal::displayInfo( MString( message ) );
  }
  objects.clear();
}

void AfterOpenCallback( void* clientData )
{
  FixupPrimFlags();
  FixupWaterPlaneMurkiness();
}

MStatus CallbacksCreate() 
{
	MStatus stat;

  g_CallbackIDs.append((int)MDGMessage::addTimeChangeCallback(&CameraController::timeChangeCallback,
                                                  NULL, &stat));
  if (!stat)
  	MGlobal::displayError(MString("Unable to add callback for ")+
                                  CameraController::s_TypeName);  

  g_CallbackIDs.append((int)MSceneMessage::addCallback( MSceneMessage::kBeforeImport, ExportNodeSet::PreImportCB, NULL, &stat));
  if (!stat)
    MGlobal::displayError("Unable to add Before Import callback for ExportNodeSet.\n");  

  g_CallbackIDs.append((int)MSceneMessage::addCallback( MSceneMessage::kAfterImport, ExportNodeSet::PostImportCB, NULL, &stat));
  if (!stat)
    MGlobal::displayError("Unable to add Before Import callback for ExportNodeSet.\n");  

  g_CallbackIDs.append((int)MDGMessage::addNodeAddedCallback ( ExportNode::NodeAddedCB, ExportNode::s_TypeName, NULL, &stat));
  if (!stat)
    MGlobal::displayError("Unable to add Node Added callback for ExportNode.\n");  

  g_CallbackIDs.append((int)MSceneMessage::addCallback( MSceneMessage::kAfterOpen, AfterOpenCallback, NULL, &stat ) );
  if ( !stat )
    MGlobal::displayError("Unable to add AfterOpenCallback.\n");

	return MS::kSuccess; 
}

MStatus CallbacksDelete()
{
	MStatus stat;
	// Remove all the callbacks
	stat = MMessage::removeCallbacks(g_CallbackIDs);
	if (!stat)
  	MGlobal::displayError("Unable to delete callbacks");  

	return MS::kSuccess;
}
