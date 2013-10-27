#pragma once

#include "Math/EulerAngles.h"

#include "EditorScene/Manipulator.h"
#include "EditorScene/HierarchyNode.h"

namespace Helium
{
	namespace Editor
	{
		class Scene;
		class Transform;
		typedef Helium::StrongPtr< Editor::Transform > TransformPtr;

		class HELIUM_EDITOR_SCENE_API Transform : public HierarchyNode
		{
		public:
			HELIUM_DECLARE_CLASS( Transform, HierarchyNode );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

		public:
			Transform();
			~Transform();

			virtual void Initialize() HELIUM_OVERRIDE;

			virtual Editor::Transform* GetTransform() HELIUM_OVERRIDE;
			virtual const Editor::Transform* GetTransform() const HELIUM_OVERRIDE;

			//
			// Scale
			//

			virtual Scale GetScale() const;
			virtual void SetScale( const Scale& value );

			virtual Vector3 GetScalePivot() const;
			virtual void SetScalePivot( const Vector3& value );

			//
			// Rotate
			//

			virtual EulerAngles GetRotate() const;
			virtual void SetRotate( const EulerAngles& value );

			virtual Vector3 GetRotatePivot() const;
			virtual void SetRotatePivot( const Vector3& value );

			//
			// Translate
			//

			virtual Vector3 GetTranslate() const;
			virtual void SetTranslate( const Vector3& value );

			virtual Vector3 GetTranslatePivot() const;
			virtual void SetTranslatePivot( const Vector3& value );

			//
			// Object Transform (transformation of object coordinates into local space)
			//

			Matrix4 GetObjectTransform() const
			{
				return m_ObjectTransform;
			}

			Matrix4 GetInverseObjectTransform() const
			{
				return m_InverseObjectTransform;
			}

			void SetObjectTransform( const Matrix4& transform );

			//
			// Parent Transform (transformation of the frame this object is within into world space)
			//

			Matrix4 GetParentTransform() const
			{
				return m_InverseObjectTransform * m_GlobalTransform;
			}

			Matrix4 GetInverseParentTransform() const
			{
				return m_InverseGlobalTransform * m_ObjectTransform;
			}

			//
			// Global Transform (transformation of object coordinates into world space)
			//

			Matrix4 GetGlobalTransform() const
			{
				return m_GlobalTransform;
			}

			Matrix4 GetInverseGlobalTransform() const
			{
				return m_InverseGlobalTransform;
			}

			void SetGlobalTransform( const Matrix4& transform );

			//
			// Binding Matrices
			//

			virtual Matrix4 GetBindTransform() const;
			virtual Matrix4 GetInverseBindTransform() const;

			//
			// Inherit Transform
			//

			bool GetInheritTransform() const;
			void SetInheritTransform(bool inherit);

		public:
			// compute scaling component
			virtual Matrix4 GetScaleComponent() const;

			// compute rotation component
			virtual Matrix4 GetRotateComponent() const;

			// compute translation component
			virtual Matrix4 GetTranslateComponent() const;

			// resets transform to identity
			virtual UndoCommandPtr ResetTransform();

			// recomputes local components from the global matrix
			virtual UndoCommandPtr ComputeObjectComponents();

			// centers the pivot points
			virtual UndoCommandPtr CenterTransform();

			// compute all member matrices
			virtual void Evaluate( GraphDirection direction ) HELIUM_OVERRIDE;

			// render to viewport
			virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;

			// do intersection testing
			virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

			// manipulator support
			virtual void ConnectManipulator(ManiuplatorAdapterCollection* collection) HELIUM_OVERRIDE;

		protected:
			// Reflected
			Scale         m_Scale;
			EulerAngles   m_Rotate;
			Vector3       m_Translate;
			Matrix4       m_ObjectTransform;
			Matrix4       m_GlobalTransform;
			bool          m_InheritTransform;     // Do we transform with our parent?

			// Non-reflected
			Matrix4       m_InverseObjectTransform;
			Matrix4       m_InverseGlobalTransform;
			bool          m_BindIsDirty;
			Matrix4       m_BindTransform;
			Matrix4       m_InverseBindTransform;
		};

		class TransformScaleManipulatorAdapter : public ScaleManipulatorAdapter
		{
		protected:
			Editor::Transform* m_Transform;

		public:
			TransformScaleManipulatorAdapter(Editor::Transform* transform)
				: m_Transform (transform)
			{

			}

			virtual Editor::HierarchyNode* GetNode() HELIUM_OVERRIDE
			{
				return m_Transform;
			}

			virtual Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
			virtual Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
			virtual Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

			virtual Vector3 GetPivot() HELIUM_OVERRIDE
			{
				return m_Transform->GetScalePivot();
			}

			virtual Scale GetValue() HELIUM_OVERRIDE
			{
				return m_Transform->GetScale();
			}

			virtual UndoCommandPtr SetValue(const Scale& v) HELIUM_OVERRIDE
			{
				return new PropertyUndoCommand<Scale> ( new Helium::MemberProperty<Editor::Transform, Scale> (m_Transform, &Editor::Transform::GetScale, &Editor::Transform::SetScale), v );
			}
		};

		class TransformScalePivotManipulatorAdapter : public TranslateManipulatorAdapter
		{
		protected:
			Editor::Transform* m_Transform;

		public:
			TransformScalePivotManipulatorAdapter(Editor::Transform* transform)
				: m_Transform (transform)
			{

			}

