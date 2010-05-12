#pragma once

#include "platform.h"
#include "d3d9.h"
#include "d3dx9.h"

#include "ObjectLoader.h"
#include "Environment.h"
#include "Mesh.h"

namespace igDXRender
{
  class Render;

  class Scene
  {
  public:
    Scene(Render* render);
    ~Scene();

    void SetMeshHandle(u32 handle);
    void SetEnvironmentHandle(u32 handle);

    void AddShader( const u32 shaderHandle )
    {
      m_shader_table.push_back( shaderHandle );
    }

  private:
    u32 LoadNewMesh(const char* name,ObjectLoaderPtr loader = NULL, int bangleIndex = -1);
  public:
    u32 LoadMesh(const char* name,ObjectLoaderPtr loader = NULL, int bangleIndex = -1);
    u32 ExtractMesh(const char* name, ObjectLoaderPtr loader, int bangleIndex);

  private:
    u32 LoadNewEnvironment(const char* fname,u32 clear_color);
  public:
    u32 LoadEnvironment(const char* name, u32 clear_color);

    static Mesh*        ResolveMeshHandle( u32 handle );
    static Environment* ResolveEnvironmentHandle( u32 handle );

    static void RemoveAllMeshes();
    static void RemoveAllEnvironments();

    // render class
    Render*               m_render_class;

    // handle to an environment
    u32                   m_environment;

    // the viewport area, where to render on the back buffer 
    u32                   m_xpos;
    u32                   m_ypos;
    u32                   m_width;
    u32                   m_height; 

    float                 m_scene_scale;                // set before loading a mesh!
    float                 m_normalscale;                // set before loading a mesh

    float                 m_expsoure;                   // global exposure
    float                 m_diffuse_light_scale;        // SH scale
    bool                  m_render_reference_grid;      // render the reference axis
    bool                  m_render_viewport_outline;    // render the 2D outline of the viewport(good for multiple viewports)
    bool                  m_render_wireframe;           // overlay wireframe  
    bool                  m_render_normals;
    bool                  m_render_tangents;
    bool                  m_render_env_cube;
    u32                   m_draw_mode;                  // the main draw mode

    // the index in the mesh array
    u32                   m_mesh_handle;
    std::vector< u32 >    m_shader_table;
    std::vector< Light* > m_lights;                     // these lights are accumualted into the env sh
    D3DXVECTOR4           m_ambient;                    // this ambient is accumulated into the env sh

    // bounding info computed when the mesh is loaded
    D3DXVECTOR3           m_origin;
    D3DXVECTOR3           m_min;
    D3DXVECTOR3           m_max;

    // change these matrices as you wish, use the D3DX functions for ease
    D3DXMATRIX            m_projmat;
    D3DXMATRIX            m_viewmat;
    D3DXMATRIX            m_worldmat;
  };
}
