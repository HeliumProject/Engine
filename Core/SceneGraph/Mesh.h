#pragma once

#include "Foundation/Math/CalculateBounds.h"
#include "Foundation/Math/AlignedBox.h"

#include "Core/SceneGraph/PivotTransform.h"
#include "Core/SceneGraph/Shader.h"
#include "Core/SceneGraph/IndexResource.h"
#include "Core/SceneGraph/VertexResource.h"

namespace Helium
{
    namespace SceneGraph
    {
        // the shader we reference
        typedef std::vector< Shader* > V_ShaderDumbPtr;

        // these gather arrayed components in the content class into a concrete vertex
        typedef std::vector< StandardVertex > V_StandardVertex;

        class MeshEdge
        {
        public:
            MeshEdge()
            {
                m_VertIndices[0] = 0;
                m_VertIndices[1] = 0;
            }

            MeshEdge( u32 vertIndex1, u32 vertIndex2 )
            {
                HELIUM_ASSERT( vertIndex1 != vertIndex2 );

                if( vertIndex1 > vertIndex2 )
                {
                    m_VertIndices[0] = vertIndex1;
                    m_VertIndices[1] = vertIndex2;
                }
                else
                {
                    m_VertIndices[1] = vertIndex1;
                    m_VertIndices[0] = vertIndex2;
                }
            }

            bool operator <( const MeshEdge& rhs ) const
            {
                if( m_VertIndices[0] < rhs.m_VertIndices[0] )
                    return true;
                else
                    return false;

                if( m_VertIndices[1] < rhs.m_VertIndices[1] )
                    return true;

                return false;        
            }

            u32 m_VertIndices[2];
        };
        typedef std::set<MeshEdge> S_MeshEdge;

        class Mesh : public PivotTransform
        {
        public:
            REFLECT_DECLARE_CLASS( Mesh, PivotTransform );
            static void EnumerateClass( Reflect::Compositor<Mesh>& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            Mesh();
            ~Mesh();

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual void Initialize(Scene* scene) HELIUM_OVERRIDE;
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

            u32 GetVertexCount() const
            {
                return (u32)m_Positions.size();
            }

            u32 GetTriangleCount() const
            {
                return (u32)m_TriangleVertexIndices.size()/3;
            }

            void ComputeTNBs();
            bool ComputeTNB( u32 triIndex );

            void GetAlignedBoundingBox( Math::AlignedBox& box ) const;
            void GetBoundingSphere( Math::BoundingVolumeGenerator::BSphere& bsphere ) const;

            // surface area of the mesh in m^2
            f32 SurfaceArea( Math::Scale* scale = NULL ) const;

            // can multiply each component by that component's scale^2 and sum to get scaled area
            // scaledArea = (areaVec.x * scale.x^2) + (areaVec.y * scale.y^2) + (areaVec.z * scale.z^2)
            f32 SurfaceAreaComponents( Math::Vector3& areaVec ) const;

            // verts-per-meter
            f32 VertDensity() const;

            void GetEdges( S_MeshEdge& edges ) const;
            void GetTriangle( u32 triIndex, Math::Vector3& v0, Math::Vector3& v1, Math::Vector3& v2, Math::Matrix4* transform = NULL );
            void WeldMeshVerts(const f32 vertex_merge_threshold);
            u32  GetEdgeIdForVerts(u32 vert_a, u32 vert_b);
            void AddTri(u32 vert_a, u32 vert_b, u32 vert_c);
            u32  GetClosestTri(const Math::Vector3& sphere_pos, const f32& sphere_rad);
            void NopTrisByTriList(const std::vector< u32 >& ip_tris);
            void NopTrisByVertList(const std::vector< u32 >& ip_verts);
            void NopTrisByEdgeList( const std::vector< u32 >& ip_edges );
            void PruneVertsNotInTris();
            void DeleteTris(const std::vector< u32 >& ip_tris);
            void DeleteVerts(const std::vector< u32 >& ip_verts);
            void DeleteEdges( const std::vector< u32 >& ip_edges );
            u32  GetClosestVert(const Math::Vector3& sphere_start_pos, const f32& sphere_Rad, const Math::Vector3& swept_dir, const f32& len);
            u32  GetClosestVert(const Math::Matrix4& view_proj_mat, const f32 porj_space_threshold_sqr, Math::Vector2 proj_pt);
            u32  GetClosestEdge(const Math::Vector3& sphere_start_pos, const f32& sphere_Rad, const Math::Vector3& swept_dir, const f32& len);
            u32  GetClosestTri(const Math::Vector3& sphere_start_pos, const f32& sphere_rad, const Math::Vector3& swept_dir, const f32& len);
            void PruneInvalidTris();
            void MergeVertToClosest(u32 vert_id);
            void PunchCubeHole(Math::Matrix4& mat, Math::Matrix4& inv_mat, f32 vert_merge_threshold);

            // temp hack
            friend class Skin;

        public:
            //
            // Per vertex data
            //  - All of these arrays have a common size because they are indexed with the same indices
            //  - All the elements of these are sorted by the shader they utilize
            //

            // The vertices that define the geometry of the mesh
            Math::V_Vector3     m_Positions;

            // The vertex normals (disjoint from m_Vertices)
            Math::V_Vector3     m_Normals;

            // vertex tangents -- these are not currently serialized
            Math::V_Vector3     m_Tangents;

            // vertex binormals -- these are not currently serialized
            Math::V_Vector3     m_Binormals;

            // The vertex colors
            Math::V_Vector4     m_Colors;

            // The uv values for the base uv set
            Math::V_Vector2     m_BaseUVs;

            //
            // Indexed mesh data, indexes into above data
            //

            // The indices for the wireframe drawing (a line list)
            std::vector< u32 >  m_WireframeVertexIndices;

            // The indices into the above arrays (vertex position, normal, color, etc)
            std::vector< u32 >  m_TriangleVertexIndices;

            //
            // Per triangle data, all of the sizes of these will be bounded by 1/3 of the vertex count
            //

            // The index of the polygon each triangle is triangulated from
            std::vector< u32 >  m_PolygonIndices; // size is number of tris

            // The shader index for each triangle
            std::vector< u32 >  m_ShaderIndices; // size is number of tris

            //
            // Per mesh data
            //

            // The shaders referenced by all of the polygons of this mesh
            Helium::V_TUID      m_ShaderIDs;

            // The counts of the number of triangles for each shader used
            std::vector< u32 >  m_ShaderTriangleCounts;

        protected:
            // Non-reflected
            bool                m_HasAlpha;             // does this mesh draw with alpha?
            bool                m_HasColor;             // does this mesh have per-vertex color?
            bool                m_HasTexture;           // does thie mesh have texture data?
            V_ShaderDumbPtr     m_Shaders;              // the shader objects this mesh uses
            u32                 m_LineCount;            // the number of lines to draw
            u32                 m_VertexCount;          // the number of vertices
            u32                 m_TriangleCount;        // the number of triangles to draw
            std::vector< u32 >  m_ShaderStartIndices;   // the start index of each shader-sorted segment of indices
            static D3DMATERIAL9 s_WireMaterial;
            static D3DMATERIAL9 s_FillMaterial;
            IndexResourcePtr    m_Indices;
            VertexResourcePtr   m_Vertices;
        };
    }
}