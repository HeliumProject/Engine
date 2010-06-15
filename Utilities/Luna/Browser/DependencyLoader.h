#pragma once

#include "Luna/API.h"

#include "Foundation/File/Path.h"
#include "Luna/UI/ThreadMechanism.h"

namespace Luna
{
    class DependencyCollection;

    ///////////////////////////////////////////////////////////////////////////////
    /// class DependencyLoader
    ///////////////////////////////////////////////////////////////////////////////
    class DependencyLoader : public Luna::ThreadMechanism
    {
    public:
        DependencyLoader( const std::string& rootDirectory, const std::string& configDirectory, DependencyCollection* collection );
        virtual ~DependencyLoader();

    protected:
        virtual void InitData();
        virtual void ThreadProc( i32 threadID );

        void OnEndThread( const Luna::ThreadProcArgs& args );

    private:
        std::string           m_RootDirectory;
        std::string           m_ConfigDirectory;
        DependencyCollection* m_Collection;
        Nocturnal::S_Path     m_AssetPaths;
    };

}
