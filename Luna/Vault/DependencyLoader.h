#pragma once

#include "Luna/API.h"

#include "Foundation/File/Path.h"
#include "Application/UI/ThreadMechanism.h"

namespace Luna
{
    class DependencyCollection;

    ///////////////////////////////////////////////////////////////////////////////
    /// class DependencyLoader
    ///////////////////////////////////////////////////////////////////////////////
    class DependencyLoader : public Nocturnal::ThreadMechanism
    {
    public:
        DependencyLoader( const Nocturnal::Path& rootDirectory, DependencyCollection* collection );
        virtual ~DependencyLoader();

    protected:
        virtual void InitData();
        virtual void ThreadProc( i32 threadID );

        void OnEndThread( const Nocturnal::ThreadProcArgs& args );

    private:
        Nocturnal::Path             m_RootDirectory;
        DependencyCollection*       m_Collection;
        std::set< Nocturnal::Path > m_AssetPaths;
    };

}
