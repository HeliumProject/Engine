#pragma once

#include "Pipeline/API.h"

#include <vector>
#include <set>

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Foundation/TUID.h"

namespace Helium
{
    namespace ES
    { 
        class PIPELINE_API Event : public Helium::RefCountBase< Event >
        {
        public:
            Event
                (
                const tuid id = TUID::Null,
                uint64_t created = 0,
                const tstring& username = TXT( "" ),
                const tstring& data = TXT( "" )
                )
                : m_Id( id )
                , m_Created( created )
                , m_Username( username )
                , m_Data( data )
            {
            }

            bool IsValid() const
            {
                return m_Id != TUID::Null;
            }

            tstring Dump() const;

        public:
            tuid          m_Id;       // Events's unique id
            uint64_t           m_Created;  // time of Event record creation
            tstring   m_Username; // username@computer
            tstring   m_Data;           
        };

        typedef Helium::SmartPtr< Event > EventPtr;
        typedef std::vector< EventPtr > V_EventPtr;
    }
}