#pragma once

#include "API.h"

#include "MayaUtils/ErrorHelpers.h"

class MAYANODES_API Locator : public MPxLocatorNode
{
public:
  static const MTypeId s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new Locator;}

  static MObject wireframe; 
  static MObject materialindex;
  static MObject bangleindex;
  static MObject density;

  static MObject forceDrawSolid;

  //prim flags
  static MObject primLowRez;
  static MObject primMove;
  static MObject primProjectile;
  static MObject primPhysics;
  static MObject primGround;
  static MObject primEffects;
  static MObject primNoMapping;
  static MObject primNoColl;
  static MObject primUnUsed1; //legacy, will map to physics
  static MObject primUnUsed2; //legacy, will map to ground
  static MObject primUnUsed3; //legacy, will map to effects
  static MObject numParticles;
  static MObject particleSpeedFactor;
  static MObject primContributesToBSphere;

  void checkGlErrors( const char* msg );
  void draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status );

  bool drawWireframe( M3dView::DisplayStyle style );
};
