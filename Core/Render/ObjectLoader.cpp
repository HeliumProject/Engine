/*#include "Precompile.h"*/
#include "ObjectLoader.h"

#include "Foundation/Math/Utils.h"

#include <map>
#include <set>

using namespace Helium;
using namespace Helium::Render;

//  Index gathering and ordering structure
namespace Helium
{
    namespace Render
    {
        struct IdxSet 
        {
            u32 pIndex;
            u32 nIndex;
            u32 tIndex;
            u32 tanIndex;
            u32 cIndex;

            bool operator< ( const IdxSet &rhs) const 
            {
                if (pIndex < rhs.pIndex)
                    return true;
                else if (pIndex == rhs.pIndex) 
                {
                    if (nIndex < rhs.nIndex)
                        return true;
                    else if (nIndex == rhs.nIndex) 
                    {            
                        if ( tIndex < rhs.tIndex)
                            return true;
                        else if ( tIndex == rhs.tIndex) 
                        {
                            if (tanIndex < rhs.tanIndex)
                                return true;
                            else if (tanIndex == rhs.tanIndex)
                                return (cIndex < rhs.cIndex);
                        }
                    }
                }

                return false;
            }
        };
    }
}

ObjectLoader::ObjectLoader()
: m_posSize( 3 )
, m_tcSize( 2 )
{
}

ObjectLoader::~ObjectLoader()
{
}

void ObjectLoader::Compile(bool flip)
{
    // make sure the mesh is fully specified
    InsertColors();
    ComputeNormals();
    ComputeTangents();
    InsertTexCoords();

    //SetNoTangents();

    //merge the points
    std::map<IdxSet, u32> pts;

    //find whether a position is unique
    std::set<u32> ptSet;

    for (u32 f=0;f<(u32)m_fragments.size();f++)
    {
        std::vector<u32>::iterator pit = m_fragments[f].m_pIndex.begin();
        std::vector<u32>::iterator nit = m_fragments[f].m_nIndex.begin();
        std::vector<u32>::iterator tit = m_fragments[f].m_tIndex.begin();
        std::vector<u32>::iterator tanit = m_fragments[f].m_tanIndex.begin();
        std::vector<u32>::iterator cit = m_fragments[f].m_cIndex.begin();

        while ( pit < m_fragments[f].m_pIndex.end()) 
        {
            IdxSet idx;
            idx.pIndex = *pit;
            idx.nIndex = *nit;
            idx.tIndex = *tit;
            idx.tanIndex = *tanit;
            idx.cIndex = *cit;

            //      Log::Print( TXT( "p idx = %d, n idx = %d, t idx = %d, tan idx = %d, c idx = %d\n" ), idx.pIndex, idx.nIndex, idx.tIndex, idx.tanIndex, idx.cIndex);

            std::map<IdxSet,u32>::iterator mit = pts.find(idx);

            if (mit == pts.end()) 
            {
                //        Log::Print( TXT( "New vertex\n" ) );
                m_fragments[f].m_indices.push_back( (u32)pts.size());
                pts.insert( std::map<IdxSet,u32>::value_type(idx, (u32)pts.size()));

                //position
                m_vertices.push_back( m_positions[idx.pIndex*m_posSize]);
                m_vertices.push_back( m_positions[idx.pIndex*m_posSize + 1]);
                m_vertices.push_back( m_positions[idx.pIndex*m_posSize + 2]);

                m_vertices.push_back( m_normals[idx.nIndex*3]);
                m_vertices.push_back( m_normals[idx.nIndex*3 + 1]);
                m_vertices.push_back( m_normals[idx.nIndex*3 + 2]);

                m_vertices.push_back( m_sTangents[idx.tanIndex*4]);
                m_vertices.push_back( m_sTangents[idx.tanIndex*4 + 1]);
                m_vertices.push_back( m_sTangents[idx.tanIndex*4 + 2]);
                m_vertices.push_back( m_sTangents[idx.tanIndex*4 + 3]);

                m_vertices.push_back( m_texcoords[idx.tIndex*m_tcSize]);
                m_vertices.push_back( m_texcoords[idx.tIndex*m_tcSize + 1]);

                m_vertices.push_back( m_colors[idx.cIndex*4]);
                m_vertices.push_back( m_colors[idx.cIndex*4 + 1]);
                m_vertices.push_back( m_colors[idx.cIndex*4 + 2]);
                m_vertices.push_back( m_colors[idx.cIndex*4 + 3]);
            }
            else 
            {
                //Log::Print( TXT( "Found idx = %d\n" ),mit->second);
                m_fragments[f].m_indices.push_back( mit->second);
            }

            //advance the iterators if the components are present (All components are always present)
            pit++;
            nit++;
            tit++;
            tanit++;
            cit++;
        }
    }
    //create selected prim

    //set the offsets and vertex size
    m_pOffset = 0; //always first
    m_vtxSize = m_posSize;

    m_nOffset = m_vtxSize;
    m_vtxSize += 3;

    m_sTanOffset = m_vtxSize;
    m_vtxSize += 4;

    m_tcOffset = m_vtxSize;
    m_vtxSize += m_tcSize;

    m_cOffset = m_vtxSize;
    m_vtxSize += 4;
}


