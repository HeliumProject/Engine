#include "Precompile.h"

#include "Maya/NodeTypes.h"
#include "Maya/ErrorHelpers.h"

#include "CameraController.h"
#include "CameraControllerUICmd.h"

#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Version.h"

#include <maya/MAnimControl.h>
#include <maya/MEventMessage.h>
#include <maya/MSceneMessage.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MItDag.h>
#include <maya/MSelectionList.h>

#include <math.h>

using namespace Nocturnal;

#define EPSILON (0.001)

MString g_unknownCamera("unknownCam");

MTypeId CameraController::s_TypeID( IGL_CAMERA_CONTROLLER_ID );
MString CameraController::s_TypeName( "igCameraController" );

// attributes of an CameraController
MObject CameraController::s_attr_lock;
MObject CameraController::s_attr_inTime;
MObject CameraController::s_attr_input;
MObject CameraController::s_attr_i_keys;
MObject CameraController::s_attr_i_keys_cutStart;
MObject CameraController::s_attr_i_worldMatrix;
MObject CameraController::s_attr_i_focalLength;
MObject CameraController::s_attr_i_nearClip;
MObject CameraController::s_attr_i_farClip;

MObject CameraController::s_attr_cameraControl;

MObject CameraController::s_attr_output;
MObject CameraController::s_attr_o_translateX;
MObject CameraController::s_attr_o_translateY;
MObject CameraController::s_attr_o_translateZ;
MObject CameraController::s_attr_o_translate;
MObject CameraController::s_attr_o_rotateX;
MObject CameraController::s_attr_o_rotateY;
MObject CameraController::s_attr_o_rotateZ;
MObject CameraController::s_attr_o_rotate;
MObject CameraController::s_attr_o_rotateOrder;
MObject CameraController::s_attr_o_scaleX;
MObject CameraController::s_attr_o_scaleY;
MObject CameraController::s_attr_o_scaleZ;
MObject CameraController::s_attr_o_scale;
MObject CameraController::s_attr_o_shearXY;
MObject CameraController::s_attr_o_shearXZ;
MObject CameraController::s_attr_o_shearYZ;
MObject CameraController::s_attr_o_shear;
MObject CameraController::s_attr_o_rotatePivotX;
MObject CameraController::s_attr_o_rotatePivotY;
MObject CameraController::s_attr_o_rotatePivotZ;
MObject CameraController::s_attr_o_rotatePivot;
MObject CameraController::s_attr_o_rotatePTranX;
MObject CameraController::s_attr_o_rotatePTranY;
MObject CameraController::s_attr_o_rotatePTranZ;
MObject CameraController::s_attr_o_rotatePTran;
MObject CameraController::s_attr_o_scalePivotX;
MObject CameraController::s_attr_o_scalePivotY;
MObject CameraController::s_attr_o_scalePivotZ;
MObject CameraController::s_attr_o_scalePivot;
MObject CameraController::s_attr_o_scalePTranX;
MObject CameraController::s_attr_o_scalePTranY;
MObject CameraController::s_attr_o_scalePTranZ;
MObject CameraController::s_attr_o_scalePTran;
MObject CameraController::s_attr_o_rotateAxisX;
MObject CameraController::s_attr_o_rotateAxisY;
MObject CameraController::s_attr_o_rotateAxisZ;
MObject CameraController::s_attr_o_rotateAxis;
MObject CameraController::s_attr_o_focalLength;
MObject CameraController::s_attr_o_nearClip;
MObject CameraController::s_attr_o_farClip;

MObject CameraController::s_attr_guid;

