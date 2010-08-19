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
            const T& m_Value;   // always a reference to the attribute's value member variable
            const T& m_Other;   // the new value in Changing(), and the old value in Changed()

            ChangeArgs( const T& value, const T& other )
                : m_Value( value )
                , m_Other( other )
            {

            }
        };

        typedef Helium::Signature< bool, const ChangeArgs& > ChangingSignature;
        typedef Helium::Signature< void, const ChangeArgs& > ChangedSignature;

        Attribute( T& value )
            : m_Value ( value )
        {

        }

        T& Value()
        {
            return m_Value;
        }

        operator T&()
        {
            return Value();
        }

        const T& Get() const
        {
            return m_Value;
        }

        operator const T&() const
        {
            return Get();
        }

        bool Set(const T& value)
        {
            if ( m_Value != value && m_Changing.RaiseWithReturn( ChangeArgs( m_Value, value ) ) )
            {
                T previous = m_Value;
                m_Value = value;
                m_Changed.Raise( ChangeArgs( m_Value, previous ) );
                return true;
            }

            return false;
        }

        T operator=( const T& rhs )
        {
            Set( rhs );
            return m_Value;
        }

        typename ChangingSignature::Event& Changing()
        {
            return m_Changing;
        }

        typename ChangedSignature::Event& Changed()
        {
            return m_Changed;
        }

        void RaiseChanged( const T& previous )
        {
            m_Changed.Raise( ChangeArgs( m_Value, previous ) );
        }

    protected:
        T&                                  m_Value;
        typename ChangingSignature::Event   m_Changing;
        typename ChangedSignature::Event    m_Changed;
    };
}