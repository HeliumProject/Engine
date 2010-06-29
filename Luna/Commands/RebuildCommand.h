#pragma once

#include <string>

#include "Foundation/InitializerStack.h"
#include "Foundation/CommandLine/Command.h"
#include "Foundation/File/Path.h"

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
        tstring m_Batch;
        tstring m_Input;
        tstring m_Output;

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

        std::vector< tstring > m_RebuildResults[REBUILD_CODE_COUNT];

    public:
        RebuildCommand();
        virtual ~RebuildCommand();

        virtual bool Initialize( tstring& error ) NOC_OVERRIDE;
        virtual void Cleanup() NOC_OVERRIDE;

        virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) NOC_OVERRIDE;

    private:
        int ProcessFile(const tstring& input, const tstring& output);
    };
}