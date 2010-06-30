#pragma once

#include "API.h"

#include <maya/MGlobal.h>
#include <maya/MPxNode.h>
#include <maya/MNodeMessage.h>
#include <maya/MVector.h>

#include <vector>

struct CameraControllerInput
{
  MString      m_cameraName;
  double       m_cutStart;
  MVector      m_translate;
  MVector      m_rotate;
  unsigned int m_rotateOrder;
  MVector      m_scale;
  MVector      m_shear;
  MVector      m_rotatePivot;
  MVector      m_rotatePivotTranslate;
  MVector      m_scalePivot;
  MVector      m_scalePivotTranslate;
  MVector      m_rotateAxis;
  double       m_focalLength;
  double       m_nearClipPlane;
  double       m_farClipPlane;
};

typedef std::vector<CameraControllerInput> V_CameraControllerInput;

class MAYA_API CameraController : public MPxNode
{
public:
  static MTypeId s_TypeID;
  static MString s_TypeName;

  // attributes of an CameraController
  static MObject s_attr_lock;
  static MObject s_attr_inTime;
  static MObject s_attr_input;
  static MObject s_attr_i_keys;
  static MObject s_attr_i_keys_cutStart;
  static MObject s_attr_i_worldMatrix;
  static MObject s_attr_i_focalLength;
  static MObject s_attr_i_nearClip;
  static MObject s_attr_i_farClip;

  static MObject s_attr_cameraControl;

  static MObject s_attr_output;
  static MObject s_attr_o_translateX;
  static MObject s_attr_o_translateY;
  static MObject s_attr_o_translateZ;
  static MObject s_attr_o_translate;
  static MObject s_attr_o_rotateX;
  static MObject s_attr_o_rotateY;
  static MObject s_attr_o_rotateZ;
  static MObject s_attr_o_rotate;
  static MObject s_attr_o_rotateOrder;
  static MObject s_attr_o_scaleX;
  static MObject s_attr_o_scaleY;
  static MObject s_attr_o_scaleZ;
  static MObject s_attr_o_scale;
  static MObject s_attr_o_shearXY;
  static MObject s_attr_o_shearXZ;
  static MObject s_attr_o_shearYZ;
  static MObject s_attr_o_shear;
  static MObject s_attr_o_rotatePivotX;
  static MObject s_attr_o_rotatePivotY;
  static MObject s_attr_o_rotatePivotZ;
  static MObject s_attr_o_rotatePivot;
  static MObject s_attr_o_rotatePTranX;
  static MObject s_attr_o_rotatePTranY;
  static MObject s_attr_o_rotatePTranZ;
  static MObject s_attr_o_rotatePTran;
  static MObject s_attr_o_scalePivotX;
  static MObject s_attr_o_scalePivotY;
  static MObject s_attr_o_scalePivotZ;
  static MObject s_attr_o_scalePivot;
  static MObject s_attr_o_scalePTranX;
  static MObject s_attr_o_scalePTranY;
  static MObject s_attr_o_scalePTranZ;
  static MObject s_attr_o_scalePTran;
  static MObject s_attr_o_rotateAxisX;
  static MObject s_attr_o_rotateAxisY;
  static MObject s_attr_o_rotateAxisZ;
  static MObject s_attr_o_rotateAxis;
  static MObject s_attr_o_focalLength;
  static MObject s_attr_o_nearClip;
  static MObject s_attr_o_farClip;

  static MObject s_attr_guid;

  // find a CameraController if one already exists
  static MObject existingController();

  // create an instance of an CameraController
  static void *  Creator();

  // initialize the node type
  static MStatus Initialize();

  // various callbacks to support computing at the right moments
  static void    timeChangeCallback( MTime & time, void * clientData );
  static void    animationTimeRangeChangedCallback(void* node);
  static void    beforeSaveCallback(void * node );
  static void    afterSaveCallback(void * node );
  static void    nodeRemovedCallback( MObject & node, void * clientData );

  // constructor to initialize member data
  CameraController();
  // destructor to de-register any callbacks
  virtual ~CameraController();

  // a hook to allow callbacks to be registered for this node
  virtual void    postConstructor();

  // the dependency graph compute
  virtual MStatus compute( const MPlug & plug, MDataBlock & dataBlock );

  unsigned int currentIndex();

  // returns the number of cameras connected and also causes plugs to be re-read
  unsigned int cameraCount();

  void cameraName( unsigned int index, MString & name );
  int  firstIndex( const MString & cameraName );
  void outputCameraName( MString & name );

  void setLock( bool lock );
  bool isLocked()   { return m_lock; }

  double getFirstFrame( unsigned int index );
  double getLastFrame(  unsigned int index );

  double getCutStart( unsigned int index );
  double getCutFinal( unsigned int index );

  void setCutStart( unsigned int index, double value );

protected:
  MCallbackId             m_animationTimeRangeChangedCallback;
  MCallbackId             m_beforeSaveCallback;
  MCallbackId             m_afterSaveCallback;
  MCallbackId             m_nodeRemovedCallback;
  MTime                   m_temporaryMinTime;
  MTime                   m_temporaryMaxTime;
  bool                    m_lock;
  double                  m_time;
  V_CameraControllerInput m_input;

  static void updateUI();
  void loadInput( MDataBlock & dataBlock );
  void validateCutValues();
  void storeOutput( unsigned int index, MDataBlock & dataBlock );
  bool rendersAtThisTime( unsigned int index );
};
