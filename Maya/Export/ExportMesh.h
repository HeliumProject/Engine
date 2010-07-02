#pragma once

#include "ExportBase.h"

#include "Pipeline/Content/Nodes/Mesh.h"
#include "Pipeline/Content/Nodes/Skin.h"
#include "Pipeline/Content/Nodes/Shader.h"

#include "ExportScene.h"
#include "ExportShader.h"

#include <Maya/MDagPathArray.h>
#include <Maya/MFnBlendShapeDeformer.h>

typedef std::multimap<i32, i32> MM_i32;

namespace MayaContent
{
  //
  // VertexIndex
  //

  struct MAYA_API VertexIndex
  {
    u32   m_Index;
    bool  m_Wire;

    VertexIndex()
    {
    
    }
    
    VertexIndex( u32 i, bool wire )
      : m_Index(i)
      , m_Wire(wire)
    {
    
    }
  };
  typedef std::vector<VertexIndex> V_VertexIndex;


  //
  // WireEdge
  //

  struct MAYA_API WireEdge
  {
    int m_A;
    int m_B;

    WireEdge( int a, int b )
      : m_A (a)
      , m_B (b)
    {
    
    }

    friend bool operator<( const WireEdge & a, const WireEdge& b );
  };

  inline bool operator<( const WireEdge & a, const WireEdge & b )
  {
    return (a.m_A < b.m_A) || ((a.m_A == b.m_A) && (a.m_B < b.m_B));
  }

  typedef std::set<WireEdge> S_WireEdge; // for detecting duplicate wires in wireframe


  //
  // ExportVertex
  //

  static const float ACCURACY_FOR_VECTOR_COMP = 0.0001f;
  static const float ACCURACY_FOR_NORMAL_COMP = 0.99f;

  class MAYA_API ExportVertex : public Nocturnal::RefCountBase<ExportVertex>
  {
  public:
    u32 m_ShaderIndex;

    Math::Vector3 m_Position;
    Math::Vector3 m_Normal;
    Math::Vector3 m_Tangent;
    Math::Vector4 m_Color;

    Math::Vector2 m_BaseUV;
    Math::Vector2 m_LightmapUV;
    Math::Vector2 m_BlendmapUV;

    ExportVertex()
    {
	    m_Color.Set( 0.5f, 0.5f, 0.5f, 1.f );
    }

    bool ExportVertex::operator == ( const ExportVertex &rhs )
    {
      if ( m_ShaderIndex != rhs.m_ShaderIndex )
        return false;

      if ( !m_Position.Equal( rhs.m_Position, ACCURACY_FOR_VECTOR_COMP ) )
        return false;

      if ( !m_BaseUV.Equal( rhs.m_BaseUV, ACCURACY_FOR_VECTOR_COMP ) )
        return false;

      if ( (m_Normal.Dot( rhs.m_Normal)) < ACCURACY_FOR_NORMAL_COMP )
        return false;

      if ( !m_Color.Equal( rhs.m_Color, ACCURACY_FOR_VECTOR_COMP ) )
        return false;

      if ( !m_LightmapUV.Equal( rhs.m_LightmapUV, ACCURACY_FOR_VECTOR_COMP ) )
        return false;

      if ( !m_BlendmapUV.Equal( rhs.m_BlendmapUV, ACCURACY_FOR_VECTOR_COMP ) )
        return false;

      if ( (m_Tangent.Dot( rhs.m_Tangent)) < ACCURACY_FOR_NORMAL_COMP )
        return false;

      return true;
    }
  };

  typedef Nocturnal::SmartPtr< ExportVertex > ExportVertexPtr;
  typedef std::vector< ExportVertexPtr > V_ExportVertex;


  //
  // ExportTriangle
  //

  class MAYA_API ExportTriangle
  {
  public:
    u32             m_Index;
    VertexIndex     m_Vertices[3];

    Math::Vector3   m_Normal;
    Math::Vector3   m_Tangent;

    ExportShaderPtr m_ExportShader;

    ExportTriangle()
      : m_Index( 0 )
    {
    }
    ExportTriangle( const VertexIndex& indexData0, const VertexIndex& indexData1, const VertexIndex& indexData2 )
      : m_Index (0)
    {
      m_Vertices[0] = indexData0;
      m_Vertices[1] = indexData1;
      m_Vertices[2] = indexData2;
    }
  };


  //
  // ExportPolygon
  //

  struct MAYA_API ExportPolygon
  {
    int m_Shader;
    int m_PolygonNumber;
    int m_FirstIndex;
    int m_Count;

