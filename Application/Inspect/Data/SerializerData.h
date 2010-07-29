#pragma once

#include "Application/API.h"
#include "Data.h"

#include <iomanip>

namespace Inspect
{
  // 
  // Base class for all serializer data types
  // 

  template< class T >
  class SerializerData : public DataTemplate< T >
  {
  public:
    INSPECT_TYPE( DataTypes::Serializer );
  };

  //
  // SerializerPropertyFormatter handles conversion between a property of T and string
  //

  template< class T >
  class SerializerPropertyFormatter : public SerializerData< T >
  {
  protected:
    Helium::SmartPtr< Helium::Property< T > > m_Property;

  public:
    SerializerPropertyFormatter(const Helium::SmartPtr< Helium::Property< T > >& property)
      : m_Property(property)
    {

    }

    virtual ~SerializerPropertyFormatter()
    {

    }

    virtual bool Set(const T& value, const DataChangedSignature::Delegate& emitter = NULL) HELIUM_OVERRIDE
    {
      bool result = false;

      Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( value ) );
      if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
      {
        T newValue;
        Reflect::Serializer::GetValue< T >( serializer, newValue );
        m_Property->Set( newValue );
        m_Changed.Raise( this, emitter );
        result = true;
      }

      return result;
    }

    virtual void Get(T& value) const HELIUM_OVERRIDE
    {
      value = m_Property->Get();
    }
  };


  //
  // MultiSerializerFormatter handles conversion between a property of T and string
  //

  template< class T >
  class MultiSerializerFormatter : public SerializerData< T >
  {
  protected:
    std::vector< Reflect::SerializerPtr > m_Serializers;
    const Reflect::Field* m_Field;

  public:
    MultiSerializerFormatter( const std::vector< Reflect::SerializerPtr >& serializers, const Reflect::Field* field )
      : m_Serializers( serializers )
      , m_Field( field )
    {

    }

    virtual ~MultiSerializerFormatter()
    {

    }

    const Reflect::Field* GetField() const
    {
      return m_Field;
    }

    virtual bool Set(const T& value, const DataChangedSignature::Delegate& emitter = DataChangedSignature::Delegate ()) HELIUM_OVERRIDE
    {
      bool result = false;

      Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( value ) );
      if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
      {
        T newValue;
        Serializer::GetValue< T >( serializer, newValue );
        std::vector< Reflect::SerializerPtr >::iterator itr = m_Serializers.begin();
        std::vector< Reflect::SerializerPtr >::iterator end = m_Serializers.end();
        for ( ; itr != end; ++itr )
        {
          Reflect::Serializer::SetValue< T >( *itr, newValue );
          result = true;
        }

        m_Changed.Raise( this, emitter );
      }

      return result;
    }

    virtual bool SetAll(std::vector< T >& values, const DataChangedSignature::Delegate& emitter = DataChangedSignature::Delegate ()) HELIUM_OVERRIDE
    {
      bool result = false;

      if ( values.size() == m_Serializers.size() )
      {
        std::vector< Reflect::SerializerPtr >::const_iterator itr = values.begin();
        std::vector< Reflect::SerializerPtr >::const_iterator end = values.end();
        for ( size_t index=0; itr != end; ++itr, ++index )
        {
          Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( *itr ) );
          if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
          {
            T newValue;
            Serializer::GetValue< T >( serializer, newValue );
            m_Serializers[ index ]->SetValue( newValue );
            result = true;
          }
        }

        m_Changed.Raise( this, emitter );
      }
      else
      {
        HELIUM_BREAK();
      }

      return result;
    }

    virtual void Get( T& value ) const
    {
      if ( m_Serializers.size() == 1 )
      {
        Reflect::Serializer::GetValue< T >( m_Serializers.front(), value );
      }
    }

    virtual void GetAll(std::vector< T >& values) const HELIUM_OVERRIDE
    {
      values.resize( m_Serializers.size() );
      std::vector< Reflect::SerializerPtr >::const_iterator itr = m_Serializers.begin();
      std::vector< Reflect::SerializerPtr >::const_iterator end = m_Serializers.end();
      for ( size_t index = 0; itr != end; ++itr, ++index )
      {
        T v;
        Reflect::Serializer::GetValue< T >( *itr, v );
        values[ index ] = v;
      }
    }
  };
}
