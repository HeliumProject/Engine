#include "stdafx.h"

#include "CameraControllerUICmd.h"

#include "MayaUtils/NodeTypes.h"

//-----------------------------------------------------------------------------
// the name of the CameraControllerUI command
//-----------------------------------------------------------------------------
MString CameraControllerUICmd::CommandName( "igCameraControllerUI" );

//-----------------------------------------------------------------------------
// various names of ui controls and layout
//-----------------------------------------------------------------------------
MString CameraControllerUICmd::s_ui_layoutName(       "AEigCameraControllerUILayout" );
MString CameraControllerUICmd::s_ui_cameraPrefix(     "AEigCCUICamera"               );
MString CameraControllerUICmd::s_ui_firstFramePrefix( "AEigCCUIFirstFrame"           );
MString CameraControllerUICmd::s_ui_headPrefix(       "AEigCCUIHead"                 );
MString CameraControllerUICmd::s_ui_cutPrefix(        "AEigCCUICut"                  );
MString CameraControllerUICmd::s_ui_tailPrefix(       "AEigCCUITail"                 );
MString CameraControllerUICmd::s_ui_lastFramePrefix(  "AEigCCUILastFrame"            );
MString CameraControllerUICmd::s_ui_upPrefix(         "AEigCCUIUp"                   );
MString CameraControllerUICmd::s_ui_swapPrefix(       "AEigCCUISwap"                 );
MString CameraControllerUICmd::s_ui_deletePrefix(     "AEigCCUIDelete"               );

//-----------------------------------------------------------------------------
// arguments to the CameraControllerUI command
//-----------------------------------------------------------------------------
const char * CameraControllerUICmd::s_flag_long_node                    = "-node";
const char * CameraControllerUICmd::s_flag_short_node                   = "-n";
const char * CameraControllerUICmd::s_flag_long_forceUpdate             = "-forceUpdate";
const char * CameraControllerUICmd::s_flag_short_forceUpdate            = "-fu";
const char * CameraControllerUICmd::s_flag_long_inputCamera             = "-inputCamera";
const char * CameraControllerUICmd::s_flag_short_inputCamera            = "-ic";
const char * CameraControllerUICmd::s_flag_long_head                    = "-head";
const char * CameraControllerUICmd::s_flag_short_head                   = "-h";
const char * CameraControllerUICmd::s_flag_long_tail                    = "-tail";
const char * CameraControllerUICmd::s_flag_short_tail                   = "-t";
const char * CameraControllerUICmd::s_flag_long_cut                     = "-cutStart";
const char * CameraControllerUICmd::s_flag_short_cut                    = "-cs";
const char * CameraControllerUICmd::s_flag_long_up                      = "-moveUp";
const char * CameraControllerUICmd::s_flag_short_up                     = "-mu";
const char * CameraControllerUICmd::s_flag_long_swap                    = "-swap";
const char * CameraControllerUICmd::s_flag_short_swap                   = "-s";
const char * CameraControllerUICmd::s_flag_long_delete                  = "-delete";
const char * CameraControllerUICmd::s_flag_short_delete                 = "-d";
const char * CameraControllerUICmd::s_flag_long_cameraIndex             = "-cameraIndex";
const char * CameraControllerUICmd::s_flag_short_cameraIndex            = "-ci";
const char * CameraControllerUICmd::s_flag_long_createCinematicCamera   = "-createCinematicCamera";
const char * CameraControllerUICmd::s_flag_short_createCinematicCamera  = "-ccc";


//-----------------------------------------------------------------------------
// state information for the attribute editor
//-----------------------------------------------------------------------------
CameraControllerUICmd::AttributeEditorState CameraControllerUICmd::s_state =
                                            { MString(""), MObject::kNullObj, 0 };

