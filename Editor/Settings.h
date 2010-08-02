#pragma once

#include "Editor/API.h"

#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Version.h"

namespace Helium
{
    namespace Editor
    {
        class EDITOR_EDITOR_API MRUData : public Reflect::ConcreteInheritor< MRUData, Reflect::Element >
        {
        public:
            const std::vector< tstring >& GetPaths() const
            {
                return m_Paths;
            }

            void SetPaths( const std::vector< tstring >& paths )
            {
                if ( paths != m_Paths )
                {
                    m_Paths = paths;
                    RaiseChanged( GetClass()->FindField( &MRUData::m_Paths ) );
                }
            }

        private:
            std::vector< tstring > m_Paths;

        public:
            static void EnumerateClass( Reflect::Compositor<MRUData>& comp )
            {
                comp.AddField( &MRUData::m_Paths, "m_Paths" );
            }
        };
        typedef Helium::SmartPtr< MRUData > MRUDataPtr;

        struct PreferencesLoadedArgs {};
        typedef Helium::Signature< void, const PreferencesLoadedArgs& > PreferencesLoadedSignature;

        class EDITOR_EDITOR_API Settings HELIUM_ABSTRACT : public Reflect::AbstractInheritor< Settings, Reflect::Element >
        {
        public:
            virtual void PreSerialize() HELIUM_OVERRIDE;
            virtual tstring GetCurrentVersion() const;

            virtual bool LoadFromFile( const Helium::Path& path );
            virtual bool SaveToFile( const Helium::Path& path, tstring& error, Reflect::VersionPtr version = new Reflect::Version() );

            // Gets the value of the specified field.
            template< class T >
            bool Get( const Reflect::Field* field, T& value ) const
            {
                Reflect::ElementPtr thisPtr = this;
                Reflect::SerializerPtr serializer = field->CreateSerializer( thisPtr );
                return Reflect::Serializer::GetValue( serializer, value );
            }

            // Sets the value of the specified field.
            template< class T >
            bool Set( const Reflect::Field* field, const T& value )
            {
                Reflect::SerializerPtr serializer = field->CreateSerializer( this );
                if ( serializer.ReferencesObject() )
                {
                    T oldValue;
                    if ( Get< T >( field, oldValue ) )
                    {
                        if ( oldValue == value )
                        {
                            return true;
                        }
                        else
                        {
                            return Reflect::Serializer::SetValue( serializer, value );
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                return false;
            }

            // Sets the value of the specified field: specialization for Reflect::Element
            // Untested!
            template<>
            bool Set( const Reflect::Field* field, const Reflect::ElementPtr& value )
            {
                Reflect::SerializerPtr serializer = field->CreateSerializer( this );
                if ( serializer.ReferencesObject() )
                {
                    Reflect::ElementPtr oldValue;
                    if ( Get< Reflect::ElementPtr >( field, oldValue ) )
                    {
                        if ( oldValue->Equals( value ) )
                        {
                            return true;
                        }
                        else
                        {
                            return Reflect::Serializer::SetValue( serializer, value );
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                return false;
            }

            // Enumerations are special and need their own getter
            template< class T >
            bool GetEnum( const Reflect::Field* field, T& value )
            {
                Reflect::ElementPtr thisPtr = this;
                Reflect::EnumerationSerializerPtr serializer = Reflect::TryCast< Reflect::EnumerationSerializer >( field->CreateSerializer( thisPtr ) );
                if ( serializer->m_Enumeration )
                {
                    value = static_cast< T >( serializer->m_Data.Val() );
                    return true;
                }
                return false;
            }

            // Enumerations are special and need their own setter
            template< class T >
            bool SetEnum( const Reflect::Field* field, const T& value )
            {
                Reflect::ElementPtr thisPtr = this;
                Reflect::EnumerationSerializerPtr serializer = Reflect::TryCast< Reflect::EnumerationSerializer >( field->CreateSerializer( thisPtr ) );
                if ( serializer->m_Enumeration )
                {
                    if ( static_cast< T >( serializer->m_Data.Val() ) != value )
                    {
                        serializer->m_Data.Set( value );
                        RaiseChanged( field );
                    }
                    return true;
                }
                return false;
            }

            // Listeners
        private:
            mutable PreferencesLoadedSignature::Event m_Loaded;
        public:
            void AddPreferencesLoadedListener( const PreferencesLoadedSignature::Delegate& listener ) const
            {
                m_Loaded.Add( listener );
            }
            void RemovePreferencesLoadedListener( const PreferencesLoadedSignature::Delegate& listener ) const
            {
                m_Loaded.Remove( listener );
            }

        private:
            tstring m_SavedVersion;

        public:
            static void EnumerateClass( Reflect::Compositor<Settings>& comp )
            {
                comp.AddField( &Settings::m_SavedVersion, "m_SavedVersion", Reflect::FieldFlags::Force | Reflect::FieldFlags::Hide );
            }
        };
        typedef Helium::SmartPtr< Settings > SettingsPtr;
    }
}