#pragma once

#include "API.h"

#include "MayaUtils/ErrorHelpers.h"


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Plane()
//
//  written by: Giac Veltri
//
//  Plane locator
//
////////////////////////////////////////////////////////////////////////////////////////////////

class MAYANODES_API Plane : public MPxLocatorNode
{
public:
  static const MTypeId s_TypeID;
	static const char* s_TypeName;
  
  static MObject wireframe; 
  static MObject forceDrawSolid;

  static MStatus Initialize();
  static void* Creator() {return new Plane;}

  bool isBounded() const {return true;}
  MBoundingBox boundingBox() const; 

  void draw ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status );

  bool drawWireframe( M3dView::DisplayStyle style );
};
