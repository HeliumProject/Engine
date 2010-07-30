#pragma once

#include "ExportBase.h"
#include "Foundation/Reflect/Element.h"
#include "Pipeline/Content/Scene.h"

namespace Helium
{
    namespace MayaContent
    {
        typedef std::set< Reflect::Element* > S_ElementDumbPtr;

        class MAYA_API ExportScene : public ExportBase
        {
        public:
            Content::Scene m_ContentScene;

            V_ExportBase m_ExportObjects;

            Reflect::V_Element m_Spool;

            void Reset()
            {
                m_ExportObjects.clear();
                m_Spool.clear();
                m_ContentScene.Reset();
            }

            // Gather the necessary maya data
            void GatherMayaData();

            // Process the Content data
            void ProcessMayaData();

            // Export the gathered data as the appropriate Content object
            void ExportData();

            // Add an export object for the export
            void Add( const ExportBasePtr &node );

        private:
            void ExportNode( Content::SceneNode* node, S_ElementDumbPtr& duplicateCheck ); 
        };

        typedef Helium::SmartPtr<ExportScene> ExportScenePtr;
    }
}