#include "EnginePch.h"
#include "Engine/Cache.h"

#include "Foundation/FileStream.h"
#include "Foundation/MemoryStream.h"
#include "Foundation/StringConverter.h"

#include "Engine/Asset.h"
#include "Engine/FileLocations.h"
#include "Engine/AsyncLoader.h"

#define USE_BSON_FOR_CACHE_FORMAT 0
#define USE_JSON_FOR_CACHE_FORMAT 1

#if USE_BSON_FOR_CACHE_FORMAT
#include "Persist/ArchiveBson.h"
typedef Helium::Persist::ArchiveWriterBson CacheArchiveWriter;
typedef Helium::Persist::ArchiveReaderBson CacheArchiveReader;
#elif USE_JSON_FOR_CACHE_FORMAT
#include "Persist/ArchiveJson.h"
typedef Helium::Persist::ArchiveWriterJson CacheArchiveWriter;
typedef Helium::Persist::ArchiveReaderJson CacheArchiveReader;
#else
#include "Persist/ArchiveMessagePack.h"
typedef Helium::Persist::ArchiveWriterMessagePack CacheArchiveWriter;
typedef Helium::Persist::ArchiveReaderMessagePack CacheArchiveReader;
#endif

using namespace Helium;

/// TOC header magic number.
static const uint32_t TOC_MAGIC = 0xcac4e70c;
/// TOC header magic number (byte-swapped).
static const uint32_t TOC_MAGIC_SWAPPED = 0x0ce7c4ca;
/// Cache format version number.
const uint32_t Cache::sm_Version = 0;

/// Constructor.
Cache::Cache()
: m_name( NULL_NAME )
, m_platform( PLATFORM_INVALID )
, m_bTocLoaded( false )
, m_asyncLoadId( Invalid< size_t >() )
, m_pTocBuffer( NULL )
, m_tocSize( Invalid< uint32_t >() )
, m_pEntryPool( NULL )
{
}

/// Destructor.
Cache::~Cache()
{
	Shutdown();
}

/// Initialize this cache.
///
/// This will verify the existence of the given files and prepare for cache loading.  Note that no file loading is
/// performed at this time.
///
/// Once initialization is performed, the table of contents must be loaded using BeginLoadToc().
///
/// @param[in] name            Name identifying this cache.
/// @param[in] platform        Cache platform identifier.
/// @param[in] pTocFileName    FilePath name of the table of contents file.
/// @param[in] pCacheFileName  FilePath name of the cache file.
///
/// @return  True if initialization was successful, false if not.
///
/// @see Shutdown(), BeginLoadToc()
bool Cache::Initialize( Name name, EPlatform platform, const char* pTocFileName, const char* pCacheFileName )
{
	HELIUM_ASSERT( !name.IsEmpty() );
	HELIUM_ASSERT( static_cast< size_t >( platform ) < static_cast< size_t >( PLATFORM_MAX ) );
	HELIUM_ASSERT( pTocFileName );
	HELIUM_ASSERT( pCacheFileName );

	Shutdown();

	m_name = name;
	m_platform = platform;

	Status status;
	status.Read( pTocFileName );
	int64_t tocSize64 = status.m_Size;
	if( tocSize64 != -1 && static_cast< uint64_t >( tocSize64 ) >= UINT32_MAX )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "Cache::Initialize(): TOC file \"%s\" exceeds the maximum allowed size for TOC files (2 GB).\n" ),
			pTocFileName );

		return false;
	}

	m_tocFileName = pTocFileName;
	m_cacheFileName = pCacheFileName;

	m_tocSize = static_cast< uint32_t >( tocSize64 );

	HELIUM_ASSERT( !m_pEntryPool );
	m_pEntryPool = new ObjectPool< Entry >( ENTRY_POOL_BLOCK_SIZE );
	HELIUM_ASSERT( m_pEntryPool );

	return true;
}