//-----------------------------------------------------------------------------
// CameraControllerUICmd::newSyntax
// definition of the syntax for command
//-----------------------------------------------------------------------------
MSyntax CameraControllerUICmd::newSyntax()
{
  MSyntax syntax;

  // the node flag allows an CameraController to be specified
  syntax.addFlag( s_flag_short_node, s_flag_long_node, MSyntax::kString );
  // the force update flag forces a rebuild of the ui controls
  syntax.addFlag( s_flag_short_forceUpdate, s_flag_long_forceUpdate );
  // return the lowest number index with specified camera name
  syntax.addFlag( s_flag_short_cameraIndex, s_flag_long_cameraIndex, MSyntax::kString );
  // various commands are executed to respond to user input on particular rows
  syntax.addFlag( s_flag_short_inputCamera, s_flag_long_inputCamera, MSyntax::kLong );
  syntax.addFlag( s_flag_short_head,        s_flag_long_head,        MSyntax::kLong );
  syntax.addFlag( s_flag_short_tail,        s_flag_long_tail,        MSyntax::kLong );
  syntax.addFlag( s_flag_short_cut,         s_flag_long_cut,         MSyntax::kLong );
  syntax.addFlag( s_flag_short_up,          s_flag_long_up,          MSyntax::kLong );
  syntax.addFlag( s_flag_short_swap,        s_flag_long_swap,        MSyntax::kLong );
  syntax.addFlag( s_flag_short_delete,      s_flag_long_delete,      MSyntax::kLong );
  // special flag for creating camera controller or selecting existing controller
  syntax.addFlag( s_flag_short_createCinematicCamera, s_flag_long_createCinematicCamera );
  return syntax;
}

//-----------------------------------------------------------------------------
// CameraControllerUICmd::parseArgs
// Load the arguments and validate.
//-----------------------------------------------------------------------------
bool CameraControllerUICmd::parseArgs( const MArgList & args )
{
  MStatus status = MS::kSuccess;

  // set default values for unspecified arguments
  m_node = MObject::kNullObj;
  m_forceUpdate = false;
  // parse the command line arguments using the declared syntax
  MArgParser argParser( syntax(), args, &status );
  if (status!=MS::kSuccess)
  {
    MGlobal::displayError(CommandName+": error parsing arguments.");
  }
  else
  {
    bool hasFlagNode        = argParser.isFlagSet( s_flag_short_node );
    bool hasFlagForceUpdate = argParser.isFlagSet( s_flag_short_forceUpdate );
    bool hasFlagInputCamera = argParser.isFlagSet( s_flag_short_inputCamera );
    bool hasFlagHead        = argParser.isFlagSet( s_flag_short_head );
    bool hasFlagTail        = argParser.isFlagSet( s_flag_short_tail );
    bool hasFlagCut         = argParser.isFlagSet( s_flag_short_cut );
    bool hasFlagUp          = argParser.isFlagSet( s_flag_short_up );
    bool hasFlagSwap        = argParser.isFlagSet( s_flag_short_swap );
    bool hasFlagDelete      = argParser.isFlagSet( s_flag_short_delete );
    bool hasFlagCameraIndex = argParser.isFlagSet( s_flag_short_cameraIndex );
    bool hasFlagCreate      = argParser.isFlagSet( s_flag_short_createCinematicCamera );

    m_updateType = kUpdateDefault;
    m_updateRow  = 0;
    int uniqueFlagCount = 0;
    if ( hasFlagNode   ) 
    {
      uniqueFlagCount++;
    }

    if ( hasFlagInputCamera )
    {
      uniqueFlagCount++;
      m_updateType = kSelectCamera;
      argParser.getFlagArgument( s_flag_short_inputCamera, 0, m_updateRow );
    }

    if ( hasFlagHead   )
    {
      uniqueFlagCount++;
      m_updateType = kUpdateHead;
      argParser.getFlagArgument( s_flag_short_head, 0, m_updateRow );
    }

    if ( hasFlagTail   )
    {
      uniqueFlagCount++;
      m_updateType = kUpdateTail;
      argParser.getFlagArgument( s_flag_short_tail, 0, m_updateRow );
    }

    if ( hasFlagCut    )
    {
      uniqueFlagCount++;
      m_updateType = kUpdateCut;
      argParser.getFlagArgument( s_flag_short_cut, 0, m_updateRow );
    }
    if ( hasFlagUp     )
    {
      uniqueFlagCount++;
      m_updateType = kUpdateUp;
      argParser.getFlagArgument( s_flag_short_up, 0, m_updateRow );
    }

    if ( hasFlagSwap   )
    {
      uniqueFlagCount++;
      m_updateType = kUpdateSwap;
      argParser.getFlagArgument( s_flag_short_swap, 0, m_updateRow );
    }

    if ( hasFlagDelete )
    {
      uniqueFlagCount++;
      m_updateType = kUpdateDelete;
      argParser.getFlagArgument( s_flag_short_delete, 0, m_updateRow );
    }	
		
    if ( hasFlagCameraIndex )
    {
      uniqueFlagCount++;
      m_updateType = kCameraIndex;
      argParser.getFlagArgument( s_flag_short_cameraIndex, 0, m_queryCamera );
    }

    if ( hasFlagCreate )
    {
      uniqueFlagCount++;
      m_updateType = kUpdateCreate;
    }
		
    if (uniqueFlagCount > 1)
    {
      // can only have a single flag required to be unique
      return false;
    }


    m_forceUpdate = hasFlagForceUpdate;
    if ( hasFlagNode )
    {
      MString nodeName;
      status = argParser.getFlagArgument( s_flag_short_node, 0, nodeName );
      if ( status == MS::kSuccess && nodeName.length() > 0 )
      {
        MSelectionList list;
        status = MGlobal::getSelectionListByName( nodeName, list );
        if ( status == MS::kSuccess && !list.isEmpty() )
        {
          list.getDependNode( 0, m_node );
        }
      }
    }
  }
  return (status==MS::kSuccess);
}

