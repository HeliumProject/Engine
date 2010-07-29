#pragma once

#include "Application/Inspect/Data/Data.h"
#include "Pipeline/Content/ParametricKey/ParametricKey.h"

namespace Inspect
{
  ///////////////////////////////////////////////////////////////////////////
  // Base class for all parametric key data types
  // 
  class ParametricKeyData : public DataTemplate< Content::V_ParametricKeyPtr >
  {
  protected:
    Reflect::CreateObjectFunc m_Creator;

  public:
    ParametricKeyData( Reflect::CreateObjectFunc creatorFunc )
      : m_Creator( creatorFunc )
    {

    }

    Content::ParametricKeyPtr CreateParametricKey()
    {
      return Reflect::AssertCast< Content::ParametricKey >( m_Creator() );
    }
  };

  typedef Helium::Property< Content::V_ParametricKeyPtr > ParametricKeyArrayProperty;
  typedef Helium::SmartPtr< ParametricKeyArrayProperty > ParametricKeyArrayPropertyPtr;

  ///////////////////////////////////////////////////////////////////////////
  // Manipulates an array of parametric keys through a property getter/setter.
  // 
  class ParametricKeyPropertyFormatter : public ParametricKeyData
  {
  protected:
    ParametricKeyArrayPropertyPtr m_Property;

  public:
    ParametricKeyPropertyFormatter( Reflect::CreateObjectFunc creatorFunc, const ParametricKeyArrayPropertyPtr& property )
      : ParametricKeyData( creatorFunc )
      , m_Property( property )
    {

    }

    virtual ~ParametricKeyPropertyFormatter()
    {

    }

    virtual bool Set( const Content::V_ParametricKeyPtr& value, const DataChangedSignature::Delegate& emitter = NULL ) HELIUM_OVERRIDE
    {
      bool result = false;

      Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( value ) );
      if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
      {
        Content::V_ParametricKeyPtr newValue;
        Reflect::Serializer::GetValue< Reflect::V_Element >( serializer, (Reflect::V_Element&)newValue );
        m_Property->Set( newValue );
        m_Changed.Raise( this, emitter );
        result = true;
      }

      return result;
    }

