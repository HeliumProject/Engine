#pragma once

#include "Pipeline/API.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/AssetFlags.h"

#include "Foundation/File/Directory.h"
#include "Foundation/File/Path.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"
#include "Foundation/Reflect/Field.h"

namespace Asset
{
    //
    // Forwards
    //
    class CacheDB;
    typedef PIPELINE_API Nocturnal::SmartPtr< CacheDB > CacheDBPtr;

    class AssetFile;
    typedef Nocturnal::SmartPtr< AssetFile > AssetFilePtr;
    typedef std::map< tuid, AssetFilePtr > M_AssetFiles;

    class AssetVisitor;

    /////////////////////////////////////////////////////////////////////////////
    struct TrackerArgs
    {
        bool m_IsTracking;

        TrackerArgs( bool isTracking )
            : m_IsTracking( isTracking )
        {
        }
    };
    typedef Nocturnal::Signature< void, const TrackerArgs& > TrackerSignature;

    /////////////////////////////////////////////////////////////////////////////
    class PIPELINE_API Tracker
    {
    public:
        Tracker( const std::string& rootDirectory, const std::string& configDirectory );
        Tracker( const Tracker& rhs );
        Tracker& operator=( const Tracker& rhs );
        ~Tracker();

        bool IsTracking() const;
        void StartThread();
        void StopThread();

        bool InitialIndexingCompleted() const { return m_InitialIndexingCompleted; }
        bool DidIndexingFail() const { return m_IndexingFailed; }

        u32 GetTrackingProgress();
        u32 GetTrackingTotal();

    private:

        // Thread entry points
        static DWORD WINAPI TrackEverythingThread(LPVOID pTracker);

        bool TrackFile( const std::string& path );
        bool TrackFile( Nocturnal::Path& filePath );
        bool TrackAssetFile( Nocturnal::Path& filePath, M_AssetFiles* assetFiles );
        void TrackEverything();

        void HandleDirectoryItem( const Nocturnal::DirectoryItem& dirItem );

    private:
        std::set< Nocturnal::Path >     m_FoundPaths;
        Asset::CacheDBPtr     m_AssetCacheDB;

        static std::string    s_GlobalRootDirectory;
        const std::string     m_RootDirectory;

        HANDLE                m_Thread;
        DWORD                 m_ThreadID;
        bool                  m_StopTracking;

        M_AssetFiles          m_AssetFiles;

        bool                  m_InitialIndexingCompleted;
        bool                  m_IndexingFailed;
        u32                   m_CurrentProgress;
        u32                   m_Total;
    };
}
