#pragma once

#include "Application/API.h"
#include "Application/Inspect/Inspect.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Automation/Property.h"
#include "Foundation/Reflect/Serializers.h"
#include "Foundation/Undo/Command.h"

#include <iomanip>

namespace Helium
{
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
        // Serializer support
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
        inline void Extract(tistream& stream, Helium::Void* val)
        {

        }

        template<>
        inline void Insert(tostream& stream, const Helium::Void* val)
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
        typedef Helium::Signature<bool, DataChangingArgs&> DataChangingSignature;

        struct DataChangedArgs
        {
            const Data* m_Data;

            DataChangedArgs( const Data* data )
                : m_Data (data)
            {

            }
        };
        typedef Helium::Signature<void, const DataChangedArgs&> DataChangedSignature;

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

        class Data : public Helium::RefCountBase< Data >
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

        typedef Helium::SmartPtr<Data> DataPtr;

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
            typedef Helium::SmartPtr< DataTemplate > Ptr;

        public:
            virtual void Refresh() HELIUM_OVERRIDE
            {
                T temp;
                Get( temp );
                Set( temp );
            }

            virtual Undo::CommandPtr GetUndoCommand() const HELIUM_OVERRIDE
            {
                return new DataCommand<T>( this );
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

        class StringData : public DataTemplate< tstring >
        {
        public:
            INSPECT_TYPE( DataTypes::String );
        };
        typedef Helium::SmartPtr< StringData > StringDataPtr;

        //
        // StringFormatter handles conversion between T and string
        //

        template<class T>
        class StringFormatter : public StringData
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

                Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create< tstring >( s ) );
                if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
                {
                    tstring newValue;
                    Reflect::Serializer::GetValue< tstring >( serializer, newValue );
                    Extract< T >( tstringstream ( newValue ), m_Data );
                    m_Changed.Raise( this, emitter );
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
        class MultiStringFormatter : public StringData
        {
        public:
            typedef Helium::SmartPtr< MultiStringFormatter<T> > Ptr;

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

            virtual bool Set(const tstring& s, const DataChangedSignature::Delegate& emitter = NULL) HELIUM_OVERRIDE
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

            virtual bool SetAll(const std::vector< tstring >& values, const DataChangedSignature::Delegate& emitter = NULL) HELIUM_OVERRIDE
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
        class PropertyStringFormatter : public StringData
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

                Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( s ) );
                if ( m_Changing.RaiseWithReturn( DataChangingArgs ( this, serializer ) ) )
                {
                    T value;
                    tstring newValue;
                    Reflect::Serializer::GetValue< tstring >( serializer, newValue );
                    Extract< T >( tstringstream( newValue ), &value );
                    m_Property->Set( value );
                    m_Changed.Raise( this, emitter );
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
        class MultiPropertyStringFormatter : public StringData
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

                Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create< tstring >( s ) );
                if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
                {
                    tstring newValue;
                    Reflect::Serializer::GetValue< tstring >( serializer, newValue );
                    T value;
                    std::vector< Helium::SmartPtr< Helium::Property<T> > >::iterator itr = m_Properties.begin();
                    std::vector< Helium::SmartPtr< Helium::Property<T> > >::iterator end = m_Properties.end();
                    for ( ; itr != end; ++itr )
                    {
                        Extract<T>( tstringstream ( newValue ), &value );
                        (*itr)->Set( value );
                        result = true;
                    }

                    m_Changed.Raise( this, emitter );
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
                        Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create( *itr ) );
                        if ( m_Changing.RaiseWithReturn( DataChangingArgs( this, serializer ) ) )
                        {
                            T value;
                            tstring newValue;
                            Reflect::Serializer::GetValue< tstring >( serializer, newValue );
                            Extract<T>( tstringstream ( newValue ), &value );
                            m_Properties[ index ]->Set(value);
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
}