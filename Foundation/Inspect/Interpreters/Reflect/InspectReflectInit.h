#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Control.h"

namespace Helium
{
    namespace Inspect
    {
        class List;

        class ClientDataFilter : public ClientData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( ClientDataFilter, ClientData );

            ClientDataFilter( List* list, int32_t classType, const tstring& filter )
                : m_List( list )
                , m_ClassType( classType )
                , m_FileFilter( filter )
            {

            }

            List*       m_List;
            int32_t         m_ClassType;
            tstring     m_FileFilter;
        };
    }

    namespace InspectReflect
    {
        FOUNDATION_API void Initialize();
        FOUNDATION_API void Cleanup();
    }
}