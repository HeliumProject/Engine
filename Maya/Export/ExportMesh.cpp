#include "Precompile.h"

#include "MayaContentCmd.h"

#include "ExportMesh.h"

#include "Foundation/Container/Insert.h"

#include "Maya/NodeTypes.h"

#include <maya/mFnDependencyNode.h>
#include <maya/MFnWeightGeometryFilter.h>
#include <maya/MItGeometry.h>

#include <cmath>

using namespace Reflect;
using namespace Content;
using namespace MayaContent;
using namespace Helium;
using Helium::Insert;

#define g_kMinDeltaTol  0.0001f  // .1mm

static const u32 s_DefaultMaterialIndex = 0;

inline static float LengthSquared( const MFloatVector & v )
{
    return ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z );
}


inline static void DisableBlendShapes( MFnBlendShapeDeformer& morpherFn )
{
    MStatus status;

    // get the envelope attribute plug
    MPlug plug = morpherFn.findPlug("en");

    // set to 0 to disable FFD effect
    plug.setValue(0.0f);
}

inline static void EnableBlendShapes( MFnBlendShapeDeformer& morpherFn )
{
    MStatus status;

    // get the envelope attribute plug
    MPlug plug = morpherFn.findPlug("en");

    // set to 1 to enable FFD effect
    plug.setValue(1.0f);
}



