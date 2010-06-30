#include "Precompile.h"
#include "Cuboid.h"

#include "MayaUtils/NodeTypes.h"

const MTypeId Cuboid::s_TypeID( IGL_CUBOID );
const char* Cuboid::s_TypeName = "cuboid";

MStatus Cuboid::Initialize()
{
  return inheritAttributesFrom( Locator::s_TypeName );
}

MBoundingBox Cuboid::boundingBox() const
{   
  // Get the size
  MPoint corner1( -100.0, -100.0, -100.0 );
  MPoint corner2( 100.0, 100.0, 100.0 );

  return MBoundingBox( corner1, corner2 );
}

static void face4( int v0, int v1, int v2, int v3, bool solid )
{
  if( solid )
  {
    glBegin( GL_POLYGON );
  }
  else
  {
    glBegin( GL_LINE_LOOP );
  }

  const static GLfloat vertices[][3] =
  {
    {100.0, 100.0, 100.0},
    {-100.0, 100.0, 100.0},
    {-100.0, -100.0, 100.0},
    {100.0, -100.0, 100.0},
    {100.0, 100.0, -100.0},
    {-100.0, 100.0, -100.0},
    {-100.0, -100.0, -100.0},
    {100.0, -100.0, -100.0}
  };

  glVertex3fv( vertices[v0] );
  glVertex3fv( vertices[v1] );
  glVertex3fv( vertices[v2] );
  glVertex3fv( vertices[v3] );

  glEnd();
}

void Cuboid::draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status )
{
  Locator::draw( view, path, style, status );

  view.beginGL(); 

  glPushMatrix();	

  bool solid = !drawWireframe( style );


  // Render the face which has 4 vertices with indices v1, v2, v3, v4
  face4( 0, 1, 2, 3, solid );
  face4( 0, 3, 7, 4, solid );
  face4( 4, 7, 6, 5, solid );
  face4( 1, 5, 6, 2, solid );
  face4( 0, 4, 5, 1, solid );
  face4( 2, 6, 7, 3, solid );   

  glPopMatrix();

  view.endGL();
}