/// Shut down this cache and free all allocated memory.
///
/// @see Initialize()
void Cache::Shutdown()
{
	m_name = NULL_NAME;
	m_platform = PLATFORM_INVALID;

	m_tocFileName.Clear();
	m_cacheFileName.Clear();

	if( IsValid( m_asyncLoadId ) )
	{
		AsyncLoader* pAsyncLoader = AsyncLoader::GetInstance();
		HELIUM_ASSERT( pAsyncLoader );

		pAsyncLoader->SyncRequest( m_asyncLoadId );
		SetInvalid( m_asyncLoadId );
	}

	DefaultAllocator().Free( m_pTocBuffer );
	m_pTocBuffer = NULL;
	SetInvalid( m_tocSize );

	m_bTocLoaded = false;

	m_entries.Clear();
	m_entryMap.Clear();

	delete m_pEntryPool;
	m_pEntryPool = NULL;
}

/// Begin asynchronous loading of the cache table of contents.
///
/// This must be called after calling Initialize() in order to begin using an existing cache.
///
/// @return  True if loading was started successfully, false if not.
///
/// @see TryFinishLoadToc(), IsTocLoaded(), Initialize()
bool Cache::BeginLoadToc()
{
	if( IsValid( m_asyncLoadId ) )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			TXT( "Cache::BeginLoadToc(): Async load of TOC file \"%s\" already in progress.\n" ),
			*m_tocFileName );

		return true;
	}

	if( m_tocFileName.IsEmpty() )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Cache::BeginLoadToc(): Called without having initialized the cache.\n" ) );

		return false;
	}

	if( IsInvalid( m_tocSize ) )
	{
		HELIUM_TRACE( TraceLevels::Info, TXT( "Cache::BeginLoadToc(): TOC file does not seem to exist.  MOVING ON...\n" ) );

		// Since the TOC doesn't exist, we can consider its loading to be complete.
		m_bTocLoaded = true;

		return false;
	}

	HELIUM_ASSERT( !m_pTocBuffer );
	DefaultAllocator allocator;
	m_pTocBuffer = static_cast< uint8_t* >( allocator.Allocate( m_tocSize ) );
	HELIUM_ASSERT( m_pTocBuffer );
	if( !m_pTocBuffer )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "Cache::BeginLoadToc(): Failed to allocate %" ) PRIu32 TXT( " bytes for TOC file \"%s\".\n" ),
			m_tocSize,
			*m_tocFileName );

		return false;
	}

	AsyncLoader* pAsyncLoader = AsyncLoader::GetInstance();
	HELIUM_ASSERT( pAsyncLoader );

	m_asyncLoadId = pAsyncLoader->QueueRequest( m_pTocBuffer, m_tocFileName, 0, m_tocSize );
	HELIUM_ASSERT( IsValid( m_asyncLoadId ) );
	if( IsInvalid( m_asyncLoadId ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "Cache::BeginLoadToc(): Failed to begin asynchronous load of TOC file \"%s\".\n" ),
			*m_tocFileName );

		allocator.Free( m_pTocBuffer );
		m_pTocBuffer = NULL;

		return false;
	}

	m_bTocLoaded = false;

	return true;
}

