#pragma once

#include "Core/API.h"
#include "Core/Content/Nodes/PivotTransform.h"

namespace Helium
{
    namespace Content
    {
        class CORE_API Instance HELIUM_ABSTRACT : public PivotTransform
        {
        private:
            REFLECT_DECLARE_ABSTRACT(Instance, PivotTransform);
            static void EnumerateClass( Reflect::Compositor<Instance>& comp );

        public:
            Instance ();
            Instance (const Helium::TUID& id);

        public:
            // Visibility preferences for this instance
            bool  m_Solid;
            bool  m_SolidOverride;
            bool  m_Transparent;
            bool  m_TransparentOverride;
        };

        typedef Helium::SmartPtr<Instance> InstancePtr;
        typedef std::vector<InstancePtr> V_Instance;
    }
}