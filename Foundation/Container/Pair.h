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

    /// Function class for extracting the first value from a heterogeneous pair.
    template< typename PairType >
    class SelectFirst
    {
    public:
        /// @name Overloaded Operators
        //@{
        typename PairType::FirstType& operator()( PairType& rPair ) const;
        const typename PairType::FirstType& operator()( const PairType& rPair ) const;
        //@}
    };

    /// Function class for extracting the second value from a heterogeneous pair.
    template< typename PairType >
    class SelectSecond
    {
    public:
        /// @name Overloaded Operators
        //@{
        typename PairType::SecondType& operator()( PairType& rPair ) const;
        const typename PairType::SecondType& operator()( const PairType& rPair ) const;
        //@}
    };

    /// Function class for extracting the first value from a KeyValue pair.
    ///
    /// Using SelectFirst with a KeyValue type as its template parameter causes compiler errors due to the fact it does
    /// not implement a First() that returns a non-constant reference (the compiler still tries to compile the operator
    /// overload that takes a non-constant PairType reference and returns a non-constant PairType::FirstType reference
    /// even though we don't actually want to use it).
    template< typename PairType >
    class SelectKey
    {
    public:
        /// @name Overloaded Operators
        //@{
        typename const PairType::FirstType& operator()( const PairType& rPair ) const;
        //@}
    };
}

#include "Foundation/Container/Pair.inl"
