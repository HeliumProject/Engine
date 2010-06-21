#pragma once

#include "Insert.h"
#include "Platform/Assert.h"

#include <map>

namespace Nocturnal
{
    /////////////////////////////////////////////////////////////////////////////
    // This class is only efficient with register-sized types
    // 
    template< typename A, typename B >
    class ReversibleMap
    {
    public:
        typedef std::map< A, const B* > M_AToB;
        typedef std::map< B, const A* > M_BToA;

    private:
        M_AToB m_AToB;
        M_BToA m_BToA;

    public:
        ReversibleMap()
        {
        }

        virtual ~ReversibleMap()
        {
        }

        const M_AToB& AToB() const
        {
            return m_AToB;
        }

        const M_BToA& BToA() const
        {
            return m_BToA;
        }

        bool Insert( const A& a, const B& b )
        {
            bool result = false;

            Nocturnal::Insert<M_AToB>::Result insertedAtoB = m_AToB.insert( M_AToB::value_type( a, NULL ) );
            if ( insertedAtoB.second )
            {
                Nocturnal::Insert<M_BToA>::Result insertedBtoA = m_BToA.insert( M_BToA::value_type ( b, &( insertedAtoB.first->first ) ) );
                if ( insertedBtoA.second )
                {
                    insertedAtoB.first->second = &(insertedBtoA.first->first);
                    result = true;
                }
                else
                {
                    m_AToB.erase( insertedAtoB.first );
                }
            }

            return result;
        }

        bool RemoveA( const A& a )
        {
            bool wasRemoved = false;
            M_AToB::iterator foundForward = m_AToB.find( a );
            if ( foundForward != m_AToB.end() )
            {
                m_BToA.erase( *foundForward->second );
                m_AToB.erase( foundForward );
                wasRemoved = true;
            }
            return wasRemoved;
        }

        bool RemoveB( const B& b )
        {
            bool wasRemoved = false;
            M_BToA::iterator foundReverse = m_BToA.find( b );
            if ( foundReverse != m_BToA.end() )
            {
                m_AToB.erase( *foundReverse->second );
                m_BToA.erase( foundReverse );
                wasRemoved = true;
            }
            return wasRemoved;
        }

        void Clear()
        {
            m_AToB.clear();
            m_BToA.clear();
        }

        bool Empty() const
        {
            return m_AToB.empty() && m_BToA.empty();
        }
    };
}
