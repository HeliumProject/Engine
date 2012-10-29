#pragma once

#include "API.h"
#include "Platform/Types.h"

namespace Helium
{
    namespace Reflect
    {
        class HELIUM_REFLECT_API TypeID
        {
        public:
            TypeID()
                : m_Type ( NULL )
            {

            }

            TypeID( const Type* type )
                : m_Type ( type )
            {

            }

            const Type* GetType() const
            {
                return m_Type;
            }

            bool operator==( const TypeID& rhs ) const
            {
                return m_Type == rhs.m_Type;
            }

            bool operator!=( const TypeID& rhs ) const
            {
                return m_Type != rhs.m_Type;
            }

            bool operator<( const TypeID& rhs ) const
            {
                return m_Type < rhs.m_Type;
            }

            operator const Type*() const
            {
                return m_Type;
            }

        private:
            const Type* m_Type;
        };
    }
}