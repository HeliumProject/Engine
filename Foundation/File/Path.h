#pragma once

#include <set>
#include <vector>

#include "Foundation/API.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/String/Utilities.h"
#include "Platform/String.h"

namespace Helium
{
    struct Stat;
}

namespace Helium
{
    const static tchar s_InternalPathSeparator = '/';

    class FOUNDATION_API Path : public Helium::RefCountBase< Path >
    {
    private:
        tstring m_Path;

        void Init( const tchar* path );

    public:
        static void Normalize( tstring& path );
        static void MakeNative( tstring& path );
        static void GuaranteeSeparator( tstring& path );

        static bool Exists( const tstring& path );
        static bool IsAbsolute( const tstring& path );
        static bool IsUnder( const tstring& location, const tstring& path );

    public:
        Path( const tchar* path );
        Path( const tstring& path = TXT( "" ) );
        Path( const Path& path );

        Path& operator=( const Path& rhs );
        bool operator==( const Path& rhs ) const;

        bool operator<( const Path& rhs ) const;

        Path operator+( const tchar* rhs ) const;
        Path operator+( const tstring& rhs ) const;
        Path operator+( const Path& rhs ) const;

        Path& operator+=( const tchar* rhs );
        Path& operator+=( const tstring& rhs );
        Path& operator+=( const Helium::Path& rhs );

        const tstring& Get() const;
        const tstring& Set( const tstring& path );

        void Split( tstring& directory, tstring& filename ) const;
        void Split( tstring& directory, tstring& filename, tstring& extension ) const;

        tstring Basename() const;
        tstring Filename() const;
        tstring Directory() const;
        std::vector< tstring > DirectoryAsVector() const;

        tstring Extension() const;
        tstring FullExtension() const;
        void RemoveExtension();
        void RemoveFullExtension();
        void ReplaceExtension( const tstring& newExtension );
        void ReplaceFullExtension( const tstring& newExtension );

        tstring Native() const;
        tstring Absolute() const;
        tstring Normalized() const;

        u64 Hash() const;
        tstring Signature();

        bool Exists() const;
        bool Stat( Helium::Stat& stat ) const;
        bool IsAbsolute() const;
        bool IsUnder( const tstring& location );
        bool IsFile() const;
        bool IsDirectory() const;
        bool Writable() const;
        bool Readable() const;
        bool ChangedSince( u64 lastTime ) const;
        u64 ModifiedTime() const;
        u64 CreatedTime() const;
        u64 AccessTime() const;
        i64 Size() const;

        bool MakePath() const;
        bool Create() const;
        bool Copy( const Helium::Path& target, bool overwrite = true ) const;
        bool Move( const Helium::Path& target ) const;
        bool Delete() const;

        tstring FileCRC() const;
        bool VerifyFileCRC( const tstring& hash ) const;

        tstring FileMD5() const;
        bool VerifyFileMD5( const tstring& hash ) const;

    public:

        Helium::Path GetAbsolutePath( const Helium::Path& basisPath ) const;
        Helium::Path GetRelativePath( const Helium::Path& basisPath ) const;

    public:

        size_t length() const;
        bool empty() const;
        const tchar* c_str() const;
        operator const tchar*() const
        {
            return c_str();
        }
        operator const tstring&() const
        {
            return m_Path;
        }

        friend FOUNDATION_API tostream& operator<<( tostream& outStream, const Path& p );
        friend FOUNDATION_API tistream& operator>>( tistream& inStream, Path& p );
        friend FOUNDATION_API std::wostream& operator<<( std::wostream& outStream, const Path& p );
        friend FOUNDATION_API std::wistream& operator>>( std::wistream& inStream, Path& p );
    };

    inline tostream& operator<<( tostream& outStream, const Path& p )
    {
        return outStream << p.Get();
    }

    inline tistream& operator>>( tistream& inStream, Path& p )
    {
        tstring str;
        std::streamsize size = inStream.rdbuf()->in_avail();
        str.resize( (size_t) size );
        inStream.read( const_cast<tchar*>( str.c_str() ), size );

        p.Set( str );

        return inStream;
    }
}
