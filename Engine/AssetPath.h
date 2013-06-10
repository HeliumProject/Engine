#pragma once

#include "Platform/Locks.h"

#include "Foundation/Name.h"
#include "Foundation/ObjectPool.h"
#include "Foundation/ReferenceCounting.h"

#include "Engine/Engine.h"

/// @defgroup objectpathdelims Asset FilePath Delimiter Characters
//@{

/// Package delimiter character.
#define HELIUM_PACKAGE_PATH_CHAR TXT( '/' )
/// Asset delimiter character.
#define HELIUM_OBJECT_PATH_CHAR TXT( ':' )
/// Asset instance index delimiter character.
#define HELIUM_INSTANCE_PATH_CHAR TXT( '*' )

/// Package delimiter character string.
#define HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "/" )
/// Asset delimiter character string.
#define HELIUM_OBJECT_PATH_CHAR_STRING TXT( ":" )
/// Asset instance index delimiter character.
#define HELIUM_INSTANCE_PATH_CHAR_STRING TXT( "*" )

//@}

namespace Helium
{
    class Asset;

    /// Hashed object path name for fast lookups and comparisons.
    class HELIUM_ENGINE_API AssetPath
    {
    public:
        /// Number of object path hash table buckets (prime numbers are recommended).
        static const size_t TABLE_BUCKET_COUNT = 37;
        /// Asset path stack memory heap block size.
        static const size_t STACK_HEAP_BLOCK_SIZE = sizeof( char ) * 8192;
        /// Block size for pool of pending links
        static const size_t PENDING_LINKS_POOL_BLOCK_SIZE = 64;

        /// @name Construction/Destruction
        //@{
        inline AssetPath();
        inline AssetPath( ENullName );
		inline AssetPath( const char* pString );
        //@}

        /// @name FilePath Access
        //@{
        bool Set( const char* pString );
        bool Set( const String& rString );
        bool Set( Name name, bool bPackage, AssetPath parentPath, uint32_t instanceIndex = Invalid< uint32_t >() );

        bool Join( AssetPath rootPath, AssetPath subPath );
        bool Join( AssetPath rootPath, const char* pSubPath );
        bool Join( const char* pRootPath, AssetPath subPath );
        bool Join( const char* pRootPath, const char* pSubPath );

        inline Name GetName() const;
        inline uint32_t GetInstanceIndex() const;
        inline bool IsPackage() const;
        inline AssetPath GetParent() const;

        void ToString( String& rString ) const;
        inline String ToString() const;

        void ToFilePathString( String& rString ) const;
        inline String ToFilePathString() const;

        inline bool IsEmpty() const;
        void Clear();

        inline size_t ComputeHash() const;
        //@}

        /// @name Overloaded Operators
        //@{
        inline bool operator==( AssetPath path ) const;
        inline bool operator!=( AssetPath path ) const;
        //@}

        /// @name Static Initialization
        //@{
        static void Shutdown();
        //@}

        /// @name File Support
        //@{
        static void ConvertStringToFilePath( String& rFilePath, const String& rPackagePath );
        //@}

    private:

        struct PendingLink;

        /// Asset path entry.
        struct Entry
        {
            /// Parent entry.
            Entry* pParent;
            /// Asset name.
            Name name;
            /// Asset instance index.
            uint32_t instanceIndex;
            /// True if the object is a package.
            bool bPackage;
            
            /// Pointer to instance of object
            //class Asset *instance; // NOTE: Hate raw pointers but Asset depends on AssetPath
                                        //       so can't use smart pointer
        };

        /// Asset path hash table bucket.
        class TableBucket
        {
        public:
            /// @name Access
            //@{
            Entry* Find( const Entry& rEntry, size_t& rEntryCount );
            Entry* Add( const Entry& rEntry, size_t previousEntryCount );
            //@}

        private:
            /// Array of entry pointers.
            DynamicArray< Entry* > m_entries;
            /// Read-write lock for synchronizing access.
            ReadWriteLock m_lock;
        };

        /// Asset path entry.
        Entry* m_pEntry;

        /// Asset path hash table.
        static TableBucket* sm_pTable;
        /// Stack-based memory heap for object path entry allocations.
        static StackMemoryHeap<>* sm_pEntryMemoryHeap;
        static ObjectPool<PendingLink> *sm_pPendingLinksPool;

        /// @name Private Utility Functions
        //@{
        void Set( const Name* pNames, const uint32_t* pInstanceIndices, size_t nameCount, size_t packageCount );
        //@}

        /// @name Static Utility Functions
        //@{
        static bool Parse(
            const char* pString, StackMemoryHeap<>& rStackHeap, Name*& rpNames, uint32_t*& rpInstanceIndices,
            size_t& rNameCount, size_t& rPackageCount );

        static Entry* Add( const Entry& rEntry );

        static void EntryToString( const Entry& rEntry, String& rString );
        static void EntryToFilePathString( const Entry& rEntry, String& rString );

        static size_t ComputeEntryStringHash( const Entry& rEntry );
        static bool EntryContentsMatch( const Entry& rEntry0, const Entry& rEntry1 );
        //@}
    };
}

namespace Helium
{
    /// Default AssetPath hash.
    template<>
    class HELIUM_ENGINE_API Hash< Helium::AssetPath >
    {
    public:
        inline size_t operator()( const Helium::AssetPath& rKey ) const;
    };
}

#include "Engine/AssetPath.inl"
