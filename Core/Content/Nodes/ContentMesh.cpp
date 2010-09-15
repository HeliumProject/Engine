#include "ContentMesh.h"

#include "Foundation/Math/AlignedBox.h"
#include "Foundation/Math/CalculateBounds.h"
#include "Foundation/Math/Frustum.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS(Mesh);

void Mesh::ComputeTNBs()
{
    u32 numTris = GetTriangleCount();
    u32 numVerts = GetVertexCount();

    //m_Normals.resize( numVerts );
    m_Tangents.resize( numVerts );
    m_Binormals.resize( numVerts );

    for( u32 i = 0; i < numTris; ++i )
        ComputeTNB( i );
}

bool Mesh::ComputeTNB( u32 triIndex )
{

    static const float kLengthSqrTolerance  =  1.0e-30f;
    const float EPSILON = 0.000001f;

    triIndex *= 3;
    u32 idx0 = m_TriangleVertexIndices[ triIndex ];
    u32 idx1 = m_TriangleVertexIndices[ triIndex + 1 ];
    u32 idx2 = m_TriangleVertexIndices[ triIndex + 2 ];

    Math::Vector3 v0 = m_Positions[ idx0 ];
    Math::Vector3 v1 = m_Positions[ idx1 ];
    Math::Vector3 v2 = m_Positions[ idx2 ];

    Math::Vector2 uv0 = m_BaseUVs[ idx0 ];
    Math::Vector2 uv1 = m_BaseUVs[ idx1 ];
    Math::Vector2 uv2 = m_BaseUVs[ idx2 ];

    Math::Vector3 triNormal;
    Math::Vector3 triTangent;
    Math::Vector3 triBinormal;

    triNormal = ( v1 - v0 ).Cross( (v2 - v0 ) );

    if (triNormal.LengthSquared() <= kLengthSqrTolerance)
    {
        return false;
    }
    triNormal.Normalize();

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

    //compute parametric offset along edge02 to the middle coordinate
    if (abs(uv2.y - uv0.y) < EPSILON)
    {
        interp = 1.0f;
    }
    else
    {
        interp = (uv1.y - uv0.y)/(uv2.y - uv0.y);
    }

    //use iterpolation parameter to compute the vertex position along edge02 that has same v as vert1
    interp_vec = (v0*(1.0f - interp)) + (v2*interp);
    interpuv.y = uv1.y;
    interpuv.x = uv0.x*(1.0f - interp) + uv2.x*interp;

    //tangent vector is the ray from middle vert to the interploated vector
    triTangent  = (interp_vec - v1);

    //make sure tangent points in the right direction
    if (interpuv.x < uv1.x)
    {
        triTangent *= -1.0f;
    }

    if (triTangent.LengthSquared() <= kLengthSqrTolerance)
    {
        return false;
    }

    triTangent.Normalize();

    //make sure tangent is perpendicular to the normal (unecessary step. Check and remove later. -Reddy)
    float dot = triNormal.Dot( triTangent );
    triTangent = triTangent - (triNormal*dot);

    if (triTangent.LengthSquared() <= kLengthSqrTolerance)
    {
        return false;
    }
    triTangent.Normalize();

    //Compute Binormal vector
    //sort vectors by u 
    if (uv0.x < uv1.x)
    {
        tempCoord = v0;
        tempuv = uv0;
        v0 = v1;
        uv0 = uv1;
        v1 = tempCoord;
        uv1 = tempuv;
    }
    if (uv0.x < uv2.x)
    {
        tempCoord = v0;
        tempuv = uv0;
        v0 = v2;
        uv0 = uv2;
        v2 = tempCoord;
        uv2 = tempuv;
    }
    if (uv1.x < uv2.x)
    {
        tempCoord = v1;
        tempuv = uv1;
        v1 = v2;
        uv1 = uv2;
        v2 = tempCoord;
        uv2 = tempuv;
    }

    //compute parametric offset along edge02 to the middle coordinate

    if (abs(uv2.x - uv0.x) < EPSILON)
    {
        interp = 1.0f;
    }
    else
    {
        interp = (uv1.x - uv0.x)/(uv2.x - uv0.x);
    }

    //use iterpolation parameter to compute the vertex position along edge02 that has same u as vert1
    interp_vec = (v0*(1.0f - interp)) + (v2*interp);
    interpuv.x = uv1.x;
    interpuv.y = uv0.y*(1.0f - interp) + uv2.y*interp;

    //binormal vector is the ray from middle vert to the interploated vector
    triBinormal  = (interp_vec - v1);

    //make sure binormal points in the right direction
    if (interpuv.y < uv1.y)
    {
        triBinormal *= -1.0f;
    }

    //make sure binormal is perpendicular to the normal
    dot = triBinormal.Dot( triNormal );
    triBinormal = triBinormal - (triNormal*dot);

    if ( triBinormal.Equal( Math::Vector3::Zero ) )
    {
        triBinormal = triTangent.Cross( triNormal );
    }
    else
    {
        triBinormal.Normalize();
    }

    if( triBinormal.LengthSquared() < kLengthSqrTolerance )
    {
        return false;
    }

    m_Tangents[idx0] = triTangent;
    m_Tangents[idx1] = triTangent;
    m_Tangents[idx2] = triTangent;

    m_Binormals[idx0] = triBinormal;
    m_Binormals[idx1] = triBinormal;
    m_Binormals[idx2] = triBinormal;

    return true;
}

void Mesh::GetAlignedBoundingBox( Math::AlignedBox& box ) const
{
    V_Vector3::const_iterator itr = m_Positions.begin();
    V_Vector3::const_iterator end = m_Positions.end();
    for ( ; itr != end; ++itr )
    {
        const Vector3& point = *itr;
        box.Merge( point );
    }
}

void Mesh::GetBoundingSphere( Math::BoundingVolumeGenerator::BSphere& bsphere ) const
{
    Math::BoundingVolumeGenerator generator( (Math::Vector3*)&m_Positions.front(), (i32)m_Positions.size() );
    bsphere = generator.GetPrincipleAxisBoundingSphere();
}