//-----------------------------------------------------------------------------
// CameraControllerUICmd::rememberNode
//-----------------------------------------------------------------------------
void CameraControllerUICmd::rememberNode( MObject & node )
{
  s_state.m_node = node;
}

//-----------------------------------------------------------------------------
// CameraControllerUICmd::forgetNode
//-----------------------------------------------------------------------------
void CameraControllerUICmd::forgetNode()
{
  rememberNode( MObject::kNullObj );
}

//-----------------------------------------------------------------------------
// CameraControllerUICmd::doIt
// execution of the command
//-----------------------------------------------------------------------------
MStatus CameraControllerUICmd::doIt( const MArgList& args )
{
  MStatus status;
  clearResult();
  if ( parseArgs(args) )
  {
    if ( m_updateType == kUpdateCreate )
    {
      if ( !selectExistingCinematicCamera() )
      {
        createCinematicCamera();
      }
    }
    else
    {
      bool uiActive = isUIActive( true );
      if ( uiActive
       && (m_node != MObject::kNullObj || s_state.m_node != MObject::kNullObj) )
      {
        if ( m_node != MObject::kNullObj && m_node != s_state.m_node )
        {
          // change the node currently displayed in the attribute editor
          s_state.m_node = m_node;
          m_forceUpdate  = true;
        }
        CameraController * controller = getController();
        if ( controller != NULL )
        {
          unsigned int rowCount = controller->cameraCount();
          if ( !MAnimControl::isPlaying() && rowCount != s_state.m_rowCount )
          {
            eraseUI();
            s_state.m_rowCount = rowCount;
            buildUI( controller );
          }
          else
          {
            switch (m_updateType)
            {
              case kSelectCamera :
              {
                selectCamera( m_updateRow );
                break;
              }

              case kUpdateCut :
              {
                updateCut( m_updateRow );
                break;
              }
              case kUpdateUp :
              {
                updateUp( m_updateRow );
                break;
              }

              case kUpdateSwap :
              {
                updateSwap( m_updateRow );
                break;
              }

              case kUpdateDelete :
              {
                updateDelete( m_updateRow );
                break;
              }
							
              case kCameraIndex :
              {
                cameraIndex( m_queryCamera );
                break;
              }
							
              default :
              {
                updateUI( controller );
                break;
              }
            }
          }
        }
      }
    }
  }
  return MS::kSuccess;
}

