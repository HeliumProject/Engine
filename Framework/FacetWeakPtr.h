
#include "Facets.h"

#include "Game/Engine/PooledObjects/PooledObjects.h"

namespace Facets
{
  template <class ObjectType, class FacetType>
  class FacetWeakPtr
  {
  public:
    FacetWeakPtr()
      : m_Object(),
        m_FacetGood(false),
        m_Facet(0)
    {

    }

    FacetWeakPtr(ObjectType *_ptr)
      : m_Object(_ptr)
    {

    } 

    FacetWeakPtr(ObjectType &_ptr)
      : m_Object(_ptr)
    {

    } 

    FacetWeakPtr(PooledObjects::Handle _handle)
      : m_Object(_handle)
    {

    } 

    void Set(ObjectType* _ptr)
    {
      m_Object.Set(_ptr);
    }

    void Set(PooledObjects::Handle _handle)
    {
      m_Object.Set(_handle);
    }

    void Clear()
    {
      m_Object.Clear();
      m_FacetGood = false;
      m_Facet = 0;
    }

    FacetType *Resolve()
    {
      ObjectType *object_ptr = m_Object.Resolve();
      if (object_ptr)
      {
        if (m_FacetGood)
        {
          return m_Facet;
        }
        else
        {
          m_Facet = object_ptr->GetFacet<FacetType>();
          m_FacetGood = true;
          return m_Facet;
        }
      }
      else
      {
        m_FacetGood = false;
        m_Facet = 0;
      }

      return 0;
    }

  private:
    PooledObjects::PooledObjectWeakPtr<ObjectType> m_Object;
    FacetType*                                     m_Facet;
    bool                                           m_FacetGood;
  };
}