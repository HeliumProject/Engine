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

            ClientDataFilter( List* list, const Reflect::Class* instanceClass, const tstring& filter )
                : m_List( list )
                , m_InstanceClass( instanceClass )
                , m_FileFilter( filter )
            {

            }

            List*                   m_List;
            const Reflect::Class*   m_InstanceClass;
            tstring                 m_FileFilter;
        };
    }

    namespace InspectReflect
    {
        FOUNDATION_API void Initialize();
        FOUNDATION_API void Cleanup();
    }
}