//////////////////////////////////////////////////////////////////////
// Insert dummy white colors in the mesh if it wasn't loaded with any
void ObjectLoader::InsertColors()
{
    // we already have colors
    if (m_colors.size()>0)
        return;

    // insert a default color
    m_colors.push_back(1.0f);
    m_colors.push_back(1.0f);
    m_colors.push_back(1.0f);
    m_colors.push_back(1.0f);     // flip factor

    for (u32 f=0;f<(u32)m_fragments.size();f++)
    {
        //for every position index we have a color index of zero
        for (int ii = 0; ii < (int)m_fragments[f].m_pIndex.size(); ii++) 
        {
            m_fragments[f].m_cIndex.push_back(0);
        }
    }
}

//////////////////////////////////////////////////////////////////////
// Insert spherically mapped UVs
void ObjectLoader::InsertTexCoords()
{
    // we already have colors
    if (m_texcoords.size()>0)
        return;

    m_tcSize = 2;

    // for each normal generate a texture coordinate
    for (u32 n=0;n<m_normals.size();n+=3)
    {
        float x = m_normals[n];
        float y = m_normals[n+1];
        //float z = m_normals[n+2];

        m_texcoords.push_back( (x+1.0f)/2 );
        m_texcoords.push_back( (y+1.0f)/2 );
    }

    // for each fragment duplicate the normal index array into the texture coordinate index
    for (u32 f=0;f<(u32)m_fragments.size();f++)
    {
        m_fragments[f].m_tIndex.clear();
        m_fragments[f].m_tIndex = m_fragments[f].m_nIndex;
    }
}


//////////////////////////////////////////////////////////////////////
void ObjectLoader::SetNoTangents()
{
    // delete the existing tangents

    // add default tangents
    m_sTangents.push_back(1.0f);
    m_sTangents.push_back(0.0f);
    m_sTangents.push_back(0.0f);
    m_sTangents.push_back(1.0f);

    for (u32 f=0;f<(u32)m_fragments.size();f++)
    {
        //for every position index we have a tangent index of zero
        for (int ii = 0; ii < (int)m_fragments[f].m_pIndex.size(); ii++) 
        {
            m_fragments[f].m_tanIndex.push_back(0);
        }
    }
}

