#pragma once

#include <set>

#include "Core/Scene/Object.h"

#include "Editor/API.h"
#include "Platform/Types.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Manages the state of a scene outliner so that it can be saved and restored.
        // The state includes which nodes are expanded, and the postions of the scroll
        // bars in the tree control.  Currently, this is used so that there only needs
        // to be one outliner for a scene, and when that scene is no longer the current
        // one, it's information can be saved and restored later.
        // 
        class SceneOutlinerState
        {
        protected:
            i32 m_HorizScrollPos;
            i32 m_VertScrollPos;
            Core::S_Objects m_ExpandedObjects;

        public:
            SceneOutlinerState();
            virtual ~SceneOutlinerState();
            void Reset();

            i32 GetHorizontalScrollBarPos() const;
            void SetHorizontalScrollBarPos( i32 pos );
            i32 GetVerticalScrollBarPos() const;
            void SetVerticalScrollBarPos( i32 pos );
            void AddExpandedObject( Core::Object* object );
            void RemoveExpandedObject( Core::Object* object );
            bool IsExpanded( Core::Object* object );
            const Core::S_Objects& GetExpandedObjects() const;
        };
    }
}