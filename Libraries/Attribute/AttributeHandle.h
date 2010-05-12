#pragma once

#include "Attribute.h"
#include "AttributeCollection.h"
#include "Exceptions.h"

namespace Attribute
{
  /////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //  AttributeHandle - Base class
  //
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template<class AttributeType, class CollectionType>
  class AttributeHandle
  {
  protected:
    CollectionType* m_Collection;
    Nocturnal::SmartPtr<AttributeType> m_Attribute;

  public:
    AttributeHandle( CollectionType* collection )
      : m_Collection ( collection )
    {

    }

    AttributeHandle( CollectionType* collection, const Nocturnal::SmartPtr<AttributeType>& attribute )
      : m_Collection ( collection )
      , m_Attribute ( attribute )
    {

    }

    bool Valid()
    {
      return m_Attribute.ReferencesObject() && m_Attribute->m_Enabled;
    }

    AttributeType* operator -> ()
    {
      if (!Valid())
      {
        const char* attributeTypeName = "Unknown";
        const Reflect::Class* attributeClass = Reflect::GetClass< AttributeType >();
        if ( attributeClass )
        {
          attributeTypeName = attributeClass->m_ShortName.c_str();
        }

        if (m_Attribute.ReferencesObject())
        {
          throw DisabledAttributeException( attributeTypeName );
        }
        else
        {
          throw MissingAttributeException( attributeTypeName );
        }
      }

      return m_Attribute.Ptr();
    }
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //  AttributeEditor
  //
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template<class AttributeType>
  class AttributeEditor : public AttributeHandle<AttributeType, AttributeCollection>
  {
  public:
    AttributeEditor( AttributeCollection* collection )
      : AttributeHandle ( collection )
    {
      // downcast attribute from the collection, if it exists
      Nocturnal::SmartPtr<AttributeType> attr = Reflect::ObjectCast<AttributeType>( m_Collection->GetAttribute( Reflect::GetType<AttributeType>() ) );

      // if it exists
      if (attr.ReferencesObject())
      {
        // if its already in another collection (the override case)
        if ( attr->GetCollection() && attr->GetCollection() != collection )
        {
          // clone the attribute from the other collection for use in this collection
          m_Attribute = Reflect::TryCast<AttributeType>( attr->Clone() );
        }
        else
        {
          // no need to type check again in release
          m_Attribute = Reflect::TryCast<AttributeType>( attr );
        }
      }
      else
      {
        // no existing attribute, create a new one
        m_Attribute = Reflect::TryCast<AttributeType>( Reflect::Registry::GetInstance()->CreateInstance( Reflect::GetType<AttributeType>() ) );
      }

      // ensure its part of the collection
      m_Collection->SetAttribute( m_Attribute );
    }

    AttributeEditor( AttributeCollection* collection, const Nocturnal::SmartPtr<AttributeType>& attribute )
      : AttributeHandle ( collection, attribute )
    {

    }

    void Commit()
    {
      // this is fishy, but for completeness
      m_Attribute->m_Enabled = true;

      // Fire change event
      m_Attribute->RaiseChanged();
    }
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //  AttributeViewer
  //
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template<class AttributeType>
  class AttributeViewer : public AttributeHandle<AttributeType, const AttributeCollection>
  {
  public:
    AttributeViewer()
      : AttributeHandle ( NULL )
    {

    }

    AttributeViewer( const AttributeCollection* collection, bool useDefault = false )
      : AttributeHandle ( collection )
    {
      View( collection, useDefault );
    }

    void View( const AttributeCollection* collection, bool useDefault = false )
    {
      m_Collection = collection;

      m_Attribute = Reflect::ConstObjectCast<AttributeType>( m_Collection->GetAttribute( Reflect::GetType<AttributeType>() ) );

      if ( ( !m_Attribute.ReferencesObject() || !m_Attribute->m_Enabled ) && useDefault )
      {
        m_Attribute = new AttributeType();
      }
    }
  };
}
