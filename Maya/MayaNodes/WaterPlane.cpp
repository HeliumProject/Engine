#include "stdafx.h"

#include "MayaUtils/NodeTypes.h"
#include "MayaUtils/Utils.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"

// Maya includes
#include <maya/MImage.h>
#include <maya/MNodeMessage.h>
#include <maya/MImageFileInfo.h>
#include <maya/MGLFunctionTable.h>
#include <maya/MGLdefinitions.h>
#include <maya/MHardwareRenderer.h>
#include <maya/MFnStringData.h>
#include <maya/MGlobal.h>
#include <algorithm>
#include <boost/regex.hpp>

#include "WaterPlane.h"
#include "TUID/TUID.h"

const MTypeId WaterPlane::s_TypeID( IGL_WATER_PLANE );
const char* WaterPlane::s_TypeName = "igWaterPlane";
static MString s_tuid_name("tuid");

MObject     WaterPlane::amplitude;
MObject     WaterPlane::max_wavelength;
MObject     WaterPlane::murkiness;
MObject     WaterPlane::water_alpha;
MObject     WaterPlane::color;
MObject     WaterPlane::clipmap_name;
MObject     WaterPlane::tuid;

static const char* s_prog = "!!ARBfp1.0\n\n"                                   \
                            "TEMP tmp;\n"                                      \
                            "OUTPUT out = result.color;\n"                     \
                            "TEX tmp, fragment.texcoord[0], texture[0], 2D;\n" \
                            "MOV out.rgb, fragment.color;\n"                   \
                            "MOV out.a, tmp.r;\n"                              \
                            "END\n";

static void checkGlErrors(const char* msg, MGLFunctionTable* ft)
{
  char temp[256] = { '\0' };

#define MYERR(n) case n: sprintf(temp, "%s ",#n); break

  GLenum err;
  bool errors = false;

  while ((err = ft->glGetError()) != MGL_NO_ERROR)
  {
    if (!errors)
    {
      // Print this the first time through the loop
      //
      sprintf(temp, "OpenGl errors: %s\n", msg);
      printf(temp);
    }

    errors = true;

    switch (err)
    {
      MYERR(GL_INVALID_ENUM);
      MYERR(GL_INVALID_VALUE);
      MYERR(GL_INVALID_OPERATION);
      MYERR(GL_STACK_OVERFLOW);
      MYERR(GL_STACK_UNDERFLOW);
      MYERR(GL_OUT_OF_MEMORY);
    default:
      {
        char tmp[32];
        sprintf(tmp, "%d", err);
        sprintf(temp, "    GL Error #\n", tmp);
      }
    }

    printf(temp);
  }
#undef MYERR
}

MStatus WaterPlane::Initialize()
{
  MStatus status;
  status = inheritAttributesFrom( Plane::s_TypeName );
  MCheckErr( status, "Failed to inheritAttributesFrom: igPlane" );

  MFnNumericAttribute attribute; 

  amplitude = attribute.create( "amplitude", "amp", MFnNumericData::kDouble );
  attribute.setDefault( 1.0 );
  status = addAttribute( amplitude );
  MCheckErr( status, "Failed to addAttribute: amplitude" );

  max_wavelength = attribute.create( "maxWavelength", "mwl", MFnNumericData::kDouble );
  attribute.setDefault( 1.0 );
  status = addAttribute( max_wavelength );
  MCheckErr( status, "Failed to addAttribute: maxWavelength" );

  murkiness = attribute.create( "murkiness", "mrk", MFnNumericData::kDouble );
  attribute.setDefault( -1.0 );
  attribute.setHidden( true );
  status = addAttribute( murkiness );
  MCheckErr( status, "Failed to addAttribute: murkiness" );

  water_alpha = attribute.create( "waterAlpha", "wal", MFnNumericData::kDouble );
  attribute.setDefault( 0.0 );
  status = addAttribute( water_alpha );
  MCheckErr( status, "Failed to addAttribute: waterAlpha" );

  color = attribute.createColor( "waterColor", "wcl", &status );
  MCheckErr( status, "Failed to createColor: waterColor" );

  attribute.setDefault( 1.0f, 1.0f, 1.0f );
  status = addAttribute( color );
  MCheckErr( status, "Failed to addAttribute: waterColor" );

  MObject       defaultString;
  MFnStringData fnStringData;
  defaultString = fnStringData.create( "" );

  // clipmap texture
  MFnTypedAttribute attributeFn;
  clipmap_name = attributeFn.create( "clipMap", "clpmp", MFnData::kString );
  attributeFn.setDefault ( defaultString );
  attributeFn.setStorable( false );
  attributeFn.setUsedAsFilename( true );
  status = addAttribute( clipmap_name );
  MCheckErr( status, "Failed to addAttribute: clipMap" );

  // TUID
  std::stringstream idStr;
  idStr << TUID::HexFormat << TUID::Null;

  defaultString = fnStringData.create( idStr.str().c_str() );
  tuid = attributeFn.create( "tuid", "wpid", MFnData::kString );
  attributeFn.setDefault ( defaultString );
  attributeFn.setHidden ( true );
  status = addAttribute( tuid );
  MCheckErr( status, "Failed to addAttribute: tuid" );
  
  return status;
}

