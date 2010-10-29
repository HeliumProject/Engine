#pragma once

#include "HierarchyNode.h"

#include "Core/SceneGraph/Manipulator.h"

namespace Helium
{
    namespace SceneGraph
    {
        class CurveControlPoint : public HierarchyNode
        {
        public:
            REFLECT_DECLARE_CLASS( CurveControlPoint, HierarchyNode );
            static void EnumerateClass( Reflect::Compositor<CurveControlPoint>& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            CurveControlPoint();
            ~CurveControlPoint();

            virtual int32_t GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            const Vector3& GetPosition() const;
            void SetPosition( const Vector3& value );

            float32_t GetPositionX() const;
            void SetPositionX( float32_t value );

            float32_t GetPositionY() const;
            void SetPositionY( float32_t value );

            float32_t GetPositionZ() const;
            void SetPositionZ( float32_t value );

            virtual void ConnectManipulator( ManiuplatorAdapterCollection* collection ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;
            virtual void Evaluate( GraphDirection direction ) HELIUM_OVERRIDE;
            virtual bool ValidatePanel( const tstring& name ) HELIUM_OVERRIDE;
            static void CreatePanel( CreatePanelArgs& args );

        protected:
            Vector3 m_Position;
        };

        typedef SmartPtr< CurveControlPoint > CurveControlPointPtr;

        class CurveControlPointTranslateManipulatorAdapter : public TranslateManipulatorAdapter
        {
        protected:
            CurveControlPoint* m_Point;

        public:
            CurveControlPointTranslateManipulatorAdapter( CurveControlPoint* point )
                : m_Point( point )
            {
                HELIUM_ASSERT( m_Point );
            }

            virtual HierarchyNode* GetNode() override
            {
                return m_Point;
            }

            virtual Matrix4 GetFrame(ManipulatorSpace space) override;
            virtual Matrix4 GetObjectMatrix() override;
            virtual Matrix4 GetParentMatrix() override;

            virtual Vector3 GetPivot() override
            {
                return m_Point->GetPosition();
            }

            virtual Vector3 GetValue() override
            {
                return m_Point->GetPosition();
            }

            virtual Undo::CommandPtr SetValue( const Vector3& v ) override
            {
                return new Undo::PropertyCommand<Vector3> ( new Helium::MemberProperty<CurveControlPoint, Vector3> (m_Point, &CurveControlPoint::GetPosition, &CurveControlPoint::SetPosition), v);
            }
        };

        typedef Helium::SmartPtr<CurveControlPoint> PointPtr;
        typedef std::vector<PointPtr> V_Point;
    }
}