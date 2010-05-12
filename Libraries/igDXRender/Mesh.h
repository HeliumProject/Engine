#pragma once

#include "platform.h"

#include "d3d9.h"
#include "d3dx9.h"

#include "ShaderLoader.h"

namespace igDXRender
{
  class ShaderFrag
  {
  public:
    ShaderFrag()
      : m_Id( 0 )
      , m_shader( "" )
      , m_shader_loader( NULL )
      , m_bangle_index( 0 )
    {
    }

    virtual ~ShaderFrag()
    {
    }

    u64                 m_Id;
    std::string         m_shader;
    ShaderLoaderPtr     m_shader_loader;

    std::vector<u32>    m_pIndex;
    std::vector<u32>    m_nIndex;
    std::vector<u32>    m_tIndex;
    std::vector<u32>    m_tanIndex;
    std::vector<u32>    m_cIndex;

    //data structures optimized for rendering, compiled model
    std::vector<u32>    m_indices;

    //bangle index for this shader fragment
    u32                 m_bangle_index;
  };

  struct Fragment
  {
    u32     m_orig_shader;
    u32     m_prim_count;
    u32     m_base_index;
  };

  class Mesh
  {
  public:
    Mesh( const char* name );
    virtual ~Mesh();

    std::string               m_filename;       
    u32                       m_crc;            // crc of the filename
    u64                       m_timestamp;      // -1 if the mesh is a local mesh
    u32                       m_load_count;

    IDirect3DVertexBuffer9*   m_dbg_normal;
    IDirect3DVertexBuffer9*   m_dbg_tangent;
    IDirect3DVertexBuffer9*   m_verts;
    IDirect3DIndexBuffer9*    m_indices;
    u32                       m_vert_count;
    u32                       m_vert_size;
    u32                       m_index_count;
    u32                       m_fragment_count;
    Fragment*                 m_fragments;

    // bounding info
    D3DXVECTOR3       m_origin;
    D3DXVECTOR3       m_min;
    D3DXVECTOR3       m_max;

    u32 DecrementUsage()
    {
      if (m_load_count>0)
      {
        m_load_count--;
      }

      return m_load_count;
    }
  };
}
