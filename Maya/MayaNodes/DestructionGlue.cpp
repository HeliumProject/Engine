#include "stdafx.h"
#include "DestructionGlue.h"
#include "Platform/Assert.h" 

#include <cmath>

#include "MayaUtils/NodeTypes.h"

// Maya includes
#include <maya/MImage.h>
#include <maya/MNodeMessage.h>
#include <maya/MImageFileInfo.h>
#include <maya/MGLFunctionTable.h>
#include <maya/MGLdefinitions.h>
#include <maya/MHardwareRenderer.h>
#include <maya/MFnStringData.h>
#include <maya/MGlobal.h>


static double destructDefMatELN[] = 
{
  0.01,
  0.02,
  0.1,
  0.001,
  0.25,
  0.05,
};

static double destructDefMatELT[] = 
{
  0.02,
  0.01,
  0.001,
  0.2,
  0.25,
  0.05
};

static double destructDefMatPLN[] =
{
  0.03,
  0.01,
  0.01,
  0.03,
  0.05,
  0.3
};

static double destructDefMatPLT[] =
{
  0.04,
  0.04,
  0.01,
  0.03,
  0.1,
  0.7
};

enum DestructionGlueMaterialTypes
{
  DGMT_BRICK,
  DGMT_CONCRETE,
  DGMT_PLASTER,
  DGMT_GLASS,
  DGMT_WOOD,
  DGMT_METAL,
  DGMT_NUM_TYPES,
  DGMT_CUSTOM = DGMT_NUM_TYPES,
};


const MTypeId DestructionGlue::s_TypeID( IGL_DESTRUCTION_GLUE );
const char* DestructionGlue::s_TypeName = "igDestructionGlue";

MObject DestructionGlue::elastic_limit_normal;
MObject DestructionGlue::elastic_limit_torsion;
MObject DestructionGlue::plastic_limit_normal;
MObject DestructionGlue::plastic_limit_torsion;

MObject DestructionGlue::wireframe;
MObject DestructionGlue::forceDrawSolid;

MObject DestructionGlue::material_type;

MObject DestructionGlue::bangle_name_a;
MObject DestructionGlue::bangle_name_b;

MObject DestructionGlue::joint_name_a;
MObject DestructionGlue::joint_name_b;

MObject DestructionGlue::joint_a_valid;
MObject DestructionGlue::joint_b_valid;

