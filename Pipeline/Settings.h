#pragma once

#include "Pipeline/API.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    class PIPELINE_API Settings : public Reflect::Element
    {
    public:
        Settings()
        {
        }

        virtual ~Settings()
        {
        }

        virtual bool UserVisible()
        {
            return true;
        }

        REFLECT_DECLARE_CLASS( Settings, Reflect::Element );
    };

    typedef Helium::StrongPtr< Settings > SettingsPtr;
}