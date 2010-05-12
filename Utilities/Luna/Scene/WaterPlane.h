#pragma once

#include "PivotTransform.h"
#include "IndexResource.h"
#include "VertexResource.h"
#include "Math/Vector3.h"
#include "Shader.h"

// Forwards
namespace Content
{
  class WaterPlane;
}

namespace Luna
{
  // the shader we reference
  typedef std::vector< Shader* > V_ShaderDumbPtr;

  // these gather arrayed components in the content class into a concrete vertex
  typedef std::vector< StandardVertex > V_StandardVertex;

  class WaterPlane : public Luna::PivotTransform
  {
    //
    // Member variables
    //
    
  protected:
  
    // index list
    V_u32 m_IndexList;
    
    // vertex list
    Math::V_Vector3 m_VertexList;
    Math::V_Vector2 m_UVList;
    
    // water color
    Math::Color3 m_Color;
    
    // clip map
    IDirect3DTexture9* m_ClipMap;
    
    // object counts
    u32 m_LineCount;
    u32 m_TriangleCount;

    // resources
    IndexResourcePtr m_Indices;
    VertexResourcePtr m_Vertices;
    
    // materials
    static D3DMATERIAL9 s_WireMaterial;
    static D3DMATERIAL9 s_FillMaterial;


    //
    // Runtime Type Info
    //

    LUNA_DECLARE_TYPE( Luna::WaterPlane, Luna::PivotTransform );
    static void InitializeType();
    static void CleanupType();


    // 
    // Member functions
    //

  public:
    WaterPlane( Luna::Scene* scene, Content::WaterPlane* waterPlane );
    virtual ~WaterPlane();

    virtual void Initialize() NOC_OVERRIDE;
    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;
    virtual void Populate( PopulateArgs* args );
    virtual void Evaluate( GraphDirection direction ) NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    
    static void SetupNormalObject( IDirect3DDevice9* device, const SceneNode* object );
    static void ResetNormalObject( IDirect3DDevice9* device, const SceneNode* object );

    static void SetupNormalWire( IDirect3DDevice9* device );
    static void SetupSelectedWire( IDirect3DDevice9* device );
    static void SetupHighlightedWire( IDirect3DDevice9* device );
    static void SetupLiveWire( IDirect3DDevice9* device );
    static void DrawNormalWire( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    static void SetupNormal( IDirect3DDevice9* device );
    static void DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    
    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;
  };
}
