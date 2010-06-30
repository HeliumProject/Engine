#pragma once

#include "API.h"

#include "MayaUtils/ErrorHelpers.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DestructionGlue()
//
//  written by: Eric Christensen
//
//  Glue for Destruction Bangles
//
////////////////////////////////////////////////////////////////////////////////////////////////
class MAYA_API DestructionGlue : public MPxLocatorNode
{
public:
  //DestructionGlue();
  static const MTypeId s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new DestructionGlue;}

  ~DestructionGlue();

  virtual void postConstructor();

  bool isBounded() const {return true;}
  MBoundingBox boundingBox() const;

  static MObject wireframe;
  static MObject forceDrawSolid;
  static MObject material_type;
  static MObject elastic_limit_normal;
  static MObject elastic_limit_torsion;
  static MObject plastic_limit_normal;
  static MObject plastic_limit_torsion;
  static MObject bangle_name_a;
  static MObject bangle_name_b;
  static MObject joint_name_a;
  static MObject joint_name_b;
  static MObject joint_a_valid;
  static MObject joint_b_valid;

  MCallbackId  m_cb_id;

  void checkGlErrors( const char* msg );
  void draw (M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status);
  bool drawWireframe( M3dView::DisplayStyle style );
};
