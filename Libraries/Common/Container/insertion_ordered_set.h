#pragma once

#include "Insert.h" 
#include "../Assert.h"

#include <set>
#include <vector>

namespace Nocturnal
{
  //
  // Ordered set of data
  //

  template <class T>
  class insertion_ordered_set
  {
  public:
    template< bool F > class IteratorTemplate;
    typedef IteratorTemplate<true> iterator;
    typedef const IteratorTemplate<true> const_iterator;

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
    insertion_ordered_set()
      : m_Front (NULL)
      , m_Back (NULL)
    {

    }

    insertion_ordered_set(const insertion_ordered_set& rhs)
      : m_Front (NULL)
      , m_Back (NULL)
    {
      operator=(rhs);
    }

    insertion_ordered_set& operator=(const insertion_ordered_set& rhs)
    {
      clear();

      iterator itr = rhs.begin();
      iterator end = rhs.end();
      for ( ; itr != end; ++itr )
      {
        insert( *itr );
      }

      return *this;
    }

    bool operator==(const insertion_ordered_set& rhs) const
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
    std::pair< iterator, bool > insert(const T& value)
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
        return std::pair< iterator, bool >( iterator( m_Back ), true );
      }

      return std::pair< iterator, bool >( find( value ), false );
    }

    // Should not be needed if insert( iterator, value ) is implemented
    //// Inserts an item at the front of the list
    //bool Prepend(const T& value)
    //{
    //  Nocturnal::Insert<S_Element>::Result inserted = m_Elements.insert( Element (value) );

    //  // if we succeeded
    //  if (inserted.second)
    //  {
    //    Element* element = const_cast<Element*>( &(*inserted.first) ); 

    //    // if there is at least one element
    //    if (m_Front)
    //    {
    //      // Put the new element at the front of the list
    //      m_Front->m_Prev = element;

    //      element->m_Next = m_Front;
    //    }
    //    else
    //    {
    //      // else make this the back
    //      m_Back = element;
    //    }

    //    // our inserted one is now the front
    //    m_Front = element;

    //    return true;
    //  }
    //  else
    //  {
    //    return false;
    //  }
    //}

    // Inserts 'value' into the set before 'hint'.
    std::pair< iterator, bool > insert( const_iterator hint, const T& value )
    {
      Nocturnal::Insert<S_Element>::Result inserted = m_Elements.insert( Element (value) );

      // if we succeeded
      if (inserted.second)
      {
        Element* element = const_cast<Element*>( &(*inserted.first) ); 

        // Find the element that we want to insert before.
        Element* current = m_Front;
        while ( current && current->m_Value != ( *hint ) )
        {
          current = current->m_Next;
        }

        if ( !current )
        {
          // The element that you want to insert before is not in the list. 
          // This is probably a programming error.
          NOC_BREAK();
          m_Elements.erase( inserted.first );
          return std::pair< iterator, bool >( end(), false );
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
        return std::pair< iterator, bool >( iterator( element ), true );
      }

      // Item was already in the set
      return std::pair< iterator, bool >( &*inserted.first, false );
    }

    // should actually be size_type for return value
    size_t erase(const T& value)
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

        return 1;
      }
      else
      {
        return 0;
      }
    }

    iterator erase( const_iterator position )
    {
      iterator found = find( *position );
      if ( found != end() )
      {
        ++found;
        erase( *position );
        return found;
      }
      return end();
    }

    //bool Contains(const T& value) const
    //{
    //  S_Element::const_iterator found = m_Elements.find( Element(value) );

    //  if (found != m_Elements.end())
    //  {
    //    return true;
    //  }
    //  else
    //  {
    //    return false;
    //  }
    //}

    //void ToVector( std::vector< T >& vec ) const
    //{
    //  //vec.clear()????
    //  vec.reserve( Size() );
    //  Iterator itr = Begin();
    //  Iterator end = End();
    //  for ( ; itr != end; ++itr )
    //  {
    //    vec.push_back( *itr );
    //  }
    //}

    // 
    // Returns false if not all elements could be copied from the
    // vector (probably because of duplicates).
    // 
    //bool FromVector( const std::vector< T >& vec )
    //{
    //  //clear()???
    //  bool ok = true;
    //  std::vector< T >::const_iterator itr = vec.begin();
    //  std::vector< T >::const_iterator end = vec.end();
    //  for ( ; itr != end; ++itr )
    //  {
    //    ok &= Append( *itr );
    //  }
    //  return ok;
    //}

    //
    // The iterator template for an ordered set
    //  F is the direction (forward is true)
    //

    template<bool F>
    class IteratorTemplate
    {
    public:
      mutable Element* m_Position;

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

      void operator++() const
      {
        m_Position = F ? m_Position->m_Next : m_Position->m_Prev;
      }

      void operator++(int)
      {
        m_Position = F ? m_Position->m_Next : m_Position->m_Prev;
      }

      bool operator==(const IteratorTemplate& rhs) const
      {
        return m_Position == rhs.m_Position;
      }

      bool operator!=(const IteratorTemplate& rhs) const
      {
        return m_Position != rhs.m_Position;
      }
    };

    iterator begin() const
    {
      return m_Front;
    }
    iterator end() const
    {
      return NULL;
    }

    //typedef IteratorTemplate<false> reverse_iterator;
    //reverse_iterator rbegin() const
    //{
    //  return reverse_iterator(m_Back);
    //}
    //reverse_iterator rend() const
    //{
    //  return ReverseIterator(NULL);
    //}

    size_t size() const
    {
      return m_Elements.size();
    }

    bool empty() const
    {
      return m_Elements.empty();
    }

    void clear()
    {
      m_Elements.clear();
      m_Front = NULL;
      m_Back = NULL;
    }

    T& front()
    {
      NOC_ASSERT( m_Front );
      return m_Front->m_Value;
    }

    const T& front() const
    {
      NOC_ASSERT( m_Front );
      return m_Front->m_Value;
    }

    T& back()
    {
      NOC_ASSERT( m_Back );
      return m_Back->m_Value;
    }

    const T& back() const
    {
      NOC_ASSERT( m_Back );
      return m_Back->m_Value;
    }

    void pop_front()
    {
      NOC_ASSERT( m_Front );
      erase( m_Front->m_Value ); // its ok to remove by value here, since remove by Element* would have to traverse the map anyway
    }

    void pop_back()
    {
      NOC_ASSERT( m_Back );
      erase( m_Back->m_Value ); // its ok to remove by value here, since remove by Element* would have to traverse the map anyway
    }

    iterator find( const T& value ) const
    {
      insertion_ordered_set< T >* nonConstThis = const_cast< insertion_ordered_set< T >* >( this );
      S_Element::iterator found = nonConstThis->m_Elements.find( Element(value) );
      if (found != m_Elements.end())
      {
        return &( *found );
      }
      return end();
    }

    //Iterator FindNextSibling( const T& value ) const
    //{
    //  S_Element::const_iterator found = m_Elements.find( Element(value) );
    //  NOC_ASSERT( found != m_Elements.end() );
    //  return Iterator( ( *found ).m_Next );
    //}

    //Iterator FindPrevSibling( const T& value ) const
    //{
    //  S_Element::const_iterator found = m_Elements.find( Element(value) );
    //  NOC_ASSERT( found != m_Elements.end() );
    //  return Iterator( ( *found ).m_Prev );
    //}
  };
}