static void AttrChangedCB(MNodeMessage::AttributeMessage msg, MPlug & plug, MPlug & otherPlug, void* vpClientData)
{
  MStatus stat;

  MObject modified_node = plug.node();
  MFnDependencyNode nodeFn( modified_node );
  DestructionGlue* destruction_glue = static_cast< DestructionGlue * >( nodeFn.userNode( &stat ) );
  if( !stat )
  {
    printf( "something went horribly wrong handling attribute change for DestructionGlue\n" );
    return;
  }

  // If an attribute value changes
  if ( msg & MNodeMessage::kAttributeSet ) 
  {
    MFnDependencyNode nodeFn( modified_node );

    if( plug.attribute() == DestructionGlue::material_type )
    {
      // material type has changed
      // check to see if it is type 6 which is custom
      int mat_type;
      plug.getValue(mat_type);

      if(mat_type < DGMT_NUM_TYPES) // not custom, so set the defaults for the material type
      {
        MMessage::removeCallback( destruction_glue->m_cb_id );

        MPlug elnPlug( modified_node, DestructionGlue::elastic_limit_normal);
        MPlug eltPlug( modified_node, DestructionGlue::elastic_limit_torsion);
        MPlug plnPlug( modified_node, DestructionGlue::plastic_limit_normal);
        MPlug pltPlug( modified_node, DestructionGlue::plastic_limit_torsion);

        elnPlug.setValue(destructDefMatELN[mat_type]);
        eltPlug.setValue(destructDefMatELT[mat_type]);
        plnPlug.setValue(destructDefMatPLN[mat_type]);
        pltPlug.setValue(destructDefMatPLT[mat_type]);

        destruction_glue->m_cb_id = 
          MNodeMessage::addAttributeChangedCallback( modified_node , AttrChangedCB, NULL, &stat);

      }
      else // trying to set custom, this is really crap.. but we'll try to match values and set the appropriate
           // material type
      {
        MMessage::removeCallback( destruction_glue->m_cb_id );

        MPlug elnPlug( modified_node, DestructionGlue::elastic_limit_normal);
        MPlug eltPlug( modified_node, DestructionGlue::elastic_limit_torsion);
        MPlug plnPlug( modified_node, DestructionGlue::plastic_limit_normal);
        MPlug pltPlug( modified_node, DestructionGlue::plastic_limit_torsion);

        double eln_val;
        double elt_val;
        double pln_val;
        double plt_val;

        elnPlug.getValue(eln_val);
        eltPlug.getValue(elt_val);
        plnPlug.getValue(pln_val);
        pltPlug.getValue(plt_val);

        for(int x=0;x<DGMT_NUM_TYPES;x++)
        {
          double eln_test = destructDefMatELN[x];
          double elt_test = destructDefMatELT[x];
          double pln_test = destructDefMatPLN[x];
          double plt_test = destructDefMatPLT[x];

          if(eln_val == eln_test &&
             elt_val == elt_test &&
             pln_val == pln_test &&
             plt_val == plt_test)
          {
            plug.setValue(x);
            break;
          }

        }

        destruction_glue->m_cb_id = 
          MNodeMessage::addAttributeChangedCallback( modified_node , AttrChangedCB, NULL, &stat);
      }
    }
    else if (plug.attribute() == DestructionGlue::elastic_limit_normal ||
             plug.attribute() == DestructionGlue::elastic_limit_torsion ||
             plug.attribute() == DestructionGlue::plastic_limit_normal ||
             plug.attribute() == DestructionGlue::plastic_limit_torsion)
    {
        MMessage::removeCallback( destruction_glue->m_cb_id );

        double limit_val;
        plug.getValue(limit_val);
        if(limit_val < 0.0)
        {
          char warning_message[256] = "";
          sprintf(warning_message, 
            "%f is an invalid value. limit values must be positive. setting to 0.0",limit_val);
          MGlobal::displayWarning(warning_message);
          plug.setValue(0.0);
        }


        MPlug matTypePlug( modified_node, DestructionGlue::material_type);

        matTypePlug.setValue(DGMT_CUSTOM);

        destruction_glue->m_cb_id = 
          MNodeMessage::addAttributeChangedCallback( modified_node , AttrChangedCB, NULL, &stat);
    }
  }

}


void DestructionGlue::postConstructor ()
{
  MStatus stat;
  MObject moNode = this->thisMObject();

  m_cb_id = MNodeMessage::addAttributeChangedCallback(moNode, AttrChangedCB, NULL, &stat);
}

DestructionGlue::~DestructionGlue()
{
  if( m_cb_id != 0 )
  {
    MMessage::removeCallback( m_cb_id );
  }
}