f32 Mesh::SurfaceArea( Math::Scale* scale ) const
{
    HELIUM_ASSERT( m_TriangleVertexIndices.size()%3 == 0 );

    f32 area = 0.0f;

    if( !scale )
    {
        std::vector< u32 >::const_iterator itr = m_TriangleVertexIndices.begin();
        std::vector< u32 >::const_iterator end  = m_TriangleVertexIndices.end();
        for( ; itr != end; itr+=3 )
        {

            Vector3 edge1 = m_Positions[*(itr+1)] - m_Positions[*itr];
            Vector3 edge3 = m_Positions[*(itr+2)] - m_Positions[*itr];

            f32 dot = edge1.Dot( edge3 );
            f32 triArea = sqrt( ( edge1.LengthSquared()*edge3.LengthSquared() ) -  ( dot*dot ) ) * 0.5f;
            if( !_isnan( triArea ) )
                area += triArea;

        }
    }
    else
    {
        std::vector< u32 >::const_iterator itr = m_TriangleVertexIndices.begin();
        std::vector< u32 >::const_iterator end  = m_TriangleVertexIndices.end();
        for( ; itr != end; itr+=3 )
        {
            Vector3 edge1 = m_Positions[*(itr+1)] * *scale - m_Positions[*itr] * *scale;
            Vector3 edge3 = m_Positions[*(itr+2)] * *scale - m_Positions[*itr] * *scale;

            f32 dot = edge1.Dot( edge3 );
            f32 triArea = sqrt( ( edge1.LengthSquared()*edge3.LengthSquared() ) -  ( dot*dot ) ) * 0.5f;
            if( !_isnan( triArea ) )
                area += triArea;

        }
    }
    return area;
}

f32 Mesh::SurfaceAreaComponents( Math::Vector3& areaVec ) const
{
    /*
    static const Math::Vector3 xAxis(1.0f, 0.0f, 0.0f );
    static const Math::Vector3 yAxis(0.0f, 1.0f, 0.0f );
    static const Math::Vector3 zAxis(0.0f, 0.0f, 1.0f );

    f32 area = 0.0f;

    std::vector< u32 >::const_iterator itr = m_TriangleVertexIndices.begin();
    std::vector< u32 >::const_iterator end  = m_TriangleVertexIndices.end();
    for( ; itr != end; itr+=3 )
    {
    const Math::Vector3& v0 = m_Positions[*itr];
    const Math::Vector3& v1 = m_Positions[*(itr+1)];
    const Math::Vector3& v2 = m_Positions[*(itr+2)];

    f32 a = (v0 - v1).Length();
    f32 b = (v1 - v2).Length();
    f32 c = (v2 - v0).Length();

    f32 p1 = (a+b+c) * 0.5f;
    f32 p2 = p1 * (p1-a)*(p1-b)*(p1-c);
    f32 triArea = sqrt(p2);

    if( !_isnan( triArea ) )
    {
    f32 xMag = v0.Dot(xAxis) + v1.Dot(xAxis) + v2.Dot(xAxis);
    f32 yMag = v0.Dot(yAxis) + v1.Dot(yAxis) + v2.Dot(yAxis);
    f32 zMag = v0.Dot(zAxis) + v1.Dot(zAxis) + v2.Dot(zAxis);

    f32 d = (xMag + yMag + zMag )/triArea;     

    Math::Vector3 triAreaVec( xMag, yMag, zMag );
    triAreaVec /= d;

    area += triArea;
    areaVec += triAreaVec;
    }
    }
    return area;
    */
    static const Math::Vector3 xAxis(1.0f, 0.0f, 0.0f );
    static const Math::Vector3 yAxis(0.0f, 1.0f, 0.0f );
    static const Math::Vector3 zAxis(0.0f, 0.0f, 1.0f );

    f32 area = 0.0f;

    Math::Scale scale( 1.5f, 0.5f, 0.5f );

    std::vector< u32 >::const_iterator itr = m_TriangleVertexIndices.begin();
    std::vector< u32 >::const_iterator end  = m_TriangleVertexIndices.end();
    for( ; itr != end; itr+=3 )
    {
        f32 triArea = 0.0f;

        const Math::Vector3& v0 = m_Positions[*itr];
        const Math::Vector3& v1 = m_Positions[*(itr+1)];
        const Math::Vector3& v2 = m_Positions[*(itr+2)];

        {

            f32 a = (v0 - v1).Length();
            f32 b = (v1 - v2).Length();
            f32 c = (v2 - v0).Length();

            f32 p1 = (a+b+c) * 0.5f;
            f32 p2 = p1 * (p1-a)*(p1-b)*(p1-c);
            triArea = sqrt(p2);
        }

        f32 scaledTriArea = 0.0f;
        const Math::Vector3& ScaledV0 = m_Positions[*itr] * scale;
        const Math::Vector3& ScaledV1 = m_Positions[*(itr+1)] * scale;
        const Math::Vector3& ScaledV2 = m_Positions[*(itr+2)] * scale;

        {
            f32 a = (ScaledV0 - ScaledV1).Length();
            f32 b = (ScaledV1 - ScaledV2).Length();
            f32 c = (ScaledV2 - ScaledV0).Length();

            f32 p1 = (a+b+c) * 0.5f;
            f32 p2 = p1 * (p1-a)*(p1-b)*(p1-c);
            scaledTriArea = sqrt(p2);
        }

        if( !_isnan( triArea ) )
        {
            f32 xMag = abs(v0.Dot(xAxis)) + abs(v1.Dot(xAxis)) + abs(v2.Dot(xAxis));
            f32 yMag = abs(v0.Dot(yAxis)) + abs(v1.Dot(yAxis)) + abs(v2.Dot(yAxis));
            f32 zMag = abs(v0.Dot(zAxis)) + abs(v1.Dot(zAxis)) + abs(v2.Dot(zAxis));

            //f32 d = (xMag + yMag + zMag )/triArea;     

            //Math::Vector3 triAreaVec( xMag/triArea, yMag/triArea, zMag/triArea );
            // triAreaVec /= d;

            f32 magSum = xMag + yMag + zMag;
            Math::Vector3 triAreaVec( xMag/magSum, yMag/magSum, zMag/magSum );

            f32 sqrtTriArea = sqrt(triArea);

            area += triArea;
            areaVec += triAreaVec;
        }
    }
    return area;
}

void Mesh::GetTriangle( u32 triIndex, Math::Vector3& v0, Math::Vector3& v1, Math::Vector3& v2, Math::Matrix4* transform )
{
    HELIUM_ASSERT( m_TriangleVertexIndices.size()%3 == 0 );
    HELIUM_ASSERT( triIndex < GetTriangleCount() );

    triIndex *= 3;


    v0 = m_Positions[m_TriangleVertexIndices[triIndex]];
    v1 = m_Positions[m_TriangleVertexIndices[triIndex+1]];
    v2 = m_Positions[m_TriangleVertexIndices[triIndex+2]];

    if( transform )
    {
        transform->TransformVertex( v0 );
        transform->TransformVertex( v1 );
        transform->TransformVertex( v2 );
    }
}

