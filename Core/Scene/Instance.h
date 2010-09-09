#pragma once

#include "PivotTransform.h"

#include "Core/Content/Nodes/ContentInstance.h"

namespace Helium
{
    namespace Core
    {
        class InstanceCodeSet;
        class Instance;

        struct InstancePropertiesChangeArgs
        {
            Core::Instance* m_Instance;
            tstring     m_OldType;
            tstring     m_NewType;

            InstancePropertiesChangeArgs( Core::Instance*    instance,
                const tstring& oldType,
                const tstring& newType)
                : m_Instance( instance )
                , m_OldType (oldType)
                , m_NewType (newType)
            {}
        };
        typedef Helium::Signature< const InstancePropertiesChangeArgs& > InstancePropertiesChangeSignature;

        /////////////////////////////////////////////////////////////////////////////
        // Editor's wrapper for an entity instance.
        // 
        class CORE_API Instance HELIUM_ABSTRACT : public Core::PivotTransform
        {
            //
            // Members
            //

        protected:

            //
            // Runtime Type Info
            //

        public:
            SCENE_DECLARE_TYPE( Core::Instance, Core::PivotTransform );
            static void InitializeType();
            static void CleanupType();


            //
            // Member functions
            //

            Instance(Core::Scene* s, Content::Instance* entity);
            virtual ~Instance();

            virtual void Pack() HELIUM_OVERRIDE;
            virtual void Unpack() HELIUM_OVERRIDE;

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual SceneNodeTypePtr CreateNodeType( Core::Scene* scene ) const HELIUM_OVERRIDE;

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

        typedef std::vector< Core::Instance* > V_InstanceDumbPtr;
    }
}