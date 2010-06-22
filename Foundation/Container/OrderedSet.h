#pragma once

#include "Insert.h" 
#include "Platform/Assert.h"

#include <set>
#include <vector>

namespace Nocturnal
{
    //
    // Ordered set of data
    //

    template <class T>
    class OrderedSet
    {
    private:
        //
        // The elemental data of an ordered set
        //

        struct Element
        {
            Element* m_Next;
            Element* m_Prev;
            T m_Value;

            Element(const T& value)
                : m_Next (NULL)
                , m_Prev (NULL)
                , m_Value (value)
            {

            }

            bool operator<(const Element& rhs) const
            {
                return m_Value < rhs.m_Value;
            }

            bool operator==(const Element& rhs) const
            {
                return m_Value == rhs.m_Value;
            }
        };

        typedef std::set<Element> S_Element; 

        // the set of element data
        S_Element m_Elements;

        // the front of the list
        Element* m_Front;

        // the back of the list
        Element* m_Back;

    public:
        OrderedSet()
            : m_Front (NULL)
            , m_Back (NULL)
        {

        }

        OrderedSet(const OrderedSet& rhs)
            : m_Front (NULL)
            , m_Back (NULL)
        {
            operator=(rhs);
        }

        OrderedSet& operator=(const OrderedSet& rhs)
        {
            Clear();

            Iterator itr = rhs.Begin();
            Iterator end = rhs.End();
            for ( ; itr != end; ++itr )
            {
                Append( *itr );
            }

            return *this;
        }

        bool operator==(const OrderedSet& rhs) const
        {
            // Early out if the sizes don't match
            if ( this->Size() != rhs.Size() )
            {
                return false;
            }

            bool isEqual = true;

            Iterator rhsItr = rhs.Begin();
            Iterator rhsEnd = rhs.End();

            Iterator lhsItr = Begin();
            Iterator lhsEnd = End();

            // Each member in location i in one set is equal to the the member in location i in the other set.
            for ( ; rhsItr != rhsEnd; ++rhsItr, ++lhsItr )
            {
                if ( (*rhsItr) != (*lhsItr) )
                {
                    isEqual = false;
                    break;
                }
            }

            return isEqual;
        }

        // Inserts an item at the end of the list
        bool Append(const T& value)
        {
            Nocturnal::Insert<S_Element>::Result inserted = m_Elements.insert( Element (value) );

            // if we succeeded
            if (inserted.second)
            {
                Element* element = const_cast<Element*>( &(*inserted.first) ); 

                // if there is at least one element
                if (m_Back)
                {
                    // append this to the end of it
                    m_Back->m_Next = element;

                    // save a pointer back to our prev
                    element->m_Prev = m_Back;
                }
                else
                {
                    // else make this the front
                    m_Front = element;
                }

                // our inserted one is now the back
                m_Back = element;

                return true;
            }
            else
            {
                return false;
            }
        }

        // Inserts an item at the front of the list
        bool Prepend(const T& value)
        {
            Nocturnal::Insert<S_Element>::Result inserted = m_Elements.insert( Element (value) );

            // if we succeeded
            if (inserted.second)
            {
                Element* element = const_cast<Element*>( &(*inserted.first) ); 

                // if there is at least one element
                if (m_Front)
                {
                    // Put the new element at the front of the list
                    m_Front->m_Prev = element;

                    element->m_Next = m_Front;
                }
                else
                {
                    // else make this the back
                    m_Back = element;
                }

                // our inserted one is now the front
                m_Front = element;

                return true;
            }
            else
            {
                return false;
            }
        }

        // Inserts 'value' into the set before 'before'.
        bool Insert( const T& value, const T& before )
        {
            Nocturnal::Insert<S_Element>::Result inserted = m_Elements.insert( Element (value) );

            // if we succeeded
            if (inserted.second)
            {
                Element* element = const_cast<Element*>( &(*inserted.first) ); 

                // Find the element that we want to insert before.
                Element* current = m_Front;
                while ( current && current->m_Value != before )
                {
                    current = current->m_Next;
                }

                if ( !current )
                {
                    // The element that you want to insert before is not in the list. 
                    // This is probably a programming error.
                    NOC_BREAK();
                    m_Elements.erase( inserted.first );
                    return false;
                }

                // Do the insertion
                Element* previous = current->m_Prev;
                if ( previous )
                {
                    previous->m_Next = element;
                    element->m_Prev = previous;
                }
                else
                {
                    // Inserting at front of list, update front pointer
                    m_Front = element;
                }

                element->m_Next = current;
                current->m_Prev = element;
                return true;
            }

            // Item was already in the set
            return false;
        }

