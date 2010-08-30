#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Controls/InspectControl.h"

namespace Helium
{
    namespace Inspect
    {
        class List;

        class ClientDataFilter : public Reflect::AbstractInheritor<ClientDataFilter, Reflect::Object>
        {
        public:
            List*       m_List;
            i32         m_ClassType;
            tstring m_FileFilter;

            ClientDataFilter( List* list, i32 classType, const tstring& filter )
                : m_List( list )
                , m_ClassType( classType )
                , m_FileFilter( filter )
            {

            }

            virtual ~ClientDataFilter()
            {

            }
        };
    }

    namespace InspectReflect
    {
        FOUNDATION_API void Initialize();
        FOUNDATION_API void Cleanup();
    }
}