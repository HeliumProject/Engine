/*#include "Precompile.h"*/
#include "RBObjectLoader.h"
#include "RBShaderLoader.h"

#include "Core/Content/ContentInit.h"
#include "Core/Content/Scene.h"

#include <map>
#include <set>

using namespace Helium;

Content::RBObjectLoader::RBObjectLoader()
: Render::ObjectLoader()
{
  Reflect::Initialize();
  Content::Initialize();
}

Content::RBObjectLoader::~RBObjectLoader()
{
  Content::Cleanup();
  Reflect::Cleanup();
}

u32 Content::RBObjectLoader::ParseFile( const tchar* filename, bool winding )
{
  Content::Scene scene;

  try
  {
    scene.Load( filename );
  }
  catch( Helium::Exception& e )
  {
    Log::Warning( e.What() );
    m_parse_error = Render::PARSE_FILE_FAILED;
    return Render::PARSE_FILE_FAILED;
  }


  Content::V_Mesh meshes;
  scene.GetAll< Content::Mesh >( meshes );

  std::map<u64,u32> frag_finder;

  m_parse_warnings = 0;

  u32 mesh_count = (u32)meshes.size();
  for ( u32 m=0;m<mesh_count;m++)
  {
    Content::Mesh* mesh = meshes[m];
    
    u32 master_base_position = (u32)m_positions.size()/m_posSize;
    u32 master_base_normal = (u32)m_normals.size()/3;
    u32 master_base_uv = (u32)m_texcoords.size()/m_tcSize;
    u32 master_base_color = (u32)m_colors.size()/4;
    u32 master_base_tangent = (u32)m_sTangents.size()/4;

    // for a mesh such as this all the master base values should be the same

    // verts
    for ( Math::V_Vector3::const_iterator posItr = mesh->m_Positions.begin(), posEnd = mesh->m_Positions.end(); posItr != posEnd; ++posItr )
    {
      m_positions.push_back( (*posItr).x*-1.0f );
      m_positions.push_back( (*posItr).y );
      m_positions.push_back( (*posItr).z*-1.0f );
    }

    // normals
    for ( Math::V_Vector3::const_iterator normalItr = mesh->m_Normals.begin(), normalEnd = mesh->m_Normals.end(); normalItr != normalEnd; ++normalItr )
    {
      m_normals.push_back( (*normalItr).x );
      m_normals.push_back( (*normalItr).y );
      m_normals.push_back( (*normalItr).z );
    }

    // uvs
    for ( Math::V_Vector2::const_iterator uvItr = mesh->m_BaseUVs.begin(), uvEnd = mesh->m_BaseUVs.end(); uvItr != uvEnd; ++uvItr )
    {
      m_texcoords.push_back( (*uvItr).x );
      m_texcoords.push_back( -((*uvItr).y)+1 );
    }

    // colors
    for ( Math::V_Vector4::const_iterator colItr = mesh->m_Colors.begin(), colEnd = mesh->m_Colors.end(); colItr != colEnd; ++colItr )
    {
      m_colors.push_back( (*colItr).x );
      m_colors.push_back( (*colItr).y );
      m_colors.push_back( (*colItr).z );
      m_colors.push_back( (*colItr).w );
    }

    // tangents
    for ( Math::V_Vector3::const_iterator tanItr = mesh->m_Tangents.begin(), tanEnd = mesh->m_Tangents.end(); tanItr != tanEnd; ++tanItr )
    {
      m_sTangents.push_back( (*tanItr).x );
      m_sTangents.push_back( (*tanItr).y );
      m_sTangents.push_back( (*tanItr).z );
      m_sTangents.push_back(1.0f);    // flip
    }

    // shaders - go through all the shaders in this mesh and create fragments for all the shaders that have not yet been seen. If a shader already
    // exists the use the existing fragment for that shader.
    for ( V_TUID::const_iterator shaderItr = mesh->m_ShaderIDs.begin(), shaderEnd = mesh->m_ShaderIDs.end(); shaderItr != shaderEnd; ++shaderItr )
    {
      std::map<u64,u32>::iterator i = frag_finder.find((u64) (*shaderItr));

      if (i == frag_finder.end())
      {
        //A fragment with this shader ID does not exist, create a new fragment        
        Render::ShaderFrag new_frag;  

        Content::Shader* shader = scene.Get< Content::Shader >( (*shaderItr ) );

        if ( shader )
        {
            new_frag.m_file = shader->m_AssetPath.c_str();
            new_frag.m_shader = shader->m_AssetPath.c_str();
            new_frag.m_shader_loader = new RBShaderLoader ();
        }

        m_fragments.push_back( new_frag );
        frag_finder[(u64) (*shaderItr)] = (u32)m_fragments.size()-1;
      }
    }
   
    // handle faces
    u32 faces = (u32)mesh->m_TriangleVertexIndices.size()/3;    
    for ( u32 f=0;f<faces;f++ )
    {
      u32 shader_idx = mesh->m_ShaderIndices[f];
      u64 shader_turd = mesh->m_ShaderIDs[shader_idx];
      std::map<u64,u32>::iterator frag_idx = frag_finder.find(shader_turd);

      // there is no way we should not be able to find the fragment, we just added them all above
      HELIUM_ASSERT(frag_idx!=frag_finder.end());

      Render::ShaderFrag& frag = m_fragments[ (*frag_idx).second ];
    
      frag.m_pIndex.push_back( mesh->m_TriangleVertexIndices[f*3+0]+master_base_position );
      frag.m_pIndex.push_back( mesh->m_TriangleVertexIndices[f*3+1]+master_base_position );
      frag.m_pIndex.push_back( mesh->m_TriangleVertexIndices[f*3+2]+master_base_position );

      frag.m_tIndex.push_back( mesh->m_TriangleVertexIndices[f*3+0]+master_base_uv );
      frag.m_tIndex.push_back( mesh->m_TriangleVertexIndices[f*3+1]+master_base_uv );
      frag.m_tIndex.push_back( mesh->m_TriangleVertexIndices[f*3+2]+master_base_uv );

      frag.m_nIndex.push_back( mesh->m_TriangleVertexIndices[f*3+0]+master_base_normal );
      frag.m_nIndex.push_back( mesh->m_TriangleVertexIndices[f*3+1]+master_base_normal );
      frag.m_nIndex.push_back( mesh->m_TriangleVertexIndices[f*3+2]+master_base_normal );

      if (mesh->m_Colors.size()>0)
      {
        // we must have added the same number of colors as the other elements, all meshes within the set have
        // to be the same format/have the same components
        HELIUM_ASSERT(master_base_color==master_base_position);
        // if we added some colors add the indices for them too
        frag.m_cIndex.push_back( mesh->m_TriangleVertexIndices[f*3+0]+master_base_color );
        frag.m_cIndex.push_back( mesh->m_TriangleVertexIndices[f*3+1]+master_base_color );
        frag.m_cIndex.push_back( mesh->m_TriangleVertexIndices[f*3+2]+master_base_color );
      }

      if (mesh->m_Tangents.size()>0)
      {
        // we must have added the same number of tangents and positions, all meshes within the set have
        // to be the same format
        HELIUM_ASSERT(master_base_tangent==master_base_position);
        // if we added some tangents we'll use them as they are, the indices for them
        frag.m_tanIndex.push_back( mesh->m_TriangleVertexIndices[f*3+0]+master_base_tangent );
        frag.m_tanIndex.push_back( mesh->m_TriangleVertexIndices[f*3+1]+master_base_tangent );
        frag.m_tanIndex.push_back( mesh->m_TriangleVertexIndices[f*3+2]+master_base_tangent );
      }
    }
  }
  
  m_parse_error = m_parse_warnings?Render::PARSE_FILE_FIXUP:Render::PARSE_FILE_OK;;
  return m_parse_error;
}
