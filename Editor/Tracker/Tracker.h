#pragma once

#include "Editor/API.h"

#include "TrackerDBGenerated.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/File/Directory.h"
#include "Platform/Thread.h"

namespace Helium
{
    namespace Editor
    {
        class Tracker
        {
        public:
            Tracker();
            virtual ~Tracker();

            void SetDirectory( const Helium::Path& directory );
            const Helium::Directory& GetDirectory() const;

            void StartThread();
            void StopThread();
            bool IsThreadRunning();

            void TrackEverything();

            // Status update functions
            bool InitialIndexingCompleted() const;
            bool DidIndexingFail() const;
            u32 GetCurrentProgress() const;
            u32 GetTrackingTotal() const;

        protected:
            Helium::Thread m_Thread;
            bool m_StopTracking;

            TrackerDBGenerated m_TrackerDB;
            Helium::Directory m_Directory;

            // Status update
            bool m_InitialIndexingCompleted;
            bool m_IndexingFailed;
            u32 m_CurrentProgress;
            u32 m_Total;

        protected:
            static int s_InitCount;
            static Helium::InitializerStack s_InitializerStack;
        };

    }
}