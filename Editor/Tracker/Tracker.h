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
            uint32_t GetCurrentProgress() const;
            uint32_t GetTrackingTotal() const;

        protected:
            Helium::Thread m_Thread;
            bool m_StopTracking;

            TrackerDBGenerated m_TrackerDB;
            Helium::Directory m_Directory;

            // Status update
            bool m_InitialIndexingCompleted;
            bool m_IndexingFailed;
            uint32_t m_CurrentProgress;
            uint32_t m_Total;

        protected:
            static int s_InitCount;
            static Helium::InitializerStack s_InitializerStack;
        };

    }
}