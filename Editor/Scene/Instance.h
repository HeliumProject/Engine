#pragma once

#include "PivotTransform.h"

#include "Pipeline/Content/Nodes/Instance.h"

namespace Helium
{
    namespace Editor
    {
        class InstanceCodeSet;
        class Instance;

        struct InstancePropertiesChangeArgs
        {
            Editor::Instance* m_Instance;
            tstring     m_OldType;
            tstring     m_NewType;

            InstancePropertiesChangeArgs( Editor::Instance*    instance,
                const tstring& oldType,
                const tstring& newType)
                : m_Instance( instance )
                , m_OldType (oldType)
                , m_NewType (newType)
            {}
        };
        typedef Helium::Signature< void, const InstancePropertiesChangeArgs& > InstancePropertiesChangeSignature;

        /////////////////////////////////////////////////////////////////////////////
        // Editor's wrapper for an entity instance.
        // 
        class LUNA_SCENE_API Instance HELIUM_ABSTRACT : public Editor::PivotTransform
        {
            //
            // Members
            //

        protected:

            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Editor::Instance, Editor::PivotTransform );
            static void InitializeType();
            static void CleanupType();


            //
            // Member functions
            //

            Instance(Editor::Scene* s, Content::Instance* entity);
            virtual ~Instance();

            virtual void Pack() HELIUM_OVERRIDE;
            virtual void Unpack() HELIUM_OVERRIDE;

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual SceneNodeTypePtr CreateNodeType( Editor::Scene* scene ) const HELIUM_OVERRIDE;

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;
            static void CreatePanel( CreatePanelArgs& args );

            bool GetSolid() const;
            void SetSolid( bool b );
            bool GetSolidOverride() const;
            void SetSolidOverride( bool b );

            bool GetTransparent() const;
            void SetTransparent( bool b );
            bool GetTransparentOverride() const;
            void SetTransparentOverride( bool b );
        };

        typedef std::vector< Editor::Instance* > V_InstanceDumbPtr;
    }
}