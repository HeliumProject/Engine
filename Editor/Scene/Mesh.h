#pragma once

#include "PivotTransform.h"
#include "IndexResource.h"
#include "VertexResource.h"
#include "Shader.h"

// Forwards
namespace Content
{
  class Mesh;
}

namespace Editor
{
  // the shader we reference
  typedef std::vector< Shader* > V_ShaderDumbPtr;

  // these gather arrayed components in the content class into a concrete vertex
  typedef std::vector< StandardVertex > V_StandardVertex;

  class Mesh : public Editor::PivotTransform
  {
    //
    // Member variables
    //
    
  protected:
    // does this mesh draw with alpha?
    bool m_HasAlpha;

    // does this mesh have per-vertex color?
    bool m_HasColor;

    // does thie mesh have texture data?
    bool m_HasTexture;

    // the shader objects this mesh uses
    V_ShaderDumbPtr m_Shaders;

    // the number of lines to draw
    u32 m_LineCount;

    // the number of vertices
    u32 m_VertexCount;

    // the number of triangles to draw
    u32 m_TriangleCount;

    // the start index of each shader-sorted segment of indices
    std::vector< u32 > m_ShaderStartIndices;

    // materials
    static D3DMATERIAL9 s_WireMaterial;
    static D3DMATERIAL9 s_FillMaterial;

    // resources
    IndexResourcePtr m_Indices;
    VertexResourcePtr m_Vertices;


    //
    // Runtime Type Info
    //

    LUNA_DECLARE_TYPE( Editor::Mesh, Editor::PivotTransform );
    static void InitializeType();
    static void CleanupType();


    // 
    // Member functions
    //

  public:
    Mesh( Editor::Scene* scene, Content::Mesh* mesh );
    virtual ~Mesh();

    virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
    virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

    virtual void Initialize() HELIUM_OVERRIDE;
    virtual void Create() HELIUM_OVERRIDE;
    virtual void Delete() HELIUM_OVERRIDE;
    virtual void Populate( PopulateArgs* args );
    virtual void Evaluate( GraphDirection direction ) HELIUM_OVERRIDE;

    virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;

    static void SetupNormalObject( IDirect3DDevice9* device, const SceneNode* object );
    static void SetupFlippedObject( IDirect3DDevice9* device, const SceneNode* object );
    static void ResetFlippedObject( IDirect3DDevice9* device, const SceneNode* object );

    static void SetupNormalWire( IDirect3DDevice9* device );
    static void DrawNormalWire( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawUnselectableWire( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    static void SetupUnselectableWire( IDirect3DDevice9* device );
    static void SetupSelectedWire( IDirect3DDevice9* device );
    static void SetupHighlightedWire( IDirect3DDevice9* device );
    static void SetupLiveWire( IDirect3DDevice9* device );

    static void SetupAlpha( IDirect3DDevice9* device );
    static void ResetAlpha( IDirect3DDevice9* device );
    static void SetupNormal( IDirect3DDevice9* device );
    static void ResetNormal( IDirect3DDevice9* device );
    static void DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

    // temp hack
    friend class Skin;
  };
}