/////////////////////////////////////////////////////////////
// welds mesh verts for a given threshold
/////////////////////////////////////////////////////////////
void Mesh::WeldMeshVerts(const f32 vertex_merge_threshold)
{
    Math::V_Vector3  pos_array;
    MM_i32 pos_lookup;

    std::vector< i32 > old_to_new_vert_mapping;
    i32 iv = 0;

    pos_array.reserve(m_Positions.size());
    old_to_new_vert_mapping.reserve(m_Positions.size());

    for (Math::V_Vector3::iterator ivert = m_Positions.begin(); ivert != m_Positions.end(); ++ivert, ++iv)
    {
        // compute a min and max key based on vertex position
        float fkey = (ivert->x + ivert->y + ivert->z) * 100.0f;
        i32 min_key = (i32)( fkey - 0.1f );
        i32 max_key = (i32)( fkey + 0.1f );

        // check if this vertex position is very similar to one already in the vertex position array
        i32 match_idx = Math::LookupPosInArray(*ivert, min_key, max_key, pos_array, pos_lookup, vertex_merge_threshold );

        // add there was no matching vertex position in the array...
        if (match_idx < 0)
        {
            // add an entry in the lookup multi-map for the new vertex position
            pos_lookup.insert( std::make_pair( (i32)fkey, (i32)pos_array.size() ) );
            old_to_new_vert_mapping.push_back((i32)pos_array.size() );
            // add this vertex position to the master list
            pos_array.push_back( *ivert);
        }
        else
        {
            // add this vert to the smoothable list for the matching position
            old_to_new_vert_mapping.push_back(match_idx );
        }
    }
    m_Positions = pos_array;

    //fix tri data
    std::vector< u32 >::iterator itr = m_TriangleVertexIndices.begin();
    std::vector< u32 >::iterator end  = m_TriangleVertexIndices.end();
    for( ; itr != end; ++itr )
    {
        *itr = old_to_new_vert_mapping[*itr];
    }
    itr = m_WireframeVertexIndices.begin();
    end  = m_WireframeVertexIndices.end();
    for( ; itr != end; ++itr )
    {
        *itr = old_to_new_vert_mapping[*itr];
    }
}

u32  Mesh::GetEdgeIdForVerts(u32 vert_a, u32 vert_b)
{
    std::vector< u32 >::const_iterator iter =  m_WireframeVertexIndices.begin();
    u32 edge_id = 0;
    for (; iter!= m_WireframeVertexIndices.end(); ++edge_id)
    {
        u32 vert_ids[2];
        vert_ids[0] = *iter;
        ++iter;
        vert_ids[1] = *iter;
        ++iter;
        if  ((vert_ids[0] == vert_a || vert_ids[0] == vert_b) &&
            (vert_ids[1] == vert_a || vert_ids[1] == vert_b) )
        {
            return edge_id;
        }
    }
    return 0xFFFFFFFF;
}

void Mesh::AddTri(u32 vert_a, u32 vert_b, u32 vert_c)
{
    m_TriangleVertexIndices.push_back(vert_a);
    m_TriangleVertexIndices.push_back(vert_b);
    m_TriangleVertexIndices.push_back(vert_c);
    if (GetEdgeIdForVerts(vert_a, vert_b) == 0xFFFFFFFF)
    {
        m_WireframeVertexIndices.push_back(vert_a);
        m_WireframeVertexIndices.push_back(vert_b);
    }
    if (GetEdgeIdForVerts(vert_b, vert_c) == 0xFFFFFFFF)
    {
        m_WireframeVertexIndices.push_back(vert_b);
        m_WireframeVertexIndices.push_back(vert_c);
    }
    if (GetEdgeIdForVerts(vert_c, vert_a) == 0xFFFFFFFF)
    {
        m_WireframeVertexIndices.push_back(vert_c);
        m_WireframeVertexIndices.push_back(vert_a);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lifted from our igintersection (for now no bsphere broad culling. if performance of this sucks then I will put one in)
//  NearestPointInTriangle()
//  - given a point in the plane of the triangle, finds the nearest point in the triangle.
//    returns 0 if exterior and 1 if interior
//  Parameters:
//   - dv1:            (v0->v1)
//   - dv2:            (v0->v2)
//   - n:              normal
//   - dpp:            (v0->pp) -- this value is modified and returned as the result
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
u32 NearestPointInTri(Math::Vector3 dv1, Math::Vector3 dv2, Math::Vector3 normal, Math::Vector3& dpp)
{
    Math::Vector3 snap_a, snap_b;
    // check pp against edge (v0->v1)
    Math::Vector3 edge = dpp.Cross(dv1);
    if ( edge.Dot(normal) < 0.0f)
    {
        snap_a.Set(0.0f, 0.0f, 0.0f);
        snap_b = dv1;
    }
    else
    {
        // check pp against edge (v0->v2)
        edge = dv2.Cross(dpp);
        if ( edge.Dot(normal)< 0.0f)
        {
            snap_a = dv2;
            snap_b.Set(0.0f, 0.0f, 0.0f);
        }
        else
        {
            // check pp against edge (v1->v2)
            Math::Vector3 ev2 = dv2 - dv1;
            Math::Vector3 epp = dpp - dv1;

            edge = epp.Cross(ev2);
            if ( edge.Dot(normal) < 0.0f)
            {
                snap_a = dv1;
                snap_b = dv2;
            }
            else
            {
                // return 'interior'
                return 1;
            }
        }
    }
    // snap to nearest point on line segment if pp is outside triangle 
    {
        Math::Vector3 lvb  = snap_b - snap_a;
        Math::Vector3 lpp  = dpp - snap_a;
        f32  dotn = lpp.Dot(lvb);
        f32  dotd = lvb.Dot(lvb);
        f32  t    = (dotn / dotd);
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        dpp = lvb * t;
        dpp = dpp + snap_a;
        // return 'exterior'
        return 0;
    }
}

u32 Mesh::GetClosestTri(const Math::Vector3& sphere_pos, const f32& sphere_rad)
{
    u32 res_tri_id = 0xFFFFFFFF;
    f32   best_dist_sq = sphere_rad * sphere_rad;
    for (u32 i=0; i< ((u32)m_TriangleVertexIndices.size()/3); ++i)
    {
        Math::Vector3 tri_verts[3];
        tri_verts[0] = m_Positions[m_TriangleVertexIndices[3*i]];
        tri_verts[1] = m_Positions[m_TriangleVertexIndices[3*i+1]];
        tri_verts[2] = m_Positions[m_TriangleVertexIndices[3*i+2]];
        Math::Vector3 dv1 = tri_verts[1] - tri_verts[0];
        Math::Vector3 dv2 = tri_verts[2] - tri_verts[0];
        Math::Vector3 dp  = sphere_pos - tri_verts[0];
        Math::Vector3 normal = dv2.Cross(dv1);
        f32 dotn = dp.Dot(normal);
        /* //IF we need to not include the back faced tris then uncomment this block
        if (dotn < 0.0f)
        {
        continue;
        }
        */
        f32 dotd = normal.LengthSquared();
        // abort if normal is too short (degenerate triangle)
        if (dotd < 0.0000001f)//need to see where tools Epsilon is) 
        {
            continue;
        }
        f32 t = (dotn / dotd);
        Math::Vector3 dpp = normal * (-t);
        dpp = dpp + dp;
        // abort if nearest-point on plane is farther than radius
        f32 dsq = normal.LengthSquared() * t * t;
        if (best_dist_sq < dsq)
        {
            continue;
        }
        NearestPointInTri(dv1, dv2, normal, dpp);
        Math::Vector3 ip_to_closest_pt = dpp - dp;
        dsq = ip_to_closest_pt.LengthSquared();
        if (best_dist_sq < dsq)
        {
            continue;
        }
        best_dist_sq= dsq;
        res_tri_id = i;
    }
    return res_tri_id;
}

void Mesh::NopTrisByTriList(const std::vector< u32 >& ip_tris)
{
    for (std::vector< u32 >::const_iterator it=ip_tris.begin(); it!=ip_tris.end(); ++it)
    {
        HELIUM_ASSERT(*it <= (m_TriangleVertexIndices.size()/3));
        m_TriangleVertexIndices[(*it)*3] = 0xFFFFFFFF;
        m_TriangleVertexIndices[(*it)*3+1] = 0xFFFFFFFF;
        m_TriangleVertexIndices[(*it)*3+2] = 0xFFFFFFFF;
    }
}

void Mesh::NopTrisByVertList(const std::vector< u32 >& ip_verts)
{
    u32 num_tris = (u32)(m_TriangleVertexIndices.size()/3);
    for (u32 i=0; i<num_tris; ++i)
    {
        u32 tr_vert_ids[3];
        tr_vert_ids[0] = m_TriangleVertexIndices[i*3];
        tr_vert_ids[1] = m_TriangleVertexIndices[i*3+1];
        tr_vert_ids[2] = m_TriangleVertexIndices[i*3+2];
        std::vector< u32 >::const_iterator it = std::find(ip_verts.begin(), ip_verts.end(), tr_vert_ids[0]);
        if (it != ip_verts.end())
        {
            m_TriangleVertexIndices[(i)*3] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+1] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+2] = 0xFFFFFFFF;
            continue;
        }
        it = std::find(ip_verts.begin(), ip_verts.end(), tr_vert_ids[1]);
        if (it != ip_verts.end())
        {
            m_TriangleVertexIndices[(i)*3] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+1] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+2] = 0xFFFFFFFF;
            continue;
        }
        it = std::find(ip_verts.begin(), ip_verts.end(), tr_vert_ids[2]);
        if (it != ip_verts.end())
        {
            m_TriangleVertexIndices[(i)*3] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+1] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+2] = 0xFFFFFFFF;
            continue;
        }
    }
}