// ////////////////////////////////////////////////////////////////////////////
// retrieve all of the relevant mesh data from Maya
//
void ExportMesh::GatherMayaData( V_ExportBase &newExportObjects )
{
    EXPORT_SCOPE_TIMER( ("") );

    MStatus status;
    static const MString baseMapUVName( "map1" );
    static const MString blendMapUVName( "blendmap" );
    static const MString lightMapUVName( "lightmap" );
    MFnMesh meshFn(m_MayaObject, &status);

    // Get the morph target and disable all blend shapes before retrieving mesh data,
    // otherwise we'll get the deformed mesh data rather than the base version
    MFnBlendShapeDeformer morpherFn;
    bool gatherMorphTargets = GetBlendShapeDeformer( meshFn, morpherFn );

    if ( gatherMorphTargets )
    {
        DisableBlendShapes( morpherFn );
    }

    //
    // Gather all the mesh data
    //
    status = meshFn.getVertices( m_VertexCount, m_VertexList );
    status = meshFn.getPoints( m_Points );
    unsigned int numberOfPolys = m_VertexCount.length();
    unsigned int numberOfVerts = m_Points.length();
    status = meshFn.getNormals( m_Normals );
    status = meshFn.getNormalIds( m_NormalIdCounts, m_NormalIds );
    status = meshFn.getVertexColors( m_Colors );

    // We are using Maya for triangulation of polygons.  Store off the number
    // of trianges per polygon so that we don't have to keep querying for it
    // later.
    MIntArray unusedTriangleVertices;
    status = meshFn.getTriangles( m_TriangleCounts, unusedTriangleVertices );
    HELIUM_ASSERT( status );

    HELIUM_ASSERT( m_VertexCount.length() == m_NormalIdCounts.length() );
    HELIUM_ASSERT( m_VertexList.length() == m_NormalIds.length() );

    status = meshFn.getUVs( m_BaseUArray, m_BaseVArray, &baseMapUVName );
    status = meshFn.getAssignedUVs( m_BaseUVCounts, m_BaseUVIds, &baseMapUVName );

    status = meshFn.getUVs( m_LightMapUArray, m_LightMapVArray, &lightMapUVName );
    status = meshFn.getAssignedUVs( m_LightMapUVCounts, m_LightMapUVIds, &lightMapUVName );

    status = meshFn.getUVs( m_BlendMapUArray, m_BlendMapVArray, &blendMapUVName );
    status = meshFn.getAssignedUVs( m_BlendMapUVCounts, m_BlendMapUVIds, &blendMapUVName );


    u32 lenbase  = m_BaseUVIds.length();
    u32 lenlight = m_LightMapUVIds.length();
    u32 lenblend = m_BlendMapUVIds.length();

    status = meshFn.getConnectedShaders( MDagPath::getAPathTo(m_MayaObject).instanceNumber( &status ), m_Shaders, m_ShaderIndices );

    m_HasVertexColors = ( m_Colors.length() == numberOfVerts );
    m_HasLightMap = ( m_LightMapUVCounts.length() > 0 );
    m_HasBlendMap = ( m_BlendMapUVCounts.length() > 0 );
    m_MayaName = meshFn.name();
    m_MayaParent = meshFn.parent(0);

    const Content::MeshPtr contentMesh = GetContentMesh();
    contentMesh->m_DefaultName = m_MayaName.asTChar();

    // this will retrieve and setup the global matrix for this mesh (because meshs are transforms in Content but not Maya)
    MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo( m_MayaObject ).inclusiveMatrix(), contentMesh->m_GlobalTransform );
    contentMesh->m_GlobalTransform.t.x /= 100.f;
    contentMesh->m_GlobalTransform.t.y /= 100.f;
    contentMesh->m_GlobalTransform.t.z /= 100.f;


    // all maya mesh objects should have a parent transform
    MFnTransform transformFn( meshFn.parent( 0 ) );

    MTransformationMatrix matrix = transformFn.transformation( &status );

    MPoint scalePivot = matrix.scalePivot( MSpace::kTransform );
    contentMesh->m_ScalePivot.x = (float)scalePivot.x * Math::CentimetersToMeters;
    contentMesh->m_ScalePivot.y = (float)scalePivot.y * Math::CentimetersToMeters;
    contentMesh->m_ScalePivot.z = (float)scalePivot.z * Math::CentimetersToMeters;

    MVector scalePivotTranslate = matrix.scalePivotTranslation( MSpace::kTransform );
    contentMesh->m_ScalePivotTranslate.x = (float)scalePivotTranslate.x * Math::CentimetersToMeters;
    contentMesh->m_ScalePivotTranslate.y = (float)scalePivotTranslate.y * Math::CentimetersToMeters;
    contentMesh->m_ScalePivotTranslate.z = (float)scalePivotTranslate.z * Math::CentimetersToMeters;

    MPoint rotatePivot = matrix.rotatePivot( MSpace::kTransform );
    contentMesh->m_RotatePivot.x = (float)rotatePivot.x * Math::CentimetersToMeters;
    contentMesh->m_RotatePivot.y = (float)rotatePivot.y * Math::CentimetersToMeters;
    contentMesh->m_RotatePivot.z = (float)rotatePivot.z * Math::CentimetersToMeters;

    MVector rotatePivotTranslate = matrix.rotatePivotTranslation( MSpace::kTransform );
    contentMesh->m_RotatePivotTranslate.x = (float)rotatePivotTranslate.x * Math::CentimetersToMeters;
    contentMesh->m_RotatePivotTranslate.y = (float)rotatePivotTranslate.y * Math::CentimetersToMeters;
    contentMesh->m_RotatePivotTranslate.z = (float)rotatePivotTranslate.z * Math::CentimetersToMeters;


    //
    // some basic tests for data validity
    //
    HELIUM_ASSERT( numberOfPolys == m_ShaderIndices.length() );
    HELIUM_ASSERT( numberOfPolys == m_NormalIdCounts.length() );
    HELIUM_ASSERT( numberOfPolys == m_BaseUVCounts.length() );
    HELIUM_ASSERT( m_LightMapUArray.length() == m_LightMapVArray.length() );
    HELIUM_ASSERT( m_BaseUArray.length() == m_BaseVArray.length() );
    HELIUM_ASSERT( m_BlendMapUArray.length() == m_BlendMapVArray.length() );


    //
    // initialize exporting
    //
    unsigned int vertexCount = 0;
    for (unsigned int polyNum=0; polyNum<numberOfPolys; polyNum++)
    {
        int count = m_VertexCount[polyNum];
        m_Polygons.insert( ExportPolygon( m_ShaderIndices[polyNum], polyNum, vertexCount, count ) );
        vertexCount += count;
    }
    m_TriangleCount = vertexCount - 2 * numberOfPolys;


    //
    // load maya skin
    //
    MPlug inMesh (m_MayaObject, meshFn.attribute(MString ("inMesh")));
    MItDependencyGraph dgItr (inMesh, MFn::kSkinClusterFilter, MItDependencyGraph::kUpstream);
    while (!dgItr.isDone())
    {
        MFnSkinCluster skinCluster(dgItr.thisNode(), &status);
        MDagPathArray influenceObjectPaths;

        // get all of maya's influence objects for this cluster
        unsigned objectCount = skinCluster.influenceObjects(influenceObjectPaths, &status);

        // cycle through each of maya's influence objects
        unsigned int meshCount = skinCluster.numOutputConnections();

        // for each output mesh
        for ( unsigned int i = 0; i < meshCount; ++i )
        {
            unsigned int meshIndex = skinCluster.indexForOutputConnection(i);

            // get the current mesh for this skin cluster
            MDagPath meshPath;
            skinCluster.getPathAtIndex(meshIndex, meshPath);

            // check to make sure its the mesh we are exporting
            if (m_MayaObject != meshPath.node())
            {
                continue;
            }

            // expand the skin clusters vector and note the current index
            unsigned int j = (unsigned int)m_SkinClusters.size();
            m_SkinClusters.resize( j+1 );

            // store the maya object
            m_SkinClusters[j].m_skinCluster = dgItr.thisNode();

            // select all the vertex components
            MFnSingleIndexedComponent verticesComponentsFn;
            MObject verticesComponent = verticesComponentsFn.create(MFn::kMeshVertComponent, &status);
            status = verticesComponentsFn.setCompleteData(numberOfVerts);

            // get the influence objects
            unsigned int influenceCount1;
            influenceCount1 = skinCluster.influenceObjects( m_SkinClusters[j].m_influences, &status);

            // get the weights for the entire object
            unsigned int influenceCount2;
            status = skinCluster.getWeights( meshPath, verticesComponent, m_SkinClusters[j].m_weights, influenceCount2 );

            /*
            // a few sanity checks
            HELIUM_ASSERT( influenceCount1 == influenceCount2 );
            HELIUM_ASSERT( influenceCount1 == m_SkinClusters[j].m_influences.length() );
            HELIUM_ASSERT( numberOfVerts * influenceCount1 == m_SkinClusters[j].m_weights.length() );
            */

            m_InfluenceCount += influenceCount1;
        }

        dgItr.next();
    }

    // If we are rigidly bound, we will have a jointCluster instead of a skin cluster
    MItDependencyNodes dgItrJointCluster( MFn::kJointCluster );
    while(!dgItrJointCluster.isDone())
    {
        MObject                   jointCluster( dgItrJointCluster.item() );
        MFnWeightGeometryFilter   fnWeightGeometryJointCluster( dgItrJointCluster.item() );
        MFnDependencyNode         fnDependencyJointCluster( dgItrJointCluster.item() );

        // find the joint that controls these verticies
        MPlug plugJoint = fnDependencyJointCluster.findPlug("matrix");
        MPlugArray connections;

        if(!plugJoint.connectedTo(connections,true,false))
        {
            dgItrJointCluster.next();
            continue;
        }

        // expand the joint clusters vector and note the current index
        unsigned int j = (unsigned int)m_JointClusters.size();
        m_JointClusters.resize( j+1 );

        m_JointClusters[j].m_jointCluster = jointCluster;
        m_JointClusters[j].m_influence    = connections[0].node();

        MFnSet          fnSetJointCluster( fnWeightGeometryJointCluster.deformerSet() );
        MSelectionList  deformedGeometry;

        fnSetJointCluster.getMembers( deformedGeometry, true );

        for( unsigned int i=0; i<deformedGeometry.length(); ++i)
        {
            MDagPath    geometry;
            MObject     affectedVerticies;
            MFloatArray weights;

            deformedGeometry.getDagPath(i,geometry,affectedVerticies);

            // check to make sure its the mesh we are exporting
            if (m_MayaObject != geometry.node())
            {
                continue;
            }

            m_JointClusters[j].m_sourceGeometry = geometry;
            m_JointClusters[j].m_sourceVerticies = affectedVerticies;
        }

        // since we are rigid, we only have 1 influencing joint
        m_InfluenceCount += 1;

        dgItrJointCluster.next();
    }

