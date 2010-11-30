#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "Platform/Assert.h"
#include "Platform/Exception.h"
#include "Foundation/Memory/SmartPtr.h"

#include "API.h"
#include "TypeID.h"
#include "ReflectionInfo.h"

namespace Helium
{
    namespace Reflect
    {
        //
        // Type, fully qualified type information
        //

        class FOUNDATION_API Type HELIUM_ABSTRACT : public ReflectionInfo
        {
        public:
            REFLECTION_BASE( ReflectionTypes::Type );

        protected:
            Type();
            virtual ~Type();

        public:
            tstring                         m_Name;         // the name of this type in the codebase
            tstring                         m_UIName;       // the friendly name for the field, its optional (will use the short name if not specified)
            uint32_t                        m_Size;         // the size of the object in bytes
        };
    }
}