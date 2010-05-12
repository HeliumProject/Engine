#pragma once

#include "API.h"
#include "Reflect/Serializers.h"

namespace Attribute
{
  typedef std::map<std::string, Reflect::ElementPtr> M_Element;

  class ATTRIBUTE_API ComponentCollection : public Reflect::ConcreteInheritor<ComponentCollection, Reflect::Element>
  {
  protected:
    M_Element m_Components;

  public:
    static void EnumerateClass( Reflect::Compositor<ComponentCollection>& comp );

    virtual bool ProcessComponent( Reflect::ElementPtr element, const std::string& fieldName ) NOC_OVERRIDE;

    const M_Element& GetComponents() const
    {
      return m_Components;
    }

    Reflect::ElementPtr GetComponent( const std::string& name, int id = Reflect::ReservedTypes::Any ) const;

    bool SetComponent( const std::string& name, Reflect::ElementPtr value );

    Reflect::ElementPtr RemoveComponent( const std::string& name );
  };

  typedef Nocturnal::SmartPtr<ComponentCollection> ComponentCollectionPtr;
}