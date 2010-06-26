#pragma once

#include "Application/API.h"
#include "Data.h"

#include <iomanip>

namespace Inspect
{
  //
  // Base class for all string-translated data types
  //

  class StringData : public DataTemplate< tstring >
  {
  public:
    INSPECT_TYPE( DataTypes::String );
  };
  typedef Nocturnal::SmartPtr< StringData > StringDataPtr;


  //
  // StringFormatter handles conversion between T and string
  //

  template<class T>
  class StringFormatter : public StringData
  {
  public:
    typedef Nocturnal::SmartPtr< StringFormatter<T> > Ptr;

  protected:
    // the data we are manipulating
    T* m_Data;

    // perishable data gets thrown away at destruction
    bool m_Perishable;

  public:
    StringFormatter(T* data, bool perishable = false)
      : m_Data (data)
      , m_Perishable (perishable)
    {

    }

    virtual ~StringFormatter()
    {
      if (m_Perishable)
      {
        delete m_Data;
      }
    }

    virtual bool Set(const tstring& s, const DataChangedSignature::Delegate& emitter = NULL) NOC_OVERRIDE
    {
      bool result = false;

      Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create< tstring >( s ) );
      if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
      {
        tstring newValue;
        Reflect::Serializer::GetValue< tstring >( serializer, newValue );
        Extract< T >( std::stringstream ( newValue ), m_Data );
        m_Changed.Raise( this, emitter );
        result = true;
      }

      return result;
    }

