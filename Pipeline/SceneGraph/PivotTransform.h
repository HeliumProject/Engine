#pragma once

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/Transform.h"

namespace Helium
{
    namespace SceneGraph
    {
        //  TOTAL TRANSFORM:
        //     -1                      -1
        //  [Sp]x[S]x[Sh]x[Sp]x[St]x[Rp]x[R]x[Rp]x[Rt]x[T]x[Tp]

        class PIPELINE_API PivotTransform : public Transform
        {
        public:
            REFLECT_DECLARE_CLASS( SceneGraph::PivotTransform, SceneGraph::Transform );
            static void AcceptCompositeVisitor( Reflect::Composite& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            PivotTransform();

            virtual int32_t GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual bool IsGroup()
            {
                return GetClass() == Reflect::GetClass<SceneGraph::PivotTransform>();
            }

            //
            // Shear
            //

            Shear GetShear() const;
            void SetShear(const Shear& value);

            //
            // ScalePivot
            //

            virtual Vector3 GetScalePivot() const HELIUM_OVERRIDE;
            virtual void SetScalePivot(const Vector3& value) HELIUM_OVERRIDE;
            void SetScalePivot(const Vector3& value, bool snapSiblings);

            //
            // ScalePivotTranslate
            //

            Vector3 GetScalePivotTranslate() const;
            void SetScalePivotTranslate(const Vector3& value);

            //
            // RotatePivot
            //

            virtual Vector3 GetRotatePivot() const HELIUM_OVERRIDE;
            virtual void SetRotatePivot(const Vector3& value) HELIUM_OVERRIDE;
            void SetRotatePivot(const Vector3& value, bool snapSiblings);

            //
            // RotatePivotTranslate
            //

            Vector3 GetRotatePivotTranslate() const;
            void SetRotatePivotTranslate(const Vector3& value);

            //
            // TranslatePivot
            //

            virtual Vector3 GetTranslatePivot() const HELIUM_OVERRIDE;
            virtual void SetTranslatePivot(const Vector3& value) HELIUM_OVERRIDE;
            void SetTranslatePivot(const Vector3& value, bool snapSiblings);

            //
            // SnapPivots
            //

            bool GetSnapPivots() const;
            void SetSnapPivots(bool value);

        public:
            virtual Matrix4 GetScaleComponent() const HELIUM_OVERRIDE;
            virtual Matrix4 GetRotateComponent() const HELIUM_OVERRIDE;
            virtual Matrix4 GetTranslateComponent() const HELIUM_OVERRIDE;

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

            float32_t GetShearYZ() const;
            void SetShearYZ(float32_t translate);
            float32_t GetShearXZ() const;
            void SetShearXZ(float32_t translate);
            float32_t GetShearXY() const;
            void SetShearXY(float32_t translate);

            float32_t GetScalePivotX() const;
            void SetScalePivotX(float32_t translate);
            float32_t GetScalePivotY() const;
            void SetScalePivotY(float32_t translate);
            float32_t GetScalePivotZ() const;
            void SetScalePivotZ(float32_t translate);

            float32_t GetScalePivotTranslateX() const;
            void SetScalePivotTranslateX(float32_t translate);
            float32_t GetScalePivotTranslateY() const;
            void SetScalePivotTranslateY(float32_t translate);
            float32_t GetScalePivotTranslateZ() const;
            void SetScalePivotTranslateZ(float32_t translate);

            float32_t GetRotatePivotX() const;
            void SetRotatePivotX(float32_t translate);
            float32_t GetRotatePivotY() const;
            void SetRotatePivotY(float32_t translate);
            float32_t GetRotatePivotZ() const;
            void SetRotatePivotZ(float32_t translate);

            float32_t GetRotatePivotTranslateX() const;
            void SetRotatePivotTranslateX(float32_t translate);
            float32_t GetRotatePivotTranslateY() const;
            void SetRotatePivotTranslateY(float32_t translate);
            float32_t GetRotatePivotTranslateZ() const;
            void SetRotatePivotTranslateZ(float32_t translate);

            float32_t GetTranslatePivotX() const;
            void SetTranslatePivotX(float32_t translate);
            float32_t GetTranslatePivotY() const;
            void SetTranslatePivotY(float32_t translate);
            float32_t GetTranslatePivotZ() const;
            void SetTranslatePivotZ(float32_t translate);

        protected:
            Shear         m_Shear;                    // shear values
            Vector3       m_ScalePivot;               // scale pivot translation vector
            Vector3       m_ScalePivotTranslate;      // compensation vector for preserving the transform when the scale pivot it moved
            Vector3       m_RotatePivot;              // rotation pivot translation vector
            Vector3       m_RotatePivotTranslate;     // compensation vector for preserving the transform when the rotation pivot it moved
            Vector3       m_TranslatePivot;           // translate pivot translation vector
            bool                m_SnapPivots;               // if this is true, use m_RotatePivot
        };

        typedef Helium::SmartPtr<SceneGraph::PivotTransform> LPivotTransformPtr;
    }
}