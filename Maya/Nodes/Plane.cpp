#include "Precompile.h"
#include "Plane.h"

#include "Maya/NodeTypes.h"


const MTypeId Plane::s_TypeID( IGL_PLANE );
const char* Plane::s_TypeName = "igPlane";

MObject Plane::wireframe;
MObject Plane::forceDrawSolid;


MStatus Plane::Initialize()
{ 
  MStatus status;
  MFnNumericAttribute nAttr; 

  // wireframe
  wireframe = nAttr.create( "wireframe", "wf", MFnNumericData::kBoolean );
  nAttr.setDefault( true );
  status = addAttribute( wireframe );
  MCheckErr( status, "Failed to addAttribute: wireframe" );


  // Hidden attributes

  // forceDrawSolid
  forceDrawSolid = nAttr.create( "forceDrawSolid", "forceDrawSolid", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  nAttr.setHidden( true );
  status = addAttribute( forceDrawSolid );
  MCheckErr( status, "Failed to addAttribute: forceDrawSolid" );

  return MS::kSuccess;
}

MBoundingBox Plane::boundingBox() const
{   
  // Get the size
  MPoint corner1( -100.0, 0.0, -100.0 );
  MPoint corner2( 100.0, 0.0, 100.0 );

  return MBoundingBox( corner1, corner2 );
}

void Plane::draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status )
{ 
  view.beginGL(); 
  glPushMatrix();	

  if ( !drawWireframe( style ) )
  {
    glBegin( GL_POLYGON );
  }
  else
  {
    glBegin( GL_LINE_LOOP );
  }

  static GLfloat vertices[][3] =
  {
    {-100.0, 0.0, 100.0},
    {-100.0, 0.0, -100.0},
    {100.0, 0.0, -100.0},
    {100.0, 0.0, 100.0},
  };

  glVertex3fv( vertices[0] );
  glVertex3fv( vertices[1] );
  glVertex3fv( vertices[2] );
  glVertex3fv( vertices[3] );

  glEnd();
  glPopMatrix();
  view.endGL();
}

bool Plane::drawWireframe( M3dView::DisplayStyle style )
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
