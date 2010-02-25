#pragma once

#include "API.h"

#include "Windows/Atomic.h"

namespace Reflect
{
  class Structure;
  class Field;

  class REFLECT_API Visitor : public ::Windows::AtomicRefCountBase
  {
  public:
    virtual bool VisitElement(Element* element)
    {
      return true;
    }

    virtual bool VisitPointer(ElementPtr& pointer)
    {
      return true;
    }

    virtual bool VisitField(Element* element, const Field* field)
    {
      return true; 
    }
  };

  typedef Nocturnal::SmartPtr<Visitor> VisitorPtr;

  class REFLECT_API FindByTypeVisitor : public Visitor
  {
  public:
    i32 m_Type;

    V_Element& m_Found;

    FindByTypeVisitor(i32 type, V_Element& found);

    virtual bool VisitElement(Element* element) NOC_OVERRIDE;
  };

  class REFLECT_API FindByTypeSetVisitor : public Visitor
  {
  public:
    const S_i32& m_Types;

    V_Element& m_Found;

    FindByTypeSetVisitor(const S_i32& types, V_Element& found);

    virtual bool VisitElement(Element* element) NOC_OVERRIDE;
  };
}