#pragma once

#include "Locator.h"


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Cuboid
//
//  written by: Chris Pfeiffer
//
//  Cuboid locator
//
////////////////////////////////////////////////////////////////////////////////////////////////

class Cuboid : public Locator
{
public:
  static const MTypeId s_TypeID;
	static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new Cuboid();}

  bool isBounded() const {return true;}
  MBoundingBox boundingBox() const; 

  void draw (M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status);
};