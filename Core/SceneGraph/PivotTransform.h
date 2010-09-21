#pragma once

#include "Core/API.h"
#include "Core/SceneGraph/Transform.h"

namespace Helium
{
    namespace SceneGraph
    {
        //  TOTAL TRANSFORM:
        //     -1                      -1
        //  [Sp]x[S]x[Sh]x[Sp]x[St]x[Rp]x[R]x[Rp]x[Rt]x[T]x[Tp]

        class CORE_API PivotTransform : public Transform
        {
        public:
            REFLECT_DECLARE_CLASS( SceneGraph::PivotTransform, SceneGraph::Transform );
            static void EnumerateClass( Reflect::Compositor<PivotTransform>& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            PivotTransform();

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual bool IsGroup()
            {
                return GetType() == Reflect::GetType<SceneGraph::PivotTransform>();
            }

            //
            // Shear
            //

            Math::Shear GetShear() const;
            void SetShear(const Math::Shear& value);

            //
            // ScalePivot
            //

            virtual Math::Vector3 GetScalePivot() const HELIUM_OVERRIDE;
            virtual void SetScalePivot(const Math::Vector3& value) HELIUM_OVERRIDE;
            void SetScalePivot(const Math::Vector3& value, bool snapSiblings);

            //
            // ScalePivotTranslate
            //

            Math::Vector3 GetScalePivotTranslate() const;
            void SetScalePivotTranslate(const Math::Vector3& value);

            //
            // RotatePivot
            //

            virtual Math::Vector3 GetRotatePivot() const HELIUM_OVERRIDE;
            virtual void SetRotatePivot(const Math::Vector3& value) HELIUM_OVERRIDE;
            void SetRotatePivot(const Math::Vector3& value, bool snapSiblings);

            //
            // RotatePivotTranslate
            //

            Math::Vector3 GetRotatePivotTranslate() const;
            void SetRotatePivotTranslate(const Math::Vector3& value);

            //
            // TranslatePivot
            //

            virtual Math::Vector3 GetTranslatePivot() const HELIUM_OVERRIDE;
            virtual void SetTranslatePivot(const Math::Vector3& value) HELIUM_OVERRIDE;
            void SetTranslatePivot(const Math::Vector3& value, bool snapSiblings);

            //
            // SnapPivots
            //

            bool GetSnapPivots() const;
            void SetSnapPivots(bool value);

        public:
            virtual Math::Matrix4 GetScaleComponent() const HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetRotateComponent() const HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetTranslateComponent() const HELIUM_OVERRIDE;

            // resets transform to identity
            virtual Undo::CommandPtr ResetTransform() HELIUM_OVERRIDE;

            // recomputes local components from the global matrix
            virtual Undo::CommandPtr ComputeObjectComponents() HELIUM_OVERRIDE;

            // repositions group nodes WRT thier children
            virtual Undo::CommandPtr CenterTransform() HELIUM_OVERRIDE;

            // ui integration
            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;

            // creator
            static void CreatePanel(CreatePanelArgs& args);

            f32 GetShearYZ() const;
            void SetShearYZ(f32 translate);
            f32 GetShearXZ() const;
            void SetShearXZ(f32 translate);
            f32 GetShearXY() const;
            void SetShearXY(f32 translate);

            f32 GetScalePivotX() const;
            void SetScalePivotX(f32 translate);
            f32 GetScalePivotY() const;
            void SetScalePivotY(f32 translate);
            f32 GetScalePivotZ() const;
            void SetScalePivotZ(f32 translate);

            f32 GetScalePivotTranslateX() const;
            void SetScalePivotTranslateX(f32 translate);
            f32 GetScalePivotTranslateY() const;
            void SetScalePivotTranslateY(f32 translate);
            f32 GetScalePivotTranslateZ() const;
            void SetScalePivotTranslateZ(f32 translate);

            f32 GetRotatePivotX() const;
            void SetRotatePivotX(f32 translate);
            f32 GetRotatePivotY() const;
            void SetRotatePivotY(f32 translate);
            f32 GetRotatePivotZ() const;
            void SetRotatePivotZ(f32 translate);

            f32 GetRotatePivotTranslateX() const;
            void SetRotatePivotTranslateX(f32 translate);
            f32 GetRotatePivotTranslateY() const;
            void SetRotatePivotTranslateY(f32 translate);
            f32 GetRotatePivotTranslateZ() const;
            void SetRotatePivotTranslateZ(f32 translate);

            f32 GetTranslatePivotX() const;
            void SetTranslatePivotX(f32 translate);
            f32 GetTranslatePivotY() const;
            void SetTranslatePivotY(f32 translate);
            f32 GetTranslatePivotZ() const;
            void SetTranslatePivotZ(f32 translate);

        protected:
            Math::Shear         m_Shear;                    // shear values
            Math::Vector3       m_ScalePivot;               // scale pivot translation vector
            Math::Vector3       m_ScalePivotTranslate;      // compensation vector for preserving the transform when the scale pivot it moved
            Math::Vector3       m_RotatePivot;              // rotation pivot translation vector
            Math::Vector3       m_RotatePivotTranslate;     // compensation vector for preserving the transform when the rotation pivot it moved
            Math::Vector3       m_TranslatePivot;           // translate pivot translation vector
            bool                m_SnapPivots;               // if this is true, use m_RotatePivot
        };

        typedef Helium::SmartPtr<SceneGraph::PivotTransform> LPivotTransformPtr;
    }
}