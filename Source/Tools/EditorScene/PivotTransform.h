#pragma once

#include "EditorScene/API.h"
#include "EditorScene/Transform.h"

namespace Helium
{
	namespace Editor
	{
		//  TOTAL TRANSFORM:
		//     -1                      -1
		//  [Sp]x[S]x[Sh]x[Sp]x[St]x[Rp]x[R]x[Rp]x[Rt]x[T]x[Tp]

		class HELIUM_EDITOR_SCENE_API PivotTransform : public Transform
		{
		public:
			HELIUM_DECLARE_CLASS( Editor::PivotTransform, Editor::Transform );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

		public:
			PivotTransform();

			virtual bool IsGroup()
			{
				return GetMetaClass() == Reflect::GetMetaClass<Editor::PivotTransform>();
			}

			//
			// Shear
			//

			Shear GetShear() const;
			void SetShear(const Shear& value);

			//
			// ScalePivot
			//

			virtual Vector3 GetScalePivot() const override;
			virtual void SetScalePivot(const Vector3& value) override;
			void SetScalePivot(const Vector3& value, bool snapSiblings);

			//
			// ScalePivotTranslate
			//

			Vector3 GetScalePivotTranslate() const;
			void SetScalePivotTranslate(const Vector3& value);

			//
			// RotatePivot
			//

			virtual Vector3 GetRotatePivot() const override;
			virtual void SetRotatePivot(const Vector3& value) override;
			void SetRotatePivot(const Vector3& value, bool snapSiblings);

			//
			// RotatePivotTranslate
			//

			Vector3 GetRotatePivotTranslate() const;
			void SetRotatePivotTranslate(const Vector3& value);

			//
			// TranslatePivot
			//

			virtual Vector3 GetTranslatePivot() const override;
			virtual void SetTranslatePivot(const Vector3& value) override;
			void SetTranslatePivot(const Vector3& value, bool snapSiblings);

			//
			// SnapPivots
			//

			bool GetSnapPivots() const;
			void SetSnapPivots(bool value);

		public:
			virtual Matrix4 GetScaleComponent() const override;
			virtual Matrix4 GetRotateComponent() const override;
			virtual Matrix4 GetTranslateComponent() const override;

			// resets transform to identity
			virtual UndoCommandPtr ResetTransform() override;

			// recomputes local components from the global matrix
			virtual UndoCommandPtr ComputeObjectComponents() override;

			// repositions group nodes WRT thier children
			virtual UndoCommandPtr CenterTransform() override;

		protected:
			Shear         m_Shear;                    // shear values
			Vector3       m_ScalePivot;               // scale pivot translation vector
			Vector3       m_ScalePivotTranslate;      // compensation vector for preserving the transform when the scale pivot it moved
			Vector3       m_RotatePivot;              // rotation pivot translation vector
			Vector3       m_RotatePivotTranslate;     // compensation vector for preserving the transform when the rotation pivot it moved
			Vector3       m_TranslatePivot;           // translate pivot translation vector
			bool          m_SnapPivots;               // if this is true, use m_RotatePivot
		};

		typedef Helium::SmartPtr<Editor::PivotTransform> LPivotTransformPtr;
	}
}