bool CameraControllerUICmd::isUIActive( bool autoActivate )
{
  bool uiActive = ( s_state.m_parentLayout.length() != 0 );

  //shitty hack to deal with docked/undocked Attribute Editor wiping out the CinematicCameraController
  if( uiActive )
  {
    int obscured;
    MGlobal::executeCommand( MString("columnLayout -q -io ")+s_state.m_parentLayout, obscured );
    if ( obscured == -858993460 && autoActivate )
    {
      uiActive=false;
      s_state.m_rowCount=0;
    }
  }

  if ( !uiActive && autoActivate )
  {
    // see if ui should be activated
    MString parentName;
    MGlobal::executeCommand( "setParent -q", parentName );
    std::string debug = parentName.asChar();
    if ( autoActivate && parentName != MString("MayaWindow") )
    {
      // activate ui and remember the parent layout name
      uiActive = true;
      s_state.m_parentLayout = parentName;
    }
  }
  else if ( uiActive )
  {
    // make certain ui isn't obscured
    int obscured;
    MGlobal::executeCommand( MString("columnLayout -q -io ")+s_state.m_parentLayout, obscured );
    if ( obscured )
    {
      // ui isn't active if it is obscured
      uiActive = false;
    }
  }
  return uiActive;
}

bool CameraControllerUICmd::selectExistingCinematicCamera()
{
  MStatus status;
  MItDag dagItr( MItDag::kBreadthFirst, MFn::kCamera );
  for ( ; !dagItr.isDone(); dagItr.next() )
  {
    MFnCamera camera( dagItr.item(), &status );
    if ( status == MS::kSuccess )
    {
      MPlugArray ncpSource;
      MPlug nearClipPlanePlug = camera.findPlug( "ncp", true, &status );
      if ( status == MS::kSuccess
        && nearClipPlanePlug.connectedTo( ncpSource, true, false )
        && ncpSource.length() > 0 )
      {
        MFnDependencyNode sourceNode( ncpSource[0].node(), &status );
        if ( status == MS::kSuccess && sourceNode.typeId() == MTypeId(IGL_CAMERA_CONTROLLER_ID) )
        {
          MGlobal::select( camera.parent(0), MGlobal::kReplaceList );
          return true;
        }
      }
    }
  }
  return false;
}

void CameraControllerUICmd::createCinematicCamera()
{
  MGlobal::executeCommand( "createCinematicCamera cinematicCamera" );
}

void CameraControllerUICmd::eraseUI()
{
  int uiExists;
  MGlobal::executeCommand(MString("rowLayout -q -ex ") + s_ui_layoutName, uiExists );
  if ( uiExists )
  {
    MGlobal::executeCommand( MString("deleteUI -lay ") + s_ui_layoutName );
  }
}