    ExportPolygon( int shader, int polyNum, int firstIndex, int count )
      : m_Shader(shader)
      , m_PolygonNumber(polyNum)
      , m_FirstIndex(firstIndex)
      , m_Count(count)
    {
    
    }

    friend bool operator<( const ExportPolygon & a, const ExportPolygon & b );
  };

  inline bool operator<( const ExportPolygon & a, const ExportPolygon & b )
  {
    return (a.m_Shader < b.m_Shader) || (a.m_Shader == b.m_Shader && a.m_PolygonNumber < b.m_PolygonNumber);
  }

  typedef std::set<ExportPolygon> S_ExportPolygon; // for presorting polys by shader



  //
  // ExportMorphTarget
  //
  
  struct MAYA_API ExportMorphTargetDelta
  {
    u32           m_VertexIndex;
    Math::Vector3 m_Vector;
  };

  typedef std::vector< ExportMorphTargetDelta > V_ExportMorphTargetDelta;

  struct MAYA_API ExportMorphTarget
  {
    tstring                  m_Name;
    Nocturnal::TUID                 m_Id;
    V_ExportMorphTargetDelta     m_PosDeltas;
    Math::V_Vector3              m_NormalDeltaVectors;
  };

  typedef std::vector< ExportMorphTarget >  V_ExportMorphTarget;

  class SceneIndicesTracker
  {
  public:
        // A one-to-many mapping from scene vertex index to unique index in Content::Mesh::m_TriangleVertexIndices    
      std::multimap< u32, u32 >  m_VertsIndices;
      std::vector< u32 >   m_NormalIndices;
  };

  //
  // MayaSkinCluster
  //

  struct MAYA_API MayaSkinCluster
  {
    MObject       m_skinCluster;
    MDagPathArray m_influences;
    MFloatArray   m_weights;
  };
  typedef std::vector<MayaSkinCluster> V_MayaSkinCluster;

  //
  // MayaJointCluster
  //

  struct MAYA_API MayaJointCluster
  {
    MObject     m_jointCluster;         
    MObject     m_influence;  
    MDagPath    m_sourceGeometry;
    MObject     m_sourceVerticies;
  };
  typedef std::vector<MayaJointCluster> V_MayaJointCluster;

  //
  // Mesh Export Object
  //

  class MAYA_API ExportMesh : public ExportBase
  {
  public:
    Content::SkinPtr        m_ContentSkin;

  private:
    // data to write into Content::Mesh
    bool                    m_HasVertexColors;
    bool                    m_HasLightMap;
    bool                    m_HasBlendMap;
    MFloatPointArray        m_Points;
    MFloatVectorArray       m_Normals;
    MColorArray             m_Colors;
    MFloatArray             m_BaseUArray;
    MFloatArray             m_BaseVArray;
    MIntArray               m_BaseUVCounts;
    MIntArray               m_BaseUVIds;
    MFloatArray             m_LightMapUArray;
    MFloatArray             m_LightMapVArray;
    MIntArray               m_LightMapUVCounts;
    MIntArray               m_LightMapUVIds;
    MFloatArray             m_BlendMapUArray;
    MFloatArray             m_BlendMapVArray;
    MIntArray               m_BlendMapUVCounts;
    MIntArray               m_BlendMapUVIds;
    MObjectArray            m_Shaders;
    MIntArray               m_ShaderIndices;

    // Number of triangles per polygon (indexed by poly number)
    MIntArray               m_TriangleCounts;

    // skin data
    V_MayaSkinCluster       m_SkinClusters;
    V_MayaJointCluster      m_JointClusters;

    // data from maya to help build Content::Mesh
    MString                 m_MayaName;
    MObject                 m_MayaParent;
    MIntArray               m_VertexCount;
    MIntArray               m_VertexList;
    MIntArray               m_NormalIds;
    MIntArray               m_NormalIdCounts;

    // additional data to help build Content::Mesh
    unsigned int            m_TriangleCount;
    unsigned int            m_InfluenceCount;

    mutable S_WireEdge      m_SourceWires;
    mutable S_ExportPolygon m_Polygons;

    mutable V_ExportVertex  m_Verts;
    mutable MM_i32          m_VertIndexMap;

    M_UIDExportShader       m_ExportShaderMap;

    // data for building the blend shape info to help build Content::Mesh
    V_ExportMorphTarget     m_ExportMorphTargets;
    SceneIndicesTracker     m_SceneIndicesTracker;

