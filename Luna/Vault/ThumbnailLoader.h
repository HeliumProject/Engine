#pragma once

#include "Thumbnail.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Container/OrderedSet.h"
#include "Foundation/File/Path.h"
#include "Platform/Mutex.h"
#include "Platform/Semaphore.h"

namespace Render
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
        ThumbnailLoader( Render::D3DManager* d3dManager, const tstring& thumbnailDirectory );
        ~ThumbnailLoader();

        void Enqueue( const std::set< Nocturnal::Path >& files );
        void Stop();


    public:
        struct ResultArgs
        {
            Nocturnal::Path* m_Path;
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
        Platform::Locker< Nocturnal::OrderedSet< Nocturnal::Path > >  m_FileQueue; // The queue of files to load (mutex locked)
        Platform::Semaphore                     m_Signal; // Signalling semaphore to wake up load thread
        bool                                    m_Quit;
        Render::D3DManager*                 m_D3DManager;
        tstring                             m_ThumbnailDirectory;
    };
}
