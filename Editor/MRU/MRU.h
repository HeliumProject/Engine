#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"

#include "Foundation/Container/OrderedSet.h"
#include "Foundation/Reflect/Element.h"

#include "Editor/API.h"

namespace Helium
{
    namespace Editor
    {
        static const int DefaultMaxCount = 20;

        typedef Helium::OrderedSet< tstring > OS_string;

        /////////////////////////////////////////////////////////////////////////////
        // Ordered set of strings used by MRU to store resently searched strings
        //
        template<class T>
        class MRU : public Helium::RefCountBase
        {
        public:
            typedef Helium::OrderedSet< T > OS_OrderedTypeSet;

        protected:
            int                 m_MaxCount;
            OS_OrderedTypeSet   m_OrderedSet;

        public:
            MRU( int maxCount = DefaultMaxCount )
                : m_MaxCount( maxCount )
            {}

            virtual ~MRU()
            {
            }

            virtual bool Insert( const T& value )
            {
                bool result = false;

                // remove the value if it already exists
                if ( m_OrderedSet.Contains( value ) )
                {
                    m_OrderedSet.Remove( value );
                }

                // append the value at the end
                result = m_OrderedSet.Append( value );

                // Only keep m_MaxCount stored
                if ( ( int ) m_OrderedSet.Size() > m_MaxCount )
                {
                    m_OrderedSet.Remove( m_OrderedSet.Front() );
                }

                return result;
            }

            virtual void Remove( const T& value )
            {
                m_OrderedSet.Remove( value );
            }

            virtual void Clear() 
            { 
                m_OrderedSet.Clear(); 
            }

            virtual void FromVector( std::vector< T > vec )
            {
                m_OrderedSet.Clear();
                if ( m_MaxCount > 0 && vec.size() > ( size_t )( m_MaxCount ) )
                {
                    vec.resize( m_MaxCount );
                }
                m_OrderedSet.FromVector( vec );
            }

            virtual void ToVector( std::vector< T >& vec )
            {
                vec.clear();
                m_OrderedSet.ToVector( vec );
            }

            const OS_OrderedTypeSet& GetItems() const 
            { 
                return m_OrderedSet; 
            }
        };

        // A set of Ordered strings
        typedef MRU< tstring > ManagedStringSet;
        typedef Helium::SmartPtr< ManagedStringSet > ManagedStringSetPtr;
    }
}