#pragma once

#include <string>

#include "Foundation/InitializerStack.h"
#include "Foundation/CommandLine/Command.h"
#include "Foundation/File/Path.h"

#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Content/ContentInit.h"

/*
namespace AssetBuilder
{
  class AssetBuiltArgs;
  typedef Helium::SmartPtr< AssetBuiltArgs > AssetBuiltArgsPtr;
};

namespace Editor
{
    class BuildCommand : public Helium::CommandLine::Command
    {
    private:
        Helium::InitializerStack m_InitializerStack;

        tstring m_SearchQuery;
        std::set< Helium::Path > m_AssetPaths;

        bool m_HelpFlag;
        bool m_AllFlag;
        bool m_NoMultipleFlag;
        bool m_GenerateReportFlag;
        bool m_ForceFlag;
        bool m_HaltOnErrorFlag;
        bool m_DisableCacheFilesFlag;
        bool m_SingleThreadFlag;
        bool m_WorkerFlag;
        tstring m_HackFileSpecOption;
        std::vector< tstring> m_RegionOption;

    public:
        BuildCommand();
        virtual ~BuildCommand();

        static void AssetBuilt( const AssetBuilder::AssetBuiltArgsPtr& args );

        virtual bool Initialize( tstring& error ) HELIUM_OVERRIDE;
        virtual void Cleanup() HELIUM_OVERRIDE;

        virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) HELIUM_OVERRIDE;

    private:
        static bool QueryAssetPaths( const tstring& searchQuery, bool noMultiple, bool all, std::set< Helium::Path >& assetPaths );
        void Except( const Helium::Exception& ex, const Asset::AssetClassPtr& assetClass = NULL );
        void Report( Asset::AssetClass* assetClass );
        bool Build( Dependencies::DependencyGraph& depGraph, std::set< Helium::Path >& assets, const std::vector< tstring >& options );
        bool Build( Dependencies::DependencyGraph& depGraph, std::set< Helium::Path >& assets, const AssetBuilder::BuilderOptionsPtr& options );
        bool RunAsBuildWorker( Dependencies::DependencyGraph& depGraph, bool debug = false, bool wait = false );
    };

}

*/