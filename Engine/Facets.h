#pragma once

#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

//TODO: Should facets be allowed to tick?
//TODO: Should facets be allowed to communicate?
namespace Helium
{
  namespace Facets
  {
    namespace Private
    {
      typedef std::vector<uintptr_t> V_uintptr;
      typedef std::map<Reflect::TypeID, uintptr_t> OffsetStructure;

      struct TypeData
      {
        OffsetStructure Offsets;
      };
    }

    struct LUNAR_ENGINE_API IFacet : public Reflect::Object
    {
      REFLECT_DECLARE_ABSTRACT(IFacet, Reflect::Object);
      static void IF_YOU_SEE_THIS_THE_FACET_YOU_ARE_TRYING_TO_REGISTER_DOES_NOT_IMPLEMENT_IFACET() { }
    };

    struct LUNAR_ENGINE_API FacetIterator
    {
    public:
      FacetIterator(uintptr_t _object_address, Private::OffsetStructure::iterator &_iter)
        : m_ObjectAddress(_object_address),
        m_OffsetIterator(_iter)
      {

      }

      FacetIterator operator++()
      {
        ++m_OffsetIterator;
        return *this;
      }

      FacetIterator operator++(int)
      {
        FacetIterator iter = *this;
        ++m_OffsetIterator;
        return *this;
      }

      IFacet &operator*()
      {
        return *reinterpret_cast<IFacet *>(m_ObjectAddress + m_OffsetIterator->second);
      }

      IFacet *operator->()
      {
        return reinterpret_cast<IFacet *>(m_ObjectAddress + m_OffsetIterator->second);
      }

      bool operator==(FacetIterator &_rhs)
      {
        return (m_ObjectAddress == _rhs.m_ObjectAddress) && (m_OffsetIterator == _rhs.m_OffsetIterator);
      }

      bool operator!=(FacetIterator &_rhs)
      {
        return !(*this == _rhs);
      }

    private:
      uintptr_t                             m_ObjectAddress;
      Private::OffsetStructure::iterator  m_OffsetIterator;
    };

    template <class T, class U>
    T *GetFacet(U &_host)
    {
      Facets::Private::TypeData &type_data = _host.GetFacetTypeData();

      Private::OffsetStructure::iterator iter = type_data.Offsets.find(Reflect::GetClass<T>());
      if (iter != type_data.Offsets.end())
      {
        return Reflect::AssertCast<T>(reinterpret_cast<IFacet *>(reinterpret_cast<uintptr_t>(&_host) + iter->second));
      }

      return 0;
    }

    template <class T, class U>
    T *GetFacetThatImplements(U &_host)
    {
      Reflect::Registry *registry = Reflect::Registry::GetInstance();
      Facets::Private::TypeData &type_data = _host.GetFacetTypeData();

      Class *T_type_id = Reflect::GetClass<T>();

      Private::OffsetStructure::iterator iter;
      for (iter = type_data.Offsets.begin(); iter != type_data.Offsets.end(); ++iter)
      {
        const Reflect::Class *c = registry->GetClass(iter->first);
        if (c->IsType(T_type_id))
        {
          return Reflect::AssertCast<T>(reinterpret_cast<IFacet *>(reinterpret_cast<uintptr_t>(&_host) + iter->second));
        }
      }

      return 0;
    }

    LUNAR_ENGINE_API void Initialize();
    LUNAR_ENGINE_API void Cleanup();
  }
}

#define ENGINE_FACETS_BEGIN(__OuterType)                                                    \
  typedef __OuterType ENGINE_FACETS_OUTER_TYPE;                                             \
  static Facets::Private::TypeData &GetStaticFacetTypeData()                                      \
  {                                                                                         \
    static Facets::Private::TypeData type_data;                                             \
    return type_data;                                                                       \
  }                                                                                         \
  \
  virtual Facets::Private::TypeData &GetFacetTypeData()       \
  {                                                                             \
    return GetStaticFacetTypeData();                                                                            \
  }                                                                                 \
  \
  template <class T>                                                                        \
  T *GetFacet()                                                                             \
  {                                                                                         \
  return Facets::GetFacet<T, __OuterType>(*this);                                         \
}                                                                                         \
  \
  template <class T>                                                                        \
  T *GetFacetThatImplements()                                                               \
  {                                                                                         \
  return Facets::GetFacetThatImplements()<T, __OuterType>(*this);                         \
}                                                                                         \
  \
                                                                                            \
  static void InitializeFacetOffsets()                                                      \
  {                                                                                         \
    Facets::Private::TypeData &type_data = GetStaticFacetTypeData();



#define REGISTER_FACET(__Type, __MemberName)                                                \
  __Type::IF_YOU_SEE_THIS_THE_FACET_YOU_ARE_TRYING_TO_REGISTER_DOES_NOT_IMPLEMENT_IFACET(); \
  type_data.Offsets[Reflect::GetClass<__Type>()] =                                           \
    offsetof(ENGINE_FACETS_OUTER_TYPE, __MemberName);



#define ENGINE_FACETS_END()                                                                 \
}                                                                                           \
                                                                                            \
  virtual Facets::FacetIterator FacetsBegin()                                               \
  {                                                                                         \
    return Facets::FacetIterator(                                                           \
    reinterpret_cast<uintptr_t>(this), GetFacetTypeData().Offsets.begin());             \
  }                                                                                         \
                                                                                            \
  virtual Facets::FacetIterator FacetsEnd()                                                 \
  {                                                                                         \
    return Facets::FacetIterator(                                                           \
    reinterpret_cast<uintptr_t>(this), GetFacetTypeData().Offsets.end());               \
  }
