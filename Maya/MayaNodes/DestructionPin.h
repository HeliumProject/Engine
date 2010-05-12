#pragma once

#include "API.h"

#include "MayaUtils/ErrorHelpers.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DestructionPin()
//
//  written by: Eric Christensen
//
//  Pins for Destruction Bangles
//
////////////////////////////////////////////////////////////////////////////////////////////////
class MAYANODES_API DestructionPin : public MPxLocatorNode
{
public:
  //DestructionGlue();
  static const MTypeId s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new DestructionPin;}

  bool isBounded() const {return true;}
  MBoundingBox boundingBox() const;

  static MObject wireframe;
  static MObject forceDrawSolid;
  static MObject joint_name;

  void checkGlErrors( const char* msg );
  void draw (M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status);
  bool drawWireframe( M3dView::DisplayStyle style );
};
