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

            virtual int32_t GetImageIndex() const HELIUM_OVERRIDE;
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