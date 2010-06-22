#pragma once

#include <vector>

#include "Pipeline/API.h"

#include "Pipeline/Content/Nodes/Transform/PivotTransform.h"
#include "Foundation/Container/BitArray.h"
#include "Pipeline/Content/ContentTypes.h"
#include "Foundation/Math/CalculateBounds.h"

namespace Math
{
    class AlignedBox; 
}

namespace Content
{
    typedef std::map< Content::ContentType, u32 > M_ContentTypeToIndex;

    typedef std::vector< Math::V_Vector2 > V_UVShell;

    namespace UVSetTypes
    {
        enum UVSetType
        {
            Base,
            Lightmap,
            Blend,
        };
    };

    typedef UVSetTypes::UVSetType UVSetType;

    /////////////////////////////////////////////////////////////////////////////
    class PIPELINE_API MorphTargetDelta : public Reflect::Element
    {
    public: 
        // m_VertexIndex is an index into m_TriangleVertexIndices
        u32           m_VertexIndex;
        Math::Vector3 m_PosVector;
        Math::Vector3 m_NormalVector;

        MorphTargetDelta()
            : m_VertexIndex( 0 )
        {
        }

        MorphTargetDelta( const u32 vertexIndex, const Math::Vector3& posVector, const Math::Vector3& normalVector )
            : m_VertexIndex( vertexIndex )
            , m_PosVector( posVector )
            , m_NormalVector( normalVector )
        {
        }

        REFLECT_DECLARE_CLASS( MorphTargetDelta, Reflect::Element );

        static void EnumerateClass( Reflect::Compositor<MorphTargetDelta>& comp )
        {
            Reflect::Field* fieldVertexIndex = comp.AddField( &MorphTargetDelta::m_VertexIndex, "m_VertexIndex" );
            Reflect::Field* fieldPosVector = comp.AddField( &MorphTargetDelta::m_PosVector, "m_PosVector" );
            Reflect::Field* fieldNormalVector = comp.AddField( &MorphTargetDelta::m_NormalVector, "m_NormalVector" );
        }
    };
    typedef Nocturnal::SmartPtr< MorphTargetDelta > MorphTargetDeltaPtr;
    typedef std::vector< MorphTargetDeltaPtr > V_MorphTargetDelta;


    /////////////////////////////////////////////////////////////////////////////
    class PIPELINE_API MorphTarget : public Reflect::Element
    {
    public:
        std::string          m_Name;
        Nocturnal::TUID m_Id;
        V_MorphTargetDelta   m_Deltas;

        MorphTarget()
        {
        }

        MorphTarget( const std::string& name, const Nocturnal::TUID& id )
            : m_Name( name )
            , m_Id( id )
        {
        }

        void AddTargetDelta( const u32 vertexIndex, const Math::Vector3& vector, const Math::Vector3& normalVector )
        {
            m_Deltas.push_back( new MorphTargetDelta( vertexIndex, vector, normalVector ) );
        }

        REFLECT_DECLARE_CLASS( MorphTarget, Reflect::Element );

        static void EnumerateClass( Reflect::Compositor<MorphTarget>& comp )
        {
            Reflect::Field* fieldName = comp.AddField( &MorphTarget::m_Name, "m_Name" );
            Reflect::Field* fieldId = comp.AddField( &MorphTarget::m_Id, "m_Id" );
            Reflect::Field* fieldDeltas = comp.AddField( &MorphTarget::m_Deltas, "m_Deltas" );
        }
    };
    typedef Nocturnal::SmartPtr<MorphTarget> MorphTargetPtr;
    typedef std::vector<MorphTargetPtr> V_MorphTargetPtr;

    /////////////////////////////////////////////////////////////////////////////
    // Triangulated Mesh with per vertex normals, colors, and multiple UV sets.
    //  Includes indices for drawing wireframe polygons
    //  Includes original source polygon indices
    //  Shader information per triangle
    //
    class PIPELINE_API Mesh : public PivotTransform
    {
    public:
        enum MeshOriginType
        {
            Maya = 0,
            NavHiRes,
            NavLowRes,