WaterPlane::WaterPlane()
{

  m_cb_id = 0;
  m_texture_handle = -1;
  m_program = 0xffffffff;
  m_glft = NULL;

  if( MGlobal::apiVersion() == MGlobal::kInteractive )
  {
    MHardwareRenderer* rend = MHardwareRenderer::theRenderer();
    if(rend == NULL)
    {
      printf("Could not acquire the hardware renderer\n");
      return;
    }

    m_glft = rend->glFunctionTable();

    if(!m_glft->extensionExists(kMGLext_ARB_fragment_program))
    {
      printf("fragment program extension doesn't exist\n");
      return;
    }

    // fp
    m_glft->glEnable( MGL_FRAGMENT_PROGRAM_ARB );
    checkGlErrors("MGL_FRAGMENT_PROGRAM_ARB", m_glft);

    m_glft->glGenPrograms(1, &m_program);
    checkGlErrors("glGenPrograms", m_glft);

    m_glft->glBindProgram( MGL_FRAGMENT_PROGRAM_ARB, m_program );
    checkGlErrors("glBindProgram", m_glft);

    m_glft->glProgramString( MGL_FRAGMENT_PROGRAM_ARB, MGL_PROGRAM_FORMAT_ASCII, (MGLsizei) strlen(s_prog), s_prog );
    if (m_glft->glGetError() != 0)
    {
      int position;
      printf("%s\n", m_glft->glGetString(MGL_PROGRAM_ERROR_STRING));
      m_glft->glGetIntegerv( MGL_PROGRAM_ERROR_POSITION, &position);
      printf(" @%d\n", position);

      m_program = 0xffffffff;
    }

    m_glft->glDisable( MGL_FRAGMENT_PROGRAM_ARB );
  }
}

WaterPlane::~WaterPlane()
{
  if( m_cb_id != 0 )
  {
    MMessage::removeCallback( m_cb_id );
  }

  if( MGlobal::apiVersion() == MGlobal::kInteractive )
  {
    if( m_texture_handle != -1 )
    {
      MHwTextureManager::deregisterTextureFile( m_texture_handle );
      m_texture_handle = -1;
    }

    if( m_program != 0xffffffff )
    {
      m_glft->glDeletePrograms(1, &m_program);
    }

    m_glft = NULL;
  }
}

static void AttrChangedCB(MNodeMessage::AttributeMessage msg, MPlug & plug, MPlug & otherPlug, void* vpClientData)
{
  MStatus stat;
  MString s;

  bool texchange = false;

  MObject nodeThatChanged = plug.node();
  MFnDependencyNode nodeFn( nodeThatChanged );
  WaterPlane* waterPlane = static_cast< WaterPlane * >( nodeFn.userNode( &stat ) );
  if( !stat )
  {
    printf( "jiv stat 0 for some reason\n" );
    return;
  }

  // If an attribute value changes
  if ( msg & MNodeMessage::kAttributeSet ) 
  {
    MFnDependencyNode nodeFn( nodeThatChanged );

    if( plug.attribute() == WaterPlane::tuid )
    {
      tuid id = Maya::GetTUIDAttribute(nodeThatChanged, s_tuid_name, &stat);
      if( !stat )
      {
        printf("jiv - could not get TUID attribute\n");
        return;
      }

      // must be initializing
      MFnDependencyNode nodeFn( nodeThatChanged );

      std::string path("");
      File::GlobalManager().GetPath( id, path );

      MMessage::removeCallback( waterPlane->m_cb_id );

      MString mpath(path.c_str());
      MPlug cmPlug( nodeThatChanged, WaterPlane::clipmap_name );
      cmPlug.setValue( mpath );

      // restore callback
      waterPlane->m_cb_id = MNodeMessage::addAttributeChangedCallback(nodeThatChanged, AttrChangedCB, NULL, &stat);

      s.set(path.c_str());

      texchange = true;
    }
    else if( plug.attribute() == WaterPlane::clipmap_name )
    {
      // Keep the node type and name synchronized
      plug.getValue( s );

      tuid id = TUID::Null;        
      if(s.length())
      {
        id = File::GlobalManager().Open( s.asChar() );
      }

      MMessage::removeCallback( waterPlane->m_cb_id );
      stat = Maya::SetTUIDAttribute( nodeThatChanged, s_tuid_name, id, true );
      waterPlane->m_cb_id = MNodeMessage::addAttributeChangedCallback(nodeThatChanged, AttrChangedCB, NULL, &stat); // restore callback
      if(stat == 0)
      {
        printf("jiv error set tuid\n");
        return;
      }

      texchange = true;
    }
    
    if(texchange)
    {
      if( waterPlane->m_texture_handle != -1 )
      {
        MHwTextureManager::deregisterTextureFile( waterPlane->m_texture_handle );
      }

      waterPlane->m_texture_handle = -1;
      if(s.length())
      {
        MHwTextureManager::registerTextureFile( s, waterPlane->m_texture_handle );
      }
    }
  }
}

