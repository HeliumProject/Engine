#pragma once

#include "Pipeline/API.h"

#include <vector>
#include <set>

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Foundation/TUID.h"

namespace Nocturnal
{
    namespace ES
    { 
        class PIPELINE_API Event : public Nocturnal::RefCountBase< Event >
        {
        public:
            Event
                (
                const tuid id = TUID::Null,
                u64 created = 0,
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
            u64           m_Created;  // time of Event record creation
            tstring   m_Username; // username@computer
            tstring   m_Data;           
        };

        typedef Nocturnal::SmartPtr< Event > EventPtr;
        typedef std::vector< EventPtr > V_EventPtr;
    }
}