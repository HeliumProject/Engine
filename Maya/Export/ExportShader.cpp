#include "Precompile.h"
#include "ExportShader.h"
#include "MayaContentCmd.h"

using namespace MayaContent;

void ExportShader::GatherMayaData( V_ExportBase &newExportObjects )
{
  EXPORT_SCOPE_TIMER( ("") );

  // this is the default shader case. w00t.
  if ( m_MayaObject == MObject::kNullObj )
  {
    return;
  }

  MFnLambertShader shaderFn (m_MayaObject);

  const Content::ShaderPtr contentShader = GetContentShader();
  contentShader->m_DefaultName = shaderFn.name().asTChar();

  // get color value
  MColor color = shaderFn.color();
  contentShader->m_BaseColor = Math::Vector4 (color.r, color.g, color.b, color.a);

  //
  // Iterate up "color"'s DG and discover texture file and UV settings
  //

  MPlug colorPlug = shaderFn.findPlug(MString ("color"));
  MItDependencyGraph	colorIterator (colorPlug,
    MFn::kInvalid,
    MItDependencyGraph::kUpstream,
    MItDependencyGraph::kDepthFirst,
    MItDependencyGraph::kNodeLevel);

  colorIterator.disablePruningOnFilter();

  while (!colorIterator.isDone())
  {
    MFnDependencyNode	l_NodeFn (colorIterator.thisNode());

    switch (l_NodeFn.object().apiType())
    {
    case MFn::kFileTexture:
      {
        //MString l_TexturePath;
        //MPlug	l_TextureFilePathPlug = l_NodeFn.findPlug("fileTextureName");
        //l_TextureFilePathPlug.getValue(l_TexturePath);

        //if (FileSystem::Exists(l_TexturePath.asTChar()))
        //{
        //  try
        //  {
        //    contentShader->m_BaseTextureFileID = File::GlobalManager().Add( l_TexturePath.asTChar() );
        //  }
        //  catch ( const File::Exception& e )
        //  {
        //    Log::Warning("Unable to add file '%s' to the file manager.\nReason: %s\n\n", l_TexturePath.asTChar(), e.What() );
        //  }
        //}
      }
      break;

    case MFn::kPlace2dTexture:
      {
        MPlug plug = l_NodeFn.findPlug("wrapU" );
        plug.getValue( contentShader->m_WrapU );

        plug   = l_NodeFn.findPlug("wrapV");
        plug.getValue( contentShader->m_WrapV );

        plug   = l_NodeFn.findPlug("repeatU");
        plug.getValue( contentShader->m_RepeatU );

        plug   = l_NodeFn.findPlug("repeatV");
        plug.getValue( contentShader->m_RepeatV );

        // this data isn't serialized directly...just necessary for pre-export processing
        plug   = l_NodeFn.findPlug("offsetU" );
        plug.getValue( m_OffsetU );

        plug   = l_NodeFn.findPlug("offsetV" );
        plug.getValue( m_OffsetV );

        plug   =  l_NodeFn.findPlug( "rotateUV" );
        plug.getValue( m_RotateUV );
      }
      break;
    }
    colorIterator.next();   
  }
  ExportBase::GatherMayaData( newExportObjects );
}

void ExportShader::ProcessMayaData()
{
  const Content::ShaderPtr contentShader = GetContentShader();
  float                theta(m_RotateUV * static_cast<float>(M_PI) / 180.0f);
  float                sinTheta(sinf(theta));
  float                cosTheta(cosf(theta));

  m_URotate.x =   contentShader->m_RepeatU * cosTheta;
  m_URotate.y =   contentShader->m_RepeatU * sinTheta;
  m_VRotate.x =   contentShader->m_RepeatV * -sinTheta;
  m_VRotate.y =   contentShader->m_RepeatV * cosTheta;
  m_UVTranslate.x  =  m_OffsetU;
  m_UVTranslate.y  =  m_OffsetV;
}