#pragma TODO("Detect skin?")
    m_ContentSkin = new Content::Skin();

    if( m_ContentSkin )
    {
        AddSkinData();
    }

    bool hasDefault = false;
    u32 numShaders = m_Shaders.length();
    u32 numShadersIndices = m_ShaderIndices.length();
    for( u32 i = 0; i < numShadersIndices; ++i )
    {
        if ( m_ShaderIndices[i] == -1 )
        {
            m_ShaderIndices[i] = numShaders;
            hasDefault = true;
        }
    }

    //
    // gather the shaders & populate the Content object's shader ids
    //
    contentMesh->m_ShaderIDs.reserve( numShaders );
    for( u32 i = 0; i < numShaders; ++i )
    {
        MFnSet mayaShadingGroupFn ( m_Shaders[i] );
        MPlug surfaceShaderPlug = mayaShadingGroupFn.findPlug("surfaceShader");
        MPlugArray connections;

        if (!surfaceShaderPlug.connectedTo( connections, true, false ) || connections.length() == 0 )
        {
            // FIXME - is this correct
            AddDefaultShader( newExportObjects, contentMesh );
            continue;
        }

        MObject surfaceShader = connections[0].node();

        ExportShaderPtr exportShader;
        MFnDependencyNode surfaceNodeFn( surfaceShader );

        exportShader = new ExportShader( surfaceShader, Maya::GetNodeID( surfaceShader ) );
        newExportObjects.push_back( exportShader );

        m_ExportShaderMap[exportShader->GetContentShader()->m_ID] = exportShader;
        contentMesh->m_ShaderIDs.push_back( exportShader->GetContentShader()->m_ID );
    }

    // handle the missing shader case.  We typically get this when there is broken art.
    if ( hasDefault )
    {
        AddDefaultShader( newExportObjects, contentMesh );
    }  


    //
    // gather morph target data
    //

    // here's where we actually gather the morph target data
    // then re-enable the blend shapes - this is a nicety for the user, but is not required
    if ( gatherMorphTargets )
    {
        GatherMorphTargets( morpherFn );
        EnableBlendShapes( morpherFn );
    }

    __super::GatherMayaData( newExportObjects );

    ProcessUVs();
}



