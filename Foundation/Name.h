#pragma once

#include "Foundation/API.h"

#include "Platform/Trace.h"
#include "Platform/ReadWriteLock.h"
#include "Foundation/String.h"
#include "Foundation/HashFunctions.h"

namespace Helium
{
    /// Null name constant.
    enum ENullName
    {
        NULL_NAME
    };

    /// Base support for string table entry types.
    template< typename TableType >
    class NameBase
    {
    public:
        /// Character type.
        typedef typename TableType::CharType CharType;

        /// Number of name hash table buckets (prime numbers are recommended).
        static const size_t TABLE_BUCKET_COUNT = 37;
        /// Name stack memory heap block size.
        static const size_t STACK_HEAP_BLOCK_SIZE = sizeof( CharType ) * 8192;

        /// Name hash table bucket.
        class TableBucket
        {
        public:
            /// @name Access
            //@{
            const CharType* Find( const CharType* pString, size_t& rEntryCount );
            const CharType* Add( const CharType* pString, size_t previousEntryCount );
            //@}

        private:
            /// Array of name pointers.
            DynArray< const CharType* > m_entries;
            /// Read-write lock for synchronizing access.
            ReadWriteLock m_lock;
        };

        /// @name Construction/Destruction
        //@{
        NameBase();
        NameBase( ENullName );
        explicit NameBase( const CharType* pString );
        explicit NameBase( const StringBase< CharType >& rString );
        //@}

        /// @name Name Access
        //@{
        const CharType* Get() const;
        const CharType* GetDirect() const;
        void Set( const CharType* pString );
        void Set( const StringBase< CharType >& rString );

        bool IsEmpty() const;
        void Clear();
        //@}

        /// @name Overloaded Operators
        //@{
        const CharType* operator*() const;

        bool operator<( const NameBase& rName ) const;
        bool operator>( const NameBase& rName ) const;
        bool operator<=( const NameBase& rName ) const;
        bool operator>=( const NameBase& rName ) const;
        bool operator==( const NameBase& rName ) const;
        bool operator!=( const NameBase& rName ) const;
        //@}

        /// @name Static Initialization
        //@{
        static void Shutdown();
        //@}

    private:
        /// Name entry.
        const CharType* m_pEntry;
    };

    /// CharString name table.
    class HELIUM_FOUNDATION_API CharNameTable
    {
        friend class NameBase< CharNameTable >;

    public:
        /// Character type.
        typedef char CharType;

    private:
        /// Name hash table.
        static NameBase< CharNameTable >::TableBucket* sm_pTable;
        /// Stack-based memory heap for name entry allocations.
        static StackMemoryHeap<>* sm_pNameMemoryHeap;
        /// Empty name string.
        static char sm_emptyString[ 1 ];
    };

    /// WideString name table.
    class HELIUM_FOUNDATION_API WideNameTable
    {
        friend class NameBase< WideNameTable >;

    public:
        /// Character type.
        typedef wchar_t CharType;

    private:
        /// Name hash table.
        static NameBase< WideNameTable >::TableBucket* sm_pTable;
        /// Stack-based memory heap for name entry allocations.
        static StackMemoryHeap<>* sm_pNameMemoryHeap;
        /// Empty name string.
        static wchar_t sm_emptyString[ 1 ];
    };

    /// CharString name table entry.
    typedef NameBase< CharNameTable > CharName;
    /// WideString name table entry.
    typedef NameBase< WideNameTable > WideName;

    /// String table entry.
#if HELIUM_WCHAR_T
    typedef WideName Name;
#else
    typedef CharName Name;
#endif
  
    HELIUM_FOUNDATION_API inline tostream& operator<<( tostream& stream, const Name& id )
    {
        stream << id.Get();
        return stream;
    }

    HELIUM_FOUNDATION_API inline tistream& operator>>( tistream& stream, Name& id )
    {
        tstring str;
        stream >> str;
        id.Set(str.c_str());
        return stream;
    }

    /// Default Name hash.
    template< typename TableType >
    class Hash< NameBase< TableType > >
    {
    public:
        inline size_t operator()( const NameBase< TableType >& rKey ) const;
    };

    /// @defgroup nameiostream std::iostream Name Support
    //@{

    template< typename CharType, typename CharTypeTraits, typename NameTableType >
    std::basic_ostream< CharType, CharTypeTraits >& operator<<(
        std::basic_ostream< CharType, CharTypeTraits >& rStream, const NameBase< NameTableType >& rName );

    //@}
}

#include "Foundation/Name.inl"
