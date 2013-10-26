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
            HELIUM_DECLARE_CLASS( CurveControlPoint, HierarchyNode );
            static void PopulateMetaType( Reflect::MetaStruct& comp );

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

            virtual HierarchyNode* GetNode() HELIUM_OVERRIDE
            {
                return m_Point;
            }

            virtual Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
            virtual Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
            virtual Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

            virtual Vector3 GetPivot() HELIUM_OVERRIDE
            {
                return m_Point->GetPosition();
            }

            virtual Vector3 GetValue() HELIUM_OVERRIDE
            {
                return m_Point->GetPosition();
            }

            virtual UndoCommandPtr SetValue( const Vector3& v ) HELIUM_OVERRIDE
            {
                return new PropertyUndoCommand<Vector3> ( new Helium::MemberProperty<CurveControlPoint, Vector3> (m_Point, &CurveControlPoint::GetPosition, &CurveControlPoint::SetPosition), v);
            }
        };

        typedef Helium::StrongPtr<CurveControlPoint> PointPtr;
        typedef std::vector<PointPtr> V_Point;
    }
}