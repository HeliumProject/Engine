#pragma once

#include "Foundation/Math/EulerAngles.h"

#include "Core/SceneGraph/Manipulator.h"
#include "Core/SceneGraph/HierarchyNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Scene;
        class Transform;
        typedef Helium::SmartPtr< SceneGraph::Transform > TransformPtr;

        class CORE_API Transform : public HierarchyNode
        {
        public:
            REFLECT_DECLARE_CLASS( Transform, HierarchyNode );
            static void EnumerateClass( Reflect::Compositor<Transform>& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            Transform();
            ~Transform();

            virtual void Initialize(Scene* scene) HELIUM_OVERRIDE;

            virtual SceneGraph::Transform* GetTransform() HELIUM_OVERRIDE;
            virtual const SceneGraph::Transform* GetTransform() const HELIUM_OVERRIDE;

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
            virtual Undo::CommandPtr ResetTransform();

            // recomputes local components from the global matrix
            virtual Undo::CommandPtr ComputeObjectComponents();

            // centers the pivot points
            virtual Undo::CommandPtr CenterTransform();

            // compute all member matrices
            virtual void Evaluate( GraphDirection direction ) HELIUM_OVERRIDE;

            // render to viewport
            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;

            // issue draw commands
            static void DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
            static void DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

            // do intersection testing
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

            // manipulator support
            virtual void ConnectManipulator(ManiuplatorAdapterCollection* collection) HELIUM_OVERRIDE;

            // ui integration
            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;

            // creator
            static void CreatePanel(CreatePanelArgs& args);

            f32 GetScaleX() const;
            void SetScaleX(f32 scale);
            f32 GetScaleY() const;
            void SetScaleY(f32 scale);
            f32 GetScaleZ() const;
            void SetScaleZ(f32 scale);

            f32 GetRotateX() const;
            void SetRotateX(f32 rotate);
            f32 GetRotateY() const;
            void SetRotateY(f32 rotate);
            f32 GetRotateZ() const;
            void SetRotateZ(f32 rotate);

            f32 GetTranslateX() const;
            void SetTranslateX(f32 translate);
            f32 GetTranslateY() const;
            void SetTranslateY(f32 translate);
            f32 GetTranslateZ() const;
            void SetTranslateZ(f32 translate);

        protected:
            // Reflected
            Scale         m_Scale;
            EulerAngles   m_Rotate;
            Vector3       m_Translate;
            Matrix4       m_ObjectTransform;
            Matrix4       m_GlobalTransform;
            bool                m_InheritTransform;     // Do we transform with our parent?

            // Non-reflected
            Matrix4       m_InverseObjectTransform;
            Matrix4       m_InverseGlobalTransform;
            bool                m_BindIsDirty;
            Matrix4       m_BindTransform;
            Matrix4       m_InverseBindTransform;
        };

        class TransformScaleManipulatorAdapter : public ScaleManipulatorAdapter
        {
        protected:
            SceneGraph::Transform* m_Transform;

        public:
            TransformScaleManipulatorAdapter(SceneGraph::Transform* transform)
                : m_Transform (transform)
            {

            }

            virtual SceneGraph::HierarchyNode* GetNode() HELIUM_OVERRIDE
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

            virtual Undo::CommandPtr SetValue(const Scale& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<Scale> ( new Helium::MemberProperty<SceneGraph::Transform, Scale> (m_Transform, &SceneGraph::Transform::GetScale, &SceneGraph::Transform::SetScale), v );
            }
        };

        class TransformScalePivotManipulatorAdapter : public TranslateManipulatorAdapter
        {
        protected:
            SceneGraph::Transform* m_Transform;

        public:
            TransformScalePivotManipulatorAdapter(SceneGraph::Transform* transform)
                : m_Transform (transform)
            {

            }

            virtual SceneGraph::HierarchyNode* GetNode() HELIUM_OVERRIDE
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

            virtual Undo::CommandPtr SetValue(const Vector3& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<Vector3> ( new Helium::MemberProperty<SceneGraph::Transform, Vector3> (m_Transform, &SceneGraph::Transform::GetScalePivot, &SceneGraph::Transform::SetScalePivot), v );
            }
        };

        class TransformRotateManipulatorAdapter : public RotateManipulatorAdapter
        {
        protected:
            SceneGraph::Transform* m_Transform;

        public:
            TransformRotateManipulatorAdapter(SceneGraph::Transform* transform)
                : m_Transform (transform)
            {

            }

            virtual SceneGraph::HierarchyNode* GetNode() HELIUM_OVERRIDE
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

            virtual Undo::CommandPtr SetValue(const EulerAngles& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<EulerAngles> ( new Helium::MemberProperty<SceneGraph::Transform, EulerAngles> (m_Transform, &SceneGraph::Transform::GetRotate, &SceneGraph::Transform::SetRotate), v );
            }
        };

        class TransformRotatePivotManipulatorAdapter : public TranslateManipulatorAdapter
        {
        protected:
            SceneGraph::Transform* m_Transform;

        public:
            TransformRotatePivotManipulatorAdapter(SceneGraph::Transform* transform)
                : m_Transform (transform)
            {

            }

            virtual SceneGraph::HierarchyNode* GetNode() HELIUM_OVERRIDE
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

            virtual Undo::CommandPtr SetValue(const Vector3& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<Vector3> ( new Helium::MemberProperty<SceneGraph::Transform, Vector3> (m_Transform, &SceneGraph::Transform::GetRotatePivot, &SceneGraph::Transform::SetRotatePivot), v );
            }
        };

        class TransformTranslateManipulatorAdapter : public TranslateManipulatorAdapter
        {
        protected:
            SceneGraph::Transform* m_Transform;

        public:
            TransformTranslateManipulatorAdapter(SceneGraph::Transform* transform)
                : m_Transform (transform)
            {

            }

            virtual SceneGraph::HierarchyNode* GetNode() HELIUM_OVERRIDE
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

            virtual Undo::CommandPtr SetValue(const Vector3& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<Vector3> ( new Helium::MemberProperty<SceneGraph::Transform, Vector3> (m_Transform, &SceneGraph::Transform::GetTranslate, &SceneGraph::Transform::SetTranslate), v );
            }
        };

        class TransformTranslatePivotManipulatorAdapter : public TranslateManipulatorAdapter
        {
        protected:
            SceneGraph::Transform* m_Transform;

        public:
            TransformTranslatePivotManipulatorAdapter(SceneGraph::Transform* transform)
                : m_Transform (transform)
            {

            }

            virtual SceneGraph::HierarchyNode* GetNode() HELIUM_OVERRIDE
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

            virtual Undo::CommandPtr SetValue(const Vector3& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<Vector3> ( new Helium::MemberProperty<SceneGraph::Transform, Vector3> (m_Transform, &SceneGraph::Transform::GetTranslatePivot, &SceneGraph::Transform::SetTranslatePivot), v );
            }
        };
    }
}