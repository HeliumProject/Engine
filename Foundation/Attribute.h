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
            ChangeArgs( const T& oldValue, const T& newValue );

            const T& m_OldValue;
            const T& m_NewValue;
        };

        struct ChangingArgs : public ChangeArgs
        {
            ChangingArgs( const T& oldValue, const T& newValue );

            mutable bool m_Veto;
        };

        typedef Helium::Signature< const ChangingArgs& >  ChangingSignature;
        typedef Helium::Signature< const ChangeArgs& >    ChangedSignature;

        Attribute();
        Attribute( const T& value );

        T& Value();

        const T& Get() const;
        bool Set(const T& value);

        typename ChangingSignature::Event& Changing();
        typename ChangedSignature::Event& Changed();

        void RaiseChanged( const T& previous = T() );

    protected:
        T                                   m_Value;
        typename ChangingSignature::Event   m_Changing;
        typename ChangedSignature::Event    m_Changed;
    };
}

#include "Foundation/Attribute.inl"