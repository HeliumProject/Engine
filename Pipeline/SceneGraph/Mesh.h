#pragma once

#include "Foundation/Math/CalculateBounds.h"
#include "Foundation/Math/AlignedBox.h"

#include "Pipeline/SceneGraph/PivotTransform.h"
#include "Pipeline/SceneGraph/Shader.h"
#include "Pipeline/SceneGraph/IndexResource.h"
#include "Pipeline/SceneGraph/VertexResource.h"

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

            MeshEdge( uint32_t vertIndex1, uint32_t vertIndex2 )
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

            uint32_t m_VertIndices[2];
        };
        typedef std::set<MeshEdge> S_MeshEdge;

        class Mesh : public PivotTransform
        {
        public:
            REFLECT_DECLARE_OBJECT( Mesh, PivotTransform );
            static void AcceptCompositeVisitor( Reflect::Composite& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            Mesh();
            ~Mesh();

            virtual int32_t GetImageIndex() const HELIUM_OVERRIDE;
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

            uint32_t GetVertexCount() const
            {
                return (uint32_t)m_Positions.size();
            }

            uint32_t GetTriangleCount() const
            {
                return (uint32_t)m_TriangleVertexIndices.size()/3;
            }

            void ComputeTNBs();
            bool ComputeTNB( uint32_t triIndex );

            void GetAlignedBoundingBox( AlignedBox& box ) const;
            void GetBoundingSphere( BoundingVolumeGenerator::BSphere& bsphere ) const;

            // surface area of the mesh in m^2
            float32_t SurfaceArea( Scale* scale = NULL ) const;

            // can multiply each component by that component's scale^2 and sum to get scaled area
            // scaledArea = (areaVec.x * scale.x^2) + (areaVec.y * scale.y^2) + (areaVec.z * scale.z^2)
            float32_t SurfaceAreaComponents( Vector3& areaVec ) const;

            // verts-per-meter
            float32_t VertDensity() const;

            void GetEdges( S_MeshEdge& edges ) const;
            void GetTriangle( uint32_t triIndex, Vector3& v0, Vector3& v1, Vector3& v2, Matrix4* transform = NULL );
            void WeldMeshVerts(const float32_t vertex_merge_threshold);
            uint32_t  GetEdgeIdForVerts(uint32_t vert_a, uint32_t vert_b);
            void AddTri(uint32_t vert_a, uint32_t vert_b, uint32_t vert_c);
            uint32_t  GetClosestTri(const Vector3& sphere_pos, const float32_t& sphere_rad);
            void NopTrisByTriList(const std::vector< uint32_t >& ip_tris);
            void NopTrisByVertList(const std::vector< uint32_t >& ip_verts);
            void NopTrisByEdgeList( const std::vector< uint32_t >& ip_edges );
            void PruneVertsNotInTris();
            void DeleteTris(const std::vector< uint32_t >& ip_tris);
            void DeleteVerts(const std::vector< uint32_t >& ip_verts);
            void DeleteEdges( const std::vector< uint32_t >& ip_edges );
            uint32_t  GetClosestVert(const Vector3& sphere_start_pos, const float32_t& sphere_Rad, const Vector3& swept_dir, const float32_t& len);
            uint32_t  GetClosestVert(const Matrix4& view_proj_mat, const float32_t porj_space_threshold_sqr, Vector2 proj_pt);
            uint32_t  GetClosestEdge(const Vector3& sphere_start_pos, const float32_t& sphere_Rad, const Vector3& swept_dir, const float32_t& len);
            uint32_t  GetClosestTri(const Vector3& sphere_start_pos, const float32_t& sphere_rad, const Vector3& swept_dir, const float32_t& len);
            void PruneInvalidTris();
            void MergeVertToClosest(uint32_t vert_id);
            void PunchCubeHole(Matrix4& mat, Matrix4& inv_mat, float32_t vert_merge_threshold);

            uint32_t AddShader( Shader* shader );

            // temp hack
            friend class Skin;

        public:
            //
            // Per vertex data
            //  - All of these arrays have a common size because they are indexed with the same indices
            //  - All the elements of these are sorted by the shader they utilize
            //

            // The vertices that define the geometry of the mesh
            V_Vector3     m_Positions;

            // The vertex normals (disjoint from m_Vertices)
            V_Vector3     m_Normals;

            // vertex tangents -- these are not currently serialized
            V_Vector3     m_Tangents;

            // vertex binormals -- these are not currently serialized
            V_Vector3     m_Binormals;

            // The vertex colors
            V_Vector4     m_Colors;

            // The uv values for the base uv set
            V_Vector2     m_BaseUVs;

            //
            // Indexed mesh data, indexes into above data
            //

            // The indices for the wireframe drawing (a line list)
            std::vector< uint32_t >  m_WireframeVertexIndices;

            // The indices into the above arrays (vertex position, normal, color, etc)
            std::vector< uint32_t >  m_TriangleVertexIndices;

            //
            // Per triangle data, all of the sizes of these will be bounded by 1/3 of the vertex count
            //

            // The index of the polygon each triangle is triangulated from
            std::vector< uint32_t >  m_PolygonIndices; // size is number of tris

            // The shader index for each triangle
            std::vector< uint32_t >  m_ShaderIndices; // size is number of tris

            //
            // Per mesh data
            //

            // The shaders referenced by all of the polygons of this mesh
            Helium::V_TUID      m_ShaderIDs;

            // The counts of the number of triangles for each shader used
            std::vector< uint32_t >  m_ShaderTriangleCounts;

        protected:
            // Non-reflected
            bool                m_HasAlpha;             // does this mesh draw with alpha?
            bool                m_HasColor;             // does this mesh have per-vertex color?
            bool                m_HasTexture;           // does thie mesh have texture data?
            V_ShaderDumbPtr     m_Shaders;              // the shader objects this mesh uses
            uint32_t                 m_LineCount;            // the number of lines to draw
            uint32_t                 m_VertexCount;          // the number of vertices
            uint32_t                 m_TriangleCount;        // the number of triangles to draw
            std::vector< uint32_t >  m_ShaderStartIndices;   // the start index of each shader-sorted segment of indices
            static D3DMATERIAL9 s_WireMaterial;
            static D3DMATERIAL9 s_FillMaterial;
            IndexResourcePtr    m_Indices;
            VertexResourcePtr   m_Vertices;
        };
        typedef Helium::StrongPtr< Mesh > MeshPtr;
    }
}