MStatus DestructionGlue::Initialize()
{ 
  MStatus stat;
  MFnNumericAttribute nAttr; 
  MFnTypedAttribute tAttr;
  MFnEnumAttribute eAttr;


  // wireframe
  wireframe = nAttr.create( "wireframe", "wf", MFnNumericData::kBoolean );
  nAttr.setDefault( true );
  stat = addAttribute( wireframe );
  MCheckErr( stat, "Failed to addAttribute: wireframe" );

  material_type = eAttr.create( "Material_Type", "mtrl" );
  eAttr.addField( "Brick", 0 );
  eAttr.addField( "Concrete", 1 );
  eAttr.addField( "Plaster", 2 );
  eAttr.addField( "Glass", 3 );
  eAttr.addField( "Wood", 4 );
  eAttr.addField( "Metal", 5 );
  eAttr.addField( "Custom", 6 );
  eAttr.setWritable( true );
  stat = addAttribute( material_type );
  MCheckErr( stat, "Failed to addAtttribute: Material_Type" );

  elastic_limit_normal = nAttr.create( "Elastic_Limit_Normal", "eln", MFnNumericData::kDouble);
  nAttr.setDefault( 1.0 );
  stat = addAttribute( elastic_limit_normal );
  MCheckErr( stat, "Failed to addAttribute: Elastic_Limit_Normal" );

  elastic_limit_torsion = nAttr.create( "Elastic_Limit_Torsion", "elt", MFnNumericData::kDouble);
  nAttr.setDefault( 1.0 );
  stat = addAttribute( elastic_limit_torsion );
  MCheckErr( stat, "Failed to addAttribute: Elastic_Limit_Torsion" );

  plastic_limit_normal = nAttr.create( "Plastic_Limit_Normal", "pln", MFnNumericData::kDouble);
  nAttr.setDefault( 1.0 );
  stat = addAttribute( plastic_limit_normal );
  MCheckErr( stat, "Failed to addAttribute: Plastic_Limit_Normal" );

  plastic_limit_torsion = nAttr.create( "Plastic_Limit_Torsion", "plt", MFnNumericData::kDouble);
  nAttr.setDefault( 1.0 );
  stat = addAttribute( plastic_limit_torsion );
  MCheckErr( stat, "Failed to addAttribute: Plastic_Limit_Torsion" );


  MObject       defaultString;
  MFnStringData fnStringData;
  defaultString = fnStringData.create( "None Selected" );

  bangle_name_a = tAttr.create( "Bangle_Name_A", "bna", MFnData::kString );
  tAttr.setDefault(defaultString);
  tAttr.setHidden(true);
  stat = addAttribute(bangle_name_a);
  MCheckErr( stat, "Failed to addAttribute: Bangle_Name_A" );

  bangle_name_b = tAttr.create( "Bangle_Name_B", "bnb", MFnData::kString );
  tAttr.setDefault(defaultString);
  tAttr.setHidden(true);
  stat = addAttribute(bangle_name_b);
  MCheckErr( stat, "Failed to addAttribute: Bangle_Name_B" );

  joint_name_a = tAttr.create( "Joint_Name_A", "jna", MFnData::kString );
  tAttr.setDefault(defaultString);
  tAttr.setHidden(true);
  stat = addAttribute(joint_name_a);
  MCheckErr( stat, "Failed to addAttribute: Joint_Name_A" );

  joint_name_b = tAttr.create( "Joint_Name_B", "jnb", MFnData::kString );
  tAttr.setDefault(defaultString);
  tAttr.setHidden(true);
  stat = addAttribute(joint_name_b);
  MCheckErr( stat, "Failed to addAttribute: Joint_Name_B" );

  joint_a_valid = nAttr.create( "Joint_A_Valid", "jav", MFnNumericData::kBoolean );
  nAttr.setDefault( true );
  nAttr.setHidden( true );
  stat = addAttribute( joint_a_valid );
  MCheckErr( stat, "Failed to addAttribute: Joint_A_Valid" );

  joint_b_valid = nAttr.create( "Joint_B_Valid", "jbv", MFnNumericData::kBoolean );
  nAttr.setDefault( true );
  nAttr.setHidden( true );
  stat = addAttribute( joint_b_valid );
  MCheckErr( stat, "Failed to addAttribute: Joint_B_Valid" );



  // Hidden attributes

  // forceDrawSolid
  forceDrawSolid = nAttr.create( "forceDrawSolid", "forceDrawSolid", MFnNumericData::kBoolean );
  nAttr.setDefault( false );
  nAttr.setHidden( true );
  stat = addAttribute( forceDrawSolid );
  MCheckErr( stat, "Failed to addAttribute: forceDrawSolid" );


  return MS::kSuccess;
}

bool DestructionGlue::drawWireframe( M3dView::DisplayStyle style )
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

MBoundingBox DestructionGlue::boundingBox() const
{   
  // Get the size
  MPoint corner1( -1.0, -1.0, -1.0 );
  MPoint corner2( 1.0, 1.0, 1.0 );

  return MBoundingBox( corner1, corner2 );
}

void DestructionGlue::draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status )
{ 
  view.beginGL();

  checkGlErrors( "Problem before drawing destruction glue" );

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

  checkGlErrors( "Problem drawing destruction glue" );

  view.endGL();
}

void DestructionGlue::checkGlErrors( const char* msg )
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