void Mesh::NopTrisByEdgeList( const std::vector< u32 >& ip_edges )
{
    std::vector< u32 > tris;
    u32 num_tris = (u32)(m_TriangleVertexIndices.size()/3);

    for (std::vector< u32 >::const_iterator it=ip_edges.begin(); it!=ip_edges.end(); it++)
    {
        u32 vert_1 = m_WireframeVertexIndices[(*it)*2];
        u32 vert_2 = m_WireframeVertexIndices[((*it)*2)+1];

        for (u32 i=0; i<num_tris; ++i)
        {
            u32 tr_vert_ids[3];
            tr_vert_ids[0] = m_TriangleVertexIndices[i*3];
            tr_vert_ids[1] = m_TriangleVertexIndices[i*3+1];
            tr_vert_ids[2] = m_TriangleVertexIndices[i*3+2];

            if ( (vert_1 == tr_vert_ids[0] || vert_1 == tr_vert_ids[1] || vert_1 == tr_vert_ids[2])
                && (vert_2 == tr_vert_ids[0] || vert_2 == tr_vert_ids[1] || vert_2 == tr_vert_ids[2]) )
            {
                m_TriangleVertexIndices[(i)*3] = 0xFFFFFFFF;
                m_TriangleVertexIndices[(i)*3+1] = 0xFFFFFFFF;
                m_TriangleVertexIndices[(i)*3+2] = 0xFFFFFFFF;
            }
        }
    }
}

void Mesh::PruneVertsNotInTris()
{
    std::vector< u8 > vert_is_in_tris;
    vert_is_in_tris.resize(m_Positions.size(), 0);
    for (std::vector< u32 >::const_iterator it=m_TriangleVertexIndices.begin(); it!=m_TriangleVertexIndices.end(); ++it)
    {
        if ((*it) != 0xFFFFFFFF)
        {
            vert_is_in_tris[*it] = 1;
        }
    }
    std::vector< u32 > vert_remap;
    vert_remap.resize(m_Positions.size(), 0xFFFFFFFF);
    u32 next_vert_index = 0;
    u32 vert_id = 0;
    for (std::vector< u8 >::const_iterator it=vert_is_in_tris.begin(); it!=vert_is_in_tris.end(); ++it, ++vert_id)
    {
        if ((*it))
        {
            vert_remap[vert_id] = next_vert_index;
            m_Positions[next_vert_index] = m_Positions[vert_id];
            ++next_vert_index;
        }
    }
    m_Positions.resize(next_vert_index);
    std::vector< u32 > new_tri_vert_ids;
    std::vector< u32 > new_edge_vert_ids;
    new_tri_vert_ids.reserve(m_TriangleVertexIndices.size());
    new_edge_vert_ids.reserve(m_TriangleVertexIndices.size());
    u32 old_tri_cnt = (u32)(m_TriangleVertexIndices.size()/3);
    for (u32 i=0; i<old_tri_cnt; ++i)
    {
        u32 tr_vert_ids[3];
        tr_vert_ids[0] = m_TriangleVertexIndices[i*3];
        tr_vert_ids[1] = m_TriangleVertexIndices[i*3+1];
        tr_vert_ids[2] = m_TriangleVertexIndices[i*3+2];
        if (tr_vert_ids[0] == 0xFFFFFFFF)
        {
            continue;
        }
        HELIUM_ASSERT(tr_vert_ids[1] != 0xFFFFFFFF && tr_vert_ids[2] != 0xFFFFFFFF);
        new_tri_vert_ids.push_back(vert_remap[tr_vert_ids[0]]);
        new_tri_vert_ids.push_back(vert_remap[tr_vert_ids[1]]);
        new_tri_vert_ids.push_back(vert_remap[tr_vert_ids[2]]);

        //duplicates..but who cares whole concept of having wireframe edge ids isnt good as we could render tris wireframe
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[0]]);
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[1]]);
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[1]]);
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[2]]);
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[2]]);
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[0]]);
    }
    m_TriangleVertexIndices = new_tri_vert_ids;
    m_WireframeVertexIndices = new_edge_vert_ids;
}