void CameraControllerUICmd::buildUI( CameraController * controller )
{
  MString      name;
  unsigned int i;
  MString      enableState;

  if ( controller->isLocked() )
  {
    enableState = "-en 0 ";
  }
  else
  {
    enableState = "-en 1 ";
  }

  // create a layout with six columns
  MGlobal::executeCommand( MString("setParent ") + s_state.m_parentLayout );

  MGlobal::executeCommand( MString("rowLayout -nc 8 ")
                         + "-cw 1 91 -cw 2 37 -cw 3 37 -cw 4 37 -cw 5 37 -cw 6 37 -cw 7 32 -cw 8 26 "
                         + "-cat 1 both 2 -cat 2 both 2 -cat 3 both 2 -cat 4 both 2 -cat 5 both 2 -cat 6 both 2 -cat 7 both 6 -cat 8 both 0 "
                         + "-cal 1 left -cal 2 right -cal 3 right -cal 4 right -cal 5 right -cal 6 right -cal 7 center -cal 8 center "
                         + s_ui_layoutName );

  // first column holds the camera names
  MGlobal::executeCommand( "columnLayout -adj 1" );
  MGlobal::executeCommand( "text -l \"select\"" );
  MGlobal::executeCommand( "text -l \"camera\"" );
  for (i=0; i<s_state.m_rowCount; i++)
  {
    controller->cameraName( i, name );
    MGlobal::executeCommand( MString("button -h 26 -label \"") + name + "\" "
                           + "-c \"igCameraControllerUI "+s_flag_short_inputCamera+" "+i+"\" "
                           + s_ui_cameraPrefix + i );
  }
  MGlobal::executeCommand( "setParent .." );

  // second column holds start frame
  MGlobal::executeCommand( "columnLayout -adj 1" );
  MGlobal::executeCommand( "text -l \"first\"" );
  MGlobal::executeCommand( "text -l \"frame\"" );
  for (i=0; i<s_state.m_rowCount; i++)
  {
    MGlobal::executeCommand( MString("textField -h 26 -en 0 -tx \"")
                           + controller->getFirstFrame( i ) + "\" "
                           + s_ui_firstFramePrefix + i );
  }
  MGlobal::executeCommand( "setParent .." );

  // fourth column holds the cut start
  MGlobal::executeCommand( "columnLayout -adj 1" );
  MGlobal::executeCommand( "text -l \"cut\"" );
  MGlobal::executeCommand( "text -l \"start\"" );
  for (i=0; i<s_state.m_rowCount; i++)
  {
    MGlobal::executeCommand( MString("textField -h 26 -tx \"")
                           + controller->getCutStart( i ) + "\" "
                           + enableState
                           + "-cc \"igCameraControllerUI "+s_flag_short_cut+" "+i+"\" "
                           + s_ui_cutPrefix + i );
  }
  MGlobal::executeCommand( "setParent .." );

  // sixth column holds last frame
  MGlobal::executeCommand( "columnLayout -adj 1" );
  MGlobal::executeCommand( "text -l \"last\"" );
  MGlobal::executeCommand( "text -l \"frame\"" );
  for (i=0; i<s_state.m_rowCount; i++)
  {
    MGlobal::executeCommand( MString("textField -h 26 -en 0 -tx \"")
                           + controller->getLastFrame( i ) + "\" "
                           + s_ui_lastFramePrefix + i );
  }
  MGlobal::executeCommand( "setParent .." );

  // seventh column holds the move up button
  MGlobal::executeCommand( "columnLayout -adj 1" );
  MGlobal::executeCommand( "text -l \" \"" );
  MGlobal::executeCommand( "text -l \" \"" );
  for (i=0; i<s_state.m_rowCount; i++)
  {
    bool enable = (i!=0) && !controller->isLocked();
    MGlobal::executeCommand( MString("button -h 26 -label \"^\" ")
                           + "-en " + enable + " "
                           + "-c \"igCameraControllerUI "+s_flag_short_up+" "+i+"\" "
                           + s_ui_upPrefix + i );
  }
  MGlobal::executeCommand( "setParent .." );

  // seventh column holds the move up button
  MGlobal::executeCommand( "columnLayout -adj 1" );
  MGlobal::executeCommand( "text -l \" \"" );
  MGlobal::executeCommand( "text -l \" \"" );
  for (i=0; i<s_state.m_rowCount; i++)
  {
    MGlobal::executeCommand( MString("button -h 26 -label \"Swap\" ")
                           + "-c \"igCameraControllerUI "+s_flag_short_swap+" "+i+"\" "
                           + enableState
                           + s_ui_swapPrefix + i );
  }
  MGlobal::executeCommand( "setParent .." );

  // eighth column holds the delete camera button
  MGlobal::executeCommand( "columnLayout -adj 1" );
  MGlobal::executeCommand( "text -l \" \"" );
  MGlobal::executeCommand( "text -l \" \"" );
  for (i=0; i<s_state.m_rowCount; i++)
  {
    MGlobal::executeCommand( MString("button -h 26 -label \"X\" ")
                           + "-c \"igCameraControllerUI "+s_flag_short_delete+" "+i+"\" "
                           + enableState
                           + s_ui_deletePrefix + i );
  }
  MGlobal::executeCommand( "setParent .." );
}

