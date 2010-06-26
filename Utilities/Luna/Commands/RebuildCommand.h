#pragma once

#include <string>

#include "Foundation/InitializerStack.h"
#include "Foundation/CommandLine/Command.h"
#include "Foundation/File/Path.h"

#include "Pipeline/AssetBuilder/AssetBuilder.h"
#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Content/ContentInit.h"

namespace Luna
{
    class RebuildCommand : public Nocturnal::CommandLine::Command
    {
    private:
        Nocturnal::InitializerStack m_InitializerStack;

        bool m_HelpFlag;
        bool m_RCS;
        bool m_XML;
        bool m_Binary;
        bool m_Verify;
        std::string m_Batch;
        std::string m_Input;
        std::string m_Output;

        enum RebuildCodes
        {
            REBUILD_SUCCESS = 0,
            REBUILD_BAD_INPUT,
            REBUILD_BAD_READ,
            REBUILD_BAD_WRITE,
            REBUILD_CODE_COUNT,
        };
        static const char* m_RebuildStrings[REBUILD_CODE_COUNT];

        int m_RebuildTotals[REBUILD_CODE_COUNT];

        std::vector< std::string > m_RebuildResults[REBUILD_CODE_COUNT];

    public:
        RebuildCommand();
        virtual ~RebuildCommand();

        virtual bool Initialize( std::string& error ) NOC_OVERRIDE;
        virtual void Cleanup() NOC_OVERRIDE;

        virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE;

    private:
        int ProcessFile(const std::string& input, const std::string& output);


    };
}