///////////////////////////////////////////////////////////////////////////////
// loop over all the blend shapes in the scene and try to get the blend shape node
// that's base object is the mesh we are operating on
//
bool ExportMesh::GetBlendShapeDeformer( const MFnMesh& meshFn, MFnBlendShapeDeformer& morpherFn )
{
    MStatus status;

    Log::Debug( TXT("Searching for morpher (blendShape) for mesh: %s...\n"), meshFn.name().asTChar() );

    MItDependencyNodes blendShapeItr( MFn::kBlendShape );

    for( ; ( blendShapeItr.isDone() == false ) ; blendShapeItr.next() )
    {
        MObject blendShapeObj = blendShapeItr.item();

        if ( blendShapeObj.apiType() == MFn::kBlendShape )
        {
            morpherFn.setObject( blendShapeObj );

            // get the base object
            MObjectArray baseObjects;

            status = morpherFn.getBaseObjects( baseObjects );
            HELIUM_ASSERT( status );

            if ( baseObjects.length() == 0 )
            {
                continue;
            }

            if ( baseObjects.length() != 1 )
            {
                Log::Warning(TXT("Morph target (%s) has more than one base object, this is not supported"), morpherFn.name().asTChar() );
            }

            // check to see if it's the 
            if ( baseObjects[0] == m_MayaObject )
            {
                morpherFn.setObject( blendShapeObj );

                Log::Debug( TXT("Found morpher (blendShape): %s for baseObject mesh: %s\n"), morpherFn.name().asTChar(), meshFn.name().asTChar() );

                return true;
            }
        }
    }

    Log::Debug( TXT("None found.\n") );
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// Gather the morph target data
//
void ExportMesh::GatherMorphTargets( MFnBlendShapeDeformer& morpherFn )
{
    MStatus status;

    tstring morpherFnName = morpherFn.name().asTChar();
    Log::Debug( TXT("Gathering morph targets for morpher (blendShape): %s\n"), morpherFn.name().asTChar() );


    // get the weights
    u32 numWeights = morpherFn.numWeights(&status);

    MIntArray weightIndices;
    status = morpherFn.weightIndexList(weightIndices);

    HELIUM_ASSERT(weightIndices.length() == numWeights);
    Log::Debug( TXT("NumWeights: %d\n"), numWeights );

    for( u32 weightIndex = 0; weightIndex < numWeights; ++weightIndex )
    {
        u32 weightId = weightIndices[weightIndex];

        // get the targets
        MObjectArray  targets;
        status = morpherFn.getTargets(m_MayaObject, weightId, targets);
        HELIUM_ASSERT(status);

        u32 numTargets = targets.length();
        Log::Debug( TXT("WeightIndex: %d, NumTargets: %d\n"), weightIndex, numTargets );

        for(u32 targetIndex = 0; targetIndex < numTargets; ++targetIndex)
        {
            MObject targetObject = targets[targetIndex];

            HELIUM_ASSERT( targetObject.hasFn( MFn::kMesh ) );
            HELIUM_ASSERT( targetObject.hasFn( MFn::kTransform ) == false );

            // this will give you the mesh of the blend shape
            MFnMesh targetMeshFn( targetObject, &status );
            HELIUM_ASSERT( status );

            ExportMorphTarget exportMorphTarget;

            exportMorphTarget.m_Name = targetMeshFn.name(&status).asTChar();
            exportMorphTarget.m_Id = Maya::GetNodeID( targetObject );


            //
            // Get the Normal Deltas
            //

            MFloatVectorArray normalDeltas;
            status = targetMeshFn.getNormals(normalDeltas, MSpace::kObject);
            HELIUM_ASSERT(status);
            u32 numNormalDeltas = normalDeltas.length();
            exportMorphTarget.m_NormalDeltaVectors.resize(numNormalDeltas);

            // We assume that we are getting the verts/normals of the base and target
            // meshes in the same order.
            if ( m_Normals.length() != numNormalDeltas )
            {
                throw Helium::Exception( 
                    TXT("Morph target %s does not have the same number of normals ") \
                    TXT("as the base mesh from which it was created (%s). ") \
                    TXT("This can cause strange display artifacts in the animations."),
                    exportMorphTarget.m_Name.c_str(), GetContentMesh()->GetName().c_str() );
            }

            //
            // Calculate and store the normal deltas
            //

            for( u32 deltaNormalIndex = 0; deltaNormalIndex < numNormalDeltas; ++deltaNormalIndex )
            {
                MPoint pointNormalDelta = ( normalDeltas[deltaNormalIndex] - m_Normals[deltaNormalIndex] );
                exportMorphTarget.m_NormalDeltaVectors[deltaNormalIndex].Set( ( f32 ) pointNormalDelta.x, ( f32 ) pointNormalDelta.y, ( f32 ) pointNormalDelta.z );
            }


            //
            // Get the Position Deltas
            //
            MFloatPointArray  pointDeltas;
            status = targetMeshFn.getPoints(pointDeltas, MSpace::kObject);
            HELIUM_ASSERT(status);
            u32 numPointDeltas = pointDeltas.length();
            exportMorphTarget.m_PosDeltas.reserve(numPointDeltas);

            // We assume that we are getting the verts/normals of the base and target
            // meshes in the same order.
            if ( m_Points.length() != numPointDeltas )
            {
                throw Helium::Exception( 
                    TXT("Morph target %s does not have the same number of verticies ") \
                    TXT("as the base mesh from which it was created (%s). ") \
                    TXT("This can cause strange display artifacts in the animations."),
                    exportMorphTarget.m_Name.c_str(), GetContentMesh()->GetName().c_str() );
            }

            //
            // Calculate and store the point deltas
            //

            for(u32 deltaIndex = 0; deltaIndex < numPointDeltas; deltaIndex++)
            {
                MPoint pointDelta = ( ( pointDeltas[deltaIndex] - m_Points[deltaIndex] ) * Math::CentimetersToMeters );

                // there's a better way to do these using dot product to check the angle
                if ( pointDelta.isEquivalent( MPoint( 0.0f, 0.0f, 0.0f ), g_kMinDeltaTol ) == false )
                {
                    ExportMorphTargetDelta exportDelta;
                    exportDelta.m_VertexIndex  = deltaIndex;

                    exportDelta.m_Vector.Set( ( f32 ) pointDelta.x, ( f32 ) pointDelta.y, ( f32 ) pointDelta.z );

                    exportMorphTarget.m_PosDeltas.push_back(exportDelta);
                }
            }

            if ( !exportMorphTarget.m_PosDeltas.empty() )
            {
                Log::Debug( TXT("Adding target mesh: %s\n"), exportMorphTarget.m_Name.c_str() );
                m_ExportMorphTargets.push_back( exportMorphTarget );
            }
            else
            {
                Log::Warning( 
                    TXT("Skipping target mesh %s because it has no delta points. ") \
                    TXT("This means that the mesh does not differ from the base object. ") \
                    TXT("This will cause errors in the builders.\n"), exportMorphTarget.m_Name.c_str() );
            }
        }
    }
}


void ExportMesh::AddDefaultShader( V_ExportBase &newExportObjects, const Content::MeshPtr& contentMesh )
{
    // FIXME not thread safe
    static TUID s_DefaultShaderID( TUID::Generate() );
    M_UIDExportShader::iterator itr = m_ExportShaderMap.find( s_DefaultShaderID );
    if ( itr == m_ExportShaderMap.end() )
    {
        Content::ShaderPtr contentShader = new Content::Shader( s_DefaultShaderID );
        contentShader->m_DefaultName = TXT("Default Shader");

        ExportShaderPtr defaultShader = new ExportShader( contentShader );
        itr = m_ExportShaderMap.insert( M_UIDExportShader::value_type( s_DefaultShaderID, defaultShader ) ).first;
    }

    newExportObjects.push_back( itr->second );
    contentMesh->m_ShaderIDs.push_back( s_DefaultShaderID );
}

void ExportMesh::AddSkinData()
{
    EXPORT_SCOPE_TIMER( ("") );

    unsigned int triangleCount = m_TriangleCount;
    unsigned int triangleVertexCount = triangleCount * 3;
    unsigned int sourceVertexCount = m_Points.length();

    const Content::MeshPtr contentMesh = GetContentMesh();

    // connect skin to the mesh
    m_ContentSkin->m_Mesh = contentMesh->m_ID;

    // allocate memory for the skin
    m_ContentSkin->m_Influences.resize( sourceVertexCount );
    m_ContentSkin->m_InfluenceObjectIDs.resize( m_InfluenceCount );
    m_ContentSkin->m_InfluenceIndices.reserve( triangleVertexCount );

    // store the source of the influences
    for (unsigned int i=0; i<m_InfluenceCount; i++)
    {
        MObject influenceObject = MObject::kNullObj;

        if( m_SkinClusters.size() > 0 )
        {
            V_MayaSkinCluster::const_iterator skinItr = m_SkinClusters.begin();
            V_MayaSkinCluster::const_iterator skinEnd = m_SkinClusters.end();
            for ( unsigned int count = 0; skinItr != skinEnd; ++skinItr )
            {
                unsigned int clusterLength = (*skinItr).m_influences.length();

                if ( i - count < clusterLength )
                {
                    influenceObject = (*skinItr).m_influences[ i - count ].node();
                    break;
                }

                count += clusterLength;
            }
        }
        else if( m_JointClusters.size() > 0 )
        {
            influenceObject = m_JointClusters[i].m_influence;
        }

        TUID influenceUID = Maya::GetNodeID( influenceObject );

        if (influenceUID == TUID::Null)
        {
            MGlobal::displayError("Unable to set TUID attribute on maya node! Export continuing but will probably result in bad data!");
        }

        m_ContentSkin->m_InfluenceObjectIDs[i] = influenceUID;
    }

    // create vertex influence storage
    for ( unsigned int j = 0; j < sourceVertexCount; ++j )
    {
        m_ContentSkin->m_Influences[j] = new Content::Influence();
    }

    // store the vertex influences
    if( m_SkinClusters.size() > 0 )
    {
        for ( unsigned int j = 0; j < sourceVertexCount; ++j )
        {
            Content::InfluencePtr influence = m_ContentSkin->m_Influences[j];

            for (unsigned i=0; i<m_InfluenceCount; i++)
            {
                // warning, the initial implementation of influenceWeight is quite SLOW
                float weight = 0.0f;

                V_MayaSkinCluster::const_iterator skinItr = m_SkinClusters.begin();
                V_MayaSkinCluster::const_iterator skinEnd = m_SkinClusters.end();
                for ( unsigned int count = 0; skinItr!=skinEnd; ++skinItr )
                {
                    unsigned int clusterLength = (*skinItr).m_influences.length();

                    if ( i-count < clusterLength )
                    {
                        unsigned int weightsLength = (*skinItr).m_weights.length();
                        unsigned int index = j * m_InfluenceCount + i - count;

                        weight = (float)((*skinItr).m_weights[index]);
                        break;
                    }

                    count += clusterLength;
                }

                if (fabs(weight) > 1.0e-10f)
                {
                    influence->m_Objects.push_back(i);
                    influence->m_Weights.push_back(weight);
                }
            }
        }
    }
    else if( m_JointClusters.size() > 0 )
    {  
        V_MayaJointCluster::const_iterator jointItr = m_JointClusters.begin();
        V_MayaJointCluster::const_iterator jointEnd = m_JointClusters.end();

        unsigned int jointIndex = -1;

        for( ; jointItr!=jointEnd; ++jointItr)
        {
            ++jointIndex;

            MObject components = (*jointItr).m_sourceVerticies;

            MItGeometry geometryItr( (*jointItr).m_sourceGeometry,components, NULL ) ;

            while( !geometryItr.isDone() )
            {
                m_ContentSkin->m_Influences[ geometryItr.index() ]->m_Objects.push_back( jointIndex );
                m_ContentSkin->m_Influences[ geometryItr.index() ]->m_Weights.push_back( 1.0f );
                geometryItr.next();
            }
        }
    }
}

void ExportMesh::ProcessUVs()
{
    EXPORT_SCOPE_TIMER( ("") );

    m_BaseUArray.append( 0.0f );
    m_BaseVArray.append( 0.0f );
    m_LightMapUArray.append( 0.0f );
    m_LightMapVArray.append( 0.0f );
    m_BlendMapUArray.append( 0.0f );
    m_BlendMapVArray.append( 0.0f );


    u32 dummyBaseIdx = m_BaseUArray.length()-1;
    u32 dummyLightIdx = m_LightMapUArray.length()-1;
    u32 dummyBlendIdx = m_BlendMapUArray.length()-1;

    u32 baseUVsum = 0;
    u32 lightUVsum = 0;
    u32 blendUVsum = 0;
    u32 len = m_VertexCount.length();
    for( u32 i = 0; i < len; ++i )
    {
        u32 vertCount    = m_VertexCount[i];
        u32 baseUVCount = m_BaseUVCounts[i];

        u32 lightUVCount = 0;
        if( m_HasLightMap )
            lightUVCount = m_LightMapUVCounts[i];      

        u32 blendUVCount = 0;
        if( m_HasBlendMap )
            blendUVCount = m_BlendMapUVCounts[i];      

        baseUVsum += baseUVCount;
        lightUVsum += lightUVCount;
        blendUVsum += blendUVCount;
        if( baseUVCount < vertCount )
        {
            u32 diff = vertCount - baseUVCount;
            for( u32 j = 0; j < diff; ++j )
            {
                m_BaseUVIds.insert( dummyBaseIdx, baseUVsum+j );
            }
            m_BaseUVCounts[i] += diff;
        }
        if( m_HasLightMap && lightUVCount < vertCount )
        {
            u32 diff = vertCount - lightUVCount;
            for( u32 j = 0; j < diff; ++j )
            {
                m_LightMapUVIds.insert( dummyLightIdx, lightUVsum+j );
            }
            m_LightMapUVCounts[i] += diff;
        }
        if( m_HasBlendMap && blendUVCount < vertCount )
        {
            u32 diff = vertCount - blendUVCount;
            for( u32 j = 0; j < diff; ++j )
            {
                m_BlendMapUVIds.insert( dummyBlendIdx, blendUVsum+j );
            }
            m_BlendMapUVCounts[i] += diff;
        }
    }
}

void ExportMesh::ProcessMayaData()
{
    EXPORT_SCOPE_TIMER( ("") );

    unsigned int triangleCount = m_TriangleCount;
    unsigned int triangleVertexCount = triangleCount * 3;
    unsigned int sourceVertexCount = m_Points.length();

    const Content::MeshPtr contentMesh = GetContentMesh();
    //
    // allocate the correct amount of triangle data
    //

    // these are exact amounts
    contentMesh->m_TriangleVertexIndices.reserve( triangleVertexCount );
    contentMesh->m_PolygonIndices.resize( triangleCount );
    contentMesh->m_ShaderIndices.resize( triangleCount );
    contentMesh->m_CollisionMaterialIndices.resize( triangleCount );

    //these are max amounts, we want to reserve this memory
    contentMesh->m_Positions.reserve( triangleVertexCount );
    contentMesh->m_Normals.reserve( triangleVertexCount );
    contentMesh->m_Colors.reserve( triangleVertexCount );
    contentMesh->m_BaseUVs.reserve( triangleVertexCount );
    contentMesh->m_LightMapUVs.reserve( triangleVertexCount );
    contentMesh->m_BlendMapUVs.reserve( triangleVertexCount );

    m_SceneIndicesTracker.m_NormalIndices.reserve( triangleVertexCount );

    //
    // populate the list of triangles by iterating over the polygons in shader order
    //
    int          priorShader = -1;
    unsigned int triangleIndex = 0;
    unsigned int beginShaderTriangleIndex = 0;
    S_ExportPolygon::iterator polyIterator = m_Polygons.begin();
    S_ExportPolygon::iterator polyEnd      = m_Polygons.end();
    for ( ; polyIterator!=polyEnd; ++polyIterator)
    {
        // polys are in shader order, see if a new shader index has been found
        int shader = (*polyIterator).m_Shader;
        if ( shader != priorShader && priorShader >= 0 )
        {
            // store the count of triangles for the prior shader
            contentMesh->m_ShaderTriangleCounts.push_back( triangleIndex - beginShaderTriangleIndex );
            beginShaderTriangleIndex = triangleIndex;
        }
        // remember this shader for the next time through the loop
        priorShader = shader;

        // exporting the polygon will add the number of triangles written to triangleIndex
        ProcessPolygon( triangleIndex, *polyIterator );
    }
    // store the count of triangles for the final shader
    contentMesh->m_ShaderTriangleCounts.push_back( triangleIndex - beginShaderTriangleIndex );

    // this seems bad, could it be that there are shaders in the maya node without triangles?
    while (contentMesh->m_ShaderTriangleCounts.size() < contentMesh->m_ShaderIDs.size())
    {
        contentMesh->m_ShaderTriangleCounts.push_back(0);
    }


    // now that m_SceneIndicesTracker is populated, Process morph targets
    ProcessMorphTargets( contentMesh );


    // link it into the hierarchy
    contentMesh->m_ParentID = Maya::GetNodeID( m_MayaParent );
}

///////////////////////////////////////////////////////////////////////////////
// Populate the contentMesh->m_MorphTargets vector
//
// Combines the m_PosDeltas and m_NormalDeltas
// we dont need to store normal deltas that don't have a corisponding pos delta
// throw away delta with pos delta of zero, even if there is a normal delta
//
void ExportMesh::ProcessMorphTargets( const Content::MeshPtr contentMesh )
{
    for each ( const ExportMorphTarget& exportMorphTarget in m_ExportMorphTargets )
    {
        contentMesh->m_MorphTargets.push_back( new Content::MorphTarget( exportMorphTarget.m_Name, exportMorphTarget.m_Id ) );

        const Content::MorphTargetPtr morphTarget = contentMesh->m_MorphTargets.back();

        // expand the m_PosDeltas by the unique verticies
        for each ( const ExportMorphTargetDelta& exportPosDelta in exportMorphTarget.m_PosDeltas )
        {
            std::multimap< u32, u32 >::const_iterator lowerBound = m_SceneIndicesTracker.m_VertsIndices.lower_bound( exportPosDelta.m_VertexIndex );
            if ( lowerBound != m_SceneIndicesTracker.m_VertsIndices.end() )
            {
                std::multimap< u32, u32 >::const_iterator upperBound = m_SceneIndicesTracker.m_VertsIndices.upper_bound( exportPosDelta.m_VertexIndex );
                for( ; lowerBound != upperBound; ++lowerBound )
                {          
                    const u32& newVectorIndex = lowerBound->second;

                    HELIUM_ASSERT( newVectorIndex < (u32) m_SceneIndicesTracker.m_NormalIndices.size() );

                    u32 normalDeltaIndex = m_SceneIndicesTracker.m_NormalIndices[newVectorIndex];

                    if ( normalDeltaIndex >= (u32) exportMorphTarget.m_NormalDeltaVectors.size() )
                    {
                        throw Helium::Exception(
                            TXT("Morph target %s does not have the same vertex count/order/normals ") \
                            TXT("as the base mesh from which it was created (%s)."),
                            exportMorphTarget.m_Name.c_str(), contentMesh->GetName().c_str() );
                    }

                    morphTarget->AddTargetDelta( newVectorIndex, exportPosDelta.m_Vector, exportMorphTarget.m_NormalDeltaVectors[normalDeltaIndex] );
                }        
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// The work of putting one polygon into the mesh.  Triangulates the poly using
// Maya.
//
void ExportMesh::ProcessPolygon( unsigned int & triangleIndex, const ExportPolygon& poly )
{
    EXPORT_SCOPE_TIMER( ("") );

    std::map< u32, u32 > mayaIndexToExportIndex; 
    MIntArray polyVertIndices; 
    MStatus status;
    MFnMesh meshFn( m_MayaObject, &status );
    HELIUM_ASSERT( status );
    meshFn.getPolygonVertices( poly.m_PolygonNumber, polyVertIndices );
    const u32 numPolyVertIndices = polyVertIndices.length();
    HELIUM_ASSERT( numPolyVertIndices == poly.m_Count );
    for ( int i = 0; i < poly.m_Count; i++ )
    {
        // We are going to make a new set of vertices in the export data for every
        // polygon (starting at m_FirstIndex).  We need to maintain a mapping to
        // go from the Maya index (which includes overlap) to the unique export index.
        mayaIndexToExportIndex[ polyVertIndices[i] ] = poly.m_FirstIndex + i;
    }

    const u32 numTris = m_TriangleCounts[ poly.m_PolygonNumber ];
    for ( u32 triIndex = 0; triIndex < numTris; ++triIndex )
    {
        // Get the vertex indices for each triangle
        i32 vertexList[3] = { -1, -1, -1 };
        status = meshFn.getPolygonTriangleVertices( poly.m_PolygonNumber, triIndex, vertexList );
        if ( !status )
        {
            Log::Error( TXT("Unable to fetch triangle vertices for poly #%d, tri #%d on mesh %s\n"), poly.m_PolygonNumber, triIndex, meshFn.partialPathName().asTChar() );
            HELIUM_BREAK();
        }

        i32 v0 = vertexList[0];
        i32 v1 = vertexList[1];
        i32 v2 = vertexList[2];

        if ( v0 < 0 || v1 < 0 || v2 < 0 )
        {
            Log::Error( TXT("Invalid vertex index detected during polygon triangulation (poly #%d, tri #%d, mesh %s)\n"), poly.m_PolygonNumber, triIndex, meshFn.partialPathName().asTChar() );
            HELIUM_BREAK();
        }

        // Convert these Maya vertex indices into our own export indices
        VertexIndex vi0( mayaIndexToExportIndex[v0], true );
        VertexIndex vi1( mayaIndexToExportIndex[v1], true );
        VertexIndex vi2( mayaIndexToExportIndex[v2], true );

        // Actually export the triangle, which will increment triangleIndex if successful.
        ExportTriangle tri( vi0, vi1, vi2 );
        ProcessTriangle( triangleIndex, poly.m_PolygonNumber, tri );
    }    
}

u32 ExportMesh::GetUniqueVertexIndex( const ExportVertexPtr &vert, bool &newVert ) const
{
    i32 key = (i32)((vert->m_Position.x + vert->m_Position.y + vert->m_Position.z) * 100);
    MM_i32::const_iterator lowerBound = m_VertIndexMap.lower_bound( key );

    i32 index = 0;

    newVert = true;

    if( lowerBound != m_VertIndexMap.end() )
    {
        MM_i32::const_iterator upperBound = m_VertIndexMap.upper_bound( key );
        for( ; lowerBound != upperBound; ++lowerBound )
        {
            index = lowerBound->second;
            if( *vert == *m_Verts[index] )
            {
                newVert = false;
                break;
            }
        }
    }

    if( newVert )
    {
        index = (u32)m_Verts.size();
        m_Verts.push_back( vert );
        m_VertIndexMap.insert( std::pair< i32, i32 >( key, index ) );
    }

    return index;
}

u32 ExportMesh::ProcessVertex( const ExportTriangle &tri, u32 vertNum )
{
    EXPORT_SCOPE_TIMER( ("") );

    ExportVertexPtr vert = new ExportVertex();

    const Content::MeshPtr contentMesh = GetContentMesh();

    vert->m_Position = GetPosition( tri.m_Vertices[vertNum] );
    vert->m_Normal = GetNormal( tri.m_Vertices[vertNum] );
    vert->m_BaseUV = GetBaseUV( tri.m_Vertices[vertNum] );

    if ( m_HasVertexColors )
    {
        vert->m_Color = GetColor( tri.m_Vertices[vertNum] );
    }
    if ( m_HasLightMap )
    {
        vert->m_LightmapUV = GetLightMapUV( tri.m_Vertices[vertNum] );
    }
    if ( m_HasBlendMap )
    {
        vert->m_BlendmapUV = GetBlendMapUV( tri.m_Vertices[vertNum] );
    }
    vert->m_ShaderIndex = contentMesh->m_ShaderIndices[tri.m_Index];

    //Bias bad normals towards face normals until they match (side effect is vertices will not be shared any more)
    while ( fabs( tri.m_Tangent.Dot( vert->m_Normal) ) > ACCURACY_FOR_NORMAL_COMP )
    {
        vert->m_Normal += ( tri.m_Normal - vert->m_Normal)*0.001f;
    }

    if( vert->m_Normal.x == 0 && vert->m_Normal.y == 0 && vert->m_Normal.z == 0 )
    {
        vert->m_Normal = tri.m_Normal;
    }

    //set vertex tangent to be tri tangent ( simply for comparing verts for determining uniqueness )
    vert->m_Tangent = tri.m_Tangent;

    if( tri.m_ExportShader )
    {
        Math::Vector2 temp = vert->m_BaseUV;
        vert->m_BaseUV.x = tri.m_ExportShader->m_URotate.x * temp.x + tri.m_ExportShader->m_URotate.y * temp.y + tri.m_ExportShader->m_UVTranslate.x; 
        vert->m_BaseUV.y = tri.m_ExportShader->m_VRotate.x * temp.x + tri.m_ExportShader->m_VRotate.y * temp.y + tri.m_ExportShader->m_UVTranslate.y; 
    }

    // see if this vert is unique, if not push it's data into the Content::Mesh
    bool newVert;
    u32 index = GetUniqueVertexIndex( vert, newVert );

    // we are pushing back an index into m_Normals
    unsigned int vertIndex = tri.m_Vertices[vertNum].m_Index;
    unsigned int normalIdsLength = m_NormalIds.length();
    HELIUM_ASSERT( tri.m_Vertices[vertNum].m_Index < m_NormalIds.length() );
    m_SceneIndicesTracker.m_NormalIndices.push_back( (u32)m_NormalIds[tri.m_Vertices[vertNum].m_Index] );

    if( newVert )  
    {    
        contentMesh->m_Positions.push_back( vert->m_Position );
        contentMesh->m_Normals.push_back( vert->m_Normal );
        contentMesh->m_Colors.push_back( vert->m_Color );
        contentMesh->m_BaseUVs.push_back( vert->m_BaseUV );

        if( m_HasLightMap )
        {
            contentMesh->m_LightMapUVs.push_back( vert->m_LightmapUV );
        }

        if( m_HasBlendMap )
        {
            contentMesh->m_BlendMapUVs.push_back( vert->m_BlendmapUV );
        }

        // update skin indices if necessary
        if (m_ContentSkin)
        {
            m_ContentSkin->m_InfluenceIndices.push_back( (u32)m_VertexList[tri.m_Vertices[vertNum].m_Index] );
        }
    }
    return index;
}

bool ExportMesh::ComputeTriangleData( ExportTriangle &exportTri ) const
{
    EXPORT_SCOPE_TIMER( ("") );

    static const float kLengthSqrTolerance  =  1.0e-30f;

    const Math::Matrix4& globalTransform = DangerousCast<Content::Transform>(this->m_ContentObject)->m_GlobalTransform;

    if (globalTransform.Determinant() < 0.f)
    {
        VertexIndex temp = exportTri.m_Vertices[0];
        exportTri.m_Vertices[ 0 ] = exportTri.m_Vertices[ 1 ];
        exportTri.m_Vertices[ 1 ] = temp;
    }

    Math::Vector3 v0, v1, v2;
    v0 = GetPosition( exportTri.m_Vertices[0] );
    v1 = GetPosition( exportTri.m_Vertices[1] );
    v2 = GetPosition( exportTri.m_Vertices[2] );

    globalTransform.TransformVertex( v0 );
    globalTransform.TransformVertex( v1 );
    globalTransform.TransformVertex( v2 );

    if( v0.Equal( v1, ACCURACY_FOR_VECTOR_COMP ) || v0.Equal( v2, ACCURACY_FOR_VECTOR_COMP ) || v1.Equal( v2, ACCURACY_FOR_VECTOR_COMP ) )
    {
        return false;
    }

    Math::Vector2 uv0, uv1, uv2;
    uv0 = GetBaseUV( exportTri.m_Vertices[0] );
    uv1 = GetBaseUV( exportTri.m_Vertices[1] );
    uv2 = GetBaseUV( exportTri.m_Vertices[2] );

    exportTri.m_Normal = (v1 - v0).Cross( ( v2 - v0 ) );

    exportTri.m_Normal.Normalize();
    if( exportTri.m_Normal == Math::Vector3::Zero )
    {
        return false;
    }

    float interp;
    Math::Vector3 interp_vec;
    Math::Vector2 interpuv;
    Math::Vector3 tempCoord;
    Math::Vector2 tempuv;

    // COMPUTE TANGENT VECTOR
    //sort verts by their v in uv
    if (uv0.y < uv1.y)
    {
        tempCoord = v0;
        tempuv = uv0;
        v0 = v1;
        uv0 = uv1;
        v1 = tempCoord;
        uv1 = tempuv;
    }
    if (uv0.y < uv2.y)
    {
        tempCoord = v0;
        tempuv = uv0;
        v0 = v2;
        uv0 = uv2; 
        v2 = tempCoord;
        uv2 = tempuv;
    }
    if (uv1.y < uv2.y)
    {
        tempCoord = v1;
        tempuv = uv1;
        v1 = v2;
        uv1 = uv2;
        v2 = tempCoord;
        uv2 = tempuv;
    }

    // compute parametric offset along edge02 to the middle coordinate
    if (abs(uv2.y - uv0.y) < 0.000001f)
    {
        interp = 1.0f;
    }
    else
    {
        interp = (uv1.y - uv0.y)/(uv2.y - uv0.y);
    }

    // use iterpolation parameter to compute the vertex position along edge02 that has same v as vert1
    interp_vec = (v0*(1.0f - interp)) + (v2*interp);
    interpuv.y = uv1.y;
    interpuv.x = uv0.x*(1.0f - interp) + uv2.x*interp;

    // tangent vector is the ray from middle vert to the interploated vector
    exportTri.m_Tangent  = (interp_vec - v1);

    // make sure tangent points in the right direction
    if (interpuv.x < uv1.x)
    {
        exportTri.m_Tangent *= -1.0f;
    }

    exportTri.m_Tangent.Normalize();
    if( exportTri.m_Tangent == Math::Vector3::Zero )
    {
        return false;
    }

    // make sure tangent is perpendicular to the normal (unecessary step. Check and remove later. -Reddy)
    float dot = exportTri.m_Normal.Dot( exportTri.m_Tangent );
    exportTri.m_Tangent = exportTri.m_Tangent - (exportTri.m_Normal*dot);

    exportTri.m_Tangent.Normalize();
    if( exportTri.m_Tangent == Math::Vector3::Zero )
    {
        return false;
    }

    return true;
}

//
// add vertices to the wireframeIndices, but don't duplicate source wires.
// this prevents triangles sharing an edge from putting two wires into the wireframe
//

inline static void InsertWireEdge( std::vector< u32 >& wireframeIndices, S_WireEdge& sourceWires, int sourceVert0, int sourceVert1, int vert0, int vert1)
{
    if (sourceVert1 < sourceVert0)
    {
        // swap the source so that sourceVert0 < sourceVert1
        int swap = sourceVert0;
        sourceVert0 = sourceVert1;
        sourceVert1 = swap;
    }

    // create a source edge (note that sourceVert0 < sourceVert1)
    WireEdge sourceEdge( sourceVert0, sourceVert1 );
    if ( sourceWires.find( sourceEdge ) == sourceWires.end() )
    {
        // the edge didn't already exist, so record it
        sourceWires.insert( sourceEdge );
        wireframeIndices.push_back( vert0 );
        wireframeIndices.push_back( vert1 );
    }
}

//
// the work of putting a single triangle into the content mesh
// this function increments triangleIndex to represent that a triangle was recorded
//

void ExportMesh::ProcessTriangle( unsigned int& triangleIndex, unsigned int polyIndex, ExportTriangle &exportTri )
{
    EXPORT_SCOPE_TIMER( ("") );

    exportTri.m_Index = triangleIndex;

    const Content::MeshPtr contentMesh = GetContentMesh();

    contentMesh->m_ShaderIndices[triangleIndex] = m_ShaderIndices[polyIndex];
    contentMesh->m_PolygonIndices[triangleIndex] = polyIndex;

    if( !ComputeTriangleData( exportTri ) )
    {
        return;
    }

    // only get the material if this is a collision mesh
    u32 materialIndex = s_DefaultMaterialIndex;

    contentMesh->m_CollisionMaterialIndices[triangleIndex] = materialIndex;

    const TUID& shaderID = contentMesh->m_ShaderIDs[ contentMesh->m_ShaderIndices[ exportTri.m_Index ] ];
    M_UIDExportShader::const_iterator findItor = m_ExportShaderMap.find( shaderID );
    if( findItor != m_ExportShaderMap.end() )
    {
        exportTri.m_ExportShader = findItor->second;
    }

    // exportTri.m_Vertices[X].m_Index are indexes into m_VertexList
    // so here we are getting the actual index into m_Points
    u32 v0 = (u32)m_VertexList[exportTri.m_Vertices[0].m_Index];
    u32 v1 = (u32)m_VertexList[exportTri.m_Vertices[1].m_Index];
    u32 v2 = (u32)m_VertexList[exportTri.m_Vertices[2].m_Index];

    HELIUM_ASSERT( v0 != v1 );
    HELIUM_ASSERT( v0 != v2 );
    HELIUM_ASSERT( v1 != v2 );

    // here we are getting the (unique) Content::Scene vert index
    u32 triVertIdx0 = ProcessVertex( exportTri, 0 );
    u32 triVertIdx1 = ProcessVertex( exportTri, 1 );
    u32 triVertIdx2 = ProcessVertex( exportTri, 2 );

    HELIUM_ASSERT( triVertIdx0 != triVertIdx1 );
    HELIUM_ASSERT( triVertIdx0 != triVertIdx2 );
    HELIUM_ASSERT( triVertIdx1 != triVertIdx2 );

    contentMesh->m_TriangleVertexIndices.push_back( triVertIdx0 );
    contentMesh->m_TriangleVertexIndices.push_back( triVertIdx1 );
    contentMesh->m_TriangleVertexIndices.push_back( triVertIdx2 );

    // Need to generate a one-to-many mapping from 
    // The maya scene (non-unique) vert index to the (unique) Content::Scene vert index
    // (u32)m_VertexList[exportTri.m_Vertices[X].m_Index] to triVertIdxX (the unique vert)
    // and then later (when processing the blend shapes) index into this from the ideltas
    // create a delta for each unique vert and store this struct in the contentMesh
    m_SceneIndicesTracker.m_VertsIndices.insert( std::multimap< u32, u32 >::value_type( v0, ( ( triangleIndex * 3 ) + 0 ) ) );
    m_SceneIndicesTracker.m_VertsIndices.insert( std::multimap< u32, u32 >::value_type( v1, ( ( triangleIndex * 3 ) + 1 ) ) );
    m_SceneIndicesTracker.m_VertsIndices.insert( std::multimap< u32, u32 >::value_type( v2, ( ( triangleIndex * 3 ) + 2 ) ) );


    if ( exportTri.m_Vertices[2].m_Wire )
    {
        InsertWireEdge( contentMesh->m_WireframeVertexIndices, m_SourceWires, v0, v1, triVertIdx0, triVertIdx1 );
    }
    if ( exportTri.m_Vertices[0].m_Wire )
    {
        InsertWireEdge( contentMesh->m_WireframeVertexIndices, m_SourceWires, v1, v2, triVertIdx1, triVertIdx2 );
    }
    if ( exportTri.m_Vertices[1].m_Wire )
    {
        InsertWireEdge( contentMesh->m_WireframeVertexIndices, m_SourceWires, v2, v0, triVertIdx2, triVertIdx0 );
    }

    // another triangle is done
    ++triangleIndex;
}