/// Test for and finalize asynchronous loading of the cache table of contents in a non-blocking fashion.
///
/// @return  True if loading has completed or is not in progress, false if loading is still in progress.
///
/// @see BeginLoadToc(), IsTocLoaded()
bool Cache::TryFinishLoadToc()
{
	if( IsInvalid( m_asyncLoadId ) )
	{
		HELIUM_TRACE( TraceLevels::Warning, TXT( "Cache::TryFinishLoadToc(): Called without a TOC load in progress.\n" ) );

		return true;
	}

	AsyncLoader* pAsyncLoader = AsyncLoader::GetInstance();
	HELIUM_ASSERT( pAsyncLoader );

	size_t bytesRead = 0;
	if( !pAsyncLoader->TrySyncRequest( m_asyncLoadId, bytesRead ) )
	{
		return false;
	}

	SetInvalid( m_asyncLoadId );

	if( bytesRead == 0 || IsInvalid( bytesRead ) )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			TXT( "Cache::TryFinishLoadToc(): No data loaded from TOC file \"%s\".\n" ),
			*m_tocFileName );

		SetInvalid( m_tocSize );
	}
	else
	{
		if( m_tocSize != bytesRead )
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				( TXT( "Cache::TryFinishLoadToc(): TOC file \"%s\" size (%" ) PRIu32 TXT( ") does not match the " )
				TXT( "number of bytes read (%" ) PRIuSZ TXT( ").\n" ) ),
				m_tocSize,
				bytesRead );

			m_tocSize = static_cast< uint32_t >( bytesRead );
		}

		bool bFinalizeResult = FinalizeTocLoad();

		DefaultAllocator().Free( m_pTocBuffer );
		m_pTocBuffer = NULL;

		if( !bFinalizeResult )
		{
			HELIUM_ASSERT( m_pEntryPool );

			size_t entryCount = m_entries.GetSize();
			for( size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex )
			{
				Entry* pEntry = m_entries[ entryIndex ];
				HELIUM_ASSERT( pEntry );
				m_pEntryPool->Release( pEntry );
			}

			m_entries.Clear();
			m_entryMap.Clear();
		}
	}

	m_bTocLoaded = true;

	return true;
}

/// Enforce that this cache is loaded, block-loading if necessary.
///
/// @todo  Ensure preloading of caches at game runtime.
void Cache::EnforceTocLoad()
{
	if( !IsTocLoaded() )
	{
		HELIUM_TRACE( TraceLevels::Info, TXT( "Cache::EnforceTocLoad(): Block-loading cache \"%s\".\n" ), *m_name );

		if( BeginLoadToc() )
		{
			while( !TryFinishLoadToc() )
			{
				Thread::Yield();
			}
		}
	}
}

/// Search for a cache entry with the given object path name.
///
/// @param[in] path          Asset path.
/// @param[in] subDataIndex  Sub-data index associated with the cached data.
///
/// @return  Pointer to the cache entry for the given object path if found, null pointer if not found.
const Cache::Entry* Cache::FindEntry( AssetPath path, uint32_t subDataIndex ) const
{
	EntryKey key;
	key.path = path;
	key.subDataIndex = subDataIndex;

	EntryMapType::ConstAccessor mapAccessor;
	if( !m_entryMap.Find( mapAccessor, key ) )
	{
		return NULL;
	}

	Entry* pEntry = mapAccessor->Second();
	HELIUM_ASSERT( pEntry );

	return pEntry;
}

