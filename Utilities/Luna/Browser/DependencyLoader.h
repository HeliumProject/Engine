#pragma once

#include "API.h"

#include "Common/File/Path.h"
#include "UIToolKit/ThreadMechanism.h"

namespace Luna
{
    class DependencyCollection;

    ///////////////////////////////////////////////////////////////////////////////
    /// class DependencyLoader
    ///////////////////////////////////////////////////////////////////////////////
    class DependencyLoader : public UIToolKit::ThreadMechanism
    {
    public:
        DependencyLoader( const std::string& rootDirectory, const std::string& configDirectory, DependencyCollection* collection );
        virtual ~DependencyLoader();

    protected:
        virtual void InitData() NOC_OVERRIDE;
        virtual void ThreadProc( i32 threadID ) NOC_OVERRIDE;

        void OnEndThread( const UIToolKit::ThreadProcArgs& args );

    private:
        std::string           m_RootDirectory;
        std::string           m_ConfigDirectory;
        DependencyCollection* m_Collection;
        Nocturnal::S_Path     m_AssetPaths;
    };

}
