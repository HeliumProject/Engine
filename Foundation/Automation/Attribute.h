#pragma once

#include "Event.h"

namespace Helium
{
    template< class T >
    class Attribute
    {
    public:
        struct ChangeArgs
        {
            ChangeArgs( const T& oldValue, const T& newValue )
                : m_OldValue( oldValue )
                , m_NewValue( newValue )
            {

            }

            const T& m_OldValue;
            const T& m_NewValue;
        };

        struct ChangingArgs : public ChangeArgs
        {
            ChangingArgs( const T& oldValue, const T& newValue )
                : ChangeArgs( oldValue, newValue )
                , m_Veto( false )
            {

            }

            mutable bool m_Veto;
        };

        typedef Helium::Signature< const ChangingArgs& >  ChangingSignature;
        typedef Helium::Signature< const ChangeArgs& >    ChangedSignature;

        Attribute()
            : m_Value ()
        {

        }

        Attribute( const T& value )
            : m_Value ( value )
        {

        }

        T& Value()
        {
            return m_Value;
        }

        const T& Get() const
        {
            return m_Value;
        }

        bool Set(const T& value)
        {
            if ( m_Value != value )
            {
                ChangingArgs args ( m_Value, value );
                m_Changing.Raise( args );
                if ( !args.m_Veto )
                {
                    T previous = m_Value;
                    m_Value = value;
                    m_Changed.Raise( ChangeArgs( m_Value, previous ) );
                    return true;
                }
            }

            return false;
        }

        typename ChangingSignature::Event& Changing()
        {
            return m_Changing;
        }

        typename ChangedSignature::Event& Changed()
        {
            return m_Changed;
        }

        void RaiseChanged( const T& previous = T() )
        {
            m_Changed.Raise( ChangeArgs( previous, m_Value ) );
        }

    protected:
        T                                   m_Value;
        typename ChangingSignature::Event   m_Changing;
        typename ChangedSignature::Event    m_Changed;
    };
}