/// Add or update an entry in the cache.
///
/// @param[in] path          Asset path.
/// @param[in] subDataIndex  Sub-data index associated with the cached data.
/// @param[in] pData         Data to cache.
/// @param[in] timestamp     Timestamp value to associate with the entry in the cache.
/// @param[in] size          Number of bytes to cache.
///
/// @return  True if the cache was updated successfully, false if not.
bool Cache::CacheEntry(
					   AssetPath path,
					   uint32_t subDataIndex,
					   const void* pData,
					   int64_t timestamp,
					   uint32_t size )
{
	HELIUM_ASSERT( pData || size == 0 );

	Status status;
	status.Read( m_cacheFileName.GetData() );
	int64_t cacheFileSize = status.m_Size;
	uint64_t entryOffset = ( cacheFileSize == -1 ? 0 : static_cast< uint64_t >( cacheFileSize ) );

	HELIUM_ASSERT( m_pEntryPool );
	Entry* pEntryUpdate = m_pEntryPool->Allocate();
	HELIUM_ASSERT( pEntryUpdate );
	pEntryUpdate->offset = entryOffset;
	pEntryUpdate->timestamp = timestamp;
	pEntryUpdate->path = path;
	pEntryUpdate->subDataIndex = subDataIndex;
	pEntryUpdate->size = size;

	uint64_t originalOffset = 0;
	int64_t originalTimestamp = 0;
	uint32_t originalSize = 0;

	EntryKey key;
	key.path = path;
	key.subDataIndex = subDataIndex;

	EntryMapType::Accessor entryAccessor;
	bool bNewEntry = m_entryMap.Insert( entryAccessor, KeyValue< EntryKey, Entry* >( key, pEntryUpdate ) );
	if( bNewEntry )
	{
		HELIUM_TRACE( TraceLevels::Info, TXT( "Cache: Adding \"%s\" to cache \"%s\".\n" ), *path.ToString(), *m_cacheFileName );

		m_entries.Push( pEntryUpdate );
	}
	else
	{
		HELIUM_TRACE( TraceLevels::Info, TXT( "Cache: Updating \"%s\" in cache \"%s\".\n" ), *path.ToString(), *m_cacheFileName );

		m_pEntryPool->Release( pEntryUpdate );

		pEntryUpdate = entryAccessor->Second();
		HELIUM_ASSERT( pEntryUpdate );

		originalOffset = pEntryUpdate->offset;
		originalTimestamp = pEntryUpdate->timestamp;
		originalSize = pEntryUpdate->size;

		if( originalSize < size )
		{
			pEntryUpdate->offset = entryOffset;
		}
		else
		{
			entryOffset = originalOffset;
		}

		pEntryUpdate->timestamp = timestamp;
		pEntryUpdate->size = size;
	}

	AsyncLoader* pAsyncLoader = AsyncLoader::GetInstance();
	HELIUM_ASSERT( pAsyncLoader );

	pAsyncLoader->Lock();

	bool bCacheSuccess = true;

	FileStream* pCacheStream = FileStream::OpenFileStream( m_cacheFileName, FileStream::MODE_WRITE, false );
	if( !pCacheStream )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Cache: Failed to open cache \"%s\" for writing.\n" ), *m_cacheFileName );

		bCacheSuccess = false;
	}
	else
	{
		HELIUM_TRACE(
			TraceLevels::Info,
			TXT( "Cache: Caching \"%s\" to \"%s\" (%" ) PRIu32 TXT( " bytes @ offset %" ) PRIu64 TXT( ").\n" ),
			*path.ToString(),
			*m_cacheFileName,
			size,
			entryOffset );

		uint64_t seekOffset = static_cast< uint64_t >( pCacheStream->Seek(
			static_cast< int64_t >( entryOffset ),
			SeekOrigins::Begin ) );
		if( seekOffset != entryOffset )
		{
			HELIUM_TRACE( TraceLevels::Error, TXT( "Cache: Cache file offset seek failed.\n" ) );

			if( bNewEntry )
			{
				m_entries.Pop();
				m_entryMap.Remove( entryAccessor );
				m_pEntryPool->Release( pEntryUpdate );
			}
			else
			{
				pEntryUpdate->offset = originalOffset;
				pEntryUpdate->timestamp = originalTimestamp;
				pEntryUpdate->size = originalSize;
			}

			bCacheSuccess = false;
		}
		else
		{
			size_t writeSize = pCacheStream->Write( pData, 1, size );
			if( writeSize != size )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					( TXT( "Cache: Failed to write %" ) PRIu32 TXT( " bytes to cache \"%s\" (%" ) PRIuSZ
					TXT( " bytes written).\n" ) ),
					size,
					*m_cacheFileName,
					writeSize );

				if( bNewEntry )
				{
					m_entries.Pop();
					m_entryMap.Remove( entryAccessor );
					m_pEntryPool->Release( pEntryUpdate );
				}
				else
				{
					pEntryUpdate->offset = originalOffset;
					pEntryUpdate->timestamp = originalTimestamp;
					pEntryUpdate->size = originalSize;
				}

				bCacheSuccess = false;
			}
			else
			{
				HELIUM_TRACE( TraceLevels::Info, TXT( "Cache: Rewriting TOC file \"%s\".\n" ), *m_tocFileName );

				FileStream* pTocStream = FileStream::OpenFileStream( m_tocFileName, FileStream::MODE_WRITE, true );
				if( !pTocStream )
				{
					HELIUM_TRACE( TraceLevels::Error, TXT( "Cache: Failed to open TOC \"%s\" for writing.\n" ), *m_tocFileName );
				}
				else
				{
					BufferedStream* pBufferedStream = new BufferedStream( pTocStream );
					HELIUM_ASSERT( pBufferedStream );

					pBufferedStream->Write( &TOC_MAGIC, sizeof( TOC_MAGIC ), 1 );
					pBufferedStream->Write( &sm_Version, sizeof( sm_Version ), 1 );

					uint32_t entryCount = static_cast< uint32_t >( m_entries.GetSize() );
					pBufferedStream->Write( &entryCount, sizeof( entryCount ), 1 );

					String entryPath;
					uint_fast32_t entryCountFast = entryCount;
					for( uint_fast32_t entryIndex = 0; entryIndex < entryCountFast; ++entryIndex )
					{
						Entry* pEntry = m_entries[ entryIndex ];
						HELIUM_ASSERT( pEntry );

						pEntry->path.ToString( entryPath );
						HELIUM_ASSERT( entryPath.GetSize() < UINT16_MAX );
						uint16_t pathSize = static_cast< uint16_t >( entryPath.GetSize() );
						pBufferedStream->Write( &pathSize, sizeof( pathSize ), 1 );

						pBufferedStream->Write( *entryPath, sizeof( char ), pathSize );

						pBufferedStream->Write( &pEntry->subDataIndex, sizeof( pEntry->subDataIndex ), 1 );

						pBufferedStream->Write( &pEntry->offset, sizeof( pEntry->offset ), 1 );
						pBufferedStream->Write( &pEntry->timestamp, sizeof( pEntry->timestamp ), 1 );
						pBufferedStream->Write( &pEntry->size, sizeof( pEntry->size ), 1 );
					}

					delete pBufferedStream;
					delete pTocStream;
				}
			}
		}

		delete pCacheStream;
	}

	pAsyncLoader->Unlock();

	return bCacheSuccess;
}