void CameraControllerUICmd::updateUI( CameraController * controller )
{
  MString      name;
  unsigned int i;
  MString      enableState;

  if (MAnimControl::isPlaying())
  {
    return;
  }

  if ( controller->isLocked() )
  {
    enableState = "-en 0 ";
  }
  else
  {
    enableState = "-en 1 ";
  }

  // first column holds the camera names
  for (i=0; i<s_state.m_rowCount; i++)
  {
    controller->cameraName( i, name );
    MGlobal::executeCommand( MString("button -e -label \"") + name + "\" "
                           + s_ui_cameraPrefix + i );
  }

  // second column holds start frame
  for (i=0; i<s_state.m_rowCount; i++)
  {
    MGlobal::executeCommand( MString("textField -e -tx \"")
                           + controller->getFirstFrame( i ) + "\" "
                           + s_ui_firstFramePrefix + i );
  }

  // fourth column holds the cut start
  for (i=0; i<s_state.m_rowCount; i++)
  {
    MGlobal::executeCommand( MString("textField -e -tx \"")
                           + controller->getCutStart( i ) + "\" "
                           + enableState
                           + s_ui_cutPrefix + i );
  }

  // sixth column holds last frame
  for (i=0; i<s_state.m_rowCount; i++)
  {
    MGlobal::executeCommand( MString("textField -e -tx \"")
                           + controller->getLastFrame( i ) + "\" "
                           + s_ui_lastFramePrefix + i );
  }

  // seventh column holds the move up button
  for (i=0; i<s_state.m_rowCount; i++)
  {
    bool enable = (i!=0) && !controller->isLocked();
    MGlobal::executeCommand( MString("button -e -en ") + enable + " "
                           + s_ui_upPrefix + i );
  }

  // seventh column holds the move up button
  for (i=0; i<s_state.m_rowCount; i++)
  {
    MGlobal::executeCommand( MString("button -e ") 
                           + enableState
                           + s_ui_swapPrefix + i );
  }

  // eighth column holds the delete camera button
  for (i=0; i<s_state.m_rowCount; i++)
  {
    MGlobal::executeCommand( MString("button -e ")
                           + enableState
                           + s_ui_deletePrefix + i );
  }

}

CameraController * CameraControllerUICmd::getController()
{
  MStatus status;
  MFnDependencyNode depNode( s_state.m_node, &status );
  if ( status == MS::kSuccess && depNode.typeId() == MTypeId(IGL_CAMERA_CONTROLLER_ID) )
  {
    return static_cast<CameraController*>(depNode.userNode());
  }
  return NULL;
}

void CameraControllerUICmd::selectCamera( unsigned int row )
{
  CameraController * controller = getController();
  if ( controller != NULL )
  {
    MString cameraName;
    double cutStart, cutFinal;
    if ( row == (unsigned int)(-1) )
    {
      controller->outputCameraName( cameraName );
      cutStart = MAnimControl::animationStartTime().value();
      cutFinal = MAnimControl::animationEndTime().value();
    }
    else
    {
      controller->cameraName( row, cameraName );
      cutStart = controller->getCutStart( row );
      cutFinal = controller->getCutFinal( row );
    }
    MAnimControl::setMinTime( MTime(cutStart, MTime::uiUnit() ) );
    MAnimControl::setMaxTime( MTime(cutFinal, MTime::uiUnit() ) );
    MGlobal::executeCommand( MString("select -r ") + cameraName );
  }
}

void CameraControllerUICmd::updateCut( unsigned int row )
{
  MString strVal;
  MGlobal::executeCommand( MString("textField -q -tx ") + s_ui_cutPrefix + row, strVal );
  double value = strVal.asDouble();
  CameraController * controller = getController();
  if ( controller != NULL )
  {
    controller->setCutStart( row, value );
  }
}

