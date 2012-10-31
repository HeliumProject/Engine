#pragma once

#include "Platform/Mutex.h"
#include "Platform/Semaphore.h"
#include "Foundation/Event.h"
#include "Foundation/OrderedSet.h"
#include "Foundation/FilePath.h"

#include "SceneGraph/DeviceManager.h"

#include "Editor/Vault/Thumbnail.h"

namespace Helium
{
    namespace Editor
    {
        //
        // Thumbnail loader loads textures in a thread and notifies results in that background thread via an event
        //

        class ThumbnailLoader
        {
        public:
            ThumbnailLoader( DeviceManager* d3dManager );
            ~ThumbnailLoader();

            void Enqueue( const std::set< Helium::Path >& files );
            void Stop();


        public:
            struct ResultArgs
            {
                Helium::Path m_Path;
                V_ThumbnailPtr m_Textures;
                bool m_Cancelled;
            };
            typedef Helium::Signature< const ResultArgs&> ResultSignature;

            //
            // The result event (raised in the loading thread)
            //

        private:
            ResultSignature::Event m_Result;
        public:
            void AddResultListener( ResultSignature::Delegate listener )
            {
                m_Result.Add( listener );
            }
            void RemoveResultListener( ResultSignature::Delegate listener )
            {
                m_Result.Remove( listener );
            }

        private:
            class LoadThread : public wxThread
            {
            public:
                LoadThread( ThumbnailLoader& loader )
                    : wxThread ( wxTHREAD_JOINABLE )
                    , m_Loader( loader )
                {

                }

                virtual void* Entry();

            private:
                ThumbnailLoader& m_Loader;
            } m_LoadThread; // The loading thread object

            Helium::Locker< Helium::OrderedSet< Helium::Path > >    m_FileQueue; // The queue of files to load (mutex locked)
            Helium::Semaphore                                       m_Signal; // Signalling semaphore to wake up load thread
            bool                                                    m_Quit;
            DeviceManager*                            m_DeviceManager;
        };
    }
}