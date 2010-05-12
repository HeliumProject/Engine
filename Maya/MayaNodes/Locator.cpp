#include "stdafx.h"
#include "Locator.h"
#include "Common/Assert.h" 

#include <cmath>

#include "MayaUtils/NodeTypes.h"

static GLfloat vertices[][3] =
{
  {-10.0, 0.0, 0.0},
  {10.0, 0.0, 0.0},
  {0.0, -10.0, 0.0},
  {0.0, 10.0, 0.0},
  {0.0, 0.0, -10.0},
  {0.0, 0.0, 10.0},
};

const MTypeId Locator::s_TypeID( IGL_COLL_VOLUME );
const char* Locator::s_TypeName = "igShape";

MObject Locator::wireframe;
MObject Locator::materialindex;
MObject Locator::bangleindex;
MObject Locator::density;

MObject Locator::forceDrawSolid;

//Prim falgs
MObject Locator::primLowRez;
MObject Locator::primMove;
MObject Locator::primProjectile;
MObject Locator::primPhysics;
MObject Locator::primGround;
MObject Locator::primEffects;
MObject Locator::primNoMapping;
MObject Locator::primNoColl;

//num particles for moby particalization
MObject Locator::numParticles;
MObject Locator::particleSpeedFactor;
MObject Locator::primContributesToBSphere;

MObject Locator::primUnUsed1; //legacy, will map to physics
MObject Locator::primUnUsed2; //legacy, will map to ground
MObject Locator::primUnUsed3; //legacy, will map to effects

