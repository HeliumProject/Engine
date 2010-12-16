/*#include "Precompile.h"*/
#include "RenderScene.h"
#include "Renderer.h"
#include "OBJObjectLoader.h"

#include "Foundation/Log.h"
#include "Foundation/File/Path.h"
#include "Foundation/Checksum/CRC32.h"

using namespace Helium;
using namespace Helium::Render;

static std::vector<RenderMesh*>         g_loaded_meshes;
static std::vector<RenderEnvironment*>  g_loaded_environments;
static uint32_t g_init_count = 0;

// fwd
void CreateDefaultMeshes(Renderer* render);
void CreateDefaultEnvironments(Renderer* render);

///////////////////////////////////////////////////////////////////////////////////////////////////
RenderMesh* RenderScene::ResolveMeshHandle( uint32_t handle )
{
    return g_loaded_meshes[handle];
}

RenderEnvironment* RenderScene::ResolveEnvironmentHandle( uint32_t handle )
{
    return g_loaded_environments[handle];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
RenderScene::RenderScene(Renderer* render)
{
    if ( ++g_init_count == 1 )
    {
        CreateDefaultMeshes( render );
        CreateDefaultEnvironments(render);
    }

    m_mesh_handle = 0xffffffff;
    m_environment = 0xffffffff;
    m_draw_mode =  DRAW_MODE_NORMAL;
    m_render_wireframe = false;
    m_expsoure = 1.0f;
    m_diffuse_light_scale = 1.0f;
    m_ambient.x = 0.0f;
    m_ambient.y = 0.0f;
    m_ambient.z = 0.0f;
    m_ambient.w = 0.0f;
    m_render_normals = false;
    m_render_tangents = false;
    m_render_env_cube = false;

    // the default viewport is the entire surface which we obtain from the renderer
    m_xpos=0;
    m_ypos=0;
    m_width = render->GetWidth();
    m_height = render->GetHeight();
    m_renderer = render;

    // automatically scale the scene to unit scale and translate to the origin
    m_scene_scale = 1.0f;
    m_normalscale = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
RenderScene::~RenderScene()
{
    // decrement the mesh
    SetMeshHandle( 0xffffffff );

    // decrement the env
    if (m_environment!=0xffffffff)
    {
        RenderEnvironment* env = ResolveEnvironmentHandle(m_environment);
        if (env->DecrementUsage()==0)
        {
            // delete the environment
        }
    }

    // remove references to all the shaders in the shader table

    // clean up lights
    for ( std::vector< RenderLight* >::iterator itr = m_lights.begin(), end = m_lights.end(); itr != end; ++itr )
    {
        if ( (*itr) != NULL )
        {
            delete (*itr);
        }
    }

    if ( --g_init_count == 0 )
    {
        RemoveAllMeshes();
        RemoveAllEnvironments();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RenderScene::SetMeshHandle(uint32_t handle)
{
    if (m_mesh_handle!=0xffffffff)
    {
        // decrement the refcount on the exising mesh
        if ( g_loaded_meshes[m_mesh_handle]->DecrementUsage() == 0 )
        {
            delete g_loaded_meshes[m_mesh_handle];
            g_loaded_meshes[m_mesh_handle] = 0;
        }
        m_mesh_handle = 0xffffffff;
    }

    if (handle==0xffffffff)
    {
        return;
    }

    if ( g_loaded_meshes[handle] == 0 )
    {
        // There is a lingering handle pointing to a mesh that was already deleted
        HELIUM_BREAK();
        return;
    }

    g_loaded_meshes[handle]->m_load_count++;
    m_mesh_handle = handle;
    m_shader_table.resize(g_loaded_meshes[handle]->m_fragment_count);

    // fill the scene level shader table
    for (uint32_t s=0;s<g_loaded_meshes[handle]->m_fragment_count;s++)
    {
        m_shader_table[s]=g_loaded_meshes[handle]->m_fragments[s].m_orig_shader;
    }

    // copy the bounding info from the mesh to the scene
    m_min = g_loaded_meshes[handle]->m_min;
    m_max = g_loaded_meshes[handle]->m_max;
    m_origin = g_loaded_meshes[handle]->m_origin;
}

/////////////////////////////////////////////////////////////////////////////////////////////
void RenderScene::SetEnvironmentHandle(uint32_t handle)
{
    if (m_environment!=0xffffffff)
    {
        // decrement the refcount
        g_loaded_environments[m_environment]->DecrementUsage();
        m_environment = 0xffffffff;
    }

    if (handle==0xffffffff)
    {
        return;
    }

    g_loaded_environments[handle]->m_load_count++;
    m_environment = handle;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t RenderScene::LoadNewMesh( const tchar_t* fname, ObjectLoaderPtr loader, int bangleIndex )
{
    Helium::Path path( fname );

    OBJObjectLoader objLoader;

    if ( loader == NULL )
    {
        tstring extension = path.Extension();
        if ( extension == TXT( "obj" ) )
        {
            Log::Print( TXT( "Loading OBJ file '%s'\n" ),fname);
            loader = &objLoader; 
        }
        else
        {
            return 0xffffffff;
        }
    }

    // parse the text file obj into loose arrays
    if ( loader->ParseFile(fname)==PARSE_FILE_FAILED )
    {
        Log::Error( TXT( "Failed to load '%s'\n" ),fname);
        return 0xffffffff;
    }

    Log::Print( TXT( "Computing Bounding info\n" ) );
    // compute the bounding box
    D3DXVECTOR3 min;
    D3DXVECTOR3 max;

    loader->ComputeBoundingBox(min,max);
    D3DXVECTOR3 r = 0.5f*(max - min);
    D3DXVECTOR3 center = min + r;

    if (m_scene_scale>0.0f)
    {
        Log::Print( TXT( "Relocating and rescaling\n" ) );
        loader->Rescale(m_scene_scale,r,center);
        loader->ComputeBoundingBox(min,max);
        r = 0.5f*(max - min);
    }  

    Log::Print( TXT( "Reindexing vertex data\n" ) );
    // compile the loose arrays into unique vertices, there is an index buffer per material
    loader->Compile();

    return ExtractMesh( fname, loader, bangleIndex );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t RenderScene::LoadMesh(const tchar_t* fname,ObjectLoaderPtr loader, int bangleIndex)
{
    uint32_t crc = Helium::Crc32(fname, _tcslen(fname));

    uint32_t mesh_count = (uint32_t)g_loaded_meshes.size();
    uint32_t handle = 0xffffffff;
    for (uint32_t i=0;i<mesh_count;i++)
    {
        if (g_loaded_meshes[i])
        {
            if ((g_loaded_meshes[i]->m_crc==crc))
            {
                handle = i;
                break;
            }
        }
    }

    if (handle==0xffffffff)
    {
        // wasn't found, so load it
        handle = LoadNewMesh(fname, loader, bangleIndex);
    }

    return handle;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t RenderScene::ExtractMesh(const tchar_t* name, ObjectLoaderPtr loader, int bangleIndex)
{
    tchar_t meshName[ 1024 ];
    const tchar_t* fname = name;

    if ( bangleIndex > 0 )
    {
        memset( meshName, 0, sizeof ( meshName ) );
        _stprintf( meshName, TXT( "%s_bangle_%d" ), name, bangleIndex );
        fname = meshName;
    }

    uint32_t frag_count = (uint32_t)loader->m_fragments.size();

    RenderMesh* result = new RenderMesh(fname);

    loader->ComputeBoundingBox(result->m_min,result->m_max);
    D3DXVECTOR3 r = 0.5f*(result->m_max - result->m_min);
    result->m_origin = result->m_min + r;

    uint32_t total_indices = 0;
    for (uint32_t f=0;f<frag_count;f++)
    {
        if ( ( bangleIndex >= 0 ) && ( loader->m_fragments[ f ].m_bangle_index != bangleIndex ) )
        {
            continue;
        }

        total_indices += (uint32_t)loader->m_fragments[f].m_indices.size();
    }

    Log::Print( TXT( "Building D3D vertex data\n" ) );
    result->m_vert_count = (uint32_t)loader->m_vertices.size()/loader->m_vtxSize;  // size is in floats
    result->m_index_count = total_indices;

    uint32_t bytes = result->m_vert_count*sizeof(MeshVertex);
    if ( bytes == 0 )
    {
        Log::Warning( TXT( "Possible legacy file. '%s'\n" ),fname);
        return 0xffffffff;
    }
    uint32_t vec_bytes = result->m_vert_count*sizeof(VertexDebug)*2;

    IDirect3DDevice9* device = m_renderer->GetD3DDevice();

    device->CreateVertexBuffer(bytes,D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&result->m_verts,0);
    device->CreateIndexBuffer(result->m_index_count*2,D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&result->m_indices,0);

    device->CreateVertexBuffer(vec_bytes,D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&result->m_dbg_normal,0);
    device->CreateVertexBuffer(vec_bytes,D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&result->m_dbg_tangent,0);

    // vertex buffer
    MeshVertex *p;
    if ( !result->m_verts )
    {
        Log::Warning( TXT( "Possible legacy file. '%s'\n" ),fname);
        return 0xffffffff;
    }
    result->m_verts->Lock(0,0,(void**)&p,0);
    memcpy(p,&loader->m_vertices[0],bytes);  

    /*  tchar_t buff[1024];
    for (uint32_t v=0;v<result->m_vert_count;v++)
    {
    MeshVertex* mv = (MeshVertex*)&loader->m_vertices[v*16];
    sprintf(buff,"Vertex %d\n",v);
    OutputDebugString(buff);
    sprintf(buff,"  Position   :%f  %f  %f\n",mv->m_pos.x,mv->m_pos.y,mv->m_pos.z);
    OutputDebugString(buff);
    sprintf(buff,"  Normal     :%f  %f  %f\n",mv->m_normal.x,mv->m_normal.y,mv->m_normal.z);
    OutputDebugString(buff);
    sprintf(buff,"  Tangent    :%f  %f  %f\n",mv->m_tangent.x,mv->m_tangent.y,mv->m_tangent.z);
    OutputDebugString(buff);
    sprintf(buff,"  UV         :%f  %f\n",mv->m_uv.x,mv->m_uv.y);
    OutputDebugString(buff);
    sprintf(buff,"  Color      :%f  %f  %f  %f\n",mv->m_color.x,mv->m_color.y,mv->m_color.z,mv->m_color.w);
    OutputDebugString(buff);
    sprintf(buff,"\n");
    OutputDebugString(buff);
    }*/

    result->m_verts->Unlock();

    VertexDebug* dv;
    result->m_dbg_normal->Lock(0,0,(void**)&dv,0);  
    for (uint32_t v=0;v<result->m_vert_count;v++)
    {
        dv[v*2+0].m_pos.x = loader->m_vertices[v*loader->m_vtxSize+0];
        dv[v*2+0].m_pos.y = loader->m_vertices[v*loader->m_vtxSize+1];
        dv[v*2+0].m_pos.z = loader->m_vertices[v*loader->m_vtxSize+2];
        dv[v*2+0].m_color = D3DCOLOR_ARGB(0xff,0xff,0,0);

        dv[v*2+1].m_pos.x = loader->m_vertices[v*loader->m_vtxSize+0]+(loader->m_vertices[v*loader->m_vtxSize+3]*m_normalscale);
        dv[v*2+1].m_pos.y = loader->m_vertices[v*loader->m_vtxSize+1]+(loader->m_vertices[v*loader->m_vtxSize+4]*m_normalscale);
        dv[v*2+1].m_pos.z = loader->m_vertices[v*loader->m_vtxSize+2]+(loader->m_vertices[v*loader->m_vtxSize+5]*m_normalscale);
        dv[v*2+1].m_color = D3DCOLOR_ARGB(0xff,0xff,0,0);
    }
    result->m_dbg_normal->Unlock();

    result->m_dbg_tangent->Lock(0,0,(void**)&dv,0);  
    for (uint32_t v=0;v<result->m_vert_count;v++)
    {
        dv[v*2+0].m_pos.x = loader->m_vertices[v*loader->m_vtxSize+0];
        dv[v*2+0].m_pos.y = loader->m_vertices[v*loader->m_vtxSize+1];
        dv[v*2+0].m_pos.z = loader->m_vertices[v*loader->m_vtxSize+2];
        dv[v*2+0].m_color = D3DCOLOR_ARGB(0xff,0,0xff,0);

        dv[v*2+1].m_pos.x = loader->m_vertices[v*loader->m_vtxSize+0]+(loader->m_vertices[v*loader->m_vtxSize+6]*m_normalscale);
        dv[v*2+1].m_pos.y = loader->m_vertices[v*loader->m_vtxSize+1]+(loader->m_vertices[v*loader->m_vtxSize+7]*m_normalscale);
        dv[v*2+1].m_pos.z = loader->m_vertices[v*loader->m_vtxSize+2]+(loader->m_vertices[v*loader->m_vtxSize+8]*m_normalscale);
        dv[v*2+1].m_color = D3DCOLOR_ARGB(0xff,0,0xff,0);
    }
    result->m_dbg_tangent->Unlock();

    result->m_vert_size = sizeof(MeshVertex);
    result->m_fragment_count = loader->GetNumFragments( bangleIndex );
    result->m_fragments = new Fragment[result->m_fragment_count];

    uint16_t *idx;
    result->m_indices->Lock(0,0,(void**)&idx,0);    

    uint32_t currentFragment = 0;
    uint32_t pos=0;
    for (uint32_t f=0;f<frag_count;f++)
    {
        if ( ( bangleIndex >= 0 ) && ( loader->m_fragments[ f ].m_bangle_index != bangleIndex ) )
        {
            continue;
        }

        tchar_t shader_name[ MAX_PATH ];
        Helium::Path shaderPath( loader->m_fragments[ f ].m_shader );
        _stprintf( shader_name, TXT( "%s" ), shaderPath.c_str() );

        uint32_t idx_count = (uint32_t)loader->m_fragments[f].m_indices.size();
        result->m_fragments[currentFragment].m_orig_shader = m_renderer->m_shader_manager.LoadShader(shader_name, true, loader->m_fragments[ f ].m_shader_loader);
        if (result->m_fragments[currentFragment].m_orig_shader==0xffffffff)
        {
            result->m_fragments[currentFragment].m_orig_shader = m_renderer->m_shader_manager.LoadShader( TXT( "@@default" ), true);
        }
        result->m_fragments[currentFragment].m_base_index = pos;
        result->m_fragments[currentFragment].m_prim_count = idx_count/3;

        /*    Log::Print( TXT( "Fragment %d [base idx = %d, index count = %d]\n" ),f,result->m_fragments[currentFragment].m_base_index,idx_count);
        for (uint32_t t=0;t<result->m_fragments[currentFragment].m_prim_count;t++)
        {
        Log::Print( TXT( "  Triangle %d: %d, %d, %d\n" ),t, loader->m_fragments[f].m_indices[t*3+0],loader->m_fragments[f].m_indices[t*3+1],loader->m_fragments[f].m_indices[t*3+2]);
        }*/

        for (uint32_t i=0;i<idx_count;i++)
        {
            idx[pos]=(uint16_t)loader->m_fragments[f].m_indices[i];
            pos++;
        }

        currentFragment++;
    }

    result->m_indices->Unlock();

    g_loaded_meshes.push_back(result);

    Log::Print( TXT( "Done\n" ) );

    return (uint32_t)g_loaded_meshes.size()-1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t RenderScene::LoadNewEnvironment(const tchar_t* fname,uint32_t clear_color)
{
    float r[9];
    float g[9];
    float b[9];
    IDirect3DCubeTexture9* cube_tex;
    if (FAILED(D3DXCreateCubeTextureFromFileEx(m_renderer->GetD3DDevice(),fname,0,0,0,D3DFMT_A16B16G16R16F,D3DPOOL_MANAGED ,D3DX_DEFAULT ,D3DX_DEFAULT ,0,0,0,&cube_tex)))
    {
        Log::Error( TXT( "failed to load cubemap '%s'\n" ),fname);
        return 0xffffffff;
    }

    D3DXSHProjectCubeMap(3,cube_tex,r,g,b);
    D3DXMATRIX mat(0, -1, 0,0,     
        0,  0,1,0,
        -1,  0, 0,0,
        0,0,0,1);
    float r1[9];
    float g1[9];
    float b1[9];
    D3DXSHRotate(r1,3,&mat,r);
    D3DXSHRotate(g1,3,&mat,g);
    D3DXSHRotate(b1,3,&mat,b);


    RenderEnvironment* env = new RenderEnvironment(fname);
    env->m_env_texture = cube_tex;
    env->m_clearcolor = clear_color;

    for (uint32_t sh=0;sh<9;sh++)
    {
        env->m_sh[sh].x = r1[sh];
        env->m_sh[sh].y = g1[sh];
        env->m_sh[sh].z = b1[sh];
        env->m_sh[sh].w = 1.0f;
    }

    g_loaded_environments.push_back(env);

    return (uint32_t)g_loaded_environments.size()-1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t RenderScene::LoadEnvironment(const tchar_t* fname, uint32_t clear_color)
{
    uint32_t crc = Helium::Crc32(fname, _tcslen(fname));

    uint32_t env_count = (uint32_t)g_loaded_environments.size();
    uint32_t handle = 0xffffffff;
    for (uint32_t i=0;i<env_count;i++)
    {
        if (g_loaded_environments[i])
        {
            if ((g_loaded_environments[i]->m_crc==crc))
            {
                handle = i;
                break;
            }
        }
    }

    if (handle==0xffffffff)
    {
        // wasn't found, so load it
        handle = LoadNewEnvironment(fname,clear_color);
    }

    return handle;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Torus
/////////////////////////////////////////////////////////////////////////////////////////////
static D3DXVECTOR3 torus(float u, float v)
{
    u *= 2*D3DX_PI;
    v *= 2*D3DX_PI;
    float su,cu,sv,cv;
    su=sinf(u);
    cu=cosf(u);
    sv=sinf(v);
    cv=cosf(v);
    D3DXVECTOR3 r;
    r.x = su * (2.1f + sv);
    r.y = cu * (2.1f + sv);
    r.z = cv;
    return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Deformed Torus
/////////////////////////////////////////////////////////////////////////////////////////////
static D3DXVECTOR3 torus2(float u, float v)
{
    u *= 2*D3DX_PI;
    v *= 2*D3DX_PI;

    float su,cu,sv,cv;
    su=sinf(u);
    cu=cosf(u);
    sv=sinf(v);
    cv=cosf(v);

    float rad = 1.0f + 0.12f * sinf(6*u + 3*v) * (0.55f + 0.45f*sv);
    D3DXVECTOR3 r;
    r.x = su * (2.1f + rad * sv);
    r.y = cu * (2.1f + rad * sv);
    r.z = rad * cv;
    return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Mobius Strip
/////////////////////////////////////////////////////////////////////////////////////////////
static void mobius(D3DXVECTOR3& r, float t)
{
    r.x = 2.3f * sinf(t);
    r.y = 2.3f * cosf(t);
    r.z = 0.0f;
}

static D3DXVECTOR3 mobius(float u, float v)
{
    u *= 2*D3DX_PI;
    v *= 2*D3DX_PI;

    D3DXVECTOR3 t1, t2;
    mobius(t1, u+0.01f);
    mobius(t2, u-0.01f);

    D3DXVECTOR3 t;
    t=t1-t2;
    D3DXVec3Normalize(&t,&t);

    D3DXVECTOR3 zaxis(0.0f, 0.0f, 1.0f);

    D3DXVECTOR3 b;
    D3DXVec3Cross(&b,&t,&zaxis);
    D3DXVec3Normalize(&b,&b);

    D3DXVECTOR3 n;
    D3DXVec3Cross(&n,&b,&t);
    D3DXVec3Normalize(&n,&n);

    D3DXVECTOR3 b2;
    b2 = b*cosf(1.5f*u);
    b2 += (n*sinf(1.5f*u));
    b2 *= 1.0f*sinf(v+0.5f*u);  

    D3DXVECTOR3 n2;
    n2 = b*-sinf(1.5f*u);
    n2+= (n*cosf(1.5f*u));
    n2*= 0.5f*cosf(v+0.5f*u);

    D3DXVECTOR3 r;
    mobius(r, u);
    r+=b2;
    r+=n2;
    return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Knot
////////////////////////////////////////////////////////////////////////////////////////////

static inline void knot(D3DXVECTOR3& r, float t)
{
    float rad = 1.8f + 0.8f * cosf(3*t);
    float phi = 0.2f*D3DX_PI * sinf(3*t);

    r.x = rad * cosf(phi) * sinf(2*t);
    r.y = rad * cosf(phi) * cosf(2*t);
    r.z = rad * sinf(phi);
}

static D3DXVECTOR3 knot(float u, float v)
{
    u *= 2*D3DX_PI;
    v *= 2*D3DX_PI;
    u += -0.2f * sinf(3*u) + 0.05f * sinf(6*u);

    D3DXVECTOR3 t1, t2;
    knot(t1, u + 0.01f);
    knot(t2, u - 0.01f);

    D3DXVECTOR3 t;
    t=t1-t2;
    D3DXVec3Normalize(&t,&t);

    D3DXVECTOR3 zaxis(0.0f, 0.0f, 1.0f);

    D3DXVECTOR3 b;
    D3DXVec3Cross(&b,&t,&zaxis);
    D3DXVec3Normalize(&b,&b);

    D3DXVECTOR3 n;
    D3DXVec3Cross(&n,&t,&b);
    D3DXVec3Normalize(&n,&n);

    D3DXVECTOR3 r;
    knot(r, u);
    r+=(b*sinf(v) * 0.55f);
    r+=(n*cosf(v) * 0.55f);
    return r;
}


// partial derivatives of bivariate function
const float DELTA = 0.01f;
inline D3DXVECTOR3 deriv_u(D3DXVECTOR3 (*f)(float,float), float u, float v)
{
    D3DXVECTOR3 r;
    r=f(u+DELTA, v)-f(u-DELTA, v);
    r*=(1.0f/(2*DELTA));  
    return r;
}
inline D3DXVECTOR3 deriv_v(D3DXVECTOR3 (*f)(float,float), float u, float v)
{
    D3DXVECTOR3 r;
    r = f(u, v+DELTA) - f(u, v-DELTA);
    r*=(1.0f/(2*DELTA));
    return r;
}
inline D3DXVECTOR3 deriv_uu(D3DXVECTOR3 (*f)(float,float), float u, float v)
{
    D3DXVECTOR3 r;
    r = deriv_u(f, u+DELTA, v) - deriv_u(f, u-DELTA, v);
    r*=(1.0f/(2*DELTA));
    return r;
}
inline D3DXVECTOR3 deriv_vv(D3DXVECTOR3 (*f)(float,float), float u, float v)
{
    D3DXVECTOR3 r;
    r = deriv_v(f, u, v+DELTA)- deriv_v(f, u, v-DELTA);
    r*= 1/(2*DELTA);
    return r;
}
inline D3DXVECTOR3 deriv_uv(D3DXVECTOR3 (*f)(float,float), float u, float v)
{
    D3DXVECTOR3 r;
    r = deriv_u(f, u, v+DELTA)-deriv_u(f, u, v-DELTA);
    r*= 1/(2*DELTA);
    return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////
RenderMesh* InitMesh(Renderer* render,const tchar_t* name,D3DXVECTOR3 (*func)(float,float),int32_t num_u, int32_t num_v)
{
    RenderMesh* result = new RenderMesh(name);

    result->m_vert_count = (num_u + 1) * (num_v + 1);
    result->m_index_count = num_u * num_v * 6;

    IDirect3DDevice9* device = render->GetD3DDevice();

    device->CreateVertexBuffer(result->m_vert_count*sizeof(MeshVertex),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&result->m_verts,0);
    device->CreateIndexBuffer(result->m_index_count*2,D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&result->m_indices,0);

    // vertex buffer
    {
        MeshVertex *p;
        result->m_verts->Lock(0,0,(void**)&p,0);

        for (int32_t j = 0; j <= num_v; j++)
            for (int32_t i = 0; i <= num_u; i++)
            {
                float u = float(i) / num_u;
                float v = float(j) / num_v;

                // vertex position
                D3DXVECTOR3 pos = func(u, v);
                p->m_pos.x = pos.x;
                p->m_pos.y = pos.y;
                p->m_pos.z = pos.z;

                // texture coord
                p->m_uv.x = u;
                p->m_uv.y = v;

                // partial derivatives with respect to (u, v)
                D3DXVECTOR3 Pu  = deriv_u(func, u, v);
                D3DXVECTOR3 Pv  = deriv_v(func, u, v);
                //			D3DXVECTOR3 Puu = deriv_uu(func, u, v);
                //			D3DXVECTOR3 Pvv = deriv_vv(func, u, v);
                D3DXVECTOR3 tmp;
                D3DXVec3Normalize(&tmp,&Pu);
                p->m_tangent.x = tmp.x;
                p->m_tangent.y = tmp.y;
                p->m_tangent.z = tmp.z;
                p->m_tangent.w = 1.0f;    // flip factor
                D3DXVECTOR3 nm;
                D3DXVec3Cross(&nm, &Pu, &Pv);
                D3DXVec3Normalize(&p->m_normal,&nm);

                p->m_color.x = 1.0f;
                p->m_color.y = 1.0f;
                p->m_color.z = 1.0f;
                p->m_color.w = 1.0f;

                p++;
            }

            result->m_verts->Unlock();
    }

    // indices
    {
        int32_t i, j, k;
        uint16_t *p;

        result->m_indices->Lock(0,0,(void**)&p,0);    
        for (j = 0; j < num_v; j++)
        {
            k = j * (num_u + 1);
            for (i = 0; i < num_u; i++, k++)
            {
                *p++ = (uint16_t)(k);
                *p++ = (uint16_t)(k + (num_u + 1));
                *p++ = (uint16_t)(k + 1);


                *p++ = (uint16_t)(k + (num_u + 1) + 1);
                *p++ = (uint16_t)(k + 1);
                *p++ = (uint16_t)(k + (num_u + 1));

            }
        }
        result->m_indices->Unlock();
    }

    // fragments
    result->m_vert_size = sizeof(MeshVertex);
    result->m_fragments = new Fragment[1];
    result->m_fragment_count = 1;
    result->m_fragments->m_orig_shader = render->m_shader_manager.LoadShader( TXT( "@@default" ), true);
    result->m_fragments->m_base_index = 0;
    result->m_fragments->m_prim_count = result->m_index_count/3;

    // all the default meshes are unit sized at the origin
    result->m_origin = D3DXVECTOR3(0,0,0);
    result->m_min = D3DXVECTOR3(-2,-2,-2);
    result->m_max = D3DXVECTOR3(2,2,2);

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CreateDefaultMeshes(Renderer* render)
{
    Log::Print( TXT( "Creating default meshes\n" ) );
    g_loaded_meshes.push_back(InitMesh(render,TXT( "@@torus" ),torus,40,40));
    g_loaded_meshes.push_back(InitMesh(render,TXT( "@@torus_deformed" ),torus2,40,40));
    g_loaded_meshes.push_back(InitMesh(render,TXT( "@@knot" ),knot,40,40));
    g_loaded_meshes.push_back(InitMesh(render,TXT( "@@mobius" ),mobius,40,40));
    Log::Print( TXT( "Default meshes created\n" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RenderScene::RemoveAllMeshes()
{
    uint32_t count = (uint32_t)g_loaded_meshes.size();
    for (uint32_t i=0;i<count;i++)
    {
        // There should only be default meshes left in the list
        HELIUM_ASSERT( g_loaded_meshes[i] ? g_loaded_meshes[i]->m_filename.substr( 0, 2 ).compare( TXT( "@@" ) ) == 0 : true );
        delete g_loaded_meshes[i];
        g_loaded_meshes[i] = 0;
    }
}

static void FillCubeTexture(IDirect3DCubeTexture9* tex, D3DCUBEMAP_FACES face, uint32_t val)
{
    D3DLOCKED_RECT rect;

    tex->LockRect(face,0,&rect,0,0);

    for (uint32_t y=0;y<16;y++)
    {
        uint32_t* line_data = (uint32_t*) (((uint8_t*)rect.pBits)+(y*rect.Pitch));
        for (uint32_t x=0;x<16;x++)
            line_data[x]=val;
    }
    tex->UnlockRect(face,0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void CreateDefaultEnvironments(Renderer* render)
{
    RenderEnvironment* env = new RenderEnvironment( TXT( "@@default" ) );
    env->m_clearcolor = D3DCOLOR_ARGB(0x00,0x40,0x40,0x40);
    env->m_env_bias = 0.0f;
    env->m_env_scale = 1.0f;

    IDirect3DCubeTexture9* default_cube;
    IDirect3DDevice9* device = render->GetD3DDevice();
    device->CreateCubeTexture(16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_cube,0);
    env->m_env_texture = default_cube;

    // clown color cubemap
    /*  FillCubeTexture(default_cube,D3DCUBEMAP_FACE_POSITIVE_X,D3DCOLOR_ARGB(0xff,0x80,0x00,0x00));
    FillCubeTexture(default_cube,D3DCUBEMAP_FACE_NEGATIVE_X,D3DCOLOR_ARGB(0xff,0x00,0x80,0x80));  
    FillCubeTexture(default_cube,D3DCUBEMAP_FACE_POSITIVE_Y,D3DCOLOR_ARGB(0xff,0x00,0x80,0x00));
    FillCubeTexture(default_cube,D3DCUBEMAP_FACE_NEGATIVE_Y,D3DCOLOR_ARGB(0xff,0x80,0x00,0x80));  
    FillCubeTexture(default_cube,D3DCUBEMAP_FACE_POSITIVE_Z,D3DCOLOR_ARGB(0xff,0x00,0x00,0x80));
    FillCubeTexture(default_cube,D3DCUBEMAP_FACE_NEGATIVE_Z,D3DCOLOR_ARGB(0xff,0x80,0x80,0x00)); */

    FillCubeTexture(default_cube,D3DCUBEMAP_FACE_POSITIVE_X,D3DCOLOR_ARGB(0xff,0x20,0x20,0x20));
    FillCubeTexture(default_cube,D3DCUBEMAP_FACE_NEGATIVE_X,D3DCOLOR_ARGB(0xff,0x20,0x20,0x20));  
    FillCubeTexture(default_cube,D3DCUBEMAP_FACE_POSITIVE_Y,D3DCOLOR_ARGB(0xff,0x60,0x60,0x60));
    FillCubeTexture(default_cube,D3DCUBEMAP_FACE_NEGATIVE_Y,D3DCOLOR_ARGB(0xff,0x10,0x10,0x10));  
    FillCubeTexture(default_cube,D3DCUBEMAP_FACE_POSITIVE_Z,D3DCOLOR_ARGB(0xff,0x40,0x40,0x40));
    FillCubeTexture(default_cube,D3DCUBEMAP_FACE_NEGATIVE_Z,D3DCOLOR_ARGB(0xff,0x40,0x40,0x40));

    float r[9];
    float g[9];
    float b[9];
    D3DXSHProjectCubeMap(3,default_cube,r,g,b);

    // X is the Z
    // Y is the X
    // Z is the Y
    D3DXMATRIX mat(0, -1, 0,0,     
        0,  0,1,0,
        -1,  0, 0,0,
        0,0,0,1);

    float r1[9];
    float g1[9];
    float b1[9];
    D3DXSHRotate(r1,3,&mat,r);
    D3DXSHRotate(g1,3,&mat,g);
    D3DXSHRotate(b1,3,&mat,b);

    for (uint32_t sh=0;sh<9;sh++)
    {
        env->m_sh[sh].x = r1[sh];
        env->m_sh[sh].y = g1[sh];
        env->m_sh[sh].z = b1[sh];
        env->m_sh[sh].w = 1.0f;
    }

    g_loaded_environments.push_back(env);

    Log::Print( TXT( "Default environments created\n" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RenderScene::RemoveAllEnvironments()
{
    uint32_t count = (uint32_t)g_loaded_environments.size();
    for (uint32_t i=0;i<count;i++)
    {
        delete g_loaded_environments[i];
        g_loaded_environments[i] = 0;
    }
}
