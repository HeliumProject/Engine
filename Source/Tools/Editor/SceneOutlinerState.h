#pragma once

#include <set>

#include "Reflect/Object.h"

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
            int32_t m_HorizScrollPos;
            int32_t m_VertScrollPos;
            std::set< Reflect::Object* > m_ExpandedObjects;

        public:
            SceneOutlinerState();
            virtual ~SceneOutlinerState();
            void Reset();

            int32_t GetHorizontalScrollBarPos() const;
            void SetHorizontalScrollBarPos( int32_t pos );
            int32_t GetVerticalScrollBarPos() const;
            void SetVerticalScrollBarPos( int32_t pos );
            void AddExpandedObject( Reflect::Object* object );
            void RemoveExpandedObject( Reflect::Object* object );
            bool IsExpanded( Reflect::Object* object );
            const std::set< Reflect::Object* >& GetExpandedObjects() const;
        };
    }
}