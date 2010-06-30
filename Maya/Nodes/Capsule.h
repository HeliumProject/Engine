#pragma once

#include "Sphere.h"


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Capsule()
//
//  written by: Chris Pfeiffer
//
//  Sort of a complex locator...  It uses both a capsule and a CapsuleChild to 
//  create a dual parented locator node
//
////////////////////////////////////////////////////////////////////////////////////////////////

class Capsule : public Sphere
{
public:
  static const MTypeId	s_TypeID;
  static const char* s_TypeName;

  static MObject s_DeletionSync; 

  static MStatus Initialize();
  static void* Creator() {return new Capsule;}

  void draw (M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status);

  // This attribute is 'special'.  Here we go... A capsule and a CapsuleChild
  // are needed to made a real 'capsule'.  The capsule is the end that draws the 
  // cylinder to the other end (the CapsuleChild).  To known when either side,
  // each represented by a mxplocator node, is deleted are doing the following:
  //
  // 1.  Create a capsule node (collision.mel)
  // 2.  Create a CapsuleChild node (collision.mel)
  // 3.  Create connect the deletion_sync attribute between the capsule and the capsule_child_node (collision.mel)
  // 4.  On the disconnection of the attribute (like when either side is deleted in Maya), delete the other side (in this file)
  //     Because a node cannot delete itself while running its own code, the deletion is performed with an evalDeferred
  //     ALSO, deleting a mpxlocator will leave a transform in the scene; so, we actually delete the transfrom which
  //     automatically deletes its child (the mpxlocator)
  MStatus connectionBroken(const MPlug & plug, const MPlug & otherPlug, bool asSrc);
};


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CapsuleChild()
//
//  written by: Chris Pfeiffer
//
//  Sphere locator, but is used as a child to the primary capsule locator
//
////////////////////////////////////////////////////////////////////////////////////////////////

class CapsuleChild : public Sphere
{
public: 
  static const MTypeId	s_TypeID;
  static const char* s_TypeName;

  static MObject s_DeletionSync;

  static MStatus Initialize();
  static void* Creator() {return new CapsuleChild;}

  // This attribute is 'special'.  Here we go... A capsule and a CapsuleChild
  // are needed to made a real 'capsule'.  The capsule is the end that draws the 
  // cylinder to the other end (the CapsuleChild).  To known when either side,
  // each represented by a mxplocator node, is deleted are doing the following:
  //
  // 1.  Create a capsule node (collision.mel)
  // 2.  Create a CapsuleChild node (collision.mel)
  // 3.  Create connect the deletion_sync attribute between the capsule and the capsule_child_node (collision.mel)
  // 4.  On the disconnection of the attribute (like when either side is deleted in Maya), delete the other side (in this file)
  //     Because a node cannot delete itself while running its own code, the deletion is performed with an evalDeferred
  //     ALSO, deleting a mpxlocator will leave a transform in the scene; so, we actually delete the transfrom which
  //     automatically deletes its child (the mpxlocator)
  MStatus connectionBroken(const MPlug & plug, const MPlug & otherPlug, bool asSrc);
};