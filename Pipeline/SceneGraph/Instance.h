#pragma once

#include "Pipeline/SceneGraph/PivotTransform.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Instance;

        class PIPELINE_API Instance HELIUM_ABSTRACT : public PivotTransform
        {
        public:
            REFLECT_DECLARE_ABSTRACT( Instance, PivotTransform );
            static void EnumerateClass( Reflect::Compositor<Instance>& comp );
            static void InitializeType();
            static void CleanupType();

            Instance();
            ~Instance();

            virtual int32_t GetImageIndex() const HELIUM_OVERRIDE;
            virtual SceneNodeTypePtr CreateNodeType( Scene* scene ) const HELIUM_OVERRIDE;

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

        protected:
            bool  m_Solid;
            bool  m_SolidOverride;
            bool  m_Transparent;
            bool  m_TransparentOverride;
        };

        typedef std::vector< Instance* > V_InstanceDumbPtr;
    }
}