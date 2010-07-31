#pragma once

#include "PrimitiveRadius.h"

namespace Helium
{
    namespace Editor
    {
        class PrimitiveCircle : public Editor::PrimitiveRadius
        {

        public:

            bool m_HackyRotateFlag;

            PrimitiveCircle(ResourceTracker* tracker);

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual void DrawFill( DrawArgs* args ) const;
            virtual void DrawHiddenBack( DrawArgs* args, const Editor::Camera* camera, const Math::Matrix4& m ) const;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}