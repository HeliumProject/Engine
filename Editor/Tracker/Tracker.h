#pragma once

#include "Editor/API.h"

#include "TrackerDBGenerated.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/File/Directory.h"

namespace Editor
{
    class Tracker
    {
    public:
        //m_Tracker.SetDirectory( Helium::Directory( "C:\\Projects\\github\\kramdar\\AppFramework\\Editor\\Icons\\16x16\\" ) );
        //if (!m_TrackerThread.Create(&Platform::Thread::EntryHelper<Tracker, &Tracker::TrackEverything>, &m_Tracker, TXT( "Tracker Thread" ), THREAD_PRIORITY_BELOW_NORMAL ) )
        //{
        //    throw Exception( TXT( "Unable to create thread for asset tracking." ) );
        //}
        Tracker();
        virtual ~Tracker();

        void SetDirectory( const Helium::Directory& directory );
        const Helium::Directory& GetDirectory() const;

        void TrackEverything();

        bool InitialIndexingCompleted() const { return m_InitialIndexingCompleted; }
        bool DidIndexingFail() const { return m_IndexingFailed; }

        u32 GetCurrentProgress() const
        {
            return m_CurrentProgress;
        }

        u32 GetTrackingTotal() const 
        {
            return m_Total;
        }

    protected:
        TrackerDBGenerated m_TrackerDB;

        Helium::Directory m_Directory;

        bool m_StopTracking;
        bool m_InitialIndexingCompleted;
        bool m_IndexingFailed;
        u32 m_CurrentProgress;
        u32 m_Total;

    protected:
        static int s_InitCount;
        static Helium::InitializerStack s_InitializerStack;
    };

}