MObject CameraController::existingController()
{
  const MString focalLength("focalLength");
  MStatus status;
  MItDag dagIter( MItDag::kBreadthFirst, MFn::kCamera, &status );
  if ( status == MS::kSuccess )
  {
    for ( ; !dagIter.isDone(); dagIter.next() )
    {
      MFnDagNode dagNodeFn( dagIter.item(), &status );
      if ( status == MS::kSuccess )
      {
        MPlug focalLengthPlug( dagNodeFn.findPlug( focalLength, true, &status ) );
        if ( status == MS::kSuccess )
        {
          MPlugArray focalLengthSourcePlugs;
          if ( focalLengthPlug.connectedTo( focalLengthSourcePlugs, true, false, &status )
            && status == MS::kSuccess 
            && focalLengthSourcePlugs.length() == 1 )
          {
            MObject focalLengthSourceNode = focalLengthSourcePlugs[0].node();
            if ( focalLengthSourceNode.apiType() == MFn::kPluginDependNode )
            {
              MFnDependencyNode depNodeFn( focalLengthSourceNode, &status );
              if ( status == MS::kSuccess 
                && depNodeFn.typeId() == CameraController::s_TypeID )
              {
                return focalLengthSourceNode;
              }
            }
          }
        }
      }
    }
  }
    return MObject::kNullObj;
}

CameraController::CameraController()
: MPxNode()
, m_animationTimeRangeChangedCallback( NULL )
, m_beforeSaveCallback( NULL )
, m_afterSaveCallback( NULL )
, m_nodeRemovedCallback( NULL )
, m_lock( false )
, m_time( 0.0 )
{
}

CameraController::~CameraController()
{
	// Delete attribute update callback
  MEventMessage::removeCallback( m_animationTimeRangeChangedCallback );
  MSceneMessage::removeCallback( m_beforeSaveCallback );
  MSceneMessage::removeCallback( m_afterSaveCallback );
  MDGMessage::removeCallback( m_nodeRemovedCallback );
}

void * CameraController::Creator()
{
  return new CameraController;
}

