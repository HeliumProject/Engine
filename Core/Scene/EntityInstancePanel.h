#pragma once

#include "Core/API.h"
#include "InstancePanel.h"

#include "Foundation/Inspect/Data.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectInterpreter.h"

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
            void OnEntityAssetChanging( const Inspect::ControlChangingArgs& args );
            void OnEntityAssetChanged( const Inspect::ControlChangedArgs& args );
            void OnEntityAssetRefresh( const Inspect::ButtonClickedArgs& args );
            void OnEntityAssetEditAsset( const Inspect::ButtonClickedArgs& args );
            void OnEntityAssetEditArt( const Inspect::ButtonClickedArgs& args );
        };
    }
}