#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"
#include "Platform/Utility.h"

namespace Helium
{
	//
	// File contents
	//

	namespace SeekOrigins
	{
		/// Stream seek origin.
		enum SeekOrigin
		{
			SEEK_ORIGIN_INVALID = -1,
			SEEK_ORIGIN_CURRENT,  ///< Seek relative to the current location.
			SEEK_ORIGIN_BEGIN,    ///< Seek relative to the beginning of the stream.
			SEEK_ORIGIN_END,      ///< Seek relative to the end of the stream.
			SEEK_ORIGIN_MAX,
		};
	};
	typedef SeekOrigins::SeekOrigin SeekOrigin;

	namespace FileModes
	{
		/// File access mode flags.
		enum FileMode
		{
			MODE_READ	= ( 1 << 0 ),  ///< Read access.
			MODE_WRITE	= ( 1 << 1 ),  ///< Write access.
		};
	};
	typedef FileModes::FileMode FileMode;

	class HELIUM_PLATFORM_API File
	{
	public:
		File();
		~File();

		bool IsOpen() const;
		bool Open( const tchar_t* filename, FileMode mode, bool truncate = true );
		bool Close();
		
		bool Read( void* buffer, size_t numberOfBytesToRead, size_t* numberOfBytesRead = NULL );
		bool Write( const void* buffer, size_t numberOfBytesToWrite, size_t* numberOfBytesWritten = NULL );
		bool Flush();

		int64_t Seek( int64_t offset, SeekOrigin origin );
		int64_t Tell() const;
		int64_t GetSize() const;

	private:
#ifdef HELIUM_OS_WIN
		// windows.h: HANDLE
		typedef void* Handle;
#else
#error Implement File for this platform.
#endif
		Handle m_Handle;
	};

	//
	// File status
	//

    namespace StatusModes
    {
        enum Type
        {
            None             = 0,
            Directory        = 1 << 0,
            Link             = 1 << 1,
            Pipe             = 1 << 2,
            Special          = 1 << 3,

            Read             = 1 << 8,
            Write            = 1 << 9,
            Execute          = 1 << 10
        };
    }
    typedef StatusModes::Type StatusMode;

    class HELIUM_PLATFORM_API Status
    {
	public:
        Status();

	    bool Read( const tchar_t* path );

        uint32_t     m_Mode;
        uint64_t     m_Size;
        uint64_t     m_CreatedTime;
        uint64_t     m_ModifiedTime;
        uint64_t     m_AccessTime;
	};

	//
	// Directory info
	//

	class HELIUM_PLATFORM_API DirectoryEntry
	{
	public:
		DirectoryEntry( const tstring& name = TXT( "" ) );

		tstring	m_Name;
		Status	m_Stat;
	};

	class HELIUM_PLATFORM_API Directory : NonCopyable
	{
	public:
		Directory( const tstring& path = TXT( "" ) );
		~Directory();

		bool IsOpen();
		bool FindFirst( DirectoryEntry& entry );
		bool FindNext( DirectoryEntry& entry );
		bool Close();

		inline const tstring& GetPath();
		inline void SetPath( const tstring& path );

	private:
		tstring	m_Path;

#if HELIUM_OS_WIN
		typedef void* Handle;
#else
#error Implement Directory for this platform.
#endif
		Handle m_Handle;
	};

	//
	// File system operations
	//

    HELIUM_PLATFORM_API extern const tchar_t PathSeparator;
    HELIUM_PLATFORM_API void GetFullPath( const tchar_t* path, tstring& fullPath );
    HELIUM_PLATFORM_API bool IsAbsolute( const tchar_t* path );
    HELIUM_PLATFORM_API bool MakePath( const tchar_t* path );
    HELIUM_PLATFORM_API bool Copy( const tchar_t* source, const tchar_t* dest, bool overwrite );
    HELIUM_PLATFORM_API bool Move( const tchar_t* source, const tchar_t* dest );
    HELIUM_PLATFORM_API bool Delete( const tchar_t* path );
}

#include "Platform/File.inl"