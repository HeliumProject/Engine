#pragma once

#include "Pipeline/API.h"
#include "InstancePanel.h"

#include "Foundation/Inspect/DataBinding.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectInterpreter.h"

#include "EntityInstance.h"

namespace Helium
{
    namespace SceneGraph
    {
        class EntityPanel : public InstancePanel
        {
        protected:
            Inspect::Value*     m_EntityPath;

        public:
            EntityPanel(PropertiesGenerator* generator, const OS_SceneNodeDumbPtr& selection);
 
        protected:
            // UI callbacks
            void OnEntityAssetChanging( const Inspect::ControlChangingArgs& args );
            void OnEntityAssetChanged( const Inspect::ControlChangedArgs& args );
            void OnEntityAssetRefresh( const Inspect::ButtonClickedArgs& args );
            void OnEntityAssetEditAsset( const Inspect::ButtonClickedArgs& args );
            void OnEntityAssetEditArt( const Inspect::ButtonClickedArgs& args );
        };
    }
}