/// Finalize the TOC loading process.
///
/// Note that this does not free any resources on a failed load (the caller is responsible for such clean-up work).
///
/// @return  True if the TOC load was successful, false if not.
bool Cache::FinalizeTocLoad()
{
	HELIUM_ASSERT( m_pTocBuffer );

	const uint8_t* pTocCurrent = m_pTocBuffer;
	const uint8_t* pTocMax = pTocCurrent + m_tocSize;

	StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();

	// Validate the TOC header.
	uint32_t magic;
	if( !CheckedTocRead( MemoryCopy, magic, TXT( "the header magic" ), pTocCurrent, pTocMax ) )
	{
		return false;
	}

	LOAD_VALUE_CALLBACK* pLoadFunction = NULL;
	if( magic == TOC_MAGIC )
	{
		HELIUM_TRACE(
			TraceLevels::Info,
			TXT( "Cache::FinalizeTocLoad(): TOC \"%s\" identified (no byte swapping).\n" ),
			*m_tocFileName );

		pLoadFunction = MemoryCopy;
	}
	else if( magic == TOC_MAGIC_SWAPPED )
	{
		HELIUM_TRACE(
			TraceLevels::Info,
			TXT( "Cache::FinalizeTocLoad(): TOC \"%s\" identified (byte swapping is necessary).\n" ),
			*m_tocFileName );
		HELIUM_TRACE(
			TraceLevels::Warning,
			( TXT( "Cache::TryFinishLoadToc(): Cache for TOC \"%s\" uses byte swapping, which may incur " )
			TXT( "performance penalties.\n" ) ),
			*m_tocFileName );

		pLoadFunction = ReverseByteOrder;
	}
	else
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "Cache::FinalizeTocLoad(): TOC \"%s\" has invalid file magic.\n" ),
			*m_tocFileName );

		return false;
	}

	uint32_t version;
	if( !CheckedTocRead( pLoadFunction, version, TXT( "the cache version number" ), pTocCurrent, pTocMax ) )
	{
		return false;
	}

	if( version > sm_Version )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			( TXT( "Cache::FinalizeTocLoad(): Cache version number (%" ) PRIu32 TXT( ") exceeds the maximum " )
			TXT( "supported version (%" ) PRIu32 TXT( ").\n" ) ),
			version,
			sm_Version );

		return false;
	}

	// Read the numbers of entries in the cache.
	uint32_t entryCount;
	bool bReadResult = CheckedTocRead(
		pLoadFunction,
		entryCount,
		TXT( "the number of entries in the cache" ),
		pTocCurrent,
		pTocMax );
	if( !bReadResult )
	{
		return false;
	}

	// Load the entry information.
	EntryKey key;

	uint_fast32_t entryCountFast = entryCount;
	m_entries.Reserve( entryCountFast );
	for( uint_fast32_t entryIndex = 0; entryIndex < entryCountFast; ++entryIndex )
	{
		uint16_t entryPathSize;
		bReadResult = CheckedTocRead(
			pLoadFunction,
			entryPathSize,
			TXT( "entry AssetPath string size" ),
			pTocCurrent,
			pTocMax );
		if( !bReadResult )
		{
			return false;
		}

		uint_fast16_t entryPathSizeFast = entryPathSize;

		StackMemoryHeap<>::Marker stackMarker( rStackHeap );
		char* pPathString = static_cast< char* >( rStackHeap.Allocate(
			sizeof( char ) * ( entryPathSizeFast + 1 ) ) );
		HELIUM_ASSERT( pPathString );
		pPathString[ entryPathSizeFast ] = TXT( '\0' );

		for( uint_fast16_t characterIndex = 0; characterIndex < entryPathSizeFast; ++characterIndex )
		{
			bReadResult = CheckedTocRead(
				pLoadFunction,
				pPathString[ characterIndex ],
				TXT( "entry AssetPath string character" ),
				pTocCurrent,
				pTocMax );
			if( !bReadResult )
			{
				return false;
			}
		}

		AssetPath entryPath;
		if( !entryPath.Set( pPathString ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "Cache::FinalizeTocLoad(): Failed to set AssetPath for entry %" ) PRIuFAST16 TXT( ".\n" ),
				entryIndex );

			return false;
		}

		uint32_t entrySubDataIndex;
		bReadResult = CheckedTocRead(
			pLoadFunction,
			entrySubDataIndex,
			TXT( "entry sub-data index" ),
			pTocCurrent,
			pTocMax );
		if( !bReadResult )
		{
			return false;
		}

		key.path = entryPath;
		key.subDataIndex = entrySubDataIndex;

		EntryMapType::ConstAccessor entryAccessor;
		if( m_entryMap.Find( entryAccessor, key ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "Cache::FinalizeTocLoad(): Duplicate entry found for AssetPath \"%s\", sub-data %" ) PRIu32
				TXT( ".\n" ) ),
				pPathString,
				entrySubDataIndex );

			return false;
		}

		uint64_t entryOffset;
		if( !CheckedTocRead( pLoadFunction, entryOffset, TXT( "entry offset" ), pTocCurrent, pTocMax ) )
		{
			return false;
		}

		int64_t entryTimestamp;
		if( !CheckedTocRead( pLoadFunction, entryTimestamp, TXT( "entry timestamp" ), pTocCurrent, pTocMax ) )
		{
			return false;
		}

		uint32_t entrySize;
		if( !CheckedTocRead( pLoadFunction, entrySize, TXT( "entry size" ), pTocCurrent, pTocMax ) )
		{
			return false;
		}

		Entry* pEntry = m_pEntryPool->Allocate();
		HELIUM_ASSERT( pEntry );
		pEntry->path = entryPath;
		pEntry->subDataIndex = entrySubDataIndex;
		pEntry->offset = entryOffset;
		pEntry->timestamp = entryTimestamp;
		pEntry->size = entrySize;

		m_entries.Add( pEntry );

		HELIUM_VERIFY( m_entryMap.Insert( entryAccessor, KeyValue< EntryKey, Entry* >( key, pEntry ) ) );
	}

	return true;
}

