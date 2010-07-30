#pragma once

#include "Instance.h"

#include "Pipeline/Content/Nodes/Volume.h"

namespace Helium
{
    namespace Editor
    {
        class VolumeType;

        /////////////////////////////////////////////////////////////////////////////
        // Editor's wrapper for an volume instance.
        // 
        class Volume : public Editor::Instance
        {
            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Editor::Volume, Editor::Instance );
            static void InitializeType();
            static void CleanupType();


            //
            // Member functions
            //

            Volume(Editor::Scene* s);
            Volume(Editor::Scene* s, Content::Volume* volume);
            virtual ~Volume();

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;
            virtual SceneNodeTypePtr CreateNodeType( Editor::Scene* scene ) const HELIUM_OVERRIDE;

            int GetShape() const;
            void SetShape( int shape );

            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;

            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
            static void DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;

            static void CreatePanel( CreatePanelArgs& args );

            //
            // Should we show the pointer
            //

            bool IsPointerVisible() const;
            void SetPointerVisible(bool visible);
        };

        typedef Helium::SmartPtr<Editor::Volume> VolumePtr;
        typedef std::vector< Editor::Volume* > V_VolumeDumbPtr;
    }
}