MStatus CameraController::Initialize()
{
  static const char* initializeError = "failure in CameraController::Initialize()";
  MStatus              status;
  MFnCompoundAttribute cAttr;
  MFnEnumAttribute     eAttr;
  MFnNumericAttribute  nAttr;
  MFnTypedAttribute    tAttr;
  MFnUnitAttribute     uAttr;
  MFnMatrixAttribute   mAttr;

  // create an attribute for locking the node
  s_attr_lock = nAttr.create( "lock", "l", MFnNumericData::kBoolean, 0, &status );
  MCheckErr( status, initializeError );

  // create an attribute for current time
  s_attr_inTime = uAttr.create( "inTime", "it", MFnUnitAttribute::kTime, 0, &status );
  uAttr.setStorable( false );
  MCheckErr( status, initializeError );

  // create an attribute for cut timing (start, end, handle length)
  s_attr_i_keys_cutStart = nAttr.create( "cutStart", "cs", MFnNumericData::kDouble, 0, &status );
  MCheckErr( status, initializeError );
  s_attr_i_worldMatrix = mAttr.create( "worldMatrix", "wm" );
  mAttr.setStorable( false );
  s_attr_i_focalLength = nAttr.create( "focalLength", "fl", MFnNumericData::kDouble, 35.0 );
  nAttr.setStorable( false );

  s_attr_i_nearClip = uAttr.create( "nearClipPlane", "ncp", MFnUnitAttribute::kDistance, 0.1 );
  uAttr.setStorable( false );

  s_attr_i_farClip = uAttr.create( "farClipPlane", "fcp", MFnUnitAttribute::kDistance, 1000.0 );
  uAttr.setStorable( false );

  s_attr_cameraControl = nAttr.create( "cameraControl", "cc", MFnNumericData::kBoolean, 0 );
  nAttr.setArray( true );
  nAttr.setWritable( false );
  nAttr.setStorable( false );

  // now for the output attributes
  s_attr_o_translateX = uAttr.create( "outTranslateX", "otx", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_translateY = uAttr.create( "outTranslateY", "oty", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_translateZ = uAttr.create( "outTranslateZ", "otz", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_translate = nAttr.create( "outTranslate", "ot", s_attr_o_translateX, s_attr_o_translateY, s_attr_o_translateZ );
  nAttr.setWritable( false );
  nAttr.setStorable( false );

  s_attr_o_rotateX = uAttr.create( "outRotateX", "orx", MFnUnitAttribute::kAngle );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotateY = uAttr.create( "outRotateY", "ory", MFnUnitAttribute::kAngle );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotateZ = uAttr.create( "outRotateZ", "orz", MFnUnitAttribute::kAngle );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotate = nAttr.create( "outRotate", "or", s_attr_o_rotateX, s_attr_o_rotateY, s_attr_o_rotateZ );
  nAttr.setWritable( false );

  s_attr_o_rotateOrder = eAttr.create( "outRotateOrder", "oro" );
  eAttr.addField( "xyz", 0 );
  eAttr.addField( "yzx", 1 );
  eAttr.addField( "zxy", 2 );
  eAttr.addField( "xzy", 3 );
  eAttr.addField( "yxz", 4 );
  eAttr.addField( "zyx", 5 );
  eAttr.setWritable( false );

  s_attr_o_scaleX = nAttr.create( "outScaleX", "osx", MFnNumericData::kDouble, 1.0 );
  nAttr.setWritable( false );
  s_attr_o_scaleY = nAttr.create( "outScaleY", "osy", MFnNumericData::kDouble, 1.0 );
  nAttr.setWritable( false );
  s_attr_o_scaleZ = nAttr.create( "outScaleZ", "osz", MFnNumericData::kDouble, 1.0 );
  nAttr.setWritable( false );
  s_attr_o_scale = nAttr.create( "outScale", "os", s_attr_o_scaleX, s_attr_o_scaleY, s_attr_o_scaleZ );
  nAttr.setWritable( false );

  s_attr_o_shearXY = nAttr.create( "outShearXY", "oshxy", MFnNumericData::kDouble );
  nAttr.setWritable( false );
  s_attr_o_shearXZ = nAttr.create( "outShearXZ", "oshxz", MFnNumericData::kDouble );
  nAttr.setWritable( false );
  s_attr_o_shearYZ = nAttr.create( "outShearYZ", "oshyz", MFnNumericData::kDouble );
  nAttr.setWritable( false );
  s_attr_o_shear = nAttr.create( "outShear", "osh", s_attr_o_shearXY, s_attr_o_shearXZ, s_attr_o_shearYZ );
  nAttr.setWritable( false );

  s_attr_o_rotatePivotX = uAttr.create( "outRotatePivotX", "orpx", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotatePivotY = uAttr.create( "outRotatePivotY", "orpy", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotatePivotZ = uAttr.create( "outRotatePivotZ", "orpz", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotatePivot = nAttr.create( "outRotatePivot", "orp", s_attr_o_rotatePivotX, s_attr_o_rotatePivotY, s_attr_o_rotatePivotZ );
  nAttr.setWritable( false );

  s_attr_o_rotatePTranX = uAttr.create( "outRotatePivotTranslateX", "orptx", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotatePTranY = uAttr.create( "outRotatePivotTranslateY", "orpty", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotatePTranZ = uAttr.create( "outRotatePivotTranslateZ", "orptz", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotatePTran = nAttr.create( "outRotatePivotTranslate", "orpt", s_attr_o_rotatePTranX, s_attr_o_rotatePTranY, s_attr_o_rotatePTranZ );
  nAttr.setWritable( false );

  s_attr_o_scalePivotX = uAttr.create( "outScalePivotX", "ospx", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_scalePivotY = uAttr.create( "outScalePivotY", "ospy", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_scalePivotZ = uAttr.create( "outScalePivotZ", "ospz", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_scalePivot = nAttr.create( "outScalePivot", "osp", s_attr_o_scalePivotX, s_attr_o_scalePivotY, s_attr_o_scalePivotZ );
  nAttr.setWritable( false );

  s_attr_o_scalePTranX = uAttr.create( "outScalePivotTranslateX", "osptx", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_scalePTranY = uAttr.create( "outScalePivotTranslateY", "ospty", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_scalePTranZ = uAttr.create( "outScalePivotTranslateZ", "osptz", MFnUnitAttribute::kDistance );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_scalePTran = nAttr.create( "outScalePivotTranslate", "ospt", s_attr_o_scalePTranX, s_attr_o_scalePTranY, s_attr_o_scalePTranZ );
  nAttr.setWritable( false );

  s_attr_o_rotateAxisX = uAttr.create( "outRotateAxisX", "orax", MFnUnitAttribute::kAngle );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotateAxisY = uAttr.create( "outRotateAxisY", "oray", MFnUnitAttribute::kAngle );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotateAxisZ = uAttr.create( "outRotateAxisZ", "oraz", MFnUnitAttribute::kAngle );
  uAttr.setWritable( false );
  uAttr.setStorable( false );
  s_attr_o_rotateAxis = nAttr.create( "outRotateAxis", "ora", s_attr_o_rotateAxisX, s_attr_o_rotateAxisY, s_attr_o_rotateAxisZ );
  nAttr.setWritable( false );

  s_attr_o_focalLength = nAttr.create( "outFocalLength", "ofl", MFnNumericData::kDouble, 35.0 );
  nAttr.setWritable( false );

  s_attr_o_nearClip = uAttr.create( "outNearClipPlane", "oncp", MFnUnitAttribute::kDistance, 0.1 );
  uAttr.setWritable( false );
  uAttr.setStorable( false );

  s_attr_o_farClip = uAttr.create( "outFarClipPlane", "ofcp", MFnUnitAttribute::kDistance, 1000.0 );
  uAttr.setWritable( false );
  uAttr.setStorable( false );

  s_attr_guid = tAttr.create( "GUID", "GID", MFnData::kString );
  tAttr.setConnectable( false );

  // add the special (non input/output) attributes
  addAttribute( s_attr_lock );
  addAttribute( s_attr_inTime );

  // create the (very complex) input attribute
  s_attr_input = cAttr.create( "input", "i", &status );
  MCheckErr( status, initializeError );
  cAttr.setArray( true );
  cAttr.addChild( s_attr_i_keys_cutStart        );
  cAttr.addChild( s_attr_i_focalLength );
  cAttr.addChild( s_attr_i_worldMatrix );
  cAttr.addChild( s_attr_i_nearClip    );
  cAttr.addChild( s_attr_i_farClip     );
  addAttribute( s_attr_input );

  addAttribute( s_attr_cameraControl );

  // create the (very complex) output attribute
  s_attr_output = cAttr.create( "output", "o", &status );
  MCheckErr( status, initializeError );
  cAttr.setArray( false );
  cAttr.addChild( s_attr_o_translate   );
  cAttr.addChild( s_attr_o_rotate      );
  cAttr.addChild( s_attr_o_rotateOrder );
  cAttr.addChild( s_attr_o_scale       );
  cAttr.addChild( s_attr_o_shear       );
  cAttr.addChild( s_attr_o_rotatePivot );
  cAttr.addChild( s_attr_o_rotatePTran );
  cAttr.addChild( s_attr_o_scalePivot  );
  cAttr.addChild( s_attr_o_scalePTran  );
  cAttr.addChild( s_attr_o_rotateAxis  );
  cAttr.addChild( s_attr_o_focalLength );
  cAttr.addChild( s_attr_o_nearClip    );
  cAttr.addChild( s_attr_o_farClip     );
  addAttribute( s_attr_output );

  addAttribute( s_attr_guid );

  attributeAffects( s_attr_lock,   s_attr_output );
  attributeAffects( s_attr_inTime, s_attr_output );
  attributeAffects( s_attr_input,  s_attr_output );
  attributeAffects( s_attr_inTime, s_attr_cameraControl );
  attributeAffects( s_attr_input,  s_attr_cameraControl );

  return MS::kSuccess;
}

void CameraController::timeChangeCallback( MTime & time, void * clientData )
{
  updateUI();
}

void CameraController::animationTimeRangeChangedCallback(void* node)
{
  if ( node!=NULL )
  {
    CameraController * controller = static_cast<CameraController*>(node);
    if (! controller->m_lock )
    {
      controller->validateCutValues();
      updateUI();
    }
  }
}

void CameraController::beforeSaveCallback(void* node)
{
  if ( node!=NULL )
  {
    CameraController * controller = static_cast<CameraController*>(node);
    MFnDependencyNode depNode( controller->thisMObject() );
    MPlug guidPlug = depNode.findPlug( s_attr_guid, true );
    MString guidStr;
    guidPlug.getValue( guidStr );
    if ( guidStr.length() == 0 )
    {
      TUID id( TUID::Generate() );
      tstring s;
      id.ToString(s);
      guidStr = MString(s.c_str());
      guidPlug.setLocked( false );
      guidPlug.setValue(guidStr);
      guidPlug.setLocked( true );
    }
    if (! controller->m_lock )
    {
      // before saving set the lock, so that reloading will be safe
      controller->setLock( true );
    }
    // save the correct animation range in temporaries to be restored after saving
    controller->m_temporaryMinTime = MAnimControl::minTime();
    controller->m_temporaryMaxTime = MAnimControl::maxTime();
    // set playback range to complete animation range
    MAnimControl::setMinTime( MAnimControl::animationStartTime() );
    MAnimControl::setMaxTime( MAnimControl::animationEndTime() );
  }
}

void CameraController::afterSaveCallback(void* node)
{
  if ( node!=NULL )
  {
    CameraController * controller = static_cast<CameraController*>(node);
    MAnimControl::setMinTime( controller->m_temporaryMinTime );
    MAnimControl::setMaxTime( controller->m_temporaryMaxTime );
  }
}

void CameraController::nodeRemovedCallback( MObject & node, void * clientData )
{
  CameraControllerUICmd::forgetNode();
}


void CameraController::postConstructor()
{
	// Add node callback to take action when attributes change
	m_animationTimeRangeChangedCallback = MEventMessage::addEventCallback(
                                          "playbackRangeSliderChanged", 
                                          animationTimeRangeChangedCallback, this );
  m_beforeSaveCallback = MSceneMessage::addCallback( MSceneMessage::kBeforeSave,
                                                     beforeSaveCallback, this );
  m_afterSaveCallback = MSceneMessage::addCallback( MSceneMessage::kAfterSave,
                                                     afterSaveCallback, this );
  m_nodeRemovedCallback = MDGMessage::addNodeRemovedCallback( &nodeRemovedCallback,
                                                              s_TypeName );
  CameraControllerUICmd::rememberNode( thisMObject() );
}

MStatus CameraController::compute( const MPlug & plug, MDataBlock & dataBlock )
{
  loadInput( dataBlock );
  storeOutput( currentIndex(), dataBlock );
  dataBlock.setClean( plug );
  dataBlock.setClean( s_attr_output );
  dataBlock.setClean( s_attr_cameraControl );
  updateUI();
  return MS::kSuccess;
}

unsigned int CameraController::currentIndex()
{
  double currentTime = MAnimControl::currentTime().value() + EPSILON;
  unsigned int count = (unsigned int)m_input.size();
  if ( count==0 || currentTime<MAnimControl::animationStartTime().value() )
  {
    return 0;
  }
  unsigned int maxIndex = count - 1;
  unsigned int index = 0;
  while ( index < maxIndex && m_input[index+1].m_cutStart < currentTime )
  {
    index++;
  }
  return index;
}

unsigned int CameraController::cameraCount()
{
  return (unsigned int)m_input.size();
}

void CameraController::cameraName( unsigned int index, MString & name )
{
  MStatus status;
  if (index < m_input.size())
  {
    name = m_input[index].m_cameraName;
  }
  else
  {
    name = g_unknownCamera + index;
  }
}

int CameraController::firstIndex( const MString & cameraName )
{
  int inputSize = (int)m_input.size();
  for (int i=0; i<inputSize; i++)
  {
    if ( cameraName == m_input[i].m_cameraName )
    {
      return i;
    }
  }
  return -1;
}

void CameraController::outputCameraName( MString & name )
{
  MStatus    status;
  MPlugArray destination;
  // use plugs for getting the camera name
  MFnDependencyNode depNode( thisMObject() );
  MPlug outputPlug( depNode.findPlug( s_attr_output ) );
  MPlug outTranslatePlug( outputPlug.child( s_attr_o_translate ) );
  name.clear();
  if ( outTranslatePlug.connectedTo( destination, false, true ) && destination.length() > 0 )
  {
    MFnDependencyNode destNode( destination[0].node(), &status );
    if ( status == MS::kSuccess )
    {
      name = destNode.name();
    }
  }
}

void CameraController::setLock( bool lock )
{
  MStatus status;
  MFnDependencyNode depNode( thisMObject(), &status );
  if ( status == MS::kSuccess )
  {
    MPlug lockPlug = depNode.findPlug( s_attr_lock, true, &status );
    if ( status == MS::kSuccess )
    {
      lockPlug.setValue( lock );
      updateUI();
    }
  }
}

double CameraController::getFirstFrame( unsigned int index )
{
  double firstFrame = MAnimControl::animationStartTime().value();
  if ( index > 0 && index < m_input.size())
  {
    firstFrame = std::max( firstFrame, m_input[index].m_cutStart );
  }
  return firstFrame;
}

double CameraController::getLastFrame(  unsigned int index )
{
  double lastFrame = MAnimControl::animationEndTime().value();
  unsigned int inputSize = (unsigned int)m_input.size();
  if ( index < inputSize-1 )
  {
    lastFrame = std::min( lastFrame, m_input[index+1].m_cutStart - 1 );
  }
  return lastFrame;
}

double CameraController::getCutStart( unsigned int index )
{
  if (index < m_input.size())
  {
    return m_input[index].m_cutStart;
  }
  return 0.0;
}


double CameraController::getCutFinal( unsigned int index )
{
  unsigned int cameraCount = (unsigned int)m_input.size();
  if (index+1 == cameraCount)
  {
    return MAnimControl::animationEndTime().value();
  }
  else
  if (index < cameraCount)
  {
    return (int)(m_input[index+1].m_cutStart) - 1;
  }
  return 0.0;
}

void CameraController::setCutStart( unsigned int index, double value )
{
  if (index < m_input.size() && !m_lock)
  {
    m_input[index].m_cutStart = value;
    validateCutValues();
  }
}

void CameraController::updateUI()
{
  // force an update (not really forced unless attribute editor layout is not obscured)
  if ( CameraControllerUICmd::isUIActive( false ) )
  {
    MGlobal::executeCommand( CameraControllerUICmd::CommandName
                           + " " + CameraControllerUICmd::s_flag_short_forceUpdate );
  }
}

void CameraController::loadInput( MDataBlock & dataBlock )
{
  MStatus status;

  // get the state of the lock
  MDataHandle h_lock( dataBlock.inputValue( s_attr_lock ) );
  m_lock = h_lock.asBool();

  // get the time (just for debugging)
  MDataHandle h_inTime( dataBlock.inputValue( s_attr_inTime ) );
  m_time = h_inTime.asTime().value();

  // clear out any previous return value
  m_input.clear();

  // use handles to grab most data
  MArrayDataHandle h_inputArray( dataBlock.inputArrayValue( s_attr_input ) );
  unsigned int inputCount = h_inputArray.elementCount();

  // and use plugs for getting the camera name
  MFnDependencyNode depNode( thisMObject() );
  MPlug inputArrayPlug( depNode.findPlug( s_attr_input ) );

  for (unsigned int i=0; i<inputCount; i++, h_inputArray.next())
  {
    MDataHandle h_inputElement( h_inputArray.inputValue() );
    // grab all the handles for this input element
    MDataHandle h_cutStart(    h_inputElement.child( s_attr_i_keys_cutStart ) );
    MDataHandle h_focalLength( h_inputElement.child( s_attr_i_focalLength   ) );
    MDataHandle h_nearClip(    h_inputElement.child( s_attr_i_nearClip      ) );
    MDataHandle h_farClip(     h_inputElement.child( s_attr_i_farClip       ) );

    // initialize some data
    CameraControllerInput in;
    in.m_translate                 = MVector::zero;
    in.m_rotate                    = MVector::zero;

    // grab the data
    in.m_cutStart                  = h_cutStart.asDouble();
    in.m_rotateOrder               = 0;
    in.m_scale                     = MVector::one;
    in.m_shear                     = MVector::zero;
    in.m_rotatePivot               = MVector::zero;
    in.m_rotatePivotTranslate      = MVector::zero;
    in.m_scalePivot                = MVector::zero;
    in.m_scalePivotTranslate       = MVector::zero;
    in.m_rotateAxis                = MVector::zero;
    in.m_focalLength               = h_focalLength.asDouble();
    in.m_nearClipPlane             = h_nearClip.asDouble();
    in.m_farClipPlane              = h_farClip.asDouble();

    // get the camera name
    MPlugArray sourcePlugArray;
    MPlug inputElementPlug( inputArrayPlug.elementByLogicalIndex( i ) );
    MPlug focalLengthPlug( inputElementPlug.child( s_attr_i_focalLength ) );
    if ( focalLengthPlug.connectedTo( sourcePlugArray, true, false )
      && sourcePlugArray.length() > 0 )
    {
      MFnDagNode cameraShapeNode( sourcePlugArray[0].node(), &status );
      if ( status == MS::kSuccess )
      {
        tstring shapeName = cameraShapeNode.partialPathName().asTChar();
        MFnTransform cameraTransformNode( cameraShapeNode.parent(0), &status );
        if ( status == MS::kSuccess )
        {
          MDagPath cameraDagPath;
          cameraTransformNode.getPath( cameraDagPath );
          cameraTransformNode.setObject( cameraDagPath );
          double                               rotComponent[3];
          MTransformationMatrix::RotationOrder rotOrder;
          tstring camName = cameraTransformNode.partialPathName().asTChar();
          cameraTransformNode.getRotation( rotComponent, rotOrder, MSpace::kWorld );
          
          //since there doesn't appear to be a proper way to get the rotateAxis, we go the long route...
          MPlug raPlug = cameraTransformNode.findPlug("rotateAxis");
          MPlug xra = raPlug.child(0);
          MPlug yra = raPlug.child(1);
          MPlug zra = raPlug.child(2);
          double xraVal;
          double yraVal;
          double zraVal;
          xra.getValue(xraVal);
          yra.getValue(yraVal);
          zra.getValue(zraVal);
          in.m_rotateAxis = MVector( xraVal, yraVal, zraVal );

          in.m_rotate = MVector( rotComponent[0], rotComponent[1], rotComponent[2] );
          in.m_rotateOrder = (unsigned int)(rotOrder - 1);
          in.m_translate = cameraTransformNode.translation( MSpace::kWorld );
          in.m_cameraName = cameraTransformNode.partialPathName();
        }
      }
    }
    if ( in.m_cameraName.length() == 0 )
    {
      in.m_cameraName = g_unknownCamera + i;
    }
    // store this element
    m_input.push_back( in );
  }

  if ( !m_input.empty() && !m_lock )
  {
    // force validation of cut start values
    validateCutValues();
  }
}

void CameraController::validateCutValues()
{
  MStatus status;
  unsigned int index;
  MFnDependencyNode depNode( thisMObject(), &status );
  if ( status == MS::kSuccess && depNode.typeId() == MTypeId(IGL_CAMERA_CONTROLLER_ID) )
  {
    MPlug inputArrayPlug( depNode.findPlug( s_attr_input, true, &status ) );
    if ( status == MS::kSuccess && m_input.size() > 0 )
    {
      // 
      // try set cut-times based on camera-names (igCam0_100_200_)
      // 
      for(u32 i = 0; i < m_input.size(); i++)
      {
        CameraControllerInput* camera_input = &m_input[i];
        const tchar*           camera_name  = camera_input->m_cameraName.asTChar();
        u32                    camera_id;
        u32                    camera_cut_start;
        u32                    camera_cut_end;

        u32 num = _stscanf(camera_name, TXT("igCam%d_%d_%d_"), &camera_id, &camera_cut_start, &camera_cut_end);

        char str[1024];
        if (num == 3)
        {
          sprintf(str, "CameraController: '%s' - id: %d - start: %d - end: %d", camera_name, camera_id, camera_cut_start, camera_cut_end);
          MGlobal::displayInfo(MString(str));

          camera_input->m_cutStart = camera_cut_start;
        }
        else
        {
          sprintf(str, "CameraController: '%s' name not of required format\n", camera_name);
          MGlobal::displayInfo(MString(str));
        }
      }


      // 
      // validate
      // 
      double first = MAnimControl::animationStartTime().value();
      double last  = MAnimControl::animationEndTime().value();
      V_CameraControllerInput::iterator itr = m_input.begin();
      V_CameraControllerInput::iterator end = m_input.end();
      double prev = first;
      index = 0;
      for ( ; itr!=end; ++itr )
      {
        double cutStart = std::min( std::max( prev, (*itr).m_cutStart ), last);
        (*itr).m_cutStart = cutStart;
        MPlug inputElementPlug( inputArrayPlug.elementByLogicalIndex( index ) );
        MPlug cutStartPlug( inputElementPlug.child( s_attr_i_keys_cutStart ) );
        cutStartPlug.setValue( cutStart );
        prev = (*itr).m_cutStart;
        index++;
      }

      V_CameraControllerInput::reverse_iterator ritr = m_input.rbegin();
      V_CameraControllerInput::reverse_iterator rend = m_input.rend();
      double after = last;
      for ( ; ritr!=rend; ++ritr )
      {
        index--;
        after = (*ritr).m_cutStart;
      }
    }
  }
}


void CameraController::storeOutput( unsigned int index, MDataBlock & dataBlock )
{
  MDataHandle h_output( dataBlock.outputValue( s_attr_output ) );
  // grab all the handles for this output element
  MDataHandle h_translate(   h_output.child( s_attr_o_translate     ) );
  MDataHandle h_rotate(      h_output.child( s_attr_o_rotate        ) );
  MDataHandle h_rotateOrder( h_output.child( s_attr_o_rotateOrder   ) );
  MDataHandle h_rotatePivot( h_output.child( s_attr_o_rotatePivot   ) );
  MDataHandle h_rotatePTran( h_output.child( s_attr_o_rotatePTran   ) );
  MDataHandle h_scalePivot(  h_output.child( s_attr_o_scalePivot    ) );
  MDataHandle h_scalePTran(  h_output.child( s_attr_o_scalePTran    ) );
  MDataHandle h_rotateAxis(  h_output.child( s_attr_o_rotateAxis    ) );
  MDataHandle h_focalLength( h_output.child( s_attr_o_focalLength   ) );
  MDataHandle h_nearClip(    h_output.child( s_attr_o_nearClip      ) );
  MDataHandle h_farClip(     h_output.child( s_attr_o_farClip       ) );
  if ( m_input.size() > index )
  {
    CameraControllerInput * in = &(m_input[index]);

    h_translate.set(   in->m_translate            );
    h_rotate.set(      in->m_rotate               );
    h_rotateOrder.set( (int)in->m_rotateOrder     );
    h_rotatePivot.set( in->m_rotatePivot          );
    h_rotatePTran.set( in->m_rotatePivotTranslate );
    h_scalePivot.set(  in->m_scalePivot           );
    h_scalePTran.set(  in->m_scalePivotTranslate  );
    h_rotateAxis.set(  in->m_rotateAxis           );
    h_focalLength.set( in->m_focalLength          );
    h_nearClip.set(    in->m_nearClipPlane        );
    h_farClip.set(     in->m_farClipPlane         );
    
    MArrayDataHandle h_cameraControlArray( dataBlock.outputArrayValue( s_attr_cameraControl ) );
    unsigned int cameraControlCount = h_cameraControlArray.elementCount();
    for ( unsigned int i=0; i<cameraControlCount; i++, h_cameraControlArray.next() )
    {
      MDataHandle h_cameraControl( h_cameraControlArray.outputValue() );
      h_cameraControl.set( rendersAtThisTime( i ) );
    }
  }
}

bool CameraController::rendersAtThisTime( unsigned int index )
{
  double cutStart, cutEnd;
  unsigned int inputSize = (unsigned int)m_input.size();
  if ( index >= inputSize ) {
    return false;
  }
  MString cameraName( m_input[index].m_cameraName );
  bool renders = false;
  for ( unsigned int i=index; i<inputSize; i++ )
  {
    CameraControllerInput * in = &(m_input[i]);
    if ( cameraName == in->m_cameraName )
    {
      cutStart = in->m_cutStart; 
      if ( i+1 < inputSize )
      {
        cutEnd = m_input[i+1].m_cutStart + EPSILON;
      }
      else
      {
        cutEnd = MAnimControl::animationEndTime().value() + (EPSILON + EPSILON);
      }
      double time = m_time + 0.001;
      if ( time >= cutStart && time < cutEnd )
      {
        renders = true;
      }
    }
  }
  return renders;
}
