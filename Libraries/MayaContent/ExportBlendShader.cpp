#include "stdafx.h"
#include "ExportBlendShader.h"
#include "MayaContentCmd.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"

using namespace MayaContent;

void ExportBlendShader::GatherMayaData( V_ExportBase &newExportObjects )
{
  EXPORT_SCOPE_TIMER( ("") );

  MFnLambertShader shaderFn (m_MayaObject);

  const Content::BlendShaderPtr contentShader = GetContentBlendShader();
  contentShader->m_DefaultName = shaderFn.name().asChar();

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
        MPlug plug = colorIterator.thisPlug();

        MPlugArray plugs;
        plug.connectedTo( plugs, false, true );

        MString l_TexturePath;
        MPlug	l_TextureFilePathPlug = l_NodeFn.findPlug("fileTextureName");
        l_TextureFilePathPlug.getValue(l_TexturePath);

        MString plugName = plugs[0].partialName();
        if(  plugName == "c2" )
        {
          if (FileSystem::Exists(l_TexturePath.asChar()))
          {
            try
            {
              contentShader->m_BaseTextureFileID = File::GlobalManager().Add( l_TexturePath.asChar() );
            }
            catch ( const File::Exception& e )
            {
              Console::Warning("Unable to add file '%s' to the file manager.\nReason: %s\n\n", l_TexturePath.asChar(), e.what() );
            }
          }
        }
        else if( plugName == "c1" )
        {
          if (FileSystem::Exists(l_TexturePath.asChar()))
          {
            try
            {
              contentShader->m_TopTextureFileID = File::GlobalManager().Add( l_TexturePath.asChar() );
            }
            catch ( const File::Exception& e )
            {
              Console::Warning("Unable to add file '%s' to the file manager.\nReason: %s\n\n", l_TexturePath.asChar(), e.what() );
            }
          }
        } 
        else if( plugName == "b" )
        {
          if (FileSystem::Exists(l_TexturePath.asChar()))
          {
            try
            {
              contentShader->m_BlendTextureFileID = File::GlobalManager().Add( l_TexturePath.asChar() );
            }
            catch ( const File::Exception& e )
            {
              Console::Warning("Unable to add file '%s' to the file manager.\nReason: %s\n\n", l_TexturePath.asChar(), e.what() );
            }
          }
        }
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
