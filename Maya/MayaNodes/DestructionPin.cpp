#include "stdafx.h"
#include "DestructionPin.h"
#include "Common/Assert.h" 

#include <cmath>

#include "MayaUtils/NodeTypes.h"

#include <maya/MFnStringData.h>



const MTypeId DestructionPin::s_TypeID( IGL_DESTRUCTION_PIN );
const char* DestructionPin::s_TypeName = "igDestructionPin";


MObject DestructionPin::wireframe;
MObject DestructionPin::forceDrawSolid;

MObject DestructionPin::joint_name;

MStatus DestructionPin::Initialize()
{ 
  MStatus stat;
  MFnNumericAttribute nAttr; 
  MFnTypedAttribute tAttr;


  // wireframe
  wireframe = nAttr.create( "wireframe", "wf", MFnNumericData::kBoolean );
  nAttr.setDefault( true );
  stat = addAttribute( wireframe );
  MCheckErr( stat, "Failed to addAttribute: wireframe" );

  MObject       defaultString;
  MFnStringData fnStringData;
  defaultString = fnStringData.create( "None Selected" );

  joint_name = tAttr.create( "Joint_Name", "jname", MFnData::kString );
  tAttr.setDefault(defaultString);
  tAttr.setHidden(true);
  stat = addAttribute(joint_name);
  MCheckErr( stat, "Failed to addAttribute: Joint_Name" );


  // Hidden attributes

  // forceDrawSolid
  forceDrawSolid = nAttr.create( "forceDrawSolid", "forceDrawSolid", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  nAttr.setHidden( true );
  stat = addAttribute( forceDrawSolid );
  MCheckErr( stat, "Failed to addAttribute: forceDrawSolid" );


  return MS::kSuccess;
}

bool DestructionPin::drawWireframe( M3dView::DisplayStyle style )
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

MBoundingBox DestructionPin::boundingBox() const
{   
  // Get the size
  MPoint corner1( -1.0, -1.0, -1.0 );
  MPoint corner2( 1.0, 1.0, 1.0 );

  return MBoundingBox( corner1, corner2 );
}

void DestructionPin::draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status )
{ 
  view.beginGL();

  checkGlErrors( "Problem before drawing destruction pin" );

  double ix = 10.0, iy = 10.0, iz = 10.0;
  MDistance convert_to_centimeters;
  double r = 10.0f;  // 100 Maya internal units - centimeters

  glPushMatrix();	
  glScaled( ix, iy, iz );

  GLUquadricObj *q = gluNewQuadric();
  if ( !drawWireframe( style ) )
  {
    gluQuadricDrawStyle( q, GLU_FILL );    
  }
  else
  {
    gluQuadricDrawStyle( q, GLU_LINE );
  }

  glTranslatef( 0, 0, 0 );    
  gluSphere( q, r, 10, 10 );    

  gluDeleteQuadric( q );
  glPopMatrix();

  checkGlErrors( "Problem drawing destruction pin" );

  view.endGL();
}

void DestructionPin::checkGlErrors( const char* msg )
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