void WaterPlane::postConstructor ()
{
  MStatus stat;
  MObject moNode = this->thisMObject();

  m_cb_id = MNodeMessage::addAttributeChangedCallback(moNode, AttrChangedCB, NULL, &stat);
}

void WaterPlane::draw ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status )
{
  view.beginGL();

  m_glft->glPushAttrib ( MGL_ENABLE_BIT | MGL_POLYGON_BIT /* MGL_ALL_ATTRIB_BITSMGL_ENABLE_BIT | MGL_POLYGON_BIT */ );
  m_glft->glPushMatrix();
  checkGlErrors("glPushMatrix", m_glft);

  if ( drawWireframe( style ) )
  {
    m_glft->glBegin( MGL_LINE_LOOP );
  }
  else if ( m_texture_handle == -1 )
  {
    m_glft->glPolygonMode( MGL_FRONT_AND_BACK, MGL_FILL );

    m_glft->glBegin( MGL_POLYGON );
  }
  else
  {
    MImageFileInfo::MHwTextureType tex_type   = MImageFileInfo::kHwTexture2D;
    MImageFileInfo::MImageType     image_type = MImageFileInfo::kImageTypeColor;

    m_glft->glEnable      ( MGL_TEXTURE_2D );
    checkGlErrors("GL_TEXTURE_2D", m_glft);

    m_glft->glPolygonMode ( MGL_BACK, MGL_LINE );
    checkGlErrors("glPolygon", m_glft);

    if (m_program != 0xffffffff)
    {
      m_glft->glEnable( MGL_FRAGMENT_PROGRAM_ARB );
      checkGlErrors("MGL_FRAGMENT_PROGRAM_ARB", m_glft);
        
      m_glft->glBindProgram( MGL_FRAGMENT_PROGRAM_ARB, m_program );
      checkGlErrors("glBindProgram", m_glft);

      m_glft->glEnable     ( MGL_ALPHA_TEST );
      m_glft->glAlphaFunc  ( MGL_GEQUAL, 0.5f );
    }

    MHwTextureManager::glBind( m_texture_handle, tex_type, image_type);
    checkGlErrors("glBind", m_glft);

    m_glft->glTexParameteri( MGL_TEXTURE_2D, MGL_TEXTURE_WRAP_S, MGL_CLAMP );
    m_glft->glTexParameteri( MGL_TEXTURE_2D, MGL_TEXTURE_WRAP_T, MGL_CLAMP );
    checkGlErrors("glTexParameteri", m_glft);

    if (m_program != 0xffffffff)
    {
      m_glft->glActiveTexture( MGL_TEXTURE0 );
      checkGlErrors("glActiveTexture", m_glft);
    }

    m_glft->glBegin( MGL_POLYGON );
  }

  static GLfloat vertices[][3] =
  {
    {-100.0, 0.0, -100.0},
    {-100.0, 0.0,  100.0},
    { 100.0, 0.0,  100.0},
    { 100.0, 0.0, -100.0}
  };

  static GLfloat texcoord[][2] =
  {
    { 0.0, 1.0 },
    { 0.0, 0.0 },
    { 1.0, 0.0 },
    { 1.0, 1.0 },
  };

  m_glft->glTexCoord2fv( texcoord[0] );
  m_glft->glVertex3fv  ( vertices[0] );
    
  m_glft->glTexCoord2fv( texcoord[1] );
  m_glft->glVertex3fv  ( vertices[1] );

  m_glft->glTexCoord2fv( texcoord[2] );
  m_glft->glVertex3fv  ( vertices[2] );

  m_glft->glTexCoord2fv( texcoord[3] );
  m_glft->glVertex3fv  ( vertices[3] );

  m_glft->glEnd();
  checkGlErrors("glEnd", m_glft);
  
  m_glft->glPopMatrix();
  m_glft->glDisable( MGL_FRAGMENT_PROGRAM_ARB );
  m_glft->glPopAttrib();

  view.endGL();
}
