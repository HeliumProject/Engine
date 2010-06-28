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
  typedef Nocturnal::SmartPtr< AssetBuiltArgs > AssetBuiltArgsPtr;
};

namespace Luna
{
    class BuildCommand : public Nocturnal::CommandLine::Command
    {
    private:
        Nocturnal::InitializerStack m_InitializerStack;

        std::string m_SearchQuery;
        std::set< Nocturnal::Path > m_AssetPaths;

        bool m_HelpFlag;
        bool m_AllFlag;
        bool m_NoMultipleFlag;
        bool m_GenerateReportFlag;
        bool m_ForceFlag;
        bool m_HaltOnErrorFlag;
        bool m_DisableCacheFilesFlag;
        bool m_SingleThreadFlag;
        bool m_WorkerFlag;
        std::string m_HackFileSpecOption;
        std::vector< std::string> m_RegionOption;

    public:
        BuildCommand();
        virtual ~BuildCommand();

        static void AssetBuilt( const AssetBuilder::AssetBuiltArgsPtr& args );

        virtual bool Initialize( std::string& error ) NOC_OVERRIDE;
        virtual void Cleanup() NOC_OVERRIDE;

        virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE;

    private:
        static bool QueryAssetPaths( const std::string& searchQuery, bool noMultiple, bool all, std::set< Nocturnal::Path >& assetPaths );
        void Except( const Nocturnal::Exception& ex, const Asset::AssetClassPtr& assetClass = NULL );
        void Report( Asset::AssetClass* assetClass );
        bool Build( Dependencies::DependencyGraph& depGraph, std::set< Nocturnal::Path >& assets, const std::vector< std::string >& options );
        bool Build( Dependencies::DependencyGraph& depGraph, std::set< Nocturnal::Path >& assets, const AssetBuilder::BuilderOptionsPtr& options );
        bool RunAsBuildWorker( Dependencies::DependencyGraph& depGraph );
    };

}

*/