#pragma once

#include "Locator.h"


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Sphere()
//
//  written by: Chris Pfeiffer
//
//  Sphere locator
//
////////////////////////////////////////////////////////////////////////////////////////////////

class Sphere : public Locator
{
public:
  static const MTypeId s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new Sphere;}

  bool isBounded() const {return true;}
  MBoundingBox boundingBox() const; 

  void draw (M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status);
};