        bool Remove(const T& value)
        {
            S_Element::iterator found = m_Elements.find( Element (value) );

            if (found != m_Elements.end())
            {
                Element* element = &(*found);

                // remove element from prev's next
                if (element->m_Prev)
                {
                    element->m_Prev->m_Next = element->m_Next;
                }

                // remove element from next's prev
                if (element->m_Next)
                {
                    element->m_Next->m_Prev = element->m_Prev;
                }

                // free front pointer
                if (element == m_Front)
                {
                    m_Front = element->m_Next;
                }

                // free back pointer
                if (element == m_Back)
                {
                    m_Back = element->m_Prev;
                }  

                // there should be no pointers to this anymore, so free it
                m_Elements.erase(found);

                return true;
            }
            else
            {
                return false;
            }
        }

        bool Contains(const T& value) const
        {
            S_Element::const_iterator found = m_Elements.find( Element(value) );

            if (found != m_Elements.end())
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        void ToVector( std::vector< T >& vec ) const
        {
            //vec.clear()????
            vec.reserve( Size() );
            Iterator itr = Begin();
            Iterator end = End();
            for ( ; itr != end; ++itr )
            {
                vec.push_back( *itr );
            }
        }

        // 
        // Returns false if not all elements could be copied from the
        // vector (probably because of duplicates).
        // 
        bool FromVector( const std::vector< T >& vec )
        {
            //Clear()???
            bool ok = true;
            std::vector< T >::const_iterator itr = vec.begin();
            std::vector< T >::const_iterator end = vec.end();
            for ( ; itr != end; ++itr )
            {
                ok &= Append( *itr );
            }
            return ok;
        }

        //
        // The iterator template for an ordered set
        //  F is the direction (forward is true)
        //

        template<bool F>
        class IteratorTemplate
        {
        public:
            Element* m_Position;

            IteratorTemplate()
                : m_Position (NULL)
            {
                // end
            }

            IteratorTemplate(Element* pos)
                : m_Position (pos)
            {
                // init
            }

            IteratorTemplate(const IteratorTemplate<F>& rhs)
                : m_Position (rhs.m_Position)
            {
                // copy constructor
            }

            T& operator*() const  
            {
                return m_Position->m_Value;
            }

            void operator++()
            {
                m_Position = F ? m_Position->m_Next : m_Position->m_Prev;
            }

            void operator++(int)
            {
                m_Position = F ? m_Position->m_Next : m_Position->m_Prev;
            }

            bool operator==(const IteratorTemplate& rhs)
            {
                return m_Position == rhs.m_Position;
            }

            bool operator!=(const IteratorTemplate& rhs)
            {
                return m_Position != rhs.m_Position;
            }
        };

        typedef IteratorTemplate<true> Iterator;
        Iterator Begin() const
        {
            return Iterator(m_Front);
        }
        Iterator End() const
        {
            return Iterator(NULL);
        }

        typedef IteratorTemplate<false> ReverseIterator;
        ReverseIterator ReverseBegin() const
        {
            return ReverseIterator(m_Back);
        }
        ReverseIterator ReverseEnd() const
        {
            return ReverseIterator(NULL);
        }

        size_t Size() const
        {
            return m_Elements.size();
        }

        bool Empty() const
        {
            return m_Elements.empty();
        }

        void Clear()
        {
            m_Elements.clear();
            m_Front = NULL;
            m_Back = NULL;
        }

        T& Front()
        {
            NOC_ASSERT( m_Front );
            return m_Front->m_Value;
        }

        const T& Front() const
        {
            NOC_ASSERT( m_Front );
            return m_Front->m_Value;
        }

        T& Back()
        {
            NOC_ASSERT( m_Back );
            return m_Back->m_Value;
        }

        const T& Back() const
        {
            NOC_ASSERT( m_Back );
            return m_Back->m_Value;
        }

        void PopFront()
        {
            NOC_ASSERT( m_Front );
            Remove( m_Front->m_Value ); // its ok to remove by value here, since remove by Element* would have to traverse the map anyway
        }

        void PopBack()
        {
            NOC_ASSERT( m_Back );
            Remove( m_Back->m_Value ); // its ok to remove by value here, since remove by Element* would have to traverse the map anyway
        }

        Iterator Find( const T& value ) const
        {
            S_Element::const_iterator found = m_Elements.find( Element(value) );
            if (found != m_Elements.end())
            {
                return Iterator( *found );
            }
            return End();
        }

        Iterator FindNextSibling( const T& value ) const
        {
            S_Element::const_iterator found = m_Elements.find( Element(value) );
            NOC_ASSERT( found != m_Elements.end() );
            return Iterator( ( *found ).m_Next );
        }

        Iterator FindPrevSibling( const T& value ) const
        {
            S_Element::const_iterator found = m_Elements.find( Element(value) );
            NOC_ASSERT( found != m_Elements.end() );
            return Iterator( ( *found ).m_Prev );
        }
    };
}
