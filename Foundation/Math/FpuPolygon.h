#pragma once

#include "Platform/Types.h"

#include "Foundation/API.h"
#include "Foundation/Math/Common.h"

namespace Helium
{
    template <class V, int32_t S>
    class StaticPolygon
    {
    public:
        V m_Vertices[S];
        int32_t m_Size;

        StaticPolygon()
            : m_Size (0)
        {

        }

        inline StaticPolygon(const V& v0, const V& v1, const V& v2)
            : m_Size (3)
        {
            m_Vertices[0] = v0;
            m_Vertices[1] = v1;
            m_Vertices[2] = v2;
        }

        inline void Append(const V& v)
        {
            if (m_Size < sizeof(m_Vertices)/sizeof(V))
            {
                // append
                m_Vertices[m_Size++] = v;
            }
            else
            {
                HELIUM_BREAK();
            }
        }

        inline void Insert(const V& v, int32_t index)
        {
            // bounds check
            if (index <= m_Size++)
            {
                // make a copy to propagate
                V temp = m_Vertices[index];

                // insert new value
                m_Vertices[index] = v;

                // for each additional one
                for ( int32_t i=index+1; i<m_Size; i++ )
                {
                    // propagate previous
                    m_Vertices[i] = temp;

                    // copy for next propagation
                    temp = m_Vertices[i+1];
                }
            }
            else
            {
                HELIUM_BREAK();
            }
        }

        inline void Remove(int32_t index)
        {
            // bounds check
            if (index < m_Size--)
            {
                // if we are not the last one
                if ( index != m_Size )
                {
                    // for each subsequent one
                    for ( int32_t i=index; i<m_Size; i++ )
                    {
                        // propagate subsequent
                        m_Vertices[i] = m_Vertices[i+1];
                    }
                }
            }
            else
            {
                HELIUM_BREAK();
            }
        }

        //http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/InsidePolygonWithBounds.cpp
        // returns false if the point is one of the poly's verts or on the bounds
        // only works in 2D
        inline bool PointInPolygon( V& point, bool returnValIfOnBounds = false, float32_t epsilon = 0.0f )
        {

            //cross points count of x
            int __count = 0;

            //neighbour bound vertices
            V p1, p2;

            //left vertex
            p1 = m_Vertices[0];

            //check all rays
            for(int i = 1; i <= m_Size; ++i)
            {
                //point is an vertex
                if( point.Equal(p1, epsilon) ) 
                    return returnValIfOnBounds;

                //right vertex
                p2 = m_Vertices[i % m_Size];

                //ray is outside of our interests
                if(point.y < std::min(p1.y, p2.y) || point.y > std::max(p1.y, p2.y))
                {
                    //next ray left point
                    p1 = p2; continue;
                }

                //ray is crossing over by the algorithm (common part of)
                if(point.y > std::min(p1.y, p2.y) && point.y < std::max(p1.y, p2.y))
                {
                    //x is before of ray
                    if(point.x <= std::max(p1.x, p2.x))
                    {
                        //overlies on a horizontal ray
                        if(p1.y == p2.y && point.x >= std::min(p1.x, p2.x)) 
                            return returnValIfOnBounds;

                        //ray is vertical
                        if(p1.x == p2.x)
                        {
                            //overlies on a ray
                            if(p1.x == point.x) 
                                return returnValIfOnBounds;
                            //before ray
                            else ++__count;
                        }

                        //cross point on the left side
                        else
                        {
                            //cross point of x
                            double xinters = (point.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;

                            //overlies on a ray
                            if(fabs(point.x - xinters) < epsilon ) 
                                return returnValIfOnBounds;

                            //before ray
                            if(point.x < xinters) ++__count;
                        }
                    }
                }
                //special case when ray is crossing through the vertex
                else
                {
                    //point crossing over p2
                    if(point.y == p2.y && point.x <= p2.x)
                    {
                        //next vertex
                        const V& p3 = m_Vertices[(i+1) % m_Size];

                        //point.y lies between p1.y & p3.y
                        if(point.y >= std::min(p1.y, p3.y) && point.y <= std::max(p1.y, p3.y))
                        {
                            ++__count;
                        }
                        else
                        {
                            __count += 2;
                        }
                    }
                }

                //next ray left point
                p1 = p2;
            }

            //EVEN
            if(__count % 2 == 0) 
                return false;
            //ODD
            else 
                return true;

        }


    };

    template <class V>
    class HELIUM_FOUNDATION_API DynamicPolygon
    {
    public:
        std::vector<V> m_Vertices;

        DynamicPolygon()
        {

        }

        DynamicPolygon(const V& v0, const V& v1, const V& v2)
        {
            m_Vertices.resize(3);
            m_Vertices[0] = v0;
            m_Vertices[1] = v1;
            m_Vertices[2] = v2;
        }

        void Append(const V& v)
        {
            // append
            m_Vertices.push_back(v);
        }

        void Insert(const V& v, int32_t index)
        {
            // insert
            m_Vertices.insert(index, v);
        }

        void Remove(int32_t index)
        {
            // erase
            m_Vertices.erase(index);
        }
    };
}