    virtual void Get(tstring& s) const NOC_OVERRIDE
    {
      std::stringstream stream;
      Insert<T>(stream, m_Data);
      s = stream.str();
    }
  };


  //
  // MultiStringFormatter handles conversion between multiple T's and strings
  //

  template<class T>
  class MultiStringFormatter : public StringData
  {
  public:
    typedef Nocturnal::SmartPtr< MultiStringFormatter<T> > Ptr;

  protected:
    // a link to each data we are manipulating
    std::vector<T*> m_Data;

    // perishable data gets thrown away at destruction
    bool m_Perishable;

  public:
    MultiStringFormatter(std::vector<T*> data, bool perishable = false)
      : m_Data (data)
      , m_Perishable (perishable)
    {

    }

    virtual ~MultiStringFormatter()
    {
      if (m_Perishable)
      {
        std::vector<T*>::iterator itr = m_Data.begin();
        std::vector<T*>::iterator end = m_Data.end();
        for ( ; itr != end; ++itr )
        {
          delete (*itr);
        }
      }
    }

    virtual bool Set(const tstring& s, const DataChangedSignature::Delegate& emitter = NULL) NOC_OVERRIDE
    {
      bool result = false;

      Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( s ) );
      if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
      {
          tstring newValue;
          Reflect::Serializer::GetValue< tstring >( serializer, newValue );
          std::vector<T*>::iterator itr = m_Data.begin();
          std::vector<T*>::iterator end = m_Data.end();
          for ( ; itr != end; ++itr )
          {
              Extract<T>( tstringstream( newValue ), *itr );
              result = true;
          }

          m_Changed.Raise( this, emitter );
      }

      return result;
    }

    virtual bool SetAll(const std::vector< tstring >& values, const DataChangedSignature::Delegate& emitter = NULL) NOC_OVERRIDE
    {
      bool result = false;

      if ( values.size() == m_Data.size() )
      {
        std::vector< tstring >::const_iterator itr = values.begin();
        std::vector< tstring >::const_iterator end = values.end();
        for ( size_t index = 0; itr != end; ++itr, ++index )
        {
          Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( *itr ) );
          if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
          {
              tstring newValue;
              Reflect::Serializer::GetValue< tstring >( serializer, newValue );
              Extract<T>( tstringstream ( newValue ), m_Data[ index ] );
              result = true;
          }
        }

        m_Changed.Raise( this, emitter );
      }
      else
      {
        NOC_BREAK();
      }

      return result;
    }

    virtual void Get(tstring& s) const NOC_OVERRIDE
    {
      T* value = NULL;
      tstringstream stream;

      //
      // Scan for equality
      //

      std::vector<T*>::const_iterator itr = m_Data.begin();
      std::vector<T*>::const_iterator end = m_Data.end();
      for ( ; itr != end; ++itr )
      {
        // grab the first one if we don't have a value yet
        if (value == NULL)
        {
          value = *itr;
          continue;
        }

        // do equality
        if (*value != *(*itr))
        {
          // we are not equal, break
          value = NULL;
          break;
        }
      }

      // if we were equal
      if (value != NULL)
      {
        // do insert
        Insert<T>(stream, value);
      }
      // else we are unequal
      else
      {
        // if we have data
        if (m_Data.size() > 0)
        {
          // we are a multi
          stream << MULTI_VALUE_STRING;
        }
        // we have no data
        else
        {
          // god help you if you hit this!
          stream << UNDEF_VALUE_STRING;
        }
      }

      // set the result
      s = stream.str();
    }

    virtual void GetAll(std::vector< tstring >& s) const NOC_OVERRIDE
    {
      s.resize( m_Data.size() );
      std::vector<T*>::const_iterator itr = m_Data.begin();
      std::vector<T*>::const_iterator end = m_Data.end();
      for ( size_t index = 0; itr != end; ++itr, ++index )
      {
        T* value = *itr;
        std::stringstream stream;
        Insert<T>( stream, value );
        s[ index ] = stream.str();
      }
    }
  };


  //
  // PropertyStringFormatter handles conversion between a property of T and string
  //

  template<class T>
  class PropertyStringFormatter : public StringData
  {
  public:
    typedef Nocturnal::SmartPtr< PropertyStringFormatter<T> > Ptr;

  protected:
    Nocturnal::SmartPtr< Nocturnal::Property<T> > m_Property;

  public:
    PropertyStringFormatter(const Nocturnal::SmartPtr< Nocturnal::Property<T> >& property)
      : m_Property(property)
    {

    }

    virtual ~PropertyStringFormatter()
    {

    }

    virtual bool Set(const tstring& s, const DataChangedSignature::Delegate& emitter = NULL) NOC_OVERRIDE
    {
      bool result = false;

      Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( s ) );
      if ( m_Changing.RaiseWithReturn( DataChangingArgs ( this, serializer ) ) )
      {
        T value;
        tstring newValue;
        Reflect::Serializer::GetValue< tstring >( serializer, newValue );
        Extract< T >( std::stringstream( newValue ), &value );
        m_Property->Set( value );
        m_Changed.Raise( this, emitter );
        result = true;
      }

      return result;
    }

    virtual void Get(tstring& s) const NOC_OVERRIDE
    {
      std::stringstream stream;
      T val = m_Property->Get();
      Insert<T>( stream, &val );
      s = stream.str();
    }
  };


  //
  // MultiPropertyStringFormatter handles conversion between T and string
  //

  template<class T>
  class MultiPropertyStringFormatter : public StringData
  {
  public:
    typedef Nocturnal::SmartPtr< MultiPropertyStringFormatter<T> > Ptr;

  protected:
    std::vector< Nocturnal::SmartPtr< Nocturnal::Property<T> > > m_Properties;

  public:
    MultiPropertyStringFormatter(const std::vector< Nocturnal::SmartPtr< Nocturnal::Property<T> > >& properties)
      : m_Properties (properties)
    {

    }

    virtual ~MultiPropertyStringFormatter()
    {

    }

    virtual bool Set(const tstring& s, const DataChangedSignature::Delegate& emitter = NULL) NOC_OVERRIDE
    {
      bool result = false;

      Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create< tstring >( s ) );
      if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
      {
        tstring newValue;
        Reflect::Serializer::GetValue< tstring >( serializer, newValue );
        T value;
        std::vector< Nocturnal::SmartPtr< Nocturnal::Property<T> > >::iterator itr = m_Properties.begin();
        std::vector< Nocturnal::SmartPtr< Nocturnal::Property<T> > >::iterator end = m_Properties.end();
        for ( ; itr != end; ++itr )
        {
          Extract<T>( std::stringstream ( newValue ), &value );
          (*itr)->Set( value );
          result = true;
        }

        m_Changed.Raise( this, emitter );
      }

      return result;
    }

    virtual bool SetAll(const std::vector< tstring >& s, const DataChangedSignature::Delegate& emitter = NULL) NOC_OVERRIDE
    {
      bool result = false;

      if ( s.size() == m_Properties.size() )
      {
        std::vector< tstring >::const_iterator itr = s.begin();
        std::vector< tstring >::const_iterator end = s.end();
        for ( size_t index = 0; itr != end; ++itr, ++index )
        {
          Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( *itr ) );
          if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
          {
            T value;
            tstring newValue;
            Reflect::Serializer::GetValue< tstring >( serializer, newValue );
            Extract<T>( std::stringstream ( newValue ), &value );
            m_Properties[ index ]->Set(value);
            result = true;
          }
        }

        m_Changed.Raise( this, emitter );
      }
      else
      {
        NOC_BREAK();
      }

      return result;
    }

    virtual void Get(tstring& s) const NOC_OVERRIDE
    {
      std::stringstream stream;

      //
      // Scan for equality
      //

      std::vector< Nocturnal::SmartPtr< Nocturnal::Property<T> > >::const_iterator itr = m_Properties.begin();
      std::vector< Nocturnal::SmartPtr< Nocturnal::Property<T> > >::const_iterator end = m_Properties.end();
      for ( ; itr != end; ++itr )
      {
        // grab the first one if we don't have a value yet
        if ( itr == m_Properties.begin() )
        {
          T val = (*itr)->Get();
          Insert<T>( stream, &val );
          continue;
        }
        else
        {
          T val = (*itr)->Get();
          std::stringstream temp;
          Insert<T>( temp, &val );

          if (temp.str() != stream.str())
          {
            break;
          }
        }
      }

      // if we were not equal
      if (itr == end)
      {
        s = stream.str();
      }
      else
      {
        // if we have data
        if (m_Properties.size() > 0)
        {
          // we are a multi
          s = MULTI_VALUE_STRING;
        }
        // we have no data
        else
        {
          // god help you if you hit this!
          s = UNDEF_VALUE_STRING;
        }
      }
    }

    virtual void GetAll(std::vector< tstring >& s) const NOC_OVERRIDE
    {
      s.resize( m_Properties.size() );
      std::vector< Nocturnal::SmartPtr< Nocturnal::Property<T> > >::const_iterator itr = m_Properties.begin();
      std::vector< Nocturnal::SmartPtr< Nocturnal::Property<T> > >::const_iterator end = m_Properties.end();
      for ( size_t index = 0 ; itr != end; ++itr, ++index )
      {
        T val = (*itr)->Get();
        std::stringstream stream;
        Insert<T>( stream, &val );
        s[ index ] = stream.str();
      }
    }
  };
}
