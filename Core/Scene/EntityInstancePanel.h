#pragma once

#include "Core/API.h"
#include "InstancePanel.h"

#include "Application/RCS/RCS.h"
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
            virtual ~EntityPanel();
            virtual void Create() HELIUM_OVERRIDE;

        protected:
            // UI callbacks
            bool OnEntityAssetChanging( const Inspect::ControlChangingArgs& args );
            void OnEntityAssetChanged( const Inspect::ControlChangedArgs& args );
            void OnEntityAssetRefresh( Inspect::Button* button );
            void OnEntityAssetEditAsset( Inspect::Button* button );
            void OnEntityAssetEditArt( Inspect::Button* button );
            void OnEntityAssetDrop( const Inspect::FilteredDropTargetArgs& args );
        };
    }
}