MStatus Locator::Initialize()
{
  MStatus stat;
  MFnNumericAttribute nAttr; 

  // wireframe
  wireframe = nAttr.create( "wireframe", "wf", MFnNumericData::kBoolean );
  nAttr.setDefault( true );
  stat = addAttribute( wireframe );
  MCheckErr( stat, "Failed to addAttribute: wireframe" );

  //Material index
  materialindex = nAttr.create( "Physical_Material_Index", "mi", MFnNumericData::kInt );
  nAttr.setDefault( -1 );
  stat = addAttribute( materialindex );
  MCheckErr( stat, "Failed to addAttribute: Physical_Material_Index" );

  //group index
  bangleindex = nAttr.create( "Bangle_Index", "bi", MFnNumericData::kInt );
  nAttr.setDefault( -1 );
  stat = addAttribute( bangleindex );
  MCheckErr( stat, "Failed to addAttribute: Bangle_Index" );

  //density
  density = nAttr.create( "Density", "dty", MFnNumericData::kDouble );
  nAttr.setDefault( 1.0 );
  stat = addAttribute( density );
  MCheckErr( stat, "Failed to addAttribute: Density" );

  //prim flags
  primLowRez = nAttr.create( "primLowRez", "pl", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primLowRez );
  MCheckErr( stat, "Failed to addAttribute: primLowRez" );

  primMove = nAttr.create( "primMove", "pmv", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primMove );
  MCheckErr( stat, "Failed to addAttribute: primMove" );

  primProjectile = nAttr.create( "primProjectile", "pp", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primProjectile );
  MCheckErr( stat, "Failed to addAttribute: primProjectile" );

  primPhysics = nAttr.create( "primPhysics", "pph", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primPhysics );
  MCheckErr( stat, "Failed to addAttribute: primPhysics" );

  primGround = nAttr.create( "primGround", "pg", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primGround );
  MCheckErr( stat, "Failed to addAttribute: primGround" );

  primEffects = nAttr.create( "primEffects", "pe", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primEffects );
  MCheckErr( stat, "Failed to addAttribute: primEffects" );

  primNoMapping = nAttr.create( "primNoMapping", "pnm", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primNoMapping );
  MCheckErr( stat, "Failed to addAttribute: primNoMapping" );

  primNoColl = nAttr.create( "primNoColl", "pnc", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primNoColl );
  MCheckErr( stat, "Failed to addAttribute: primNoColl" );

  //group index
  numParticles = nAttr.create( "NumParticles", "np", MFnNumericData::kInt );
  nAttr.setDefault( 0 );
  stat = addAttribute( numParticles );
  MCheckErr( stat, "Failed to addAttribute: NumParticles" );

  particleSpeedFactor = nAttr.create( "ParticlesSpeedFactor", "psf", MFnNumericData::kDouble );
  nAttr.setDefault( 1.0 );
  stat = addAttribute( particleSpeedFactor );
  MCheckErr( stat, "Failed to addAttribute: ParticlesSpeedFactor" );

  primContributesToBSphere = nAttr.create( "primInBSphere", "pib", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primContributesToBSphere );
  MCheckErr( stat, "Failed to addAttribute: primInBSphere" );

#pragma TODO( "Get rid of these legacy attributes ( unused 1, 2, 3 )" )
  primUnUsed1 = nAttr.create( "primUnUsed1", "pu1", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primUnUsed1 );
  MCheckErr( stat, "Failed to addAttribute: primUnUsed1" );

  primUnUsed2 = nAttr.create( "primUnUsed2", "pu2", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primUnUsed2 );
  MCheckErr( stat, "Failed to addAttribute: primUnUsed2" );

  primUnUsed3 = nAttr.create( "primUnUsed3", "pu3", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  stat = addAttribute( primUnUsed3 );
  MCheckErr( stat, "Failed to addAttribute: primUnUsed3" );


  // Hidden attributes

  // forceDrawSolid
  forceDrawSolid = nAttr.create( "forceDrawSolid", "forceDrawSolid", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  nAttr.setHidden( true );
  stat = addAttribute( forceDrawSolid );
  MCheckErr( stat, "Failed to addAttribute: forceDrawSolid" );

  return MS::kSuccess;
}


bool Locator::drawWireframe( M3dView::DisplayStyle style )
{
  MPlug plug( thisMObject(), forceDrawSolid );
  bool forceDrawSolidValue = false;
  plug.getValue( forceDrawSolidValue );

  if ( forceDrawSolidValue )
  {
    return false;
  }

  MPlug wireFramePlug( thisMObject(), wireframe );
  bool wireFrameValue = false;
  wireFramePlug.getValue( wireFrameValue );

  return ( wireFrameValue || ( style != M3dView::kFlatShaded && style != M3dView::kGouraudShaded ) );
}

void Locator::draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status )
{ 
  view.beginGL(); 

  checkGlErrors( "Problem before drawing volume" );

  glPushMatrix();
  checkGlErrors( "Locator: Problem pushing" );

  GLfloat matrix[4][4];
  glGetFloatv( GL_MODELVIEW_MATRIX, ( GLfloat* )matrix );
  checkGlErrors( "Locator: Getting model view matrix" );

  GLfloat xl = sqrt( matrix[0][0]*matrix[0][0] + matrix[0][1]*matrix[0][1] + matrix[0][2]*matrix[0][2] );
  GLfloat yl = sqrt( matrix[1][0]*matrix[1][0] + matrix[1][1]*matrix[1][1] + matrix[1][2]*matrix[1][2] );
  GLfloat zl = sqrt( matrix[2][0]*matrix[2][0] + matrix[2][1]*matrix[2][1] + matrix[2][2]*matrix[2][2] );

  matrix[0][0] = matrix[0][0] / xl;
  matrix[0][1] = matrix[0][1] / xl;
  matrix[0][2] = matrix[0][2] / xl;

  matrix[1][0] = matrix[1][0] / yl;
  matrix[1][1] = matrix[1][1] / yl;
  matrix[1][2] = matrix[1][2] / yl;

  matrix[2][0] = matrix[2][0] / zl;
  matrix[2][1] = matrix[2][1] / zl;
  matrix[2][2] = matrix[2][2] / zl;

  glLoadMatrixf( ( GLfloat* )matrix );
  checkGlErrors( "Locator: Loading matrix" );

  glBegin( GL_LINES );

  glVertex3fv( vertices[0] );
  glVertex3fv( vertices[1] );
  glVertex3fv( vertices[2] );
  glVertex3fv( vertices[3] );
  glVertex3fv( vertices[4] );
  glVertex3fv( vertices[5] );

  glEnd();
  checkGlErrors( "Locator: Drawing Lines" );

  glPopMatrix();

  checkGlErrors( "Problem drawing volume" );

  view.endGL();
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  checkGlErrors
//
//  written by: Chris Pfeiffer
//
//  Checks for OpenGL errors
//
////////////////////////////////////////////////////////////////////////////////////////////////
void Locator::checkGlErrors( const char* msg )
{
  char temp[256] = "";


  GLenum err;
  bool errors = false;

  while ( ( err = glGetError() ) != GL_NO_ERROR )
  {
    if ( !errors )
    {
      // Print this the first time through the loop
      //
      sprintf( temp, "OpenGL errors: %s", msg );
      MGlobal::displayError( temp );
    }

    errors = true;

#define MYERR( n )	case n: sprintf( temp, "%s ", #n ); break
    switch ( err )
    {
      MYERR( GL_INVALID_ENUM );
      MYERR( GL_INVALID_VALUE );
      MYERR( GL_INVALID_OPERATION );
      MYERR( GL_STACK_OVERFLOW );
      MYERR( GL_STACK_UNDERFLOW );
      MYERR( GL_OUT_OF_MEMORY );
    default:
      {
        sprintf( temp, "GL ERROR: 0x%.4x", err );
      }
    }
    MGlobal::displayError( temp );
  }
#undef MYERR
}