  public:
    ExportMesh( const MObject& mayaMesh, const Nocturnal::TUID& id ) 
      : ExportBase( mayaMesh )
      , m_HasVertexColors ( false )
      , m_HasLightMap ( false )
      , m_TriangleCount ( 0 )
      , m_InfluenceCount ( 0 )
    {
      m_ContentObject = new Content::Mesh( id );
    }

    const Content::MeshPtr GetContentMesh() const
    {
      return Reflect::DangerousCast< Content::Mesh >( m_ContentObject );
    }

    inline Math::Vector3 GetPosition( const VertexIndex &indexData ) const
    {
      NOC_ASSERT( indexData.m_Index < m_VertexList.length() );
      u32 idx = (u32)m_VertexList[ indexData.m_Index ];
      NOC_ASSERT( idx < m_Points.length() );
      return Math::Vector3( m_Points[ idx ].x * 0.01f, m_Points[ idx ].y * 0.01f, m_Points[idx].z * 0.01f );
    }

    inline Math::Vector3 GetNormal( const VertexIndex &indexData ) const
    {
      NOC_ASSERT( indexData.m_Index < m_NormalIds.length() );
      u32 idx = (u32)m_NormalIds[ indexData.m_Index ];
      NOC_ASSERT( idx < m_Normals.length() );
      return Math::Vector3( m_Normals[ idx ].x, m_Normals[ idx ].y, m_Normals[ idx ].z );
    }

    inline Math::Vector4 GetColor( const VertexIndex &indexData ) const
    {
      u32 idx = (u32)m_VertexList[ indexData.m_Index ];
      return Math::Vector4( m_Colors[idx].r, m_Colors[idx].g, m_Colors[idx].b, m_Colors[idx].a );
    }

    inline Math::Vector2 GetBaseUV( const VertexIndex &indexData ) const
    {
      NOC_ASSERT( indexData.m_Index < m_BaseUVIds.length() );
      u32 idx = (u32)m_BaseUVIds[ indexData.m_Index ];
      NOC_ASSERT( idx < m_BaseUArray.length() );
      return Math::Vector2( m_BaseUArray[ idx ], m_BaseVArray[ idx ] );
    }

    inline Math::Vector2 GetLightMapUV( u32 index ) const
    {
       NOC_ASSERT( index < m_LightMapUVIds.length() );
       u32 idx = (u32)m_LightMapUVIds[ index ];
       NOC_ASSERT( idx < m_LightMapUArray.length() );
       return Math::Vector2( m_LightMapUArray[ idx ], m_LightMapVArray[ idx ] );
    }

    inline Math::Vector2 GetLightMapUV( const VertexIndex &indexData ) const
    {
      NOC_ASSERT( indexData.m_Index < m_LightMapUVIds.length() );
      u32 idx = (u32)m_LightMapUVIds[ indexData.m_Index ];
      NOC_ASSERT( idx < m_LightMapUArray.length() );
      return Math::Vector2( m_LightMapUArray[ idx ], m_LightMapVArray[ idx ] );
    }

    inline Math::Vector2 GetBlendMapUV( const VertexIndex &indexData ) const
    {
      NOC_ASSERT( indexData.m_Index < m_BlendMapUVIds.length() );
      u32 idx = (u32)m_BlendMapUVIds[ indexData.m_Index ];
      NOC_ASSERT( idx < m_BlendMapUArray.length() );
      return Math::Vector2( m_BlendMapUArray[ idx ], m_BlendMapVArray[ idx ] );
    }

    void GatherMayaData( V_ExportBase &newExportObjects );
    void AddDefaultShader( V_ExportBase &newExportObjects, const Content::MeshPtr& contentMesh );
    void AddSkinData();

    bool GetBlendShapeDeformer( const MFnMesh& meshFn, MFnBlendShapeDeformer& morpherFn );
    void GatherMorphTargets( MFnBlendShapeDeformer& morpherFn );

    void ProcessMayaData();
    void ProcessMorphTargets( const Content::MeshPtr contentMesh );
    void ProcessUVs();

    void ProcessPolygon( unsigned int& triangleIndex, const ExportPolygon& poly );

    bool ComputeTriangleData( ExportTriangle &exportTri ) const;
    void ProcessTriangle( unsigned int& triangleIndex, unsigned int polyIndex, ExportTriangle &exportTri );

    u32 GetUniqueVertexIndex( const ExportVertexPtr &vert, bool &newVert ) const;
    u32 ProcessVertex( const ExportTriangle &exportTri, u32 vert );
  };

  typedef Nocturnal::SmartPtr< ExportMesh > ExportMeshPtr;
  typedef std::vector< ExportMeshPtr > V_ExportMesh;
}