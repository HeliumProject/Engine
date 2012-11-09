#pragma once

#include <set>
#include <vector>

#include "Platform/File.h"

#include "Foundation/API.h"
#include "Foundation/SmartPtr.h"
#include "Foundation/StringUtilities.h"

namespace Helium
{
    const static tchar_t s_InternalPathSeparator = '/';

    class HELIUM_FOUNDATION_API FilePath : public Helium::RefCountBase< FilePath >
    {
    private:
        tstring m_Path;

        void Init( const tchar_t* path );

    public:
        static void Normalize( tstring& path );
        static void MakeNative( tstring& path );
        static void GuaranteeSeparator( tstring& path );

        static bool Exists( const tstring& path );
        static bool IsAbsolute( const tchar_t* path );
        static bool IsUnder( const tstring& location, const tstring& path );

    public:
        FilePath( const tchar_t* path );
        FilePath( const tstring& path = TXT( "" ) );
        FilePath( const FilePath& path );

        const tchar_t* operator*() const;

        FilePath& operator=( const FilePath& rhs );
        bool operator==( const FilePath& rhs ) const;

        bool operator<( const FilePath& rhs ) const;

        FilePath operator+( const tchar_t* rhs ) const;
        FilePath operator+( const tstring& rhs ) const;
        FilePath operator+( const FilePath& rhs ) const;

        FilePath& operator+=( const tchar_t* rhs );
        FilePath& operator+=( const tstring& rhs );
        FilePath& operator+=( const Helium::FilePath& rhs );

        const tstring& Get() const;
        const tstring& Set( const tstring& path );
        void Clear();

        void TrimToExisting();

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
        bool HasExtension( const tchar_t* extension ) const;

        tstring Native() const;
        tstring Absolute() const;
        tstring Normalized() const;
        tstring Signature();

        bool Exists() const;
        bool IsAbsolute() const;
        bool IsUnder( const tstring& location ) const;
        bool IsFile() const;
        bool IsDirectory() const;
        bool Writable() const;
        bool Readable() const;

        bool MakePath() const;
        bool Create() const;
        bool Copy( const Helium::FilePath& target, bool overwrite = true ) const;
        bool Move( const Helium::FilePath& target ) const;
        bool Delete() const;

        tstring FileMD5() const;
        bool VerifyFileMD5( const tstring& hash ) const;

    public:

        Helium::FilePath GetAbsolutePath( const Helium::FilePath& basisPath ) const;
        Helium::FilePath GetRelativePath( const Helium::FilePath& basisPath ) const;

    public:

        size_t length() const;
        bool empty() const;
        const tchar_t* c_str() const;
        operator const tchar_t*() const
        {
            return c_str();
        }
        operator const tstring&() const
        {
            return m_Path;
        }

        friend HELIUM_FOUNDATION_API tostream& operator<<( tostream& outStream, const FilePath& p );
        friend HELIUM_FOUNDATION_API tistream& operator>>( tistream& inStream, FilePath& p );
        friend HELIUM_FOUNDATION_API std::wostream& operator<<( std::wostream& outStream, const FilePath& p );
        friend HELIUM_FOUNDATION_API std::wistream& operator>>( std::wistream& inStream, FilePath& p );
    };

    inline tostream& operator<<( tostream& outStream, const FilePath& p )
    {
        return outStream << p.Get();
    }

    inline tistream& operator>>( tistream& inStream, FilePath& p )
    {
        tstring str;
        std::streamsize size = inStream.rdbuf()->in_avail();
        str.resize( (size_t) size );
        inStream.read( const_cast<tchar_t*>( str.c_str() ), size );

        p.Set( str );

        return inStream;
    }
}
