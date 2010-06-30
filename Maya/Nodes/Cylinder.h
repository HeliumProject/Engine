#pragma once

#include "Locator.h"

class Cylinder : public Locator
{
public:
  static const MTypeId s_TypeID;
	static const char* s_TypeName;

  static MObject s_DeletionSync; 

  static MStatus Initialize();
  static void* Creator() {return new Cylinder;}

  bool isBounded() const {return true;}
  MBoundingBox boundingBox() const; 
  void draw (M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status);

  MStatus connectionBroken(const MPlug & plug, const MPlug & otherPlug, bool asSrc);
};

class CylinderChild : public Locator
{
public: 
  static const MTypeId s_TypeID;
	static const char* s_TypeName;

  static MObject s_DeletionSync;

  static MStatus Initialize();
  static void* Creator() { return new CylinderChild; }

  MStatus connectionBroken( const MPlug & plug, const MPlug & otherPlug, bool asSrc );
};