void Mesh::DeleteTris(const std::vector< u32 >& ip_tris)
{
    NopTrisByTriList(ip_tris);
    PruneVertsNotInTris();
}
void Mesh::DeleteVerts(const std::vector< u32 >& ip_verts)
{
    NopTrisByVertList(ip_verts);
    PruneVertsNotInTris();
}

void Mesh::DeleteEdges( const std::vector< u32 >& ip_edges )
{
    NopTrisByEdgeList( ip_edges );
    PruneVertsNotInTris();
}

u32 Mesh::GetClosestVert(const Math::Vector3& sphere_start_pos, const f32& sphere_rad, const Math::Vector3& swept_dir, const f32& len)
{
    u32 res_vert_index = 0xFFFFFFFF;
    f32 min_dist = len + 2.0f*sphere_rad;
    f32 sphere_rad_sqr = sphere_rad*sphere_rad;
    u32 i=0;
    for (Math::V_Vector3::const_iterator iter = m_Positions.begin(); iter != m_Positions.end(); ++iter, ++i)
    {
        Math::Vector3 vec_to_pt = *iter - sphere_start_pos;
        f32 dot = vec_to_pt.Dot(swept_dir);
        dot = Clamp(dot, 0.0f, len);
        Math::Vector3 closest_pt_on_axis = swept_dir*dot;
        Math::Vector3 closest_pt_to_pos = closest_pt_on_axis - vec_to_pt;
        f32 dist_sqr = closest_pt_to_pos.LengthSquared();
        if ( dist_sqr < sphere_rad_sqr)
        {
            if (dot < min_dist)
            {
                min_dist = dot;
                res_vert_index = i;
            }
        }
    }
    return res_vert_index;
}

u32 Mesh::GetClosestVert(const Math::Matrix4& view_proj_mat, const f32 porj_space_threshold_sqr, Math::Vector2 proj_pt)
{
    u32 res_vert_index = 0xFFFFFFFF;
    f32 min_z = 1.0f;
    u32 i=0;
    for (Math::V_Vector3::const_iterator iter = m_Positions.begin(); iter != m_Positions.end(); ++iter, ++i)
    {
        Math::Vector4 v ( iter->x, iter->y, iter->z, 1.f );
        view_proj_mat.Transform( v );
        v /= (v.w);

        if (v.z <0.0f || v.z > min_z || v.x < -1.0f  || v.x > 1.0f || v.y < -1.0f || v.y > 1.0f)
        {
            continue;
        }
        Math::Vector2 v_pt;
        v_pt.x = v.x;
        v_pt.y = v.y;
        v_pt -= proj_pt;
        f32 v_pt_len_sqr = v_pt.LengthSquared();
        if ( v_pt_len_sqr < porj_space_threshold_sqr)
        {
            min_z = v.z;
            res_vert_index = i;
        }
    }
    return res_vert_index;
}

void ClosestPtsOnLinesegVsLineseg(const Math::Vector3& line_a_start, const Math::Vector3& line_a_end,
                                  const Math::Vector3& line_b_start, const Math::Vector3& line_b_end,
                                  Math::Vector3& closest_pt_on_line_a, Math::Vector3& closest_pt_on_line_b, f32& linea_a_t, f32& line_b_t)
{

    Math::Vector3 l0 = line_a_end - line_a_start;
    Math::Vector3 l1 = line_b_end - line_b_start;
    Math::Vector3 ds = line_b_start - line_a_start;

    Math::Vector3 c  = l0.Cross(l1);
    Math::Vector3 c0 = ds.Cross(l1);
    Math::Vector3 c1 = ds.Cross(l0);

    f32 t0 = c0.Dot(c) / c.Dot(c);
    f32 t1 = c1.Dot(c) / c.Dot(c);

    t0 = t0<0 ? 0 : t0>1 ? 1 : t0;
    t1 = t1<0 ? 0 : t1>1 ? 1 : t1;

    f32 d0 = t1*l0.Dot(l1) + ds.Dot(l0);
    f32 d1 = t0*l0.Dot(l1) - ds.Dot(l1);

    linea_a_t = d0 / l0.Dot(l0);
    line_b_t = d1 / l1.Dot(l1);

    linea_a_t = linea_a_t<0 ? 0 : linea_a_t>1 ? 1 : linea_a_t;
    line_b_t = line_b_t<0 ? 0 : line_b_t>1 ? 1 : line_b_t;

    closest_pt_on_line_a = line_a_start + l0*linea_a_t;
    closest_pt_on_line_b = line_b_start + l1*line_b_t;
}
u32 Mesh::GetClosestEdge(const Math::Vector3& sphere_start_pos, const f32& sphere_rad, const Math::Vector3& swept_dir, const f32& len)
{
    u32 res_edge_index = 0xFFFFFFFF;
    f32 min_dist = len;
    f32 sphere_rad_sqr = sphere_rad*sphere_rad;
    Math::Vector3 ss_end = sphere_start_pos + swept_dir*len;
    u32 i=0;

    std::vector< u32 >::const_iterator iter =  m_WireframeVertexIndices.begin();
    u32 edge_id = 0;
    for (; iter!= m_WireframeVertexIndices.end(); ++edge_id)
    {
        u32 vert_ids[2];
        vert_ids[0] = *iter;
        ++iter;
        vert_ids[1] = *iter;
        ++iter;
        Math::Vector3& edge_verts_0 = m_Positions[vert_ids[0]];
        Math::Vector3& edge_verts_1 = m_Positions[vert_ids[1]];
        Math::Vector3 closest_pt_ss, closest_pt_edge;
        f32 ss_t, edge_t;
        ClosestPtsOnLinesegVsLineseg(sphere_start_pos, ss_end, edge_verts_0, edge_verts_1, closest_pt_ss, closest_pt_edge, ss_t, edge_t);
        Math::Vector3 diff = closest_pt_edge - closest_pt_ss;
        if (diff.LengthSquared() < sphere_rad_sqr)
        {
            f32 d = ss_t * len;
            if (min_dist > d)
            {
                min_dist = d;
                res_edge_index = edge_id;
            }
        }
    }
    return res_edge_index;
}

