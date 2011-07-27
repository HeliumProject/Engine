#pragma once

#include "Editor/API.h"

#include "TrackerDBGenerated.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/File/Directory.h"
#include "Platform/Thread.h"
#include "SceneGraph/Project.h"

namespace Helium
{
    namespace Editor
    {
        class Tracker
        {
        public:
            Tracker();
            virtual ~Tracker();

            void SetProject( Project* project );
            const Project* GetProject() const;

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
            Helium::CallbackThread m_Thread;
            bool m_StopTracking;

            TrackerDBGenerated* m_TrackerDB;
            Project* m_Project;

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