#pragma once

#include <map>

#include "Foundation/Event.h"
#include "Foundation/FilePath.h"

#include "Application/API.h"

namespace Helium
{
    namespace FileOperations
    {
        enum FileOperation
        {
            Unknown = 0,
            Added = 1 << 0,
            Removed = 1 << 1, 
            Modified = 1 << 2,
            Renamed = 1 << 3,
        };
    }
    typedef FileOperations::FileOperation FileOperation;


    struct HELIUM_APPLICATION_API FileChangedArgs
    {
        tstring   m_Path;
        FileOperation m_Operation;
        tstring   m_OldPath;


        FileChangedArgs( const tstring& path, const FileOperation operation = FileOperations::Unknown, const tstring& oldPath = TXT( "" ) )
            : m_Path( path )
            , m_Operation( operation )
            , m_OldPath( oldPath )
        {
        }
    };
    typedef Helium::Signature< const FileChangedArgs& > FileChangedSignature;

    typedef void* HANDLE;
    struct HELIUM_APPLICATION_API FileWatch
    {
        HANDLE                      m_ChangeHandle;
        FileChangedSignature::Event m_Event;
        Path                        m_Path;
        bool                        m_WatchSubtree;


        FileWatch()
            : m_ChangeHandle( NULL )
            , m_WatchSubtree( false )
        {
        }
    };

    typedef std::map< tstring, FileWatch > M_PathToFileWatch;

    typedef void* HANDLE;

    class HELIUM_APPLICATION_API FileWatcher
    {
    private:
        M_PathToFileWatch m_Watches;

    public:
        FileWatcher();
        ~FileWatcher();

        bool Add( const tstring& path, FileChangedSignature::Delegate& listener, bool watchSubtree = false  );
        bool Remove( const tstring& path, FileChangedSignature::Delegate& listener );
        bool Watch( int timeout = 0xFFFFFFFF );
    };
}