    virtual void Get( Content::V_ParametricKeyPtr& value ) const HELIUM_OVERRIDE
    {
      value = m_Property->Get();
    }
  };


  ///////////////////////////////////////////////////////////////////////////
  // Manipulates multiple parametric key arrays through property pointers.
  // 
  class MultiParametricKeyPropertyFormatter : public ParametricKeyData
  {
  public:
    typedef std::vector< ParametricKeyArrayPropertyPtr > V_ParametricKeyArrayPropertyPtr;

  protected:
    V_ParametricKeyArrayPropertyPtr m_Properties;

  public:
    MultiParametricKeyPropertyFormatter( Reflect::CreateObjectFunc creatorFunc, const V_ParametricKeyArrayPropertyPtr& properties )
      : ParametricKeyData( creatorFunc )
      , m_Properties( properties )
    {
    }

    virtual ~MultiParametricKeyPropertyFormatter()
    {
    }

    virtual bool Set( const Content::V_ParametricKeyPtr& value, const DataChangedSignature::Delegate& emitter = NULL ) HELIUM_OVERRIDE
    {
      bool result = false;

      Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create< Reflect::V_Element >( ( const Reflect::V_Element& )( value ) ) );
      if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
      {
        Content::V_ParametricKeyPtr newValue;
        Reflect::Serializer::GetValue< Reflect::V_Element >( serializer, (Reflect::V_Element&)newValue );
        V_ParametricKeyArrayPropertyPtr::iterator itr = m_Properties.begin();
        V_ParametricKeyArrayPropertyPtr::iterator end = m_Properties.end();
        for ( ; itr != end; ++itr )
        {
          ( *itr )->Set( newValue );
          result = true;
        }

        m_Changed.Raise( this, emitter );
      }

      return result;
    }

    virtual bool SetAll( const std::vector< Content::V_ParametricKeyPtr >& values, const DataChangedSignature::Delegate& emitter = NULL ) HELIUM_OVERRIDE
    {
      bool result = false;

      if ( values.size() == m_Properties.size() )
      {
        std::vector< Content::V_ParametricKeyPtr >::const_iterator itr = values.begin();
        std::vector< Content::V_ParametricKeyPtr >::const_iterator end = values.end();
        for ( size_t index = 0; itr != end; ++itr, ++index )
        {
          Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create< Reflect::V_Element >( ( const Reflect::V_Element& )( *itr ) ) );
          if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
          {
            Content::V_ParametricKeyPtr newValue;
            Reflect::Serializer::GetValue< Reflect::V_Element >( serializer, (Reflect::V_Element&)newValue );
            m_Properties[ index ]->Set( newValue );
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

    virtual void Get( Content::V_ParametricKeyPtr& value ) const HELIUM_OVERRIDE
    {
      if ( m_Properties.size() > 0 )
      {
        value = m_Properties.front()->Get();
      }
    }

    virtual void GetAll( std::vector< Content::V_ParametricKeyPtr >& values ) const HELIUM_OVERRIDE
    {
      values.resize( m_Properties.size() );
      V_ParametricKeyArrayPropertyPtr::const_iterator itr = m_Properties.begin();
      V_ParametricKeyArrayPropertyPtr::const_iterator end = m_Properties.end();
      for ( size_t index = 0; itr != end; ++itr, ++index )
      {
        values [ index ] = ( *itr )->Get();
      }
    }
  };

  ///////////////////////////////////////////////////////////////////////////
  // Handles data that is an array of parametric keys accessed by serializers.
  //
  class MultiParametricKeySerializerFormatter : public ParametricKeyData
  {
  protected:
    std::vector< Reflect::SerializerPtr > m_Serializers;

  public:
    MultiParametricKeySerializerFormatter( Reflect::CreateObjectFunc creatorFunc, const std::vector< Reflect::SerializerPtr >& serializers )
      : ParametricKeyData( creatorFunc )
      , m_Serializers( serializers )
    {

    }

    virtual ~MultiParametricKeySerializerFormatter()
    {

    }

    virtual bool Set( const Content::V_ParametricKeyPtr& value, const DataChangedSignature::Delegate& emitter = NULL ) HELIUM_OVERRIDE
    {
      bool result = false;

      Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( ( const Reflect::V_Element& )( value ) ) );
      if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
      {
        Reflect::V_Element newValue;
        Reflect::Serializer::GetValue< Reflect::V_Element >( serializer, newValue );
        std::vector< Reflect::SerializerPtr >::iterator itr = m_Serializers.begin();
        std::vector< Reflect::SerializerPtr >::iterator end = m_Serializers.end();
        for ( ; itr != end; ++itr )
        {
          Reflect::Serializer::SetValue< Reflect::V_Element >( *itr, newValue );
          result = true;
        }

        m_Changed.Raise( this, emitter );
      }

      return result;
    }

    virtual bool SetAll( const std::vector< Content::V_ParametricKeyPtr >& values, const DataChangedSignature::Delegate& emitter = NULL ) HELIUM_OVERRIDE
    {
      bool result = false;

      if ( values.size() == m_Serializers.size() )
      {
        std::vector< Content::V_ParametricKeyPtr >::const_iterator itr = values.begin();
        std::vector< Content::V_ParametricKeyPtr >::const_iterator end = values.end();
        for ( size_t index=0; itr != end; ++itr, ++index )
        {
          Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( ( const Reflect::V_Element& )*itr ) );
          if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
          {
            Reflect::V_Element newValue;
            Reflect::Serializer::GetValue< Reflect::V_Element >( serializer, newValue );
            Reflect::Serializer::SetValue< Reflect::V_Element >( m_Serializers[ index ], newValue );
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

    // Get a single value
    virtual void Get( Content::V_ParametricKeyPtr& value ) const
    {
      if ( m_Serializers.size() > 0 )
      {
        Reflect::Serializer::GetValue< Reflect::V_Element >( m_Serializers[0], (Reflect::V_Element&)(value) );
      }
    }

    // Get all values
    virtual void GetAll( std::vector< Content::V_ParametricKeyPtr >& values ) const HELIUM_OVERRIDE
    {
      values.resize( m_Serializers.size() );
      std::vector< Reflect::SerializerPtr >::const_iterator itr = m_Serializers.begin();
      std::vector< Reflect::SerializerPtr >::const_iterator end = m_Serializers.end();
      for ( u32 index = 0; itr != end; ++itr, ++index )
      {
        Content::V_ParametricKeyPtr v;
        Reflect::Serializer::GetValue< Reflect::V_Element >( *itr, (Reflect::V_Element&)(v) );
        values[ index ] = v;
      }
    }
  };
}
