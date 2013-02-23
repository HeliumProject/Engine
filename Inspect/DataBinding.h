#pragma once

#include "Foundation/SmartPtr.h"
#include "Foundation/Event.h"
#include "Foundation/Property.h"
#include "Application/UndoQueue.h"

#include "Reflect/DataDeduction.h"

#include "Inspect/API.h"

#include <iomanip>

#define INSPECT_BASE(__ID, __Type) \
  public: \
    typedef __Type This; \
    virtual int GetType() const \
    { \
      return __ID; \
    } \
    virtual bool HasType(int id) const \
    { \
      return __ID == id; \
    }

#define INSPECT_TYPE(__ID, __Type, __Base) \
  public: \
    typedef __Type This; \
    typedef __Base Base; \
    virtual int GetType() const \
    { \
      return __ID; \
    } \
    virtual bool HasType(int id) const \
    { \
      return __ID == id || Base::HasType(id); \
    }

namespace Helium
{
    namespace Inspect
    {
        //
        // Constants
        //

        const tchar_t UNDEF_VALUE_STRING[] = TXT( "Undef" );
        const tchar_t MULTI_VALUE_STRING[] = TXT( "Multi" );

        //
        // DataBinding conversion
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
				HELIUM_ASSERT(size < 0xFFFFFFFFL);
				val->resize( tstring::size_type(size) );
                stream.read( const_cast< tchar_t* >( val->c_str() ), size );
            }
        }

        //
        // Treat chars as numbers
        //

        template<>
        inline void Extract(tistream& stream, uint8_t* val)
        {
            uint16_t tmp;
            stream >> tmp;

            if (!stream.fail())
            {
                *val = (uint8_t)tmp;
            }
        }

        template<>
        inline void Insert(tostream& stream, const uint8_t* val)
        {
            uint16_t tmp = *val;
            stream << tmp;
        }

        template<>
        inline void Extract(tistream& stream, int8_t* val)
        {
            int16_t tmp;
            stream >> tmp;

            if (!stream.fail())
            {
                *val = (uint8_t)tmp;
            }
        }

        template<>
        inline void Insert(tostream& stream, const int8_t* val)
        {
            int16_t tmp = *val;
            stream << tmp;
        }

        //
        // Used fixed notation for floating point
        //

        template<>
        inline void Insert(tostream& stream, const float32_t* val)
        {
            float32_t tmp = *val;
            stream << std::fixed << std::setprecision(6) << tmp;
        }

        template<>
        inline void Insert(tostream& stream, const float64_t* val)
        {
            float64_t tmp = *val;
            stream << std::fixed << std::setprecision(6) << tmp;
        }

        //
        // Data support
        //

        template<>
        inline void Extract(tistream& stream, Reflect::Data* val)
        {
            stream >> *val;
        }

        template<>
        inline void Insert(tostream& stream, const Reflect::Data* val)
        {
            stream << *val;
        }

        //
        // Event support
        //

        template<>
        inline void Extract(tistream& stream, Helium::Void* val)
        {

        }

        template<>
        inline void Insert(tostream& stream, const Helium::Void* val)
        {

        }

        //
        // DataBinding base class
        //

        class DataBinding;

        struct DataChangingArgs
        {
            DataChangingArgs( const DataBinding* data, Reflect::Data* value )
                : m_Data ( data )
                , m_NewValue( value )
                , m_Veto ( false )
            {
            
            }

            const DataBinding*             m_Data;
            Reflect::DataPtr  m_NewValue;
            mutable bool            m_Veto;
        };
        typedef Helium::Signature< const DataChangingArgs& > DataChangingSignature;

        struct DataChangedArgs
        {
            DataChangedArgs( const DataBinding* data ) : m_Data (data) {}

            const DataBinding* m_Data;
        };
        typedef Helium::Signature< const DataChangedArgs& > DataChangedSignature;

        namespace DataBindingTypes
        {
            enum DataBindingType
            {
                Custom,
                String,
                Typed,
            };
        }

        template< typename T, DataBindingTypes::DataBindingType type >
        T* CastDataBinding( DataBinding* data )
        {
            return data ? (data->HasType( type ) ? static_cast<T*>( data ) : NULL) : NULL;
        }

        class DataBinding : public Helium::RefCountBase< DataBinding >
        {
        public:
            INSPECT_BASE( DataBindingTypes::Custom, DataBinding );

            DataBinding()
                : m_Significant(true)
            {

            }

            virtual ~DataBinding()
            {

            }

            virtual void Refresh() = 0;

            virtual UndoCommandPtr GetUndoCommand() const = 0;

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

        typedef Helium::SmartPtr<DataBinding> DataBindingPtr;

        //
        // Base template for data, V is the value container, which may or may not be equal to T
        //  it will not be equal if insertion or extraction translates a compiler type to a string
        //

        template<class T>
        class DataBindingCommand;

        template<class T>
        class DataBindingTemplate : public DataBinding
        {
        public:
            typedef Helium::SmartPtr< DataBindingTemplate > Ptr;

        public:
            virtual void Refresh() HELIUM_OVERRIDE
            {
                T temp;
                Get( temp );
                Set( temp );
            }

            virtual UndoCommandPtr GetUndoCommand() const HELIUM_OVERRIDE
            {
                return new DataBindingCommand<T>( this );
            }

            // set data
            virtual bool Set(const T& s, const DataChangedSignature::Delegate& emitter = DataChangedSignature::Delegate ()) = 0;

            virtual bool SetAll(const std::vector<T>& s, const DataChangedSignature::Delegate& emitter = DataChangedSignature::Delegate ())
            {
                bool result = false;
                HELIUM_ASSERT( s.size() == 1 ); // this means you did not HELIUM_OVERRIDE this function for data objects that support multi
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
        // UndoCommand object for DataBinding Undo/Redo
        //  Store state of object(s) bound by data
        //

        template<class T>
        class DataBindingCommand : public UndoCommand
        {
        protected:
            // the data object that is used to read/write from the client objects
            typename DataBindingTemplate<T>::Ptr m_Data;

            // state information
            std::vector<T> m_Values;

        public:
            DataBindingCommand( const typename DataBindingTemplate<T>::Ptr& data )
                : m_Data ( data )
            {
                if ( m_Data.ReferencesObject() )
                {
                    m_Data->GetAll(m_Values);
                }
            }

            void Undo() HELIUM_OVERRIDE
            {
                Swap();
            }

            void Redo() HELIUM_OVERRIDE
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


        //
        // Base class for all string-translated data types
        //

        class StringDataBinding : public DataBindingTemplate< tstring >
        {
        public:
            INSPECT_TYPE( DataBindingTypes::String, StringDataBinding, DataBinding );
        };
        typedef Helium::SmartPtr< StringDataBinding > StringDataPtr;

        //
        // StringFormatter handles conversion between T and string
        //

        template<class T>
        class StringFormatter : public StringDataBinding
        {
        public:
            typedef Helium::SmartPtr< StringFormatter<T> > Ptr;

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

            virtual bool Set(const tstring& s, const DataChangedSignature::Delegate& emitter = NULL) HELIUM_OVERRIDE
            {
                bool result = false;

                Reflect::DataPtr data = Reflect::AssertCast< Reflect::Data >( Reflect::Data::Create< tstring >( s ) );
                
                DataChangingArgs args ( this, data );
                m_Changing.Raise( args );
                if ( !args.m_Veto )
                {
                    tstring newValue;
                    Reflect::Data::GetValue< tstring >( data, newValue );
                    Extract< T >( tstringstream ( newValue ), m_Data );
                    m_Changed.RaiseWithEmitter( this, emitter );
                    result = true;
                }

                return result;
            }

            virtual void Get(tstring& s) const HELIUM_OVERRIDE
            {
                tstringstream stream;
                Insert<T>(stream, m_Data);
                s = stream.str();
            }
        };

        //
        // MultiStringFormatter handles conversion between multiple T's and strings
        //

        template<class T>
        class MultiStringFormatter : public StringDataBinding
        {
        public:
            typedef Helium::SmartPtr< MultiStringFormatter<T> > Ptr;

        protected:
            // a link to each data we are manipulating
            std::vector<T*> m_Data;

            // perishable data gets thrown away at destruction
            bool m_Perishable;

        public:
            MultiStringFormatter( const std::vector<T*>& data, bool perishable = false)
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

            virtual bool Set(const tstring& s, const DataChangedSignature::Delegate& emitter = NULL) HELIUM_OVERRIDE
            {
                bool result = false;

                Reflect::DataPtr data = Reflect::AssertCast< Reflect::Data >( Reflect::Data::Create( s ) );

                DataChangingArgs args ( this, data );
                m_Changing.Raise( args );
                if ( !args.m_Veto )
                {
                    tstring newValue;
                    Reflect::Data::GetValue< tstring >( data, newValue );
                    std::vector<T*>::iterator itr = m_Data.begin();
                    std::vector<T*>::iterator end = m_Data.end();
                    for ( ; itr != end; ++itr )
                    {
                        Extract<T>( tstringstream( newValue ), *itr );
                        result = true;
                    }

                    m_Changed.RaiseWithEmitter( this, emitter );
                }

                return result;
            }

            virtual bool SetAll(const std::vector< tstring >& values, const DataChangedSignature::Delegate& emitter = NULL) HELIUM_OVERRIDE
            {
                bool result = false;

                if ( values.size() == m_Data.size() )
                {
                    std::vector< tstring >::const_iterator itr = values.begin();
                    std::vector< tstring >::const_iterator end = values.end();
                    for ( size_t index = 0; itr != end; ++itr, ++index )
                    {
                        Reflect::DataPtr data = Reflect::AssertCast< Reflect::Data >( Reflect::Data::Create( *itr ) );
                        DataChangingArgs args ( this, data );
                        m_Changing.Raise( args );
                        if ( !args.m_Veto )
                        {
                            tstring newValue;
                            Reflect::Data::GetValue< tstring >( data, newValue );
                            Extract<T>( tstringstream ( newValue ), m_Data[ index ] );
                            result = true;
                        }
                    }

                    m_Changed.RaiseWithEmitter( this, emitter );
                }
                else
                {
                    HELIUM_BREAK();
                }

                return result;
            }

            virtual void Get(tstring& s) const HELIUM_OVERRIDE
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
                    HELIUM_ASSERT( m_Data.size() );

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

            virtual void GetAll(std::vector< tstring >& s) const HELIUM_OVERRIDE
            {
                s.resize( m_Data.size() );
                std::vector<T*>::const_iterator itr = m_Data.begin();
                std::vector<T*>::const_iterator end = m_Data.end();
                for ( size_t index = 0; itr != end; ++itr, ++index )
                {
                    T* value = *itr;
                    tstringstream stream;
                    Insert<T>( stream, value );
                    s[ index ] = stream.str();
                }
            }
        };

        //
        // PropertyStringFormatter handles conversion between a property of T and string
        //

        template<class T>
        class PropertyStringFormatter : public StringDataBinding
        {
        public:
            typedef Helium::SmartPtr< PropertyStringFormatter<T> > Ptr;

        protected:
            Helium::SmartPtr< Helium::Property<T> > m_Property;

        public:
            PropertyStringFormatter(const Helium::SmartPtr< Helium::Property<T> >& property)
                : m_Property(property)
            {

            }

            virtual ~PropertyStringFormatter()
            {

            }

            virtual bool Set(const tstring& s, const DataChangedSignature::Delegate& emitter = NULL) HELIUM_OVERRIDE
            {
                bool result = false;

                Reflect::DataPtr data = Reflect::AssertCast< Reflect::Data >( Reflect::Data::Create( s ) );
                DataChangingArgs args ( this, data );
                m_Changing.Raise( args );
                if ( !args.m_Veto )
                {
                    T value;
                    tstring newValue;
                    Reflect::Data::GetValue< tstring >( data, newValue );
                    Extract< T >( tstringstream( newValue ), &value );
                    m_Property->Set( value );
                    m_Changed.RaiseWithEmitter( this, emitter );
                    result = true;
                }

                return result;
            }

            virtual void Get(tstring& s) const HELIUM_OVERRIDE
            {
                tstringstream stream;
                T val = m_Property->Get();
                Insert<T>( stream, &val );
                s = stream.str();
            }
        };

        //
        // MultiPropertyStringFormatter handles conversion between T and string
        //

        template<class T>
        class MultiPropertyStringFormatter : public StringDataBinding
        {
        public:
            typedef Helium::SmartPtr< MultiPropertyStringFormatter<T> > Ptr;

        protected:
            std::vector< Helium::SmartPtr< Helium::Property<T> > > m_Properties;

        public:
            MultiPropertyStringFormatter(const std::vector< Helium::SmartPtr< Helium::Property<T> > >& properties)
                : m_Properties (properties)
            {

            }

            virtual ~MultiPropertyStringFormatter()
            {

            }

            virtual bool Set(const tstring& s, const DataChangedSignature::Delegate& emitter = NULL) HELIUM_OVERRIDE
            {
                bool result = false;

                Reflect::DataPtr data = Reflect::AssertCast< Reflect::Data >( Reflect::Data::Create< tstring >( s ) );
                DataChangingArgs args ( this, data );
                m_Changing.Raise( args );
                if ( !args.m_Veto )
                {
                    tstring newValue;
                    Reflect::Data::GetValue< tstring >( data, newValue );
                    T value;
                    std::vector< Helium::SmartPtr< Helium::Property<T> > >::iterator itr = m_Properties.begin();
                    std::vector< Helium::SmartPtr< Helium::Property<T> > >::iterator end = m_Properties.end();
                    for ( ; itr != end; ++itr )
                    {
                        Extract<T>( tstringstream ( newValue ), &value );
                        (*itr)->Set( value );
                        result = true;
                    }

                    m_Changed.RaiseWithEmitter( this, emitter );
                }

                return result;
            }

            virtual bool SetAll(const std::vector< tstring >& s, const DataChangedSignature::Delegate& emitter = NULL) HELIUM_OVERRIDE
            {
                bool result = false;

                if ( s.size() == m_Properties.size() )
                {
                    std::vector< tstring >::const_iterator itr = s.begin();
                    std::vector< tstring >::const_iterator end = s.end();
                    for ( size_t index = 0; itr != end; ++itr, ++index )
                    {
                        Reflect::DataPtr data = Reflect::AssertCast< Reflect::Data >( Reflect::Data::Create( *itr ) );
                        DataChangingArgs args ( this, data );
                        m_Changing.Raise( args );
                        if ( !args.m_Veto )
                        {
                            T value;
                            tstring newValue;
                            Reflect::Data::GetValue< tstring >( data, newValue );
                            Extract<T>( tstringstream ( newValue ), &value );
                            m_Properties[ index ]->Set(value);
                            result = true;
                        }
                    }

                    m_Changed.RaiseWithEmitter( this, emitter );
                }
                else
                {
                    HELIUM_BREAK();
                }

                return result;
            }

            virtual void Get(tstring& s) const HELIUM_OVERRIDE
            {
                tstringstream stream;

                //
                // Scan for equality
                //

                std::vector< Helium::SmartPtr< Helium::Property<T> > >::const_iterator itr = m_Properties.begin();
                std::vector< Helium::SmartPtr< Helium::Property<T> > >::const_iterator end = m_Properties.end();
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
                        tstringstream temp;
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

            virtual void GetAll(std::vector< tstring >& s) const HELIUM_OVERRIDE
            {
                s.resize( m_Properties.size() );
                std::vector< Helium::SmartPtr< Helium::Property<T> > >::const_iterator itr = m_Properties.begin();
                std::vector< Helium::SmartPtr< Helium::Property<T> > >::const_iterator end = m_Properties.end();
                for ( size_t index = 0 ; itr != end; ++itr, ++index )
                {
                    T val = (*itr)->Get();
                    tstringstream stream;
                    Insert<T>( stream, &val );
                    s[ index ] = stream.str();
                }
            }
        };


        // 
        // Base class for all data data types
        // 

        template< class T >
        class TypedDataBinding : public DataBindingTemplate< T >
        {
        public:
            INSPECT_TYPE( DataBindingTypes::Typed, TypedDataBinding, DataBinding );
        };

        //
        // TypedPropertyFormatter handles conversion between a property of T and string
        //

        template< class T >
        class TypedPropertyFormatter : public TypedDataBinding< T >
        {
        protected:
            Helium::SmartPtr< Helium::Property< T > > m_Property;

        public:
            TypedPropertyFormatter(const Helium::SmartPtr< Helium::Property< T > >& property)
                : m_Property(property)
            {

            }

            virtual ~TypedPropertyFormatter()
            {

            }

            virtual bool Set(const T& value, const DataChangedSignature::Delegate& emitter = NULL) HELIUM_OVERRIDE
            {
                bool result = false;

                Reflect::DataPtr data = Reflect::AssertCast< Reflect::Data >( Reflect::Data::Create( value ) );
                DataChangingArgs args ( this, data );
                m_Changing.Raise( args );
                if ( !args.m_Veto )
                {
                    T newValue;
                    Reflect::Data::GetValue< T >( data, newValue );
                    m_Property->Set( newValue );
                    m_Changed.RaiseWithEmitter( this, emitter );
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
        // MultiDataFormatter handles conversion between a property of T and string
        //

        template< class T >
        class MultiDataFormatter : public TypedDataBinding< T >
        {
        protected:
            std::vector< Reflect::DataPtr > m_Datas;
            const Reflect::Field* m_Field;

        public:
            MultiDataFormatter( const std::vector< Reflect::DataPtr >& datas, const Reflect::Field* field )
                : m_Datas( datas )
                , m_Field( field )
            {

            }

            virtual ~MultiDataFormatter()
            {

            }

            const Reflect::Field* GetField() const
            {
                return m_Field;
            }

            virtual bool Set(const T& value, const DataChangedSignature::Delegate& emitter = DataChangedSignature::Delegate ()) HELIUM_OVERRIDE
            {
                bool result = false;

                Reflect::DataPtr data = Reflect::AssertCast< Reflect::Data >( Reflect::Data::Create( value ) );
                DataChangingArgs args ( this, data );
                m_Changing.Raise( args );
                if ( !args.m_Veto )
                {
                    T newValue;
                    Data::GetValue< T >( data, newValue );
                    std::vector< Reflect::DataPtr >::iterator itr = m_Datas.begin();
                    std::vector< Reflect::DataPtr >::iterator end = m_Datas.end();
                    for ( ; itr != end; ++itr )
                    {
                        Reflect::Data::SetValue< T >( *itr, newValue );
                        result = true;
                    }

                    m_Changed.RaiseWithEmitter( this, emitter );
                }

                return result;
            }

            virtual bool SetAll(std::vector< T >& values, const DataChangedSignature::Delegate& emitter = DataChangedSignature::Delegate ()) HELIUM_OVERRIDE
            {
                bool result = false;

                if ( values.size() == m_Datas.size() )
                {
                    std::vector< Reflect::DataPtr >::const_iterator itr = values.begin();
                    std::vector< Reflect::DataPtr >::const_iterator end = values.end();
                    for ( size_t index=0; itr != end; ++itr, ++index )
                    {
                        Reflect::DataPtr data = Reflect::AssertCast< Reflect::Data >( Reflect::Data::Create( *itr ) );
                        DataChangingArgs args ( this, data );
                        m_Changing.Raise( args );
                        if ( !args.m_Veto )
                        {
                            T newValue;
                            Data::GetValue< T >( data, newValue );
                            m_Datas[ index ]->SetValue( newValue );
                            result = true;
                        }
                    }

                    m_Changed.RaiseWithEmitter( this, emitter );
                }
                else
                {
                    HELIUM_BREAK();
                }

                return result;
            }

            virtual void Get( T& value ) const
            {
                if ( m_Datas.size() == 1 )
                {
                    Reflect::Data::GetValue< T >( m_Datas.front(), value );
                }
            }

            virtual void GetAll(std::vector< T >& values) const HELIUM_OVERRIDE
            {
                values.resize( m_Datas.size() );
                std::vector< Reflect::DataPtr >::const_iterator itr = m_Datas.begin();
                std::vector< Reflect::DataPtr >::const_iterator end = m_Datas.end();
                for ( size_t index = 0; itr != end; ++itr, ++index )
                {
                    T v;
                    Reflect::Data::GetValue< T >( *itr, v );
                    values[ index ] = v;
                }
            }
        };
    }
}