            // Used as Count, should always be the last in the enum
            Unknown,
        };
        static void MeshOriginTypeEnumeration( Reflect::Enumeration* info )
        {
            info->AddElement(Maya, "Maya");
            info->AddElement(NavHiRes, "NavHiRes");
            info->AddElement(NavLowRes, "NavLowRes");
            info->AddElement(Unknown, "Unknown");
        }

        class Edge
        {
        public:
            u32 m_VertIndices[2];

        public:
            Edge( u32 vertIndex1, u32 vertIndex2 )
            {
                NOC_ASSERT( vertIndex1 != vertIndex2 );

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

            bool operator < ( const Edge& rhs) const
            {
                if( m_VertIndices[0] < rhs.m_VertIndices[0] )
                    return true;
                else
                    return false;

                if( m_VertIndices[1] < rhs.m_VertIndices[1] )
                    return true;

                return false;        
            }
        };
        typedef std::vector<Edge> V_Edge;
        typedef std::set<Edge> S_Edge;

    public:

        virtual void Host(ContentVisitor* visitor); 

        //
        // Per vertex data
        //  - All of these arrays have a common size because they are indexed with the same indices
        //  - All the elements of these are sorted by the shader they utilize
        //

        // The vertices that define the geometry of the mesh
        Math::V_Vector3 m_Positions;

        // The vertex normals (disjoint from m_Vertices)
        Math::V_Vector3 m_Normals;

        // vertex tangents -- these are not currently serialized
        Math::V_Vector3 m_Tangents;

        // vertex binormals -- these are not currently serialized
        Math::V_Vector3 m_Binormals;

        // The vertex colors
        Math::V_Vector4 m_Colors;

        // The uv values for the base uv set
        Math::V_Vector2 m_BaseUVs;

        // The uv values for the light map uv set (possibly empty)
        Math::V_Vector2 m_LightMapUVs;

        // The uv values for the blend map uv set for dual shaders (possibly empty)
        Math::V_Vector2 m_BlendMapUVs;

        //
        // Indexed mesh data, indexes into above data
        //

        // The indices for the wireframe drawing (a line list)
        std::vector< u32 > m_WireframeVertexIndices;

        // The indices into the above arrays (vertex position, normal, color, etc)
        std::vector< u32 > m_TriangleVertexIndices;


        // Optional morphtarget (blend shap) objects
        V_MorphTargetPtr m_MorphTargets;

        //
        // Per triangle data, all of the sizes of these will be bounded by 1/3 of the vertex count
        //

        // The index of the polygon each triangle is triangulated from
        std::vector< u32 > m_PolygonIndices; // size is number of tris

        // The shader index for each triangle
        std::vector< u32 > m_ShaderIndices; // size is number of tris

        // The collision material for each triangle
        std::vector< u32 > m_CollisionMaterialIndices; // size is number of tris

        //
        // Per mesh data
        //

        // The shaders referenced by all of the polygons of this mesh
        Nocturnal::V_TUID m_ShaderIDs;

        // The counts of the number of triangles for each shader used
        std::vector< u32 > m_ShaderTriangleCounts;

        M_ContentTypeToIndex m_ExportTypeIndex;

        MeshOriginType       m_MeshOriginType;

        Mesh ()
        {
            m_MeshOriginType = Maya;

        }

        Mesh (const Nocturnal::TUID& id)
            : PivotTransform (id)
        {
            m_MeshOriginType = Maya;

        }

        REFLECT_DECLARE_CLASS(Mesh, PivotTransform);