/// Read a value from the cache TOC, check the TOC bounds in the process.
///
/// @param[in]  pLoadFunction  Function to use for reading the value.
/// @param[out] rValue         Read value.
/// @param[in]  pDescription   Description of the value to read (for logging).
/// @param[in]  rpTocCurrent   Pointer to the current offset within the TOC file buffer.
/// @param[in]  pTocMax        Pointer to the end of the TOC file buffer.
///
/// @return  True if the value was read successfully, false if not.
template< typename T >
bool Cache::CheckedTocRead(
						   LOAD_VALUE_CALLBACK* pLoadFunction,
						   T& rValue,
						   const char* pDescription,
						   const uint8_t*& rpTocCurrent,
						   const uint8_t* pTocMax )
{
	HELIUM_UNREF( pDescription );  // Used for logging only (unused in release builds).

	HELIUM_ASSERT( pLoadFunction );
	HELIUM_ASSERT( pDescription );
	HELIUM_ASSERT( rpTocCurrent );
	HELIUM_ASSERT( pTocMax );

	if( rpTocCurrent + sizeof( rValue ) > pTocMax )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "Cache::TryFinishLoadToc(): Not enough bytes in the TOC file for %s.\n" ),
			pDescription );

		return false;
	}

	pLoadFunction( &rValue, rpTocCurrent, sizeof( rValue ) );
	rpTocCurrent += sizeof( rValue );

	return true;
}

