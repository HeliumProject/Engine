#pragma once

#include "API.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"

#include "File/ManagedFile.h"
#include "File/History.h"

#include <vector>
#include <string>
#include <set>

namespace File
{
  class FileInfo;
  typedef FILEBROWSER_API Nocturnal::SmartPtr< FileInfo > FileInfoPtr; 

  class FILEBROWSER_API FileInfo : public ManagedFile
  {
  public:

    typedef bool (*GetEntityFunc)( const tuid id, File::FileInfoPtr fileInfo );


    //
    // Ctor/Dtor
    //

    FileInfo( const tuid id = TUID::Null );
    FileInfo( const std::string& path );

    ~FileInfo();


    // operator== only compares the file paths
    bool       operator==( const FileInfo &rhs );

    // 
    // Accessor functions
    //

    void        SetPath( const std::string& path ) { m_Path = path; }
    std::string GetPath();

    std::string GetName();
    static std::string GetName( FileInfo* fileInfo ) { return fileInfo->GetName(); }

    std::string GetExtension();

    std::string GetFileType();
    static std::string GetFileType( FileInfo* fileInfo ) { return fileInfo->GetFileType(); }

    std::string GetFolder();
    static std::string GetFolder( FileInfo* fileInfo ) { return fileInfo->GetFolder(); }

    u64         GetCreated();
    std::string GetCreatedBy();
    static std::string GetCreatedBy( FileInfo* fileInfo ) { return fileInfo->GetCreatedBy(); }

    u64         GetModified();


    u64         GetSize();

    std::string GetPrintCreated();
    static std::string GetCreated( FileInfo* fileInfo ) { return fileInfo->GetPrintCreated(); }

    std::string GetPrintModified();
    static std::string GetModified( FileInfo* fileInfo ) { return fileInfo->GetPrintModified(); }

    std::string GetPrintSize();
    static std::string GetSize( FileInfo* fileInfo ) { return fileInfo->GetPrintSize(); }

    void        SetID( const tuid id ) { m_Id = id; }
    tuid        GetID();
    std::string GetPrintID();
    static std::string GetId( FileInfo* fileInfo ) { return fileInfo->GetPrintID(); }

    std::string GetThumbnailPath( ); //GetEntityFunc getEntity );
    std::string GetArtFile( );

    //
    // Public Members
    //

    std::string m_UpdateClass;
    std::string m_ArtFile;

    //
    // Friend Classes
    //

    friend class  FileBrowser;
    friend struct FileInfoComparePath;

  private:

    //
    // ManagedFile overloads 
    //

    u64         Created();
    u64         Modified();

    bool        GetHistory();
    std::string GetPrintTime( u64 time );

    void        PopulateStats( bool force = false );


    //
    // Cached Members
    //

    std::string       m_Name;
    std::string       m_PrintName;
    std::string       m_FileType;
    std::string       m_Extension;
    std::string       m_Folder;

    u64               m_Size;       // current size of the file on disk
    S_FileHistory     m_History;         // Cached history set

    std::string       m_CreatedBy;
    std::string       m_ModifiedBy;

    std::string       m_PrintLevels;

    std::string       m_ThumbnailPath;

  public:
    REFLECT_DECLARE_CLASS( FileInfo, ManagedFile );

    static void EnumerateClass( Reflect::Compositor<FileInfo>& comp );
  };


  /////////////////////////////////////////////////////////////////////////////
  // Smart pointer typedefs

  typedef FILEBROWSER_API std::vector< FileInfoPtr >        V_FileInfoPtr;

  struct FileInfoComparePath
  {
    bool operator()( const File::FileInfoPtr& rhs, const File::FileInfoPtr& lhs ) const
    {
      return rhs->m_Path.compare( lhs->m_Path ) < 0;
    }
  };
  typedef FILEBROWSER_API std::set< File::FileInfoPtr, FileInfoComparePath > S_FileInfoPtr;

} // namespace File