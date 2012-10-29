#pragma once

#include "Reflect/Data/Data.h"

namespace Helium
{
    namespace Reflect
    {
        // Delimiter to use when representing the data in the container as a 
        // single string.  If your data contains this delimiter naturally, the
        // string representation may not show up properly.
        static const tchar_t* s_ContainerItemDelimiter = TXT("\n");

        class HELIUM_REFLECT_API ContainerData : public Data
        {
        public:
            REFLECT_DECLARE_ABSTRACT( ContainerData, Data )

            ContainerData();

            virtual bool ShouldSerialize() HELIUM_OVERRIDE;

            virtual size_t GetSize() const = 0;
            virtual void Clear() = 0;
        };

        typedef Helium::StrongPtr<ContainerData> ContainerDataPtr;
    }
}