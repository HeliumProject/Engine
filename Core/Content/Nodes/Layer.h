#pragma once

#include "Core/API.h"
#include "Core/Content/Nodes/SceneNode.h"

namespace Helium
{
    namespace Content
    {
        class CORE_API Layer : public SceneNode
        {
        public:
            bool                m_Visible;
            bool                m_Selectable;
            Helium::V_TUID   m_Members;
            Math::Color3        m_Color;

            Layer();
            Layer( Helium::TUID& id );

            REFLECT_DECLARE_CLASS( Layer, SceneNode );
            static void EnumerateClass( Reflect::Compositor<Layer>& comp );
        };

        typedef Helium::SmartPtr< Layer > LayerPtr;
        typedef std::vector< LayerPtr > V_Layer;
    }
}