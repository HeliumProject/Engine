#pragma once

#include "stdafx.h"

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>

#include "CameraController.h"

//-----------------------------------------------------------------------------
// class CameraControllerUICmd
//-----------------------------------------------------------------------------
class CameraControllerUICmd : public MPxCommand
{
public:
  static MString      CommandName;
  static MString      s_ui_layoutName;
  static MString      s_ui_cameraPrefix;
  static MString      s_ui_firstFramePrefix;
  static MString      s_ui_headPrefix;
  static MString      s_ui_cutPrefix;
  static MString      s_ui_tailPrefix;
  static MString      s_ui_lastFramePrefix;
  static MString      s_ui_upPrefix;
  static MString      s_ui_swapPrefix;
  static MString      s_ui_deletePrefix;
  static const char * s_flag_long_node;
  static const char * s_flag_short_node;
  static const char * s_flag_long_forceUpdate;
  static const char * s_flag_short_forceUpdate;
  static const char * s_flag_long_inputCamera;
  static const char * s_flag_short_inputCamera;
  static const char * s_flag_long_head;
  static const char * s_flag_short_head;
  static const char * s_flag_long_tail;
  static const char * s_flag_short_tail;
  static const char * s_flag_long_cut;
  static const char * s_flag_short_cut;
  static const char * s_flag_long_up;
  static const char * s_flag_short_up;
  static const char * s_flag_long_swap;
  static const char * s_flag_short_swap;
  static const char * s_flag_long_delete;
  static const char * s_flag_short_delete;
  static const char * s_flag_long_cameraIndex;
  static const char * s_flag_short_cameraIndex;
  static const char * s_flag_long_createCinematicCamera;
  static const char * s_flag_short_createCinematicCamera;
      
  static void *  creator()        { return new CameraControllerUICmd; }
  static MSyntax newSyntax();

  static void rememberNode( MObject & node );
  static void forgetNode();

  MStatus doIt( const MArgList & args );
  virtual bool isUndoable() const { return false; }
  virtual bool hasSyntax() const  { return true; }

  static bool isUIActive( bool autoActivate );

protected:
  enum UpdateType
  {
    kUpdateDefault,
    kSelectCamera,
    kUpdateHead,
    kUpdateTail,
    kUpdateCut,
    kUpdateUp,
    kUpdateSwap,
    kUpdateDelete,
    kCameraIndex,
    kUpdateCreate,
  };
  struct AttributeEditorState
  {
    MString      m_parentLayout;
    MObject      m_node;
    unsigned int m_rowCount;
  };
  static AttributeEditorState s_state;
  MObject      m_node;
  bool         m_forceUpdate;
  UpdateType   m_updateType;
  unsigned int m_updateRow;
  MString      m_queryCamera;

  bool selectExistingCinematicCamera();
  void createCinematicCamera();

  void eraseUI();
  void buildUI( CameraController * controller );
  void updateUI( CameraController * controller );

  bool parseArgs( const MArgList & args );

  CameraController * getController();

  void selectCamera   ( unsigned int row );
  void updateCut      ( unsigned int row );
  void updateUp       ( unsigned int row );
  void updateSwap     ( unsigned int row );
  void updateDelete   ( unsigned int row );
  void cameraIndex    ( const MString & queryCamera );
};




