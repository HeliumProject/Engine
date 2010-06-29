#include "Precompile.h"
#include "Renderer.h"
#include "Misc.h"
#include "TGAHeader.h"
#include "SphericalHarmonics.h"

#include <mmsystem.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
Render::Renderer::Renderer()
: m_shader_manager (this)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
Render::Renderer::~Renderer()
{
  // release any default pool resources here
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Render::Renderer::Init(HWND hwnd, u32 width, u32 height, u32 flags)
{
  HRESULT hr = D3DManager::InitD3D(hwnd,width,height,flags);
  if (FAILED(hr))
    return false;

  m_shader_manager.CreateDefaults();
  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::Renderer::RenderEnvironment(Render::Scene* scene,Render::Environment* env,D3DXMATRIX& viewproj)
{
  IDirect3DDevice9* device = GetD3DDevice();

  if ((env->m_env_texture==0) || (!scene->m_render_env_cube))
  {
    // this is a basic clear op
    device->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_STENCIL|D3DCLEAR_ZBUFFER,(D3DCOLOR)env->m_clearcolor,1.0f,0);
  }
  else
  {
    //render the cubemap using the view vectors, we still clear depth and stencil
    device->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_STENCIL|D3DCLEAR_ZBUFFER,(D3DCOLOR)env->m_clearcolor,1.0f,0);

    device->SetVertexDeclaration( GetStockDecl(VERTEX_DECL_SCREENSPACE));
    device->SetVertexShader(GetStockVS(VERTEX_SHADER_SCREENSPACE));
    device->SetPixelShader(GetStockPS(PIXEL_SHADER_SKY));

    // set the cube texture
    device->SetTexture(0,env->m_env_texture);

    D3DXVECTOR4 cop;
    D3DXVECTOR4 corners[8];
    ExtractFromViewProj(cop,corners,viewproj);

    VertexScreen  quad[4];
    quad[0].m_pos = D3DXVECTOR4(0,0,1.0f,1.0f);
    quad[0].m_tex0 = D3DXVECTOR4(1,1,1,1);
    quad[0].m_tex1 = corners[6]-corners[2];

    quad[1].m_pos = D3DXVECTOR4(0,(float)scene->m_height,1.0f,1.0f);
    quad[1].m_tex0 = D3DXVECTOR4(1,1,1,1);
    quad[1].m_tex1 = corners[4]-corners[0];    

    quad[2].m_pos = D3DXVECTOR4((float)scene->m_width,(float)scene->m_height,1.0f,1.0f);
    quad[2].m_tex0 = D3DXVECTOR4(1,1,1,1);
    quad[2].m_tex1 = corners[5]-corners[1];

    quad[3].m_pos = D3DXVECTOR4((float)scene->m_width,0,1.0f,1.0f);
    quad[3].m_tex0 = D3DXVECTOR4(1,1,1,1);
    quad[3].m_tex1 = corners[7]-corners[3];

    D3DXVec3Normalize((D3DXVECTOR3*)&quad[0].m_tex1,(D3DXVECTOR3*)&quad[0].m_tex1);
    D3DXVec3Normalize((D3DXVECTOR3*)&quad[1].m_tex1,(D3DXVECTOR3*)&quad[1].m_tex1);
    D3DXVec3Normalize((D3DXVECTOR3*)&quad[2].m_tex1,(D3DXVECTOR3*)&quad[2].m_tex1);
    D3DXVec3Normalize((D3DXVECTOR3*)&quad[3].m_tex1,(D3DXVECTOR3*)&quad[3].m_tex1);

    device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,quad,sizeof(VertexScreen));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::Renderer::RenderWireframe(Render::Scene* scene,Render::Mesh* mesh)
{
  IDirect3DDevice9* device = GetD3DDevice();
  
  device->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);  
  static float bias = -0.0001f;
  device->SetRenderState(D3DRS_DEPTHBIAS,*(u32*)&bias);
  // set the shader state
  device->SetVertexDeclaration( GetStockDecl(VERTEX_DECL_MESH));
  device->SetVertexShader(GetStockVS(VERTEX_SHADER_MESH_DEBUG_COLOR));
  device->SetPixelShader(GetStockPS(PIXEL_SHADER_COLOR));

  // set the indices and vertices
  device->SetIndices(mesh->m_indices);
  device->SetStreamSource(0,mesh->m_verts,0,mesh->m_vert_size);

  for (u32 i=0;i<mesh->m_fragment_count;i++)
  {
    if (mesh->m_fragments[i].m_prim_count)
      device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,mesh->m_vert_count,mesh->m_fragments[i].m_base_index,mesh->m_fragments[i].m_prim_count);
  }

  device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);  
  static float reset_bias = 0.0f;
  device->SetRenderState(D3DRS_DEPTHBIAS,*(u32*)&reset_bias);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::Renderer::RenderNormals(Render::Scene* scene,Render::Mesh* mesh)
{
  IDirect3DDevice9* device = GetD3DDevice();

  if (mesh->m_dbg_normal==0)
    return;

  // set the shader state
  device->SetVertexDeclaration( GetStockDecl(VERTEX_DECL_DEBUG));
  device->SetVertexShader(GetStockVS(VERTEX_SHADER_OBJECT_SPACE));
  device->SetPixelShader(GetStockPS(PIXEL_SHADER_COLOR));

  device->SetStreamSource(0,mesh->m_dbg_normal,0,sizeof(VertexDebug));

  device->DrawPrimitive(D3DPT_LINELIST,0,mesh->m_vert_count);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::Renderer::RenderTangents(Render::Scene* scene,Render::Mesh* mesh)
{
  IDirect3DDevice9* device = GetD3DDevice();

  if (mesh->m_dbg_tangent==0)
    return;

  static float bias = -0.001f;
  device->SetRenderState(D3DRS_DEPTHBIAS,*(u32*)&bias);

  // set the shader state
  device->SetVertexDeclaration( GetStockDecl(VERTEX_DECL_DEBUG));
  device->SetVertexShader(GetStockVS(VERTEX_SHADER_OBJECT_SPACE));
  device->SetPixelShader(GetStockPS(PIXEL_SHADER_COLOR));

  device->SetStreamSource(0,mesh->m_dbg_tangent,0,sizeof(VertexDebug));

  device->DrawPrimitive(D3DPT_LINELIST,0,mesh->m_vert_count);

  static float reset_bias = 0.0f;
  device->SetRenderState(D3DRS_DEPTHBIAS,*(u32*)&reset_bias);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void SetTexture(IDirect3DDevice9* device,Render::Texture* tex,u32 sampler)
{
  //set the sampler state for the texture
  device->SetSamplerState(sampler,D3DSAMP_ADDRESSU,tex->m_wrap_u);
  device->SetSamplerState(sampler,D3DSAMP_ADDRESSV,tex->m_wrap_v);

  switch(tex->m_filter)
  {
  case Render::Texture::FILTER_POINT:
    device->SetSamplerState(sampler,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
    device->SetSamplerState(sampler,D3DSAMP_MINFILTER,D3DTEXF_POINT);
    device->SetSamplerState(sampler,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);    
    break;

  case Render::Texture::FILTER_LINEAR:
    device->SetSamplerState(sampler,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
    device->SetSamplerState(sampler,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
    device->SetSamplerState(sampler,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);    
    break;

  case Render::Texture::FILTER_ANISOTROPIC:
    device->SetSamplerState(sampler,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
    device->SetSamplerState(sampler,D3DSAMP_MINFILTER,D3DTEXF_ANISOTROPIC);
    device->SetSamplerState(sampler,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);    
    device->SetSamplerState(sampler,D3DSAMP_MAXANISOTROPY,tex->m_aniso);
    break;
  }
  
  // set the mip bias for this texture
  device->SetSamplerState(sampler,D3DSAMP_MIPMAPLODBIAS,*(DWORD*)&tex->m_mip_bias);

  //set the sampler
  device->SetTexture(sampler,tex->m_d3d_texture);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::Renderer::RenderDebug(Render::Scene* scene,Render::Mesh* mesh)
{
  IDirect3DDevice9* device = GetD3DDevice();

  if (scene->m_draw_mode==DRAW_MODE_OFF)
    return;

 // set the shader state
  device->SetVertexDeclaration( GetStockDecl(VERTEX_DECL_MESH));
  
  switch (scene->m_draw_mode)
  {
  case DRAW_MODE_DEBUG_COLOR:
    device->SetVertexShader(GetStockVS(VERTEX_SHADER_MESH_DEBUG_COLOR));
    device->SetPixelShader(GetStockPS(PIXEL_SHADER_COLOR));
    break;

  case DRAW_MODE_DEBUG_VERT_NORMAL:
    device->SetVertexShader(GetStockVS(VERTEX_SHADER_MESH_DEBUG_NORMAL));
    device->SetPixelShader(GetStockPS(PIXEL_SHADER_COLOR));
    break;

  case DRAW_MODE_DEBUG_VERT_TANGENT:
    device->SetVertexShader(GetStockVS(VERTEX_SHADER_MESH_DEBUG_TANGENT));
    device->SetPixelShader(GetStockPS(PIXEL_SHADER_COLOR));
    break;

  case DRAW_MODE_DEBUG_UV:
    device->SetVertexShader(GetStockVS(VERTEX_SHADER_MESH_DEBUG_UV));
    device->SetPixelShader(GetStockPS(PIXEL_SHADER_COLOR));
    break;

  case DRAW_MODE_DEBUG_BASEMAP_ALPHA:
    device->SetVertexShader(GetStockVS(VERTEX_SHADER_MESH_DEBUG_UV));
    device->SetPixelShader(GetStockPS(PIXEL_SHADER_TEXTURE_ALPHA));
    break;

  case DRAW_MODE_DEBUG_BASEMAP:
  case DRAW_MODE_DEBUG_NORMALMAP:
    device->SetVertexShader(GetStockVS(VERTEX_SHADER_MESH_DEBUG_UV));
    device->SetPixelShader(GetStockPS(PIXEL_SHADER_TEXTURE));

    break;

  case DRAW_MODE_DEBUG_GLOSSMAP:
  case DRAW_MODE_DEBUG_INCANMAP:
  case DRAW_MODE_DEBUG_PARALLAXMAP:
    device->SetVertexShader(GetStockVS(VERTEX_SHADER_MESH_DEBUG_UV));
    device->SetPixelShader(GetStockPS(PIXEL_SHADER_TEXTURE_GREEN));
    break;

  case DRAW_MODE_DEBUG_SHADER_TINT:
  case DRAW_MODE_DEBUG_SHADER_DIRTY:
  case DRAW_MODE_DEBUG_SHADER_ALPHATYPE:
  case DRAW_MODE_DEBUG_FRAGMENT_INDEX:
    device->SetVertexShader(GetStockVS(VERTEX_SHADER_MESH_DEBUG_CONSTCOLOR));
    device->SetPixelShader(GetStockPS(PIXEL_SHADER_COLOR));
    break;
  }
    
  // set the indices and vertices
  device->SetIndices(mesh->m_indices);
  device->SetStreamSource(0,mesh->m_verts,0,mesh->m_vert_size);

  for (u32 i=0;i<mesh->m_fragment_count;i++)
  {
    Shader* sh = 0;
    if (scene->m_shader_table[i]!=0xffffffff)
    {
      sh = m_shader_manager.ResolveShader(scene->m_shader_table[i]);
    }

    // set the shader for this fragment
    switch(scene->m_draw_mode)
    {
      case DRAW_MODE_DEBUG_BASEMAP:
      {
        if (sh)
        {
          if (sh->m_textures[Render::Texture::SAMPLER_BASE_MAP]!=0xffffffff)
          {
            Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_BASE_MAP]);
            SetTexture(device,tex,0);
          }
          else
          {
            device->SetTexture(0,0);
          }
        }
        else
        {
          device->SetTexture(0,0);
        }
        break;
      }
      case DRAW_MODE_DEBUG_NORMALMAP:
      {
        if (sh)
        {
          if (sh->m_textures[Render::Texture::SAMPLER_NORMAL_MAP]!=0xffffffff)
          {
            Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_NORMAL_MAP]);
            SetTexture(device,tex,0);
          }
          else
          {
            device->SetTexture(0,0);
          }
        }
        else
        {
          device->SetTexture(0,0);
        }
        break;
      }
      case DRAW_MODE_DEBUG_GLOSSMAP:
      {
        if (sh)
        {
          // the GPI map setting for a shader can switch fragment by fragment.
          // A shader with a GPI map must sample RGB for the debug render
          // A shader with sperate textures must use the G channel of the respective textures
          if (sh->m_flags & SHDR_FLAG_GPI_MAP)
          {
            device->SetPixelShader(GetStockPS(PIXEL_SHADER_TEXTURE));
          }
          else
          {
            device->SetPixelShader(GetStockPS(PIXEL_SHADER_TEXTURE_GREEN));
          }

          if (sh->m_textures[Render::Texture::SAMPLER_GLOSS_MAP]!=0xffffffff)
          {
            Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_GLOSS_MAP]);
            SetTexture(device,tex,0);
          }
          else
          {
            device->SetTexture(0,0);
          }
        }
        else
        {
          device->SetTexture(0,0);
        }
        break;
      }
      case DRAW_MODE_DEBUG_INCANMAP:
      {
        if (sh)
        {
          if ((sh->m_textures[Render::Texture::SAMPLER_INCAN_MAP]!=0xffffffff) && ((sh->m_flags & SHDR_FLAG_GPI_MAP)==0))
          {
            Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_INCAN_MAP]);
            SetTexture(device,tex,0);
          }
          else
          {
            // gpi map shaders always come here
            device->SetTexture(0,0);
          }
        }
        else
        {
          device->SetTexture(0,0);
        }
        break;
      }
      case DRAW_MODE_DEBUG_PARALLAXMAP:
      {
        if (sh)
        {
          if ((sh->m_textures[Render::Texture::SAMPLER_PARALLAX_MAP]!=0xffffffff)  && ((sh->m_flags & SHDR_FLAG_GPI_MAP)==0))
          {
            Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_PARALLAX_MAP]);
            SetTexture(device,tex,0);
          }
          else
          {
            device->SetTexture(0,0);
          }
        }
        else
        {
          device->SetTexture(0,0);
        }
        break;
      }
      case DRAW_MODE_DEBUG_SHADER_TINT:
      {
        float col[4];
        col[0]=0.0f;
        col[1]=1.0f;
        col[2]=0.0f;
        col[3]=1.0f;

        if (sh)
        {
          col[0]=sh->m_basetint[0];
          col[1]=sh->m_basetint[1];
          col[2]=sh->m_basetint[2];
          col[3]=sh->m_basetint[3];
        }
        device->SetVertexShaderConstantF(8,col,1);
        break;
      }
      case DRAW_MODE_DEBUG_SHADER_DIRTY:
      {
        float col[4];
        col[0]=0.0f;
        col[1]=1.0f;
        col[2]=0.0f;
        col[3]=1.0f;

        if (sh)
        {
          col[0]=sh->m_glosstint[0];
          col[1]=sh->m_glosstint[1];
          col[2]=sh->m_glosstint[2];
          col[3]=1.0f;
        }
        device->SetVertexShaderConstantF(8,col,1);
        break;
      }
      case DRAW_MODE_DEBUG_FRAGMENT_INDEX:
      {
        float col[4];

        HashU16ToRGB((u16)i+7,col[0],col[1],col[2],col[3]);
        device->SetVertexShaderConstantF(8,col,1);
        break;
      }
      case DRAW_MODE_DEBUG_SHADER_ALPHATYPE:
      {
        float col[4];
        col[0]=0.0f;
        col[1]=0.5f;
        col[2]=0.0f;
        col[3]=1.0f;

        if (sh)
        {
          if (sh->m_alpha_type==Shader::ALPHA_BLENDED)
          {
            col[0]=0.5f;
            col[1]=0.0f;
            col[2]=0.0f;
          }
          else if (sh->m_alpha_type==Shader::ALPHA_ADDITIVE)
          {
            col[0]=0.5f;
            col[1]=0.5f;
            col[2]=0.0f;
          }
        }
        device->SetVertexShaderConstantF(8,col,1);
        break;
      }

    }

    if (mesh->m_fragments[i].m_prim_count)
      device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,mesh->m_vert_count,mesh->m_fragments[i].m_base_index,mesh->m_fragments[i].m_prim_count);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::Renderer::RenderMesh(Render::Scene* scene,Render::Mesh* mesh)
{
  IDirect3DDevice9* device = GetD3DDevice();

  bool gpi=false;
  // set the shader state
  device->SetVertexDeclaration( GetStockDecl(VERTEX_DECL_MESH));
  device->SetVertexShader(GetStockVS(VERTEX_SHADER_MESH_NORMAL));
  device->SetPixelShader(GetStockPS(PIXEL_SHADER_DIFFUSE));

  // set the indices and vertices
  device->SetIndices(mesh->m_indices);
  device->SetStreamSource(0,mesh->m_verts,0,mesh->m_vert_size);

  float env_scale = 1.0f;
  float env_bias = -0.9f;

  IDirect3DCubeTexture9* cube = 0;
  if (scene->m_environment!=0xffffffff)
  {
    Render::Environment* env = scene->ResolveEnvironmentHandle(scene->m_environment);
    cube=env->m_env_texture;
    env_scale=env->m_env_scale;
    env_bias=env->m_env_bias;
  }
  device->SetSamplerState(5,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
  device->SetSamplerState(5,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
  device->SetSamplerState(5,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);    
  device->SetTexture(5,cube);

  for (u32 i=0;i<mesh->m_fragment_count;i++)
  {
    // set the shader for this fragment
    Shader* sh = m_shader_manager.ResolveShader(scene->m_shader_table[i]);

    // we only want cutout or opaque on the first pass
    if ((sh->m_alpha_type!=Shader::ALPHA_OPAQUE) && (sh->m_alpha_type !=Shader::ALPHA_CUTOUT))
      continue;

    if (sh->m_flags&SHDR_FLAG_TWO_SIDED)
    {
      device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
    }
    else
    {
      device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
    }

    if (sh->m_alpha_type ==Shader::ALPHA_CUTOUT)
    {
      device->SetRenderState(D3DRS_ALPHATESTENABLE,true); 
      device->SetRenderState(D3DRS_ALPHAREF,0x80); 
      device->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);   // keep if >0x80
    }
    else
    {
      device->SetRenderState(D3DRS_ALPHATESTENABLE,false); 
    }

    // set one of the 2 pixel shaders based on the combined GPI map
    if (sh->m_flags & SHDR_FLAG_GPI_MAP)
    {
      if (gpi==false)
      {
        device->SetPixelShader(GetStockPS(PIXEL_SHADER_DIFFUSE_GPI));
        gpi=true;
      }
    }
    else
    {
      if (gpi==true)
      {
        device->SetPixelShader(GetStockPS(PIXEL_SHADER_DIFFUSE));
        gpi=false;
      }
    }

    if (sh->m_textures[Render::Texture::SAMPLER_BASE_MAP]!=0xffffffff)
    {
      Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_BASE_MAP]);
      SetTexture(device,tex,0);
    }
    else
    {
      device->SetTexture(0,0);
    }

    if (sh->m_textures[Render::Texture::SAMPLER_NORMAL_MAP]!=0xffffffff)
    {
      Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_NORMAL_MAP]);
      SetTexture(device,tex,1);
    }
    else
    {
      device->SetTexture(1,0);
    }

    // this will also set the combined GPI map
    if (sh->m_textures[Render::Texture::SAMPLER_GLOSS_MAP]!=0xffffffff)
    {
      Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_GLOSS_MAP]);
      SetTexture(device,tex,2);
    }
    else
    {
      device->SetTexture(2,0);
    }

    if (sh->m_textures[Render::Texture::SAMPLER_PARALLAX_MAP]!=0xffffffff)
    {
      Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_PARALLAX_MAP]);
      SetTexture(device,tex,3);
    }
    else
    {
      device->SetTexture(3,0);
    }

    if (sh->m_textures[Render::Texture::SAMPLER_INCAN_MAP]!=0xffffffff)
    {
      Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_INCAN_MAP]);
      SetTexture(device,tex,4);
    }
    else
    {
      device->SetTexture(4,0);
    }

    float shader_consts[20];

    //c10
    shader_consts[0] = sh->m_normal_scale;            // normal map scale
    shader_consts[1] = 0.0f;
    shader_consts[2] = 0.0f;
    shader_consts[3] = sh->m_incan_scale;
    //c11
    shader_consts[4] = sh->m_parallax_scale;             // parallax scale
    shader_consts[5] = sh->m_parallax_bias;             // parallax bias
    shader_consts[6] = 0.0f;
    shader_consts[7] = 0.0f;
    //c12
    shader_consts[8] = env_scale;         // env scale
    shader_consts[9] = env_bias;          // env bias
    shader_consts[10]= sh->m_env_lod;     // env tex load
    shader_consts[11]= sh->m_gloss_scale; // gloss scale
    //c13
    shader_consts[12]=sh->m_glosstint[0];
    shader_consts[13]=sh->m_glosstint[1];
    shader_consts[14]=sh->m_glosstint[2];
    shader_consts[15]=sh->m_glosstint[3];
    //c14
    shader_consts[16]=sh->m_basetint[0];
    shader_consts[17]=sh->m_basetint[1];
    shader_consts[18]=sh->m_basetint[2];
    shader_consts[19]=sh->m_basetint[3];

    device->SetPixelShaderConstantF(10,shader_consts,5);

    if (mesh->m_fragments[i].m_prim_count)
    {
      device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,mesh->m_vert_count,mesh->m_fragments[i].m_base_index,mesh->m_fragments[i].m_prim_count);
    }
  }

  device->SetRenderState(D3DRS_ALPHATESTENABLE,false); 

  // don't write Z for alpha
  device->SetRenderState(D3DRS_ZWRITEENABLE,false);
  for (u32 i=0;i<mesh->m_fragment_count;i++)
  {
    // set the shader for this fragment
    Shader* sh = m_shader_manager.ResolveShader(scene->m_shader_table[i]);

    // we only want alpha on the second pass
    if ((sh->m_alpha_type==Shader::ALPHA_OPAQUE) || (sh->m_alpha_type==Shader::ALPHA_CUTOUT))
      continue;

    if (sh->m_flags&SHDR_FLAG_TWO_SIDED)
    {
      device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
    }
    else
    {
      device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
    }

    switch (sh->m_alpha_type)
    {
    case Shader::ALPHA_ADDITIVE:
      device->SetRenderState(D3DRS_ALPHABLENDENABLE,true); 
      device->SetRenderState(D3DRS_ALPHATESTENABLE,false); 
      device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
      device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
      break;
    case Shader::ALPHA_BLENDED:
      device->SetRenderState(D3DRS_ALPHABLENDENABLE,true); 
      device->SetRenderState(D3DRS_ALPHATESTENABLE,false); 
      device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
      device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
      break;
    }

    // set one of the 2 pixel shaders based on the combined GPI map
    if (sh->m_flags & SHDR_FLAG_GPI_MAP)
    {
      if (gpi==false)
      {
        device->SetPixelShader(GetStockPS(PIXEL_SHADER_DIFFUSE_GPI));
        gpi=true;
      }
    }
    else
    {
      if (gpi==true)
      {
        device->SetPixelShader(GetStockPS(PIXEL_SHADER_DIFFUSE));
        gpi=false;
      }
    }

    if (sh->m_textures[Render::Texture::SAMPLER_BASE_MAP]!=0xffffffff)
    {
      Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_BASE_MAP]);
      SetTexture(device,tex,0);
    }
    else
    {
      device->SetTexture(0,0);
    }

    if (sh->m_textures[Render::Texture::SAMPLER_NORMAL_MAP]!=0xffffffff)
    {
      Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_NORMAL_MAP]);
      SetTexture(device,tex,1);
    }
    else
    {
      device->SetTexture(1,0);
    }

    // this will also set the combined GPI map
    if (sh->m_textures[Render::Texture::SAMPLER_GLOSS_MAP]!=0xffffffff)
    {
      Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_GLOSS_MAP]);
      SetTexture(device,tex,2);
    }
    else
    {
      device->SetTexture(2,0);
    }

    if (sh->m_textures[Render::Texture::SAMPLER_PARALLAX_MAP]!=0xffffffff)
    {
      Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_PARALLAX_MAP]);
      SetTexture(device,tex,3);
    }
    else
    {
      device->SetTexture(3,0);
    }

    if (sh->m_textures[Render::Texture::SAMPLER_INCAN_MAP]!=0xffffffff)
    {
      Render::Texture* tex = m_shader_manager.ResolveTexture(sh->m_textures[Render::Texture::SAMPLER_INCAN_MAP]);
      SetTexture(device,tex,4);
    }
    else
    {
      device->SetTexture(4,0);
    }

    float shader_consts[20];

    //c10
    shader_consts[0] = sh->m_normal_scale;            // normal map scale
    shader_consts[1] = 0.0f;
    shader_consts[2] = 0.0f;
    shader_consts[3] = sh->m_incan_scale;
    //c11
    shader_consts[4] = sh->m_parallax_scale;             // parallax scale
    shader_consts[5] = sh->m_parallax_bias;             // parallax bias
    shader_consts[6] = 0.0f;
    shader_consts[7] = 0.0f;
    //c12
    shader_consts[8] = env_scale;         // env scale
    shader_consts[9] = env_bias;          // env bias
    shader_consts[10]= sh->m_env_lod;     // env tex load
    shader_consts[11]= sh->m_gloss_scale; // gloss scale
    //c13
    shader_consts[12]=sh->m_glosstint[0];
    shader_consts[13]=sh->m_glosstint[1];
    shader_consts[14]=sh->m_glosstint[2];
    shader_consts[15]=sh->m_glosstint[3];
    //c14
    shader_consts[16]=sh->m_basetint[0];
    shader_consts[17]=sh->m_basetint[1];
    shader_consts[18]=sh->m_basetint[2];
    shader_consts[19]=sh->m_basetint[3];

    device->SetPixelShaderConstantF(10,shader_consts,5);

    if (mesh->m_fragments[i].m_prim_count)
    {
      device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,mesh->m_vert_count,mesh->m_fragments[i].m_base_index,mesh->m_fragments[i].m_prim_count);
    }
  }

  device->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
  device->SetRenderState(D3DRS_ZWRITEENABLE,true);
  device->SetVertexShader(0);
  device->SetPixelShader(0);
  device->SetStreamSource(0,0,0,0);
  device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::Renderer::RenderReferenceGrid(Render::Scene* scene)
{
  IDirect3DDevice9* device = GetD3DDevice();

  if (!scene->m_render_reference_grid)
    return;

  // set the shader state
  device->SetVertexDeclaration( GetStockDecl(VERTEX_DECL_DEBUG));
  device->SetVertexShader(GetStockVS(VERTEX_SHADER_WORLD_SPACE));
  device->SetPixelShader(GetStockPS(PIXEL_SHADER_COLOR));

  VertexDebug   verts[40];

  float scale = 10.0f;
  if (scene->m_scene_scale>0.0f)
    scale = scene->m_scene_scale*2;

  // xaxis
  verts[0].m_pos[0] = 0.0f;
  verts[0].m_pos[1] = 0.0f;
  verts[0].m_pos[2] = 0.0f;
  verts[0].m_color = D3DCOLOR_ARGB(0xff,0xff,0,0);
  verts[1].m_pos[0] = scale;
  verts[1].m_pos[1] = 0.0f;
  verts[1].m_pos[2] = 0.0f;
  verts[1].m_color = D3DCOLOR_ARGB(0xff,0xff,0,0);

  // yaxis
  verts[2].m_pos[0] = 0.0f;
  verts[2].m_pos[1] = 0.0f;
  verts[2].m_pos[2] = 0.0f;
  verts[2].m_color = D3DCOLOR_ARGB(0xff,0,0xff,0);
  verts[3].m_pos[0] = 0.0f;
  verts[3].m_pos[1] = scale;
  verts[3].m_pos[2] = 0.0f;
  verts[3].m_color = D3DCOLOR_ARGB(0xff,0,0xff,0);

  // zaxis
  verts[4].m_pos[0] = 0.0f;
  verts[4].m_pos[1] = 0.0f;
  verts[4].m_pos[2] = 0.0f;
  verts[4].m_color = D3DCOLOR_ARGB(0xff,0,0,0xff);
  verts[5].m_pos[0] = 0.0f;
  verts[5].m_pos[1] = 0.0f;
  verts[5].m_pos[2] = scale;
  verts[5].m_color = D3DCOLOR_ARGB(0xff,0,0,0xff);

  device->DrawPrimitiveUP(D3DPT_LINELIST,3,verts,16);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::Renderer::RenderScene(Render::Scene* scene)
{
  RenderScenes( 1, &scene );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::Renderer::RenderScenes(int num, Render::Scene** scenes)
{
  IDirect3DDevice9* device = GetD3DDevice();

  device->BeginScene();
  device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

  device->SetRenderTarget(0,GetBackBuffer());
  device->SetDepthStencilSurface(GetDepthBuffer());

  for ( int i = 0; i < num; ++i )
  {
    Render::Scene* scene = scenes[ i ];

    // compute the view proj and set in constant 0
    D3DXMATRIX viewproj = scene->m_viewmat*scene->m_projmat;
    D3DXMATRIX vp_trans;
    D3DXMatrixTranspose(&vp_trans,&viewproj);
    device->SetVertexShaderConstantF(0,&vp_trans._11,4);

    // set the local to world mat
    D3DXMATRIX world_trans;
    D3DXMatrixTranspose(&world_trans,&scene->m_worldmat);
    device->SetVertexShaderConstantF(4,&world_trans._11,4);

    // set the viewport
    D3DVIEWPORT9 vp;
    vp.X = scene->m_xpos;
    vp.Y = scene->m_ypos;
    vp.Width = scene->m_width;
    vp.Height = scene->m_height;
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    device->SetViewport(&vp);
    
    // for the computed final spherical harmonic
    D3DXVECTOR4 final_sh[9];

    float exposure[4];
    exposure[0] = scene->m_expsoure;
    exposure[1] = scene->m_diffuse_light_scale;
    exposure[2] = 0.0f;
    exposure[3] = 0.0f;
    device->SetPixelShaderConstantF(0,exposure,1);

    float screen_info[4];
    screen_info[0] = (float)scene->m_width/2;
    screen_info[1] = (float)scene->m_height/2;
    screen_info[2] = 0.0f;
    screen_info[3] = 0.0f;
    device->SetVertexShaderConstantF(8,screen_info,1);

    float view_det;
    D3DXMATRIX inv_view;
    D3DXMatrixInverse(&inv_view,&view_det,&scene->m_viewmat);  
    device->SetVertexShaderConstantF(9,&inv_view._41,1);     // get the eye post form the inverse view mat

    if (scene->m_environment!=0xffffffff && scene->m_draw_mode==DRAW_MODE_NORMAL)
    {
      Render::Environment* env = scene->ResolveEnvironmentHandle(scene->m_environment);

      if ( i == 0 )
      {
        RenderEnvironment(scene,env,viewproj);
      }

      for (u32 s=0;s<9;s++)
        final_sh[s]=env->m_sh[s];     // don't worry about the sh exposure here, it will be accounted for in the fragment program
    }
    else
    {
      // if there is no environment or we are using a debug mode start with nothing in the sh and clear the screen to gray
      if ( i == 0 )
      {
        device->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_STENCIL|D3DCLEAR_ZBUFFER,D3DCOLOR_ARGB(0xFF,0xB8,0xB8,0xB8),1.0f,0);
      }

      for (u32 s=0;s<9;s++)
        final_sh[s]=D3DXVECTOR4(0,0,0,0);
    }
    
    AddAmbientToSH(final_sh, scene->m_ambient);

    // add the lights in the scene and finalize the sh
    for (u32 l=0;l<(u32)scene->m_lights.size();l++)
    {
      AddLightToSH(final_sh, scene->m_lights[l]->m_color, scene->m_lights[l]->m_direction);
    }
    FinalizeSH(final_sh);

    device->SetPixelShaderConstantF(1,&final_sh[0].x,9);

    if (scene->m_mesh_handle!=0xffffffff)
    {
      Render::Mesh* mesh = scene->ResolveMeshHandle(scene->m_mesh_handle);
      if (scene->m_draw_mode==DRAW_MODE_NORMAL)
        RenderMesh(scene,mesh);
      else
        RenderDebug(scene,mesh);
        
      if (scene->m_render_wireframe)
        RenderWireframe(scene,mesh);

      if (scene->m_render_normals)
        RenderNormals(scene,mesh);

      if (scene->m_render_tangents)
        RenderTangents(scene,mesh);
    }
  }

  RenderReferenceGrid( scenes[ 0 ] );

  for ( u32 stage = 0; stage < 6; ++stage )
  {
    device->SetTexture( stage, NULL );
  }

  device->EndScene();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
u32 Render::Renderer::GetPixel(u32 x, u32 y)
{
  IDirect3DSurface9* surface = GetBufferData();
  if (surface==0)
    return 0xffffffff;

  D3DLOCKED_RECT lr;

  u32 ret=0xffffffff;
  surface->LockRect(&lr,0,0);

  u32* pixels = (u32*)lr.pBits;
  u32 stride = lr.Pitch/4;

  ret = pixels[(y*stride)+x];

  surface->UnlockRect();
  surface->Release();

  return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
u32 Render::Renderer::GetIndex(u32 x, u32 y)
{
  u32 col = GetPixel(x,y);
  if (col==0)
    return 0xffffffff;
  return (u16)(HashRGBToU16(col)-7);
}