void CameraControllerUICmd::updateUp( unsigned int row )
{
  CameraController * controller = getController();
  if ( row > 0 && controller != NULL )
  {
    MString nodeName = controller->name();
    MString cameraName, priorCameraName;
    controller->cameraName( row, cameraName );
    controller->cameraName( row-1, priorCameraName );
    bool sameName = ( cameraName == priorCameraName );
    bool wasCameraFeedback = ( controller->firstIndex( cameraName ) == row );
    bool priorWasFeedback  = ( controller->firstIndex( priorCameraName ) == row-1 );
    MGlobal::executeCommand( "AEigCameraControllerDisconnect " + nodeName + " " + row + " \"" + cameraName + "\" " + wasCameraFeedback );
    MGlobal::executeCommand( "AEigCameraControllerDisconnect " + nodeName + " " + (row-1) + " \"" + priorCameraName + "\" " + priorWasFeedback );
    MGlobal::executeCommand( "AEigCameraControllerConnect " + nodeName + " " + (row-1) + " \"" + cameraName + "\" " + (( sameName && priorWasFeedback ) || ( !sameName && wasCameraFeedback )) );
    MGlobal::executeCommand( "AEigCameraControllerConnect " + nodeName + " " + row + " \"" + priorCameraName + "\" " + ( !sameName && priorWasFeedback ) );
  }
}

void CameraControllerUICmd::updateSwap( unsigned int row )
{
  CameraController * controller = getController();
  if ( controller != NULL )
  {
    MString nodeName = controller->name();
    MString cameraName;
    controller->cameraName( row, cameraName );
    MGlobal::executeCommand( "AEigCameraControllerAttrWndSwapCamera " + nodeName + " " + row + " \"" + cameraName +"\"" );
  }
}

void CameraControllerUICmd::updateDelete( unsigned int row )
{
  CameraController * controller = getController();
  if ( controller != NULL )
  {
    bool    replacedFirst = false;
    MString nodeName = controller->name();
    MString cameraName;
    unsigned int rowCount = controller->cameraCount();
    if ( row < rowCount )
    {
      if ( row + 1 == rowCount )
      {
        controller->cameraName( rowCount-1, cameraName );
        bool wasCameraFeedback = ( controller->firstIndex( cameraName ) == rowCount-1 );
        MGlobal::executeCommand( "AEigCameraControllerDisconnect " + nodeName + " " + (rowCount-1) + " \"" + cameraName + "\" " + wasCameraFeedback );
      }
      else
      {
        for ( unsigned i=row; i+1<rowCount; i++ )
        {
          controller->cameraName( i+1, cameraName );
          int firstIndex = controller->firstIndex( cameraName );
          bool wasCameraFeedback = ( firstIndex == i+1 );
          bool isCameraFeedback = wasCameraFeedback;
          if ( firstIndex == row && !replacedFirst )
          {
            isCameraFeedback = true;
            replacedFirst = true;
          }

          //this is to keep the camera cuts aligned with the cameras they belong to, after a delete
          double nextCutStart = controller->getCutStart( i+1 );
          controller->setCutStart( i, nextCutStart );

          MGlobal::executeCommand( "AEigCameraControllerDisconnect " + nodeName + " " + (i+1) + " \"" + cameraName + "\" " + wasCameraFeedback );
          MGlobal::executeCommand( "AEigCameraControllerConnect " + nodeName + " " + i + " " + cameraName + " " + isCameraFeedback );
        }
      }
      MGlobal::executeCommand( "removeMultiInstance -b 1 " + nodeName + ".i[" + (rowCount-1) + "]" );
    }
  }
}

void CameraControllerUICmd::cameraIndex( const MString & queryCamera )
{
  int result = -1;
  CameraController * controller = getController();
  if ( controller != NULL )
  {
    result = controller->firstIndex( queryCamera );
  }
  setResult( result );
}

