#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"
#include "Platform/Platform.h"

namespace Helium
{
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
#endif
		Handle m_Handle;
	};

    HELIUM_PLATFORM_API extern const tchar_t PathSeparator;
    HELIUM_PLATFORM_API void GetFullPath( const tchar_t* path, tstring& fullPath );
    HELIUM_PLATFORM_API bool IsAbsolute( const tchar_t* path );
    HELIUM_PLATFORM_API bool MakePath( const tchar_t* path );
    HELIUM_PLATFORM_API bool Copy( const tchar_t* source, const tchar_t* dest, bool overwrite );
    HELIUM_PLATFORM_API bool Move( const tchar_t* source, const tchar_t* dest );
    HELIUM_PLATFORM_API bool Delete( const tchar_t* path );
}