        static void EnumerateClass( Reflect::Compositor<Mesh>& comp )
        {
            Reflect::Field* fieldPositions = comp.AddField( &Mesh::m_Positions, "m_Positions" );
            Reflect::Field* fieldNormals = comp.AddField( &Mesh::m_Normals, "m_Normals" );
            Reflect::Field* fieldColors = comp.AddField( &Mesh::m_Colors, "m_Colors" );

            Reflect::Field* fieldBaseUVs = comp.AddField( &Mesh::m_BaseUVs, "m_BaseUVs" );
            Reflect::Field* fieldLightMapUVs = comp.AddField( &Mesh::m_LightMapUVs, "m_LightMapUVs" );
            Reflect::Field* fieldBlendMapUVs = comp.AddField( &Mesh::m_BlendMapUVs, "m_BlendMapUVs" );

            Reflect::Field* fieldWireframeVertexIndices = comp.AddField( &Mesh::m_WireframeVertexIndices, "m_WireframeVertexIndices" );
            Reflect::Field* fieldTriangleVertexIndices = comp.AddField( &Mesh::m_TriangleVertexIndices, "m_TriangleVertexIndices" );

            Reflect::Field* fieldMorphTargets = comp.AddField( &Mesh::m_MorphTargets, "m_MorphTargets" );

            Reflect::Field* fieldPolygonIndices = comp.AddField( &Mesh::m_PolygonIndices, "m_PolygonIndices" );
            Reflect::Field* fieldShaderIndices = comp.AddField( &Mesh::m_ShaderIndices, "m_ShaderIndices" );
            Reflect::Field* fieldCollisionMaterialIndices = comp.AddField( &Mesh::m_CollisionMaterialIndices, "m_CollisionMaterialIndices" );

            Reflect::Field* fieldShaderIDs = comp.AddField( &Mesh::m_ShaderIDs, "m_ShaderIDs" );
            Reflect::Field* fieldShaderTriangleCounts = comp.AddField( &Mesh::m_ShaderTriangleCounts, "m_ShaderTriangleCounts" );

            Reflect::EnumerationField* fieldMeshOriginType = comp.AddEnumerationField( &Mesh::m_MeshOriginType, "m_MeshOriginType" );
        }

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

        // warning -- only valid if this mesh is contained by a Content::Scene
        // returns the index if valid, otherwise, -1
        int GetExportTypeIndex( Content::ContentType exportType ) const;

        void GetAlignedBoundingBox( Math::AlignedBox& box ) const;

        void GetBoundingSphere( Math::BoundingVolumeGenerator::BSphere& bsphere ) const;

        inline bool LightmapUVsExist() const { return !m_LightMapUVs.empty(); }

        inline bool BlendmapUVsExist() const { return !m_BlendMapUVs.empty(); }

        bool LightmapUVsInRange() const;

        bool IsRenderableType() const;

        // surface area of the mesh in m^2
        f32 SurfaceArea( Math::Scale* scale = NULL ) const;

        // can multiply each component by that component's scale^2 and sum to get scaled area
        // scaledArea = (areaVec.x * scale.x^2) + (areaVec.y * scale.y^2) + (areaVec.z * scale.z^2)
        f32 SurfaceAreaComponents( Math::Vector3& areaVec ) const;

        // surface area of the specified uv set in uv units^2
        f32 UVSurfaceArea( UVSetType uvSet ) const;

        f32 LightmapTexelsPerMeter( u32 logTextureSize, Math::Scale* scale = NULL ) const;

        // verts-per-meter
        f32 VertDensity(  ) const;

        void GetEdges( S_Edge& edges ) const;

        void GetTriangle( u32 triIndex, Math::Vector3& v0, Math::Vector3& v1, Math::Vector3& v2, Math::Matrix4* transform = NULL );


        void WeldNavMeshVerts(const f32 vertex_merge_threshold);
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
    };


    typedef Nocturnal::SmartPtr<Mesh> MeshPtr;
    typedef std::vector<MeshPtr> V_Mesh;

    struct CompareMeshes
    {
        bool operator ()( const MeshPtr& lhs, const MeshPtr& rhs )
        {
            return lhs->m_ID < rhs->m_ID;
        }    
    };
    typedef std::set<MeshPtr, CompareMeshes> S_Mesh;
}
