#pragma once

#include "Platform/Locks.h"

#include "Foundation/Name.h"
#include "Foundation/ObjectPool.h"
#include "Foundation/ReferenceCounting.h"

#include "Engine/Engine.h"

/// @defgroup objectpathdelims GameObject FilePath Delimiter Characters
//@{

/// Package delimiter character.
#define HELIUM_PACKAGE_PATH_CHAR TXT( '/' )
/// GameObject delimiter character.
#define HELIUM_OBJECT_PATH_CHAR TXT( ':' )
/// GameObject instance index delimiter character.
#define HELIUM_INSTANCE_PATH_CHAR TXT( '*' )

/// Package delimiter character string.
#define HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "/" )
/// GameObject delimiter character string.
#define HELIUM_OBJECT_PATH_CHAR_STRING TXT( ":" )
/// GameObject instance index delimiter character.
#define HELIUM_INSTANCE_PATH_CHAR_STRING TXT( "*" )

//@}

namespace Helium
{
    class GameObject;

    /// Hashed object path name for fast lookups and comparisons.
    class HELIUM_ENGINE_API GameObjectPath
    {
    public:
        /// Number of object path hash table buckets (prime numbers are recommended).
        static const size_t TABLE_BUCKET_COUNT = 37;
        /// GameObject path stack memory heap block size.
        static const size_t STACK_HEAP_BLOCK_SIZE = sizeof( tchar_t ) * 8192;
        /// Block size for pool of pending links
        static const size_t PENDING_LINKS_POOL_BLOCK_SIZE = 64;

        /// @name Construction/Destruction
        //@{
        inline GameObjectPath();
        inline GameObjectPath( ENullName );
        //@}

        /// @name FilePath Access
        //@{
        bool Set( const tchar_t* pString );
        bool Set( const String& rString );
        bool Set( Name name, bool bPackage, GameObjectPath parentPath, uint32_t instanceIndex = Invalid< uint32_t >() );

        bool Join( GameObjectPath rootPath, GameObjectPath subPath );
        bool Join( GameObjectPath rootPath, const tchar_t* pSubPath );
        bool Join( const tchar_t* pRootPath, GameObjectPath subPath );
        bool Join( const tchar_t* pRootPath, const tchar_t* pSubPath );

        inline Name GetName() const;
        inline uint32_t GetInstanceIndex() const;
        inline bool IsPackage() const;
        inline GameObjectPath GetParent() const;

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
        inline bool operator==( GameObjectPath path ) const;
        inline bool operator!=( GameObjectPath path ) const;
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

        /// GameObject path entry.
        struct Entry
        {
            /// Parent entry.
            Entry* pParent;
            /// GameObject name.
            Name name;
            /// GameObject instance index.
            uint32_t instanceIndex;
            /// True if the object is a package.
            bool bPackage;
            
            /// Pointer to instance of object
            class GameObject *instance; // NOTE: Hate raw pointers but GameObject depends on GameObjectPath
                                        //       so can't use smart pointer

            /// Links other objects have placed on this object
            /// NOTE: For thread safety, this is a weird variables
            /// - If 0, instance is null and any AddPendingLink calls will insert into this linked list
            /// - If value of this pointer == this Entry's pointer, then instance is good and AddPendingLink calls
            ///   should immediately link the value of instance and return true
            /// - If value of this pointer != 0 and != the outer Entry's pointer, then instances is not set and
            ///   AddPendingLink will continue to insert into this linked list
            PendingLink * volatile rpFirstPendingLink;
        };

        /// GameObject path hash table bucket.
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

        /// GameObject path entry.
        Entry* m_pEntry;

        /// GameObject path hash table.
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
            const tchar_t* pString, StackMemoryHeap<>& rStackHeap, Name*& rpNames, uint32_t*& rpInstanceIndices,
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
    /// Default GameObjectPath hash.
    template<>
    class HELIUM_ENGINE_API Hash< Helium::GameObjectPath >
    {
    public:
        inline size_t operator()( const Helium::GameObjectPath& rKey ) const;
    };
}

#include "Engine/GameObjectPath.inl"
