#pragma once

#include "Thumbnail.h"
#include "Asset/AssetFile.h"
#include "Foundation/Automation/Event.h"
#include "Platform/Mutex.h"
#include "Platform/Semaphore.h"

namespace igDXRender
{
    class D3DManager;
}

namespace Luna
{
    //
    // Thumbnail loader loads textures in a thread and notifies results in that background thread via an event
    //

    class ThumbnailLoader
    {
    public:
        ThumbnailLoader( igDXRender::D3DManager* d3dManager, const std::string& thumbnailDirectory );
        ~ThumbnailLoader();

        //
        // Queue some more files to load thumbnails for
        //

        void Load( const Asset::V_AssetFiles& files );
        void Stop();

        //
        // The result data
        //

    public:
        struct ResultArgs
        {
            Asset::AssetFilePtr m_File;
            V_ThumbnailPtr m_Textures;
            bool m_Cancelled;
        };
        typedef Nocturnal::Signature<void, const ResultArgs&> ResultSignature;

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
        }                                       m_LoadThread; // The loading thread object
        Platform::Locker<Asset::OS_AssetFiles>  m_FileQueue; // The queue of files to load (mutex locked)
        Platform::Semaphore                     m_Signal; // Signalling semaphore to wake up load thread
        bool                                    m_Quit;
        igDXRender::D3DManager*                 m_D3DManager;
        std::string                             m_ThumbnailDirectory;
    };
}
