#pragma once

#include "Foundation/API.h"

namespace Helium
{
    /// Heterogeneous pair of values with an immutable first value.  This is typically useful for key-value pairs for
    /// associative containers (i.e. Map), where the key should never be allowed to be changed.
    template< typename T1, typename T2 >
    class KeyValue
    {
    public:
        /// Type of the first pair value.
        typedef T1 FirstType;
        /// Type of the second pair value.
        typedef T2 SecondType;

        /// @name Construction/Destruction
        //@{
        KeyValue();
        KeyValue( const T1& rFirst, const T2& rSecond );
        //@}

        /// @name Data Access
        //@{
        const T1& First() const;

        T2& Second();
        const T2& Second() const;
        //@}

        /// @name Overloaded Operators
        //@{
        bool operator<( const KeyValue& rOther ) const;
        bool operator==( const KeyValue& rOther ) const;
        //@}

    protected:
        /// First value.
        T1 m_first;
        /// Second value.
        T2 m_second;

        /// @name Overloaded Operators, Protected
        //@{
        KeyValue& operator=( const KeyValue& );  // No implementation; no assignment allowed for KeyValue objects.
        //@}
    };

    /// Heterogeneous pair of values.  Functionality is essentially analogous to std::pair.
    template< typename T1, typename T2 >
    class Pair : public KeyValue< T1, T2 >
    {
    public:
        /// @name Construction/Destruction
        //@{
        Pair();
        Pair( const T1& rFirst, const T2& rSecond );
        Pair( const KeyValue< T1, T2 >& rKeyValue );
        //@}

        /// @name Data Access
        //@{
        T1& First();
        //@}

        /// @name Overloaded Operators
        //@{
        Pair& operator=( const Pair< T1, T2 >& rOther );
        Pair& operator=( const KeyValue< T1, T2 >& rOther );
        //@}
    };
}

#include "Foundation/Container/Pair.inl"