			virtual Editor::HierarchyNode* GetNode() HELIUM_OVERRIDE
			{
				return m_Transform;
			}

			virtual bool AllowSelfSnap() HELIUM_OVERRIDE
			{
				return true;
			}

			virtual Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
			virtual Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
			virtual Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

			virtual Vector3 GetPivot() HELIUM_OVERRIDE
			{
				return Vector3::Zero;
			}

			virtual Vector3 GetValue() HELIUM_OVERRIDE
			{
				return m_Transform->GetScalePivot();
			}

			virtual UndoCommandPtr SetValue(const Vector3& v) HELIUM_OVERRIDE
			{
				return new PropertyUndoCommand<Vector3> ( new Helium::MemberProperty<Editor::Transform, Vector3> (m_Transform, &Editor::Transform::GetScalePivot, &Editor::Transform::SetScalePivot), v );
			}
		};

		class TransformRotateManipulatorAdapter : public RotateManipulatorAdapter
		{
		protected:
			Editor::Transform* m_Transform;

		public:
			TransformRotateManipulatorAdapter(Editor::Transform* transform)
				: m_Transform (transform)
			{

			}

			virtual Editor::HierarchyNode* GetNode() HELIUM_OVERRIDE
			{
				return m_Transform;
			}

			virtual Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
			virtual Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
			virtual Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

			virtual Vector3 GetPivot() HELIUM_OVERRIDE
			{
				return m_Transform->GetRotatePivot();
			}

			virtual EulerAngles GetValue() HELIUM_OVERRIDE
			{
				return m_Transform->GetRotate();
			}

			virtual UndoCommandPtr SetValue(const EulerAngles& v) HELIUM_OVERRIDE
			{
				return new PropertyUndoCommand<EulerAngles> ( new Helium::MemberProperty<Editor::Transform, EulerAngles> (m_Transform, &Editor::Transform::GetRotate, &Editor::Transform::SetRotate), v );
			}
		};

		class TransformRotatePivotManipulatorAdapter : public TranslateManipulatorAdapter
		{
		protected:
			Editor::Transform* m_Transform;

		public:
			TransformRotatePivotManipulatorAdapter(Editor::Transform* transform)
				: m_Transform (transform)
			{

			}

			virtual Editor::HierarchyNode* GetNode() HELIUM_OVERRIDE
			{
				return m_Transform;
			}

			virtual bool AllowSelfSnap() HELIUM_OVERRIDE
			{
				return true;
			}

			virtual Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
			virtual Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
			virtual Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

			virtual Vector3 GetPivot() HELIUM_OVERRIDE
			{
				return Vector3::Zero;
			}

			virtual Vector3 GetValue() HELIUM_OVERRIDE
			{
				return m_Transform->GetRotatePivot();
			}

			virtual UndoCommandPtr SetValue(const Vector3& v) HELIUM_OVERRIDE
			{
				return new PropertyUndoCommand<Vector3> ( new Helium::MemberProperty<Editor::Transform, Vector3> (m_Transform, &Editor::Transform::GetRotatePivot, &Editor::Transform::SetRotatePivot), v );
			}
		};

		class TransformTranslateManipulatorAdapter : public TranslateManipulatorAdapter
		{
		protected:
			Editor::Transform* m_Transform;

		public:
			TransformTranslateManipulatorAdapter(Editor::Transform* transform)
				: m_Transform (transform)
			{

			}

			virtual Editor::HierarchyNode* GetNode() HELIUM_OVERRIDE
			{
				return m_Transform;
			}

			virtual Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
			virtual Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
			virtual Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

			virtual Vector3 GetPivot() HELIUM_OVERRIDE
			{
				return m_Transform->GetTranslatePivot();
			}

			virtual Vector3 GetValue() HELIUM_OVERRIDE
			{
				return m_Transform->GetTranslate();
			}

			virtual UndoCommandPtr SetValue(const Vector3& v) HELIUM_OVERRIDE
			{
				return new PropertyUndoCommand<Vector3> ( new Helium::MemberProperty<Editor::Transform, Vector3> (m_Transform, &Editor::Transform::GetTranslate, &Editor::Transform::SetTranslate), v );
			}
		};

		class TransformTranslatePivotManipulatorAdapter : public TranslateManipulatorAdapter
		{
		protected:
			Editor::Transform* m_Transform;

		public:
			TransformTranslatePivotManipulatorAdapter(Editor::Transform* transform)
				: m_Transform (transform)
			{

			}

			virtual Editor::HierarchyNode* GetNode() HELIUM_OVERRIDE
			{
				return m_Transform;
			}

			virtual bool AllowSelfSnap() HELIUM_OVERRIDE
			{
				return true;
			}

			virtual Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
			virtual Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
			virtual Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

			virtual Vector3 GetPivot() HELIUM_OVERRIDE
			{
				return Vector3::Zero;
			}

			virtual Vector3 GetValue() HELIUM_OVERRIDE
			{
				return m_Transform->GetTranslatePivot();
			}

			virtual UndoCommandPtr SetValue(const Vector3& v) HELIUM_OVERRIDE
			{
				return new PropertyUndoCommand<Vector3> ( new Helium::MemberProperty<Editor::Transform, Vector3> (m_Transform, &Editor::Transform::GetTranslatePivot, &Editor::Transform::SetTranslatePivot), v );
			}
		};
	}
}