// compute tangents in the U direction (binormal is V direction)
//////////////////////////////////////////////////////////////////////
void ObjectLoader::ComputeTangents() 
{
    //make sure tangents don't already exist
    if ( m_sTangents.size()>0) 
        return;

    //make sure that the model has texcoords
    if ( m_texcoords.size()==0)
    {
        SetNoTangents();
        return;
    }

    m_sTangents.resize( (m_texcoords.size() / m_tcSize) * 4, 0.0f);
    for (u32 i=0;i<(m_texcoords.size() / m_tcSize) * 4;i++)
    {
        m_sTangents[i]=0.0f;
    }

    for (u32 f=0;f<(u32)m_fragments.size();f++)
    {
        //alloc memory and initialize to 0
        m_fragments[f].m_tanIndex.reserve( m_fragments[f].m_pIndex.size());

        // the collision map records any alternate locations for the tangents
        std::multimap< u32, u32> collisionMap;

        //process each face, compute the tangent and try to add it
        for (int ii = 0; ii < (int)m_fragments[f].m_pIndex.size(); ii += 3) 
        {
            D3DXVECTOR3 p0(&m_positions[m_fragments[f].m_pIndex[ii]*m_posSize]);
            D3DXVECTOR3 p1(&m_positions[m_fragments[f].m_pIndex[ii+1]*m_posSize]);
            D3DXVECTOR3 p2(&m_positions[m_fragments[f].m_pIndex[ii+2]*m_posSize]);
            D3DXVECTOR2 st0(&m_texcoords[m_fragments[f].m_tIndex[ii]*m_tcSize]);
            D3DXVECTOR2 st1(&m_texcoords[m_fragments[f].m_tIndex[ii+1]*m_tcSize]);
            D3DXVECTOR2 st2(&m_texcoords[m_fragments[f].m_tIndex[ii+2]*m_tcSize]);

            //compute the edge and tc differentials
            D3DXVECTOR3 dp0 = p1 - p0;
            D3DXVECTOR3 dp1 = p2 - p0;
            D3DXVECTOR2 dst0 = st1 - st0;
            D3DXVECTOR2 dst1 = st2 - st0;

            float factor = 1.0f / (dst0[0] * dst1[1] - dst1[0] * dst0[1]);

            if (factor<0.0f)
                factor=-1.0f;
            else
                factor=1.0f;

            //compute sTangent
            D3DXVECTOR3 sTan;
            sTan[0] = dp0[0] * dst1[1] - dp1[0] * dst0[1];
            sTan[1] = dp0[1] * dst1[1] - dp1[1] * dst0[1];
            sTan[2] = dp0[2] * dst1[1] - dp1[2] * dst0[1];

            sTan *= factor;

            //should this really renormalize?
            D3DXVec3Normalize(&sTan,&sTan);

            //Log::Print( TXT( "Face:%d (pos %d, %d, %d) (uv %d, %d, %d)\n" ), ii/3, m_fragments[f].m_pIndex[ii],m_fragments[f].m_pIndex[ii+1], m_fragments[f].m_pIndex[ii+2], m_fragments[f].m_tIndex[ii],m_fragments[f].m_tIndex[ii+1], m_fragments[f].m_tIndex[ii+2]);
            //Log::Print( TXT( "Face:%d  Tangent: %.4f, %.4f, %.4f  Flip: %.4f\n" ),ii/3,sTan[0],sTan[1],sTan[2], factor);

            //loop over the vertices, to update the tangents
            for (int jj = 0; jj < 3; jj++) 
            {
                u32 tan_idx = m_fragments[f].m_tIndex[ii + jj];

                //get the present accumulated tangnet
                D3DXVECTOR4 curTan(&m_sTangents[tan_idx*4]);

                //Log::Print( TXT( "  jj:%d  curTan: %.4f, %.4f, %.4f  Flip: %.4f\n" ),jj,curTan[0],curTan[1],curTan[2], curTan[3]);

                //check to see if it is uninitialized, if so, insert it
                if (curTan[0] == 0.0f && curTan[1] == 0.0f && curTan[2] == 0.0f) 
                {
                    m_sTangents[tan_idx*4] = sTan[0];
                    m_sTangents[tan_idx*4+1] = sTan[1];
                    m_sTangents[tan_idx*4+2] = sTan[2];
                    m_sTangents[tan_idx*4+3] = factor;

                    m_fragments[f].m_tanIndex.push_back(tan_idx);
                    //Log::Print( TXT( "Adding tangent at index %d\n" ),tan_idx);
                }
                else 
                {
                    //current tangent for this vertex is already initialized, can we combine the new tangent or not
                    D3DXVec3Normalize( (D3DXVECTOR3*)&curTan,(D3DXVECTOR3*)&curTan);

                    // if we are within range and have a matching flip factor
                    bool flip_equal = (curTan[3]==factor);
                    float dot = D3DXVec3Dot( (D3DXVECTOR3*)&curTan, &sTan);
                    //Log::Print( TXT( "Flip Equal = %d, dot3(face tan, cur tan) = %.3f\n" ),(u32)flip_equal,dot);

                    if ( flip_equal && dot >= cosf( 3.1415926f * 0.333333f))
                    {
                        //tangents are in agreement
                        m_sTangents[tan_idx*4]   += sTan[0];
                        m_sTangents[tan_idx*4+1] += sTan[1];
                        m_sTangents[tan_idx*4+2] += sTan[2];
                        // don't have to add the flip factors because they must already be the same

                        m_fragments[f].m_tanIndex.push_back(tan_idx);
                        //Log::Print( TXT( "Tangent in range, merging with index %d\n" ),tan_idx);
                    }
                    else 
                    {
                        //tangents disagree, this vertex must be split in tangent space 
                        std::multimap< u32, u32>::iterator it = collisionMap.find(tan_idx);

                        //loop through all the previous hits on this index, until one agrees
                        while ( it != collisionMap.end() && it->first == tan_idx) 
                        {
                            curTan = D3DXVECTOR4( &m_sTangents[it->second*4]);

                            D3DXVec3Normalize( (D3DXVECTOR3*)&curTan,(D3DXVECTOR3*)&curTan);
                            if ( flip_equal && D3DXVec3Dot( (D3DXVECTOR3*)&curTan, &sTan) >= cosf( 3.1415926f * 0.333333f))
                                break;
                            it++;
                        }

                        //check for agreement with an earlier collision
                        if ( it != collisionMap.end() && it->first == tan_idx) 
                        {
                            //found agreement with an earlier collision, use that one
                            m_sTangents[it->second*4] += sTan[0];
                            m_sTangents[it->second*4+1] += sTan[1];
                            m_sTangents[it->second*4+2] += sTan[2];

                            m_fragments[f].m_tanIndex.push_back(it->second);

                            //Log::Print( TXT( "Merging with index %d\n" ),it->second);
                        }
                        else 
                        {
                            //we don't match any previous tangent used by this vertex, create a new tangent
                            u32 target = (u32)m_sTangents.size() / 4;

                            m_sTangents.push_back( sTan[0]);
                            m_sTangents.push_back( sTan[1]);
                            m_sTangents.push_back( sTan[2]);
                            m_sTangents.push_back( factor);

                            m_fragments[f].m_tanIndex.push_back( target);

                            //Log::Print( TXT( "Split vertex, adding new index %d\n" ),target);
                            collisionMap.insert( std::multimap< u32, u32>::value_type( tan_idx, target));
                        }
                    }
                }
            }
        }
    }

    //normalize all the tangents
    for (int ii = 0; ii < (int)m_sTangents.size(); ii += 4) 
    {    
        D3DXVECTOR3 tan(&m_sTangents[ii]);
        D3DXVec3Normalize( &tan,&tan);
        m_sTangents[ii] = tan[0];
        m_sTangents[ii+1] = tan[1];
        m_sTangents[ii+2] = tan[2];
    }
}