u32 Mesh::GetClosestTri(const Math::Vector3& sphere_start_pos, const f32& sphere_rad, const Math::Vector3& swept_dir, const f32& len)
{
    u32 res_tri_index = 0xFFFFFFFF;
    f32 min_dist = len;
    f32 sphere_rad_sqr = sphere_rad*sphere_rad;
    Math::Vector3 ss_end = sphere_start_pos + swept_dir*len;

    std::vector< u32 >::const_iterator iter =  m_TriangleVertexIndices.begin();
    u32 tri_id = 0;
    for (; iter!= m_TriangleVertexIndices.end(); ++tri_id)
    {
        u32 vert_ids[3];
        vert_ids[0] = *iter;
        ++iter;
        vert_ids[1] = *iter;
        ++iter;
        vert_ids[2] = *iter;
        ++iter;
        Math::Vector3 v[3];
        v[0] = m_Positions[vert_ids[0]];
        v[1] = m_Positions[vert_ids[1]];
        v[2] = m_Positions[vert_ids[2]];
        Math::Vector3 dv1 = v[1] - v[0];
        Math::Vector3 dv2 = v[2] - v[0];
        Math::Vector3 plane_normal = dv2.Cross(dv1);
        plane_normal = plane_normal.Normalize();
        f32 plane_d = plane_normal.Dot(v[0]);
        f32 temp_dot = swept_dir.Dot(plane_normal);
        if (fabs(temp_dot) < 0.0001f)
        {
            continue;
        }
        f32 t = plane_d - sphere_start_pos.Dot(plane_normal);
        t = t/temp_dot;
        Math::Vector3 pt_on_plane = sphere_start_pos + swept_dir*t;
        //see if the pt is inside the tri
        bool outside = false;
        for (u32 j=0; j<3; ++j)
        {
            Math::Vector3 v_to_pt = pt_on_plane - v[j];
            Math::Vector3 edge = v[(j+1)%3] - v[j];
            Math::Vector3 cross_p = v_to_pt.Cross(edge);
            if (cross_p.Dot(plane_normal) < 0.0f)
            {
                outside = true;
                break;
            }
        }
        if (!outside && t < min_dist)
        {
            min_dist = t;
            res_tri_index = tri_id;
        }
    }
    return res_tri_index;
}

void Mesh::GetEdges( S_Edge& edges ) const 
{
    std::vector< u32 >::const_iterator itr = m_TriangleVertexIndices.begin();
    std::vector< u32 >::const_iterator end  = m_TriangleVertexIndices.end();
    for( ; itr != end; itr+=3 )
    {
        edges.insert( Edge( *itr, *(itr+1) ) );
        edges.insert( Edge( *(itr+1), *(itr+2) ) );
        edges.insert( Edge( *(itr+2), *itr ) );
    }
}

f32 Mesh::VertDensity() const
{
    S_Edge edges;
    GetEdges( edges );

    f32 edgeLength = 0.0f;

    S_Edge::iterator itr = edges.begin();
    S_Edge::iterator end  = edges.end();

    for( ; itr != end; ++itr )
    {
        edgeLength += (m_Positions[ itr->m_VertIndices[0]] - m_Positions[ itr->m_VertIndices[1] ]).Length();
    }

    return ( edges.size() * 2.0f ) / edgeLength;
}

bool ValidFloat(float f_)
{
    return (!_isnan(f_) && f_ != FLT_MAX && f_ != -FLT_MAX);
}
#pragma warning (default:4056)
#pragma warning (default:4756)
bool ValidVec3(const Math::Vector3& v_) //need to move this some where
{
    return ( ValidFloat(v_.x) && ValidFloat(v_.y) && ValidFloat(v_.z) );
}
#define ZER_AREA 1.0e-6
void Mesh::PruneInvalidTris()
{
    std::vector< u32 >::const_iterator iter =  m_TriangleVertexIndices.begin();
    u32 tri_id = 0;
    std::vector< u32 > invalid_tris;
    for (; iter!= m_TriangleVertexIndices.end(); ++tri_id)
    {
        u32 vert_ids[3];
        vert_ids[0] = *iter;
        ++iter;
        vert_ids[1] = *iter;
        ++iter;
        vert_ids[2] = *iter;
        ++iter;
        Math::Vector3 v[3];
        v[0] = m_Positions[vert_ids[0]];
        v[1] = m_Positions[vert_ids[1]];
        v[2] = m_Positions[vert_ids[2]];
        Math::Vector3 dv1 = v[1] - v[0];
        Math::Vector3 dv2 = v[2] - v[0];
        Math::Vector3 plane_normal = dv2.Cross(dv1);
        f32 area =  plane_normal.Length()/2.0f;
        if (area < ZER_AREA || (!ValidVec3(v[0])) || (!ValidVec3(v[1])) || (!ValidVec3(v[2])))
        {
            invalid_tris.push_back(tri_id);
        }
    }
    DeleteTris(invalid_tris);
}

