#pragma once

#include "HierarchyNode.h"

#include "Core/Content/Nodes/Point.h"
#include "Editor/Scene/Manipulator.h"

namespace Helium
{
    namespace Editor
    {
        class Point : public Editor::HierarchyNode
        {
            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Editor::Point, Editor::HierarchyNode );
            static void InitializeType();
            static void CleanupType();


            // 
            // Member functions
            // 

        public:
            Point(Editor::Scene* scene, Content::Point* data);
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
            Editor::Point* m_Point;

        public:
            PointTranslateManipulatorAdapter( Editor::Point* pointComponent )
                :m_Point( pointComponent )
            {
                HELIUM_ASSERT( m_Point );
            }

            virtual Editor::HierarchyNode* GetNode() override
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
                return new Undo::PropertyCommand<Math::Vector3> ( new Helium::MemberProperty<Editor::Point, Math::Vector3> (m_Point, &Editor::Point::GetPosition, &Editor::Point::SetPosition), v);
            }
        };

        typedef Helium::SmartPtr<Editor::Point> PointPtr;
        typedef std::vector<PointPtr> V_Point;
    }
}