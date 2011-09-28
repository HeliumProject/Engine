#pragma once

#include "HierarchyNode.h"

#include "SceneGraph/Manipulator.h"

namespace Helium
{
    namespace SceneGraph
    {
        class CurveControlPoint : public HierarchyNode
        {
        public:
            REFLECT_DECLARE_OBJECT( CurveControlPoint, HierarchyNode );
            static void PopulateComposite( Reflect::Composite& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            CurveControlPoint();
            ~CurveControlPoint();

			const Vector3& GetPosition() const;
			void SetPosition( const Vector3& value );

            virtual void ConnectManipulator( ManiuplatorAdapterCollection* collection ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;
            virtual void Evaluate( GraphDirection direction ) HELIUM_OVERRIDE;

		private:
            Vector3 m_Position;
        };

        typedef StrongPtr< CurveControlPoint > CurveControlPointPtr;

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

            virtual UndoCommandPtr SetValue( const Vector3& v ) override
            {
                return new PropertyUndoCommand<Vector3> ( new Helium::MemberProperty<CurveControlPoint, Vector3> (m_Point, &CurveControlPoint::GetPosition, &CurveControlPoint::SetPosition), v);
            }
        };

        typedef Helium::StrongPtr<CurveControlPoint> PointPtr;
        typedef std::vector<PointPtr> V_Point;
    }
}