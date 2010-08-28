#pragma once

#include "Core/API.h"
#include "InstancePanel.h"

#include "Application/UI/FileDialog.h"
#include "Application/Inspect/InspectData.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectInterpreter.h"

#ifdef INSPECT_REFACTOR
#include "Application/Inspect/DragDrop/FilteredDropTarget.h"
#endif

#include "EntityInstance.h"

namespace Helium
{
    namespace Core
    {
        class EntityPanel : public InstancePanel
        {
        protected:
            Inspect::Value*     m_EntityPath;

        public:
            EntityPanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection);
 
        protected:
            // UI callbacks
            bool OnEntityAssetChanging( const Inspect::ControlChangingArgs& args );
            void OnEntityAssetChanged( const Inspect::ControlChangedArgs& args );
            void OnEntityAssetRefresh( const Inspect::ButtonClickedArgs& args );
            void OnEntityAssetEditAsset( const Inspect::ButtonClickedArgs& args );
            void OnEntityAssetEditArt( const Inspect::ButtonClickedArgs& args );
#if INSPECT_REFACTOR
            void OnEntityAssetDrop( const Inspect::FilteredDropTargetArgs& args );
#endif
        };
    }
}