void Mesh::MergeVertToClosest(u32 ip_vert_id)
{
    u32 closest_vert_id = 0xFFFFFFFF;
    f32 min_dist_sqr = 10000.0f;//ok this should be big enough ever else one is screwing some thing bad
    u32 i=0;
    Math::Vector3 input_vert_pos = m_Positions[ip_vert_id];
    for (Math::V_Vector3::const_iterator iter = m_Positions.begin(); iter != m_Positions.end(); ++iter, ++i)
    {
        if (i == ip_vert_id)
        {
            continue;
        }
        Math::Vector3 diff = *iter - input_vert_pos;
        f32 diff_len_sqr = diff.LengthSquared();
        if ( diff_len_sqr < min_dist_sqr)
        {
            min_dist_sqr = diff_len_sqr;
            closest_vert_id = i;
        }
    }
    if (closest_vert_id == 0xFFFFFFFF)
    {
        return;
    }
    //swap verts in the tris which have just the input vert and not the closest vert and delete the ones which have both
    std::vector< u32 > tris_to_be_deleted;//ones with both vert_id and closest_vert_id
    {
        u32 tri_id = 0;
        std::vector< u32 > invalid_tris;
        std::vector< u32 >::const_iterator iter =  m_TriangleVertexIndices.begin();
        for (; iter!= m_TriangleVertexIndices.end(); ++tri_id)
        {
            u32 vert_ids[3];
            vert_ids[0] = *iter;
            ++iter;
            vert_ids[1] = *iter;
            ++iter;
            vert_ids[2] = *iter;
            ++iter;
            u32 has_ip_vert_id = 0;
            bool has_closest_vert_id = false;
            for (u32 j=0; j<3; ++j)
            {
                if (vert_ids[j] == ip_vert_id)
                {
                    has_ip_vert_id = j+1;
                }
                if (vert_ids[j] == closest_vert_id)
                {
                    has_closest_vert_id = true;
                }
            }
            if (has_closest_vert_id && has_ip_vert_id)
            {
                tris_to_be_deleted.push_back(tri_id);
            }
            else if (has_ip_vert_id)
            {
                //swap the ip_vert_id with the closest_vert_id
                m_TriangleVertexIndices[3*tri_id + has_ip_vert_id-1] = closest_vert_id;
            }
        }
    }
    DeleteTris(tris_to_be_deleted);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Given an convex poly P and a triangle T, this function triangulates the set difference T-P.
// All points are expressed as vec4f's but only the x & z components are used.
//
// This is done in 4 stages:
// 1: create a set of unclipped edges from the poly verts
// 2: clip the edges to each triangle halfspace in turn
// 3: join each surviving edge to an appropriate triangle vertex
// 4: add up to 3 gap-filling triangles (one wherever an original triangle edge remains fully outside)
//
// Input:
// tri    - the start of an array of 3 vec4f's
// poly   - the start of an array of any number (>=3) of vec4f's
// nverts - the number of verts in the poly
//
// Output:
// out    - this array will be filled with triplets of vec4f's representing the output triangles
// return value - the number of triangles output
//
// Note that no bounds checking is done on the output; the array 'out' should therefore be large enough
// to hold the worst-case output which is 3*nverts+9 vec4f's.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

f32 CrossVectorXZ(const Math::Vector3& v1, const Math::Vector3& v2)
{
    return(v2.x * v1.z - v1.x * v2.z);
}

void LerpVector3(Math::Vector3& r, const Math::Vector3& v1, const Math::Vector3& v2, f32 t)
{
    r.x = v1.x + (v2.x - v1.x) * t;
    r.y = v1.y + (v2.y - v1.y) * t;
    r.z = v1.z + (v2.z - v1.z) * t;
}

i32 SenseOfTriangle(Math::Vector3 &v0, Math::Vector3 &v1, Math::Vector3 &v2)
{
    Math::Vector3 dv0, dv1;
    dv0 = v1 - v0;
    dv1 = v2 - v1;
    f32 c = CrossVectorXZ(dv0, dv1);
    return (c >= 0.0f) ? 1 : -1;
}

u32 TriMinusPoly(Math::Vector3 *out, Math::Vector3 *tri, Math::Vector3 *poly, u32 nverts)
{
    Math::Vector3 vP0, vP1, vP2, dvP, vT0, vT1, dvT, dv0, dv1, dv2, *p_in, *p_out, *p_end, vP0_prev, vP1_prev, vP2_prev;
    // determine anticlockwise / clockwise sense of tri and poly
    f32 sense_of_source_tri  = (f32)SenseOfTriangle(poly[0], poly[1], poly[2]);
    i32 sense_of_source_poly = SenseOfTriangle(tri[0],  tri[1],  tri[2]);


    // initialise the set of unclipped poly edges (clockwise)
    vP1 = poly[nverts-1];
    p_end = out;
    for (u32 i=0; i<nverts; i++)
    {
        vP0 = vP1;
        vP1 = poly[i];
        *p_end++ = vP0;
        *p_end++ = vP1;
        p_end++;
    }

    // loop over the 3 halfspaces of the tri, each time clipping the current set of edges
    vT1 = tri[2];
    for (u32 i=0; i<3; i++)
    {
        // get tri edge
        vT0 = vT1;
        vT1 = tri[i];
        //SubVector4(dvT, vT1, vT0);
        dvT = vT1 - vT0;

        // loop over current set of poly edges
        p_in = p_out = out;   // we can overwrite the edges in place
        while (p_in < p_end)
        {
            // get poly edge
            vP0 = *p_in++;
            vP1 = *p_in++;
            p_in++;
            //SubVector4(dvP, vP1, vP0);
            dvP = vP1 - vP0;

            // edge endpoints relative to vT0
            //SubVector4(dv0, vP0, vT0);
            dv0 = vP0 - vT0;
            //SubVector4(dv1, vP1, vT0);
            dv1 = vP1 - vT0;

            // cross prods for classification and intersection
            f32 c0 = CrossVectorXZ(dvT, dv0)* sense_of_source_tri;
            f32 c1 = CrossVectorXZ(dvT, dv1)* sense_of_source_tri;

            // don't output degenerate edges
            if (c0<=0 && c1<=0)
            {
                continue;
            }

            // conditionally output initial endpoint
            if (c0>=0) *p_out++ = vP0;

            // conditionally output intersection
            if ((c0<0) ^ (c1<0)) LerpVector3(*p_out++, vP0, vP1, c0/(c0-c1));

            // conditionally output final endpoint
            if (c1>=0) *p_out++ = vP1;

            // conditionally output pad
            if (c0>=0 || c1>=0) p_out++;
        }

        p_end = p_out;
        if (p_end == out) return 0;
    }

    // loop over clipped edges and add a 3rd vertex to complete each triangle
    p_out  = out;
    u32 tri_verts_out  = 0;
    u32 tri_verts_used = 0;
    while (p_out < p_end)
    {
        // get poly edge
        vP0 = *p_out++;
        vP1 = *p_out++;
        //SubVector4(dvP, vP1, vP0);
        dvP = vP1 - vP0;

        // output tri vert furthest outside poly edge
        //SubVector4(dv0, tri[0], vP0);
        dv0 = tri[0] - vP0;
        //SubVector4(dv1, tri[1], vP0);
        dv1 = tri[1] - vP0;
        //SubVector4(dv2, tri[2], vP0);
        dv2 = tri[2] - vP0;

        f32 c[3];
        c[0] = CrossVectorXZ(dvP, dv0);
        c[1] = CrossVectorXZ(dvP, dv1);
        c[2] = CrossVectorXZ(dvP, dv2);


        u32 tri_vert = (c[0] < c[1]) ? (c[0] < c[2]) ? 0 : 2 : (c[1] < c[2]) ? 1 : 2;

        *p_out++ = tri[tri_vert];

        tri_verts_used |= (1 << tri_vert);
        tri_verts_out  |= ((c[0] < 0.0f) << 0);
        tri_verts_out  |= ((c[1] < 0.0f) << 1);
        tri_verts_out  |= ((c[2] < 0.0f) << 2);
    }

    // find loose vert (if any)
    u32 loose_vert_flags = tri_verts_out & ~tri_verts_used;
    i32 loose_vert = -1;
    if (loose_vert_flags & 0x1)
    {
        loose_vert = 0;
    }
    else if (loose_vert_flags & 0x2)
    {
        loose_vert = 1;
    }
    else if (loose_vert_flags & 0x4)
    {
        loose_vert = 2;
    }

    // loop over tris and fill in any gaps
    p_in = out;
    p_out = p_end;
    vP0 = p_end[-3];
    vP1 = p_end[-2];
    vP2 = p_end[-1];
    while (p_in < p_end)
    {
        vP0_prev = vP0;
        vP1_prev = vP1;
        vP2_prev = vP2;

        vP0 = *p_in++;
        vP1 = *p_in++;
        vP2 = *p_in++;

        if (vP1_prev.x==vP0.x && vP1_prev.z==vP0.z)
        {
            if (vP2_prev.x!=vP2.x || vP2_prev.z!=vP2.z)
            {
                // ensure the gap-filling tri has the correct sense
                if (SenseOfTriangle(vP0, vP2, vP2_prev) != sense_of_source_poly)
                {
                    // add a gap-filling tri
                    *p_out++ = vP0;
                    *p_out++ = vP2;
                    *p_out++ = vP2_prev;
                }
                else if (loose_vert >= 0)
                {
                    // attach loose vertex to its neighbouring tris
                    *p_out++ = vP2_prev;
                    *p_out++ = vP0;
                    *p_out++ = tri[loose_vert];

                    *p_out++ = vP0;
                    *p_out++ = vP2;
                    *p_out++ = tri[loose_vert];
                }
            }
        }
        else if (loose_vert >= 0)
        {
            // treat isolated triangles attaching to loose vertex
            if ((SenseOfTriangle(tri[loose_vert], vP0_prev, vP1_prev) != sense_of_source_poly) &&
                (SenseOfTriangle(tri[loose_vert], vP2_prev, vP1_prev) != sense_of_source_poly))
            {
                *p_out++ = vP2_prev;
                *p_out++ = vP1_prev;
                *p_out++ = tri[loose_vert];
            }

            if ((SenseOfTriangle(tri[loose_vert], vP0, vP1) != sense_of_source_poly) &&
                (SenseOfTriangle(tri[loose_vert], vP0, vP2) != sense_of_source_poly))
            {
                *p_out++ = vP0;
                *p_out++ = vP2;
                *p_out++ = tri[loose_vert];
            }
        }
    }

    return (u32)((p_out - out) / 3);
}

void Mesh::PunchCubeHole(Math::Matrix4& mat, Math::Matrix4& inv_mat, f32 vert_merge_threshold)
{
    AlignedBox unit_box;
    unit_box.minimum.Set(-1.0f, -1.0f, -1.0f);
    unit_box.maximum.Set(1.0f, 1.0f, 1.0f);
    Math::Frustum frustom(unit_box);
    Math::V_Vector3 new_tri_verts;
    //swap verts in the tris which have just the input vert and not the closest vert and delete the ones which have both
    std::vector< u32 > tris_to_be_deleted;//ones with both vert_id and closest_vert_id
    {
        u32 tri_id = 0;
        std::vector< u32 >::const_iterator iter =  m_TriangleVertexIndices.begin();
        for (; iter!= m_TriangleVertexIndices.end(); ++tri_id)
        {
            u32 vert_ids[3];
            vert_ids[0] = *iter;
            ++iter;
            vert_ids[1] = *iter;
            ++iter;
            vert_ids[2] = *iter;
            ++iter;
            Math::Vector3 v[3];
            v[0] = m_Positions[vert_ids[0]];
            v[1] = m_Positions[vert_ids[1]]; 
            v[2] = m_Positions[vert_ids[2]];
            inv_mat.TransformVertex(v[0]);
            inv_mat.TransformVertex(v[1]);
            inv_mat.TransformVertex(v[2]);
            Math::Vector3 tri_normal;
            Math::Vector3 tri_edge_0 = v[1] - v[0];
            Math::Vector3 tri_edge_1 = v[2] - v[0];
            tri_normal = tri_edge_0.Cross(tri_edge_1);
            tri_normal.Normalize();

            if (tri_normal.y < 0.0f)
            {
                Math::Vector3 temp = v[2];
                v[2] = v[1];
                v[1] = temp;
                tri_normal *= -1.0f;
            }
            f32 plane_w = tri_normal.Dot(v[0]);
            //v[0].y = v[1].y = v[2].y = 0.0f;


            if (frustom.IntersectsTriangle(v[0], v[1], v[2]))//if the box intersects the tri then send it for clipping in the XZ plane
            {
                v[0].y = v[1].y = v[2].y = 0.0f;
                tris_to_be_deleted.push_back(tri_id);
                Math::Vector3 out_put_verts[21];
                Math::Vector3 convex_poly[4];
                convex_poly[0].x = 1.0f;
                convex_poly[0].z = 1.0f;
                convex_poly[0].y = 0.0f;
                convex_poly[1].x = 1.0f;
                convex_poly[1].z = -1.0f;
                convex_poly[1].y = 0.0f;
                convex_poly[2].x = -1.0f;
                convex_poly[2].z = -1.0f;
                convex_poly[2].y = 0.0f;
                convex_poly[3].x = -1.0f;
                convex_poly[3].z = 1.0f;
                convex_poly[3].y = 0.0f;
                u32 num_clipped_tris = TriMinusPoly(out_put_verts, v, convex_poly, 4);
                for (u32 n_v=0; n_v<3*num_clipped_tris; ++n_v)
                {
                    Math::Vector3& new_v = out_put_verts[n_v];
                    new_v.y = (plane_w - new_v.Dot(tri_normal))/tri_normal.y;
                    mat.TransformVertex(new_v);
                    new_tri_verts.push_back(new_v);
                }
            }
        }
    }

    if (tris_to_be_deleted.size())
    {
        u32 num_current_verts = (u32)m_Positions.size();
        m_Positions.insert(m_Positions.end(), new_tri_verts.begin(), new_tri_verts.end());

        for (u32 n_t_id=0; n_t_id<(u32)new_tri_verts.size(); n_t_id+=3)
        {
            u32 start = num_current_verts+n_t_id;
            AddTri(start, start+1, start+2);
        }
        DeleteTris(tris_to_be_deleted);
        WeldMeshVerts(0.001f);
    }
}
