#pragma once

#include "Application/API.h"
#include "Application/Inspect/Inspect.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Automation/Property.h"
#include "Foundation/Reflect/Serializers.h"
#include "Application/Undo/Command.h"

#include <iomanip>

namespace Inspect
{
  //
  // Constants
  //

  const tchar UNDEF_VALUE_STRING[] = TXT( "Undef" );
  const tchar MULTI_VALUE_STRING[] = TXT( "Multi" );


  //
  // Data conversion
  //

  template<class T>
  inline void Extract(tistream& stream, T* val)
  {
    stream >> *val;
  }

  template<class T>
  inline void Insert(tostream& stream, const T* val)
  {
    stream << *val;
  }


  //
  // Empty string support
  //

  template<>
  inline void Extract(tistream& stream, tstring* val)
  {
      std::streamsize size = stream.rdbuf()->in_avail();
    if ( size == 0 )
    {
      val->clear();
    }
    else
    {
      val->resize( size );
      stream.read( const_cast< tchar* >( val->c_str() ), size );
    }
  }


  //
  // Treat chars as numbers
  //

  template<>
  inline void Extract(tistream& stream, u8* val)
  {
    u16 tmp;
    stream >> tmp;

    if (!stream.fail())
    {
      *val = (u8)tmp;
    }
  }

  template<>
  inline void Insert(tostream& stream, const u8* val)
  {
    u16 tmp = *val;
    stream << tmp;
  }

  template<>
  inline void Extract(tistream& stream, i8* val)
  {
    i16 tmp;
    stream >> tmp;

    if (!stream.fail())
    {
      *val = (u8)tmp;
    }
  }

  template<>
  inline void Insert(tostream& stream, const i8* val)
  {
    i16 tmp = *val;
    stream << tmp;
  }


  //
  // Used fixed notation for floating point
  //

  template<>
  inline void Insert(tostream& stream, const f32* val)
  {
    f32 tmp = *val;
    stream << std::fixed << std::setprecision(6) << tmp;
  }

  template<>
  inline void Insert(tostream& stream, const f64* val)
  {
    f64 tmp = *val;
    stream << std::fixed << std::setprecision(6) << tmp;
  }


  //
  // RTTI support
  //

  template<>
  inline void Extract(tistream& stream, Reflect::Serializer* val)
  {
    stream >> *val;
  }

  template<>
  inline void Insert(tostream& stream, const Reflect::Serializer* val)
  {
    stream << *val;
  }


  //
  // Event support
  //

  template<>
  inline void Extract(tistream& stream, Nocturnal::Void* val)
  {

  }

  template<>
  inline void Insert(tostream& stream, const Nocturnal::Void* val)
  {

  }


  //
  // Data base class
  //

  class Data;

  struct DataChangingArgs
  {
    const Data* m_Data;
    Reflect::SerializerPtr m_NewValue;

    DataChangingArgs( const Data* data, const Reflect::SerializerPtr& value )
      : m_Data (data)
      , m_NewValue( value )
    {

    }
  };
  typedef Nocturnal::Signature<bool, DataChangingArgs&> DataChangingSignature;

  struct DataChangedArgs
  {
    const Data* m_Data;

    DataChangedArgs( const Data* data )
      : m_Data (data)
    {

    }
  };
  typedef Nocturnal::Signature<void, const DataChangedArgs&> DataChangedSignature;

  namespace DataTypes
  {
    enum DataType
    {
      Custom,
      String,
      Serializer,
    };
  }

  template< typename T, DataTypes::DataType type >
  T* CastData( Data* data )
  {
    return data ? (data->HasType( type ) ? static_cast<T*>( data ) : NULL) : NULL;
  }

  class Data : public Nocturnal::RefCountBase< Data >
  {
  public:
      INSPECT_BASE( DataTypes::Custom );

    Data()
      : m_Significant(true)
    {

    }

    virtual ~Data()
    {

    }

    virtual void Refresh() = 0;

    virtual Undo::CommandPtr GetUndoCommand() const = 0;

  protected: 
    bool m_Significant; 
  public: 
    void SetSignificant(bool significant)
    {
      m_Significant = significant; 
    }
    bool IsSignificant() const
    {
      return m_Significant; 
    }

  protected:
    mutable DataChangingSignature::Event m_Changing;
  public:
    void AddChangingListener( const DataChangingSignature::Delegate& listener ) const
    {
      m_Changing.Add( listener );
    }
    void RemoveChangingListener( const DataChangingSignature::Delegate& listener ) const
    {
      m_Changing.Remove( listener );
    }

  protected:
    mutable DataChangedSignature::Event m_Changed;
  public:
    void AddChangedListener( const DataChangedSignature::Delegate& listener ) const
    {
      m_Changed.Add( listener );
    }
    void RemoveChangedListener( const DataChangedSignature::Delegate& listener ) const
    {
      m_Changed.Remove( listener );
    }
  };

  typedef Nocturnal::SmartPtr<Data> DataPtr;


  //
  // Base template for data, V is the value container, which may or may not be equal to T
  //  it will not be equal if insertion or extraction translates a compiler type to a string
  //

  template<class T>
  class DataCommand;

  template<class T>
  class DataTemplate : public Data
  {
  public:
    typedef Nocturnal::SmartPtr< DataTemplate > Ptr;

  public:
    virtual void Refresh() NOC_OVERRIDE
    {
      T temp;
      Get( temp );
      Set( temp );
    }

    virtual Undo::CommandPtr GetUndoCommand() const NOC_OVERRIDE
    {
      return new DataCommand<T>( this );
    }

    // set data
    virtual bool Set(const T& s, const DataChangedSignature::Delegate& emitter = DataChangedSignature::Delegate ()) = 0;

    virtual bool SetAll(const std::vector<T>& s, const DataChangedSignature::Delegate& emitter = DataChangedSignature::Delegate ())
    {
      bool result = false;
      NOC_ASSERT( s.size() == 1 ); // this means you did not NOC_OVERRIDE this function for data objects that support multi
      if ( s.size() > 0 )
      {
        result = Set( s.back(), emitter );
      }
      return result;
    }

    // get data
    virtual void Get(T& s) const = 0;

    virtual void GetAll(std::vector<T>& s) const
    {
      s.clear();
      T value;
      Get( value );
      s.push_back( value );
    }
  };


  //
  // Command object for Data Undo/Redo
  //  Store state of object(s) bound by data
  //

  template<class T>
  class DataCommand : public Undo::Command
  {
  protected:
    // the data object that is used to read/write from the client objects
    typename DataTemplate<T>::Ptr m_Data;

    // state information
    std::vector<T> m_Values;

  public:
    DataCommand( const typename DataTemplate<T>::Ptr& data )
      : m_Data ( data )
    {
      if ( m_Data.ReferencesObject() )
      {
        m_Data->GetAll(m_Values);
      }
    }

    void Undo() NOC_OVERRIDE
    {
      Swap();
    }

    void Redo() NOC_OVERRIDE
    {
      Swap();
    }

    virtual bool IsSignificant() const
    {
      if( m_Data )
      {
        return m_Data->IsSignificant(); 
      }
      else
      {
        return false; 
      }
    }

  private:
    void Swap()
    {
      std::vector<T> temp;

      // read current state into temp
      m_Data->GetAll( temp );

      // set previous state
      m_Data->SetAll( m_Values );

      // cache previously current state
      m_Values = temp;
    }
  };
}