//
//compute vertex normals
//////////////////////////////////////////////////////////////////////
void ObjectLoader::ComputeNormals() 
{
    // don't recompute normals
    if (m_normals.size()>0)
        return;

    //  char buff[1024];

    //allocate and initialize the normal values
    m_normals.resize( (m_positions.size() / m_posSize) * 3, 0.0f);

    // the collision map records any alternate locations for the normals
    std::multimap< u32, u32> collisionMap;

    //iterate over the faces, computing the face normal and summing it them
    for (u32 f=0;f<(u32)m_fragments.size();f++)
    {
        m_fragments[f].m_nIndex.clear();
        m_fragments[f].m_nIndex.reserve( m_fragments[f].m_pIndex.size());

        for ( int ii = 0; ii < (int)m_fragments[f].m_pIndex.size(); ii += 3) 
        {
            u32 i1,i2,i3;
            i1 = m_fragments[f].m_pIndex[ii];
            i2 = m_fragments[f].m_pIndex[ii+1];
            i3 = m_fragments[f].m_pIndex[ii+2];

            //sprintf(buff, "Indices %d, %d, %d\n",i1,i2,i3);
            //OutputDebugString(buff);

            D3DXVECTOR3 p0(&m_positions[i1*m_posSize]);
            D3DXVECTOR3 p1(&m_positions[i2*m_posSize]);
            D3DXVECTOR3 p2(&m_positions[i3*m_posSize]);

            //compute the edge vectors
            D3DXVECTOR3 dp0 = p1 - p0;
            D3DXVECTOR3 dp1 = p2 - p0;

            D3DXVECTOR3 fnormal;
            D3DXVECTOR3 nnormal;
            D3DXVec3Cross(&fnormal,&dp0,&dp1);
            D3DXVec3Normalize(&nnormal,&fnormal);

            //sprintf(buff, "Normal %.3f, %.3f, %.3f\n",nnormal[0],nnormal[1],nnormal[2]);
            //OutputDebugString(buff);

            //iterate over the vertices, adding the face normal influence to each
            for ( int jj = 0; jj < 3; jj++) 
            {
                u32 idx = m_fragments[f].m_pIndex[ii + jj];

                // get the current normal from the default location (index shared with position) 
                D3DXVECTOR3 cnormal( &m_normals[idx*3]);

                // check to see if this normal has not yet been touched 
                if ( cnormal[0] == 0.0f && cnormal[1] == 0.0f && cnormal[2] == 0.0f) 
                {
                    // first instance of this index, just store it as is
                    m_normals[idx*3] = fnormal[0];
                    m_normals[idx*3 + 1] = fnormal[1];
                    m_normals[idx*3 + 2] = fnormal[2];
                    m_fragments[f].m_nIndex.push_back(idx); 

                    //sprintf(buff, "New normal idx %d\n",idx);
                    //OutputDebugString(buff);
                }
                else 
                {
                    // check for agreement
                    D3DXVec3Normalize(&cnormal,&cnormal);

                    if ( D3DXVec3Dot(&cnormal, &nnormal)  >= cosf( 3.1415926f * 0.333333f)) 
                    {
                        //normal agrees, so merge
                        m_normals[idx*3] += fnormal[0];
                        m_normals[idx*3 + 1] += fnormal[1];
                        m_normals[idx*3 + 2] += fnormal[2];
                        m_fragments[f].m_nIndex.push_back(idx);

                        //sprintf(buff, "merging with idx %d\n",idx);
                        //OutputDebugString(buff);
                    }
                    else 
                    {
                        //normals disagree, this vertex must be along a facet edge 
                        std::multimap< u32, u32>::iterator it = collisionMap.find( m_fragments[f].m_pIndex[ii + jj]);

                        //loop through all hits on this index, until one agrees
                        while ( it != collisionMap.end() && it->first == m_fragments[f].m_pIndex[ii + jj]) 
                        {
                            D3DXVec3Normalize(&cnormal,(D3DXVECTOR3*)&m_normals[it->second*3]);

                            if ( D3DXVec3Dot(&cnormal, &nnormal) >= cosf( 3.1415926f * 0.333333f))
                                break;
                            it++;
                        }

                        //check for agreement with an earlier collision
                        if ( it != collisionMap.end() && it->first == m_fragments[f].m_pIndex[ii + jj]) 
                        {
                            //found agreement with an earlier collision, use that one
                            m_normals[it->second*3] += fnormal[0];
                            m_normals[it->second*3+1] += fnormal[1];
                            m_normals[it->second*3+2] += fnormal[2];
                            m_fragments[f].m_nIndex.push_back(it->second);
                        }
                        else 
                        {
                            //we have a new collision, create a new normal
                            u32 target = (u32)m_normals.size() / 3;
                            m_normals.push_back( fnormal[0]);
                            m_normals.push_back( fnormal[1]);
                            m_normals.push_back( fnormal[2]);
                            m_fragments[f].m_nIndex.push_back( target);
                            collisionMap.insert( std::multimap< u32, u32>::value_type( m_fragments[f].m_pIndex[ii + jj], target));
                        }
                    } // else ( if normal agrees)
                } // else (if normal is uninitialized)
            } // for each vertex in triangle
        } // for each face
    } // for each fragment


    //now normalize all the normals
    for ( int ii = 0; ii < (int)m_normals.size(); ii += 3) 
    {
        D3DXVECTOR3 norm(&m_normals[ii]);
        D3DXVec3Normalize(&norm,&norm);
        m_normals[ii] = norm[0];
        m_normals[ii+1] = norm[1];
        m_normals[ii+2] = norm[2];
    }
}


