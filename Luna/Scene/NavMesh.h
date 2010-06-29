#pragma once

#include "Mesh.h"
#include "IndexResource.h"
#include "VertexResource.h"

// Forwards
namespace Content
{
  class Mesh;
}

namespace Math
{
  class Vector3;
  class Matrix4;
}

namespace Luna
{
  class PrimitiveLocator;
 
  class NavMesh : public Luna::Mesh
  {
    //
    // Members
    //

  protected:

    // mouse locator
    Luna::PrimitiveLocator* m_Locator;

    // materials to use for rendering
    static D3DMATERIAL9 s_Material;
    static D3DMATERIAL9 s_HullMaterial;

    static D3DMATERIAL9 s_HiResVertDisplayMaterial;
    static D3DMATERIAL9 s_LowResVertDisplayMaterial;
    static D3DMATERIAL9 s_HiResEdgeDisplayMaterial;
    static D3DMATERIAL9 s_LowResEdgeDisplayMaterial;
    static D3DMATERIAL9 s_HiResTriDisplayMaterial;
    static D3DMATERIAL9 s_LowResTriDisplayMaterial;
    static D3DMATERIAL9 s_HiResTriSubDuedDisplayMaterial;
    static D3DMATERIAL9 s_LowResTriSubDuedDisplayMaterial;
    static D3DMATERIAL9 s_MouseOverTriMaterial;
    static D3DMATERIAL9 s_SelectedTriMaterial;

    // resources
    //VertexResourcePtr m_Vertices;



    //
    // RTTI
    //

    LUNA_DECLARE_TYPE( Luna::NavMesh, Luna::Mesh );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    NavMesh( Luna::Scene* scene, Content::Mesh* mesh );
    virtual ~NavMesh();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual tstring GetApplicationTypeName() const NOC_OVERRIDE;

    virtual void Initialize() NOC_OVERRIDE;

    u32 GetNumberVertices() const;

    void UpdateLocator( const Math::Vector3& position ) { m_LocatorPos = position; }

    // add a vert
    void AddVert( Math::Vector3 position );
    void AddTri();

    // move a vert to a new position
    void UpdateVert( u32 index, Math::Vector3 position );

    //
    // Resources
    //

    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;
    virtual void Populate(PopulateArgs* args);


    //
    // Evaluate/Render/Pick
    //

    virtual void Evaluate( GraphDirection direction ) NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    void    SetUpNavMeshRenderEntry(RenderVisitor* render, RenderEntry* entry);
    static void Draw( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    static void SetupNavObject( IDirect3DDevice9* device, const SceneNode* object );

    static void DrawMeshVerts( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawMeshEdges( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object );
    static void DrawMeshTris( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object );
    static bool IsNavMeshZoneActive(const Luna::NavMesh* nav_mesh);

    static void DrawMouseOverVert( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawSelectedVerts( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawToBeDeletedVerts( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawMouseOverEdge( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawSelectedEdge( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object );
    static void DrawMouseOverTri( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object );
    static void DrawSelectedTri( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object );
    static void DrawToBeDeletedTris( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object );
    static void DrawLocator( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object );

    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;


    //
    // UI
    //

    virtual bool ValidatePanel(const tstring& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

  private:

    u32 m_SelectionMode;

    //
    // Data
    //

  public:
    Math::Vector3 m_LocatorPos;
    bool m_DrawLocator;

    std::vector< u32 >  m_selected_tris;
    std::vector< u32 >  m_selected_verts;
    u32    m_selected_edge; //0, 1, 2
    std::vector< u32 >  m_cached_selected_tris;

    std::vector< u32 >  m_marquee_selected_verts;
    std::vector< u32 >  m_marquee_selected_edges;
    std::vector< u32 >  m_marquee_selected_tris;


    u32    m_mouse_over_vert;
    u32    m_mouse_over_edge;
    u32    m_mouse_over_tri;

    std::vector< u32 >  m_to_be_deleted_tris;
    std::vector< u32 >  m_to_be_deleted_verts;

    void   ClearSelectionState();
    void   ClearEditingState();

    void UpdateMouseOverVert(const Math::Vector3& mouse_ss_start, const f32 threshold_rad, const Math::Vector3& mouse_ss_dir, const f32 ss_len);
    void UpdateMouseOverEdge(const Math::Vector3& mouse_ss_start, const f32 threshold_rad, const Math::Vector3& mouse_ss_dir, const f32 ss_len);
    void UpdateMouseOverTri(const Math::Vector3& mouse_ss_start, const f32 threshold_rad, const Math::Vector3& mouse_ss_dir, const f32 ss_len);

    void UpdateMouseOverVert(const Math::Matrix4& view_proj_mat, const f32 porj_space_threshold_sqr, Math::Vector2 proj_pt);

    bool ToggleMouseOverVertSelection();
    bool ToggleMouseOverEdgeSelection();
    bool ToggleMouseOverTriSelection();
    void ChangeTriEdgeVertSelection();

    void AddMouseOverVertToDeleteQueue();
    void AddMouseOverTriToDeleteQueue();

    void DeleteVertsInQueue();
    void DeleteTrisInQueue();
    void DeleteEdgesInQueue();
    void MergeVertToClosest(u32 vert_to_be_merged);
    void PunchCubeHole(Math::Matrix4& mat, Math::Matrix4& inv_mat, f32 vert_merge_threshold);

    void DeleteVert( const Math::Vector3& position );

    void MoveSelectedVertsByOffset( const Math::Vector3& offset );
    void MoveSelectedEdgeByOffset( const Math::Vector3& offset );
    void MoveSelectedTriByOffset( const Math::Vector3& offset );

    Math::Vector3 GetMouseOverVertPosition();
    void SelectNearestEdge( const Math::Vector3& position );
    std::vector< u32 > GetSelectedVerts() { return m_selected_verts; }
    const Math::Vector3& GetVertPosition( u32 index );

    void SetSelectionMode( u32 mode );

    void GetMeshData( Math::V_Vector3& positions, std::vector< u32 >& triVerts, std::vector< u32 >& wireVerts );
    void UpdateMeshData( const Math::V_Vector3& positions, const std::vector< u32 >& triVerts, const std::vector< u32 >& wireVerts );

    enum MarqueeSelectionMode
    {
      kClearPreviousSelection,
      kAddToPreviousSelection,
      KRemovedFromPeeviousSElection,
    };

    void GetMarqueeSelectedVerts(Math::Frustum& marquee_frustom, MarqueeSelectionMode selection_mode = kClearPreviousSelection );
    void GetMarqueeSelectedTris(Math::Frustum& marquee_frustom, MarqueeSelectionMode selection_mode = kClearPreviousSelection );
    void GetMarqueeSelectedEdges(Math::Frustum& marquee_frustom, MarqueeSelectionMode selection_mode = kClearPreviousSelection );

    void SelectObtuseAngledTris();
  };

  typedef Nocturnal::SmartPtr<Luna::NavMesh> NavMeshPtr;
}
