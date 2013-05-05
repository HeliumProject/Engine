#pragma once

#include "SceneGraph/API.h"
#include "SceneGraph/Transform.h"

namespace Helium
{
    namespace SceneGraph
    {
        //  TOTAL TRANSFORM:
        //     -1                      -1
        //  [Sp]x[S]x[Sh]x[Sp]x[St]x[Rp]x[R]x[Rp]x[Rt]x[T]x[Tp]

        class HELIUM_SCENE_GRAPH_API PivotTransform : public Transform
        {
        public:
            REFLECT_DECLARE_OBJECT( SceneGraph::PivotTransform, SceneGraph::Transform );
            static void PopulateStructure( Reflect::Structure& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            PivotTransform();

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
            virtual UndoCommandPtr ResetTransform() HELIUM_OVERRIDE;

            // recomputes local components from the global matrix
            virtual UndoCommandPtr ComputeObjectComponents() HELIUM_OVERRIDE;

            // repositions group nodes WRT thier children
            virtual UndoCommandPtr CenterTransform() HELIUM_OVERRIDE;

        protected:
            Shear         m_Shear;                    // shear values
            Vector3       m_ScalePivot;               // scale pivot translation vector
            Vector3       m_ScalePivotTranslate;      // compensation vector for preserving the transform when the scale pivot it moved
            Vector3       m_RotatePivot;              // rotation pivot translation vector
            Vector3       m_RotatePivotTranslate;     // compensation vector for preserving the transform when the rotation pivot it moved
            Vector3       m_TranslatePivot;           // translate pivot translation vector
            bool          m_SnapPivots;               // if this is true, use m_RotatePivot
        };

        typedef Helium::SmartPtr<SceneGraph::PivotTransform> LPivotTransformPtr;
    }
}