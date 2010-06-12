#pragma once

#include "Foundation/API.h"
#include "Foundation/Automation/Event.h"

#include "Path.h"

namespace Nocturnal
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


    struct FOUNDATION_API FileChangedArgs
    {
        std::string   m_Path;
        FileOperation m_Operation;
        std::string   m_OldPath;


        FileChangedArgs( const std::string& path, const FileOperation operation = FileOperations::Unknown, const std::string& oldPath = "" )
            : m_Path( path )
            , m_Operation( operation )
            , m_OldPath( oldPath )
        {
        }
    };
    typedef Nocturnal::Signature< void, const FileChangedArgs& > FileChangedSignature;

    typedef void* HANDLE;
    struct FOUNDATION_API FileWatch
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

    typedef std::map< std::string, FileWatch > M_PathToFileWatch;

    typedef void* HANDLE;

    class FOUNDATION_API FileWatcher
    {
    private:
        M_PathToFileWatch m_Watches;

    public:
        FileWatcher();
        ~FileWatcher();

        bool Add( const std::string& path, FileChangedSignature::Delegate& listener, bool watchSubtree = false  );
        bool Remove( const std::string& path, FileChangedSignature::Delegate& listener );
        bool Watch( int timeout = 0xFFFFFFFF );
    };
}