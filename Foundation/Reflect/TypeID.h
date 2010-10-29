#pragma once

#include "API.h"
#include "Platform/Types.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API TypeID
        {
        public:
            TypeID()
                : m_ID ( -1 )
            {

            }

            TypeID( int32_t id )
                : m_ID ( id )
            {

            }

            int32_t GetID() const
            {
                return m_ID;
            }

            bool operator==( const TypeID& rhs ) const
            {
                return m_ID == rhs.m_ID;
            }

            bool operator!=( const TypeID& rhs ) const
            {
                return m_ID != rhs.m_ID;
            }

            bool operator<( const TypeID& rhs ) const
            {
                return m_ID < rhs.m_ID;
            }

            operator int32_t() const
            {
                return m_ID;
            }

        private:
            int32_t m_ID;
        };
    }
}