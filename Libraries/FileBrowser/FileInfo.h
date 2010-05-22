#pragma once

#include "API.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/File/File.h"

#include "File/Reference.h"

#include "Reflect/Element.h"
#include "Reflect/Serializers.h"

#include <vector>
#include <string>
#include <set>

namespace File
{
    class FileInfo;
    typedef FILEBROWSER_API Nocturnal::SmartPtr< FileInfo > FileInfoPtr; 

    class FILEBROWSER_API FileInfo : public Reflect::Element
    {
    public:

        //
        // Ctor/Dtor
        //

        FileInfo( const std::string& path = "" );
        FileInfo( const File::Reference& fileRef );

        ~FileInfo();


        // operator== only compares the file paths
        bool       operator==( const FileInfo &rhs );

        // 
        // Accessor functions
        //

        const Nocturnal::File& GetFile() const
        {
            return m_File;
        }

        static std::string GetName( FileInfo* fileInfo )
        {
            return fileInfo->m_File.GetPath().Filename();
        }

        static std::string GetDirectory( FileInfo* fileInfo )
        {
            return fileInfo->m_File.GetPath().Directory();
        }

        static std::string GetCreatedBy( FileInfo* fileInfo )
        {
#pragma TODO( "reimplement GetCreatedBy" )
            return "";
        }

        std::string GetFileType()
        {
            return m_File.GetPath().Extension();
        }
        static std::string GetFileType( FileInfo* fileInfo ) { return fileInfo->GetFileType(); }

        std::string GetCreatedAsString();
        static std::string GetCreated( FileInfo* fileInfo ) { return fileInfo->GetCreatedAsString(); }

        std::string GetModifiedAsString();
        static std::string GetModified( FileInfo* fileInfo ) { return fileInfo->GetModifiedAsString(); }

        std::string GetSizeAsString();
        static std::string GetSize( FileInfo* fileInfo ) { return fileInfo->GetSizeAsString(); }

        std::string GetThumbnailPath( ); //GetEntityFunc getEntity );
        std::string GetArtFile( );

        //
        // Friend Classes
        //

        friend class  FileBrowser;
        friend struct FileInfoComparePath;

    private:

        Nocturnal::File   m_File;
        Nocturnal::Path   m_ThumbnailPath;

    public:
        REFLECT_DECLARE_CLASS( FileInfo, Reflect::Element );

        static void EnumerateClass( Reflect::Compositor<FileInfo>& comp );
    };


    /////////////////////////////////////////////////////////////////////////////
    // Smart pointer typedefs

    typedef FILEBROWSER_API std::vector< FileInfoPtr >        V_FileInfoPtr;

    struct FileInfoComparePath
    {
        bool operator()( const FileInfoPtr& rhs, const FileInfoPtr& lhs ) const
        {
            return rhs->m_File < lhs->m_File;
        }
    };
    typedef FILEBROWSER_API std::set< FileInfoPtr, FileInfoComparePath > S_FileInfoPtr;

} // namespace File