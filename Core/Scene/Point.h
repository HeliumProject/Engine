#pragma once

#include "HierarchyNode.h"

#include "Core/Content/Nodes/Point.h"
#include "Core/Scene/Manipulator.h"

namespace Helium
{
    namespace Core
    {
        class Point : public Core::HierarchyNode
        {
            //
            // Runtime Type Info
            //

        public:
            SCENE_DECLARE_TYPE( Core::Point, Core::HierarchyNode );
            static void InitializeType();
            static void CleanupType();


            // 
            // Member functions
            // 

        public:
            Point(Core::Scene* scene, Content::Point* data);
            virtual ~Point();

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            const Math::Vector3& GetPosition() const;
            void SetPosition( const Math::Vector3& value );

            f32 GetPositionX() const;
            void SetPositionX( f32 value );

            f32 GetPositionY() const;
            void SetPositionY( f32 value );

            f32 GetPositionZ() const;
            void SetPositionZ( f32 value );

            virtual void ConnectManipulator( ManiuplatorAdapterCollection* collection ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;
            virtual void Evaluate( GraphDirection direction ) HELIUM_OVERRIDE;
            virtual bool ValidatePanel( const tstring& name ) HELIUM_OVERRIDE;
            static void CreatePanel( CreatePanelArgs& args );
        };

        class PointTranslateManipulatorAdapter : public TranslateManipulatorAdapter
        {
        protected:
            Core::Point* m_Point;

        public:
            PointTranslateManipulatorAdapter( Core::Point* pointComponent )
                :m_Point( pointComponent )
            {
                HELIUM_ASSERT( m_Point );
            }

            virtual Core::HierarchyNode* GetNode() override
            {
                return m_Point;
            }

            virtual Math::Matrix4 GetFrame(ManipulatorSpace space) override;
            virtual Math::Matrix4 GetObjectMatrix() override;
            virtual Math::Matrix4 GetParentMatrix() override;

            virtual Math::Vector3 GetPivot() override
            {
                return m_Point->GetPosition();
            }

            virtual Math::Vector3 GetValue() override
            {
                return m_Point->GetPosition();
            }

            virtual Undo::CommandPtr SetValue( const Math::Vector3& v ) override
            {
                return new Undo::PropertyCommand<Math::Vector3> ( new Helium::MemberProperty<Core::Point, Math::Vector3> (m_Point, &Core::Point::GetPosition, &Core::Point::SetPosition), v);
            }
        };

        typedef Helium::SmartPtr<Core::Point> PointPtr;
        typedef std::vector<PointPtr> V_Point;
    }
}