/// Equality comparison.
///
/// @param[in] rOther  Entry key with which to compare.
///
/// @return  True if this key is equal to the given key, false if not.
bool Cache::EntryKey::operator==( const EntryKey& rOther ) const
{
	return ( path == rOther.path && subDataIndex == rOther.subDataIndex );
}

/// Compute a hash value for the given entry key.
///
/// @param[in] rKey  Cache entry key.
///
/// @return  Hash value for the given key.
size_t Cache::EntryKeyHash::operator()( const EntryKey& rKey ) const
{
	size_t hash = rKey.path.ComputeHash();

	hash = ( ( rKey.subDataIndex * 33 ) ^ hash );

	return hash;
}

#if HELIUM_TOOLS
void Helium::Cache::WriteCacheObjectToBuffer( Reflect::Object* _object, DynamicArray< uint8_t > &_buffer )
{
	AssetIdentifier identifier;

	DynamicMemoryStream archiveStream ( &_buffer );
	CacheArchiveWriter::WriteToStream( _object, archiveStream, &identifier );
}
#endif

Reflect::ObjectPtr Helium::Cache::ReadCacheObjectFromBuffer( const DynamicArray< uint8_t > &_buffer, Reflect::ObjectResolver *_resolver )
{
	if (_buffer.GetSize() == 0)
	{
		Reflect::ObjectPtr null_object;
		return null_object;
	}

	return ReadCacheObjectFromBuffer(_buffer.GetData(), 0, _buffer.GetSize(), _resolver);
}

Reflect::ObjectPtr Helium::Cache::ReadCacheObjectFromBuffer( const uint8_t *_buffer, const size_t _offset, const size_t _count, Reflect::ObjectResolver *_resolver )
{
	if (_count == 0)
	{
		Reflect::ObjectPtr null_object;
		return null_object;
	}

	Reflect::ObjectPtr cached_object;
	StaticMemoryStream archiveStream( (char *)(_buffer + _offset), _count );
	CacheArchiveReader::ReadFromStream( archiveStream, cached_object, _resolver );
	return cached_object;
}