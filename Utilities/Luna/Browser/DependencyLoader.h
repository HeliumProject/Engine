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
        DependencyLoader( const tstring& rootDirectory, const tstring& configDirectory, DependencyCollection* collection );
        virtual ~DependencyLoader();

    protected:
        virtual void InitData();
        virtual void ThreadProc( i32 threadID );

        void OnEndThread( const Nocturnal::ThreadProcArgs& args );

    private:
        tstring           m_RootDirectory;
        tstring           m_ConfigDirectory;
        DependencyCollection* m_Collection;
        std::set< Nocturnal::Path >     m_AssetPaths;
    };

}
