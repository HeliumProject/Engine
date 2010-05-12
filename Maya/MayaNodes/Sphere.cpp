#include "stdafx.h"
#include "Sphere.h"

#include "MayaUtils/NodeTypes.h"

const MTypeId Sphere::s_TypeID( IGL_SPHERE );
const char* Sphere::s_TypeName = "sphere";

MStatus Sphere::Initialize()
{ 
  return inheritAttributesFrom( Locator::s_TypeName );
}

MBoundingBox Sphere::boundingBox() const
{   
  // Get the size
  MPoint corner1( -100.0, -100.0, -100.0 );
  MPoint corner2( 100.0, 100.0, 100.0 );

  return MBoundingBox( corner1, corner2 );
}

void Sphere::draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status )
{ 
  Locator::draw( view, path, style, status );

  view.beginGL();

  checkGlErrors( "Problem before drawing sphere" );

  double ix = 1.0, iy = 1.0, iz = 1.0;
  MDistance convert_to_centimeters;
  double r = 100.0f;  // 100 Maya internal units - centimeters

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

  checkGlErrors( "Problem drawing sphere" );

  view.endGL();
}
