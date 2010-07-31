#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/Control.h"

namespace Helium
{
    namespace Inspect
    {
        class ClientDataControl : public Reflect::AbstractInheritor<ClientDataControl, Reflect::Object>
        {
        public:
            Control* m_Control;

            ClientDataControl( Control* control )
                : m_Control ( control )
            {

            }

            virtual ~ClientDataControl()
            {

            }
        };

        class ClientDataFilter  : public Reflect::AbstractInheritor<ClientDataFilter, Reflect::Object>
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
        APPLICATION_API void Initialize();
        APPLICATION_API void Cleanup();
    }
}