//////////////////////////////////////////////////////////////////////
void ObjectLoader::ComputeBoundingBox( D3DXVECTOR3 &minVal, D3DXVECTOR3 &maxVal) 
{
    if (m_positions.empty())
        return;

    minVal = D3DXVECTOR3( 1e10f, 1e10f, 1e10f);
    maxVal = -minVal;

    for ( std::vector<float>::iterator pit = m_positions.begin(); pit < m_positions.end(); pit += m_posSize) 
    {
        D3DXVECTOR3 pos(&pit[0]);
        if (pos.x<minVal.x)
            minVal.x = pos.x;
        if (pos.x>maxVal.x)
            maxVal.x = pos.x;
        if (pos.y<minVal.y)
            minVal.y = pos.y;
        if (pos.y>maxVal.y)
            maxVal.y = pos.y;
        if (pos.z<minVal.z)
            minVal.z = pos.z;
        if (pos.z>maxVal.z)
            maxVal.z = pos.z;
    }
}

//////////////////////////////////////////////////////////////////////
void ObjectLoader::Rescale( float radius, D3DXVECTOR3& r, D3DXVECTOR3& center) 
{
    if ( m_positions.empty())
        return;

    float oldRadius = MAX(r.x, MAX(r.y, r.z));
    float scale = radius / oldRadius;

    for ( std::vector<float>::iterator pit = m_positions.begin(); pit < m_positions.end(); pit += m_posSize) 
    {
        D3DXVECTOR3 np = scale*(D3DXVECTOR3(&pit[0]) - center);
        pit[0] = np.x;
        pit[1] = np.y;
        pit[2] = np.z;
    }
}

//////////////////////////////////////////////////////////////////////
u32 ObjectLoader::GetNumFragments( int bangleIndex )
{
    if ( bangleIndex < 0 )
    {
        return (u32) m_fragments.size();
    }

    u32 numFragments = 0;
    for ( std::vector<ShaderFrag>::iterator fragItr = m_fragments.begin(), fragEnd = m_fragments.end(); fragItr != fragEnd; ++fragItr )
    {
        if ( (*fragItr).m_bangle_index == bangleIndex )
        {
            ++numFragments;
        }
    }

    return numFragments;
}
