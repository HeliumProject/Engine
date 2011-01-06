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
            : m_UserVisible( true )
        {
        }

        virtual ~Settings()
        {
        }

        REFLECT_DECLARE_CLASS( Settings, Reflect::Element );
        static void AcceptCompositeVisitor( Reflect::Composite& comp );

    public:
        bool m_UserVisible;
    };

    typedef Helium::StrongPtr< Settings > SettingsPtr;
}