#pragma once

#include "Pipeline/API.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    class PIPELINE_API Settings : public Reflect::Object
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

        REFLECT_DECLARE_CLASS( Settings, Reflect::Object );
    };

    typedef Helium::StrongPtr< Settings > SettingsPtr;
}