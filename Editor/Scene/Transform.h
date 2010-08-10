#pragma once

#include "Core/Content/Nodes/Transform.h"

#include "Editor/Scene/Manipulator.h"

#include "HierarchyNode.h"
#include "Foundation/Math/EulerAngles.h"

namespace Helium
{
    namespace Editor
    {
        // forwards
        class Scene;

        class Transform;
        typedef Helium::SmartPtr< Editor::Transform > TransformPtr;

        // this is fabulous
        namespace ManipulatorSpaces
        {
            enum ManipulatorSpace;
        }
        typedef ManipulatorSpaces::ManipulatorSpace;

        class EDITOR_SCENE_API Transform HELIUM_ABSTRACT : public Editor::HierarchyNode
        {
            // 
            // Member variables
            // 

        protected:
            // simple scale component
            Math::Scale m_Scale;

            // simple rotation component
            Math::EulerAngles m_Rotate;

            // simple translation component
            Math::Vector3 m_Translate;

            // local matrix
            Math::Matrix4 m_ObjectTransform;
            Math::Matrix4 m_InverseObjectTransform;

            // global matrix
            Math::Matrix4 m_GlobalTransform;
            Math::Matrix4 m_InverseGlobalTransform;

            // bind matrix (computed once per dirty setting)
            bool m_BindIsDirty;
            Math::Matrix4 m_BindTransform;
            Math::Matrix4 m_InverseBindTransform;


            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Editor::Transform, Editor::HierarchyNode );
            static void InitializeType();
            static void CleanupType();


            // 
            // Member functions
            // 

        public:
            Transform( Editor::Scene* scene, Content::Transform* transform );
            virtual ~Transform();

            virtual void Pack() HELIUM_OVERRIDE;
            virtual void Unpack() HELIUM_OVERRIDE;


            //
            // Returns this
            //

            virtual Editor::Transform* GetTransform() HELIUM_OVERRIDE;
            virtual const Editor::Transform* GetTransform() const HELIUM_OVERRIDE;


            //
            // Scale
            //

            virtual Math::Scale GetScale() const;
            virtual void SetScale( const Math::Scale& value );

            virtual Math::Vector3 GetScalePivot() const;
            virtual void SetScalePivot( const Math::Vector3& value );


            //
            // Rotate
            //

            virtual Math::EulerAngles GetRotate() const;
            virtual void SetRotate( const Math::EulerAngles& value );

            virtual Math::Vector3 GetRotatePivot() const;
            virtual void SetRotatePivot( const Math::Vector3& value );


            //
            // Translate
            //

            virtual Math::Vector3 GetTranslate() const;
            virtual void SetTranslate( const Math::Vector3& value );

            virtual Math::Vector3 GetTranslatePivot() const;
            virtual void SetTranslatePivot( const Math::Vector3& value );


            //
            // Object Transform (transformation of object coordinates into local space)
            //

            Math::Matrix4 GetObjectTransform() const
            {
                return m_ObjectTransform;
            }

            Math::Matrix4 GetInverseObjectTransform() const
            {
                return m_InverseObjectTransform;
            }

            void SetObjectTransform( const Math::Matrix4& transform );


            //
            // Parent Transform (transformation of the frame this object is within into world space)
            //

            Math::Matrix4 GetParentTransform() const
            {
                return m_InverseObjectTransform * m_GlobalTransform;
            }

            Math::Matrix4 GetInverseParentTransform() const
            {
                return m_InverseGlobalTransform * m_ObjectTransform;
            }


            //
            // Global Transform (transformation of object coordinates into world space)
            //

            Math::Matrix4 GetGlobalTransform() const
            {
                return m_GlobalTransform;
            }

            Math::Matrix4 GetInverseGlobalTransform() const
            {
                return m_InverseGlobalTransform;
            }

            void SetGlobalTransform( const Math::Matrix4& transform );


            //
            // Binding Matrices
            //

            virtual Math::Matrix4 GetBindTransform() const;
            virtual Math::Matrix4 GetInverseBindTransform() const;


            //
            // Inherit Transform
            //

            bool GetInheritTransform() const;
            void SetInheritTransform(bool inherit);


            //
            // Main routines
            //

        public:
            // compute scaling component
            virtual Math::Matrix4 GetScaleComponent() const;

            // compute rotation component
            virtual Math::Matrix4 GetRotateComponent() const;

            // compute translation component
            virtual Math::Matrix4 GetTranslateComponent() const;

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

            virtual Math::Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

            virtual Math::Vector3 GetPivot() HELIUM_OVERRIDE
            {
                return m_Transform->GetScalePivot();
            }

            virtual Math::Scale GetValue() HELIUM_OVERRIDE
            {
                return m_Transform->GetScale();
            }

            virtual Undo::CommandPtr SetValue(const Math::Scale& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<Math::Scale> ( new Helium::MemberProperty<Editor::Transform, Math::Scale> (m_Transform, &Editor::Transform::GetScale, &Editor::Transform::SetScale), v );
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

            virtual Math::Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

            virtual Math::Vector3 GetPivot() HELIUM_OVERRIDE
            {
                return Math::Vector3::Zero;
            }

            virtual Math::Vector3 GetValue() HELIUM_OVERRIDE
            {
                return m_Transform->GetScalePivot();
            }

            virtual Undo::CommandPtr SetValue(const Math::Vector3& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<Math::Vector3> ( new Helium::MemberProperty<Editor::Transform, Math::Vector3> (m_Transform, &Editor::Transform::GetScalePivot, &Editor::Transform::SetScalePivot), v );
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

            virtual Math::Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

            virtual Math::Vector3 GetPivot() HELIUM_OVERRIDE
            {
                return m_Transform->GetRotatePivot();
            }

            virtual Math::EulerAngles GetValue() HELIUM_OVERRIDE
            {
                return m_Transform->GetRotate();
            }

            virtual Undo::CommandPtr SetValue(const Math::EulerAngles& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<Math::EulerAngles> ( new Helium::MemberProperty<Editor::Transform, Math::EulerAngles> (m_Transform, &Editor::Transform::GetRotate, &Editor::Transform::SetRotate), v );
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

            virtual Math::Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

            virtual Math::Vector3 GetPivot() HELIUM_OVERRIDE
            {
                return Math::Vector3::Zero;
            }

            virtual Math::Vector3 GetValue() HELIUM_OVERRIDE
            {
                return m_Transform->GetRotatePivot();
            }

            virtual Undo::CommandPtr SetValue(const Math::Vector3& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<Math::Vector3> ( new Helium::MemberProperty<Editor::Transform, Math::Vector3> (m_Transform, &Editor::Transform::GetRotatePivot, &Editor::Transform::SetRotatePivot), v );
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

            virtual Math::Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

            virtual Math::Vector3 GetPivot() HELIUM_OVERRIDE
            {
                return m_Transform->GetTranslatePivot();
            }

            virtual Math::Vector3 GetValue() HELIUM_OVERRIDE
            {
                return m_Transform->GetTranslate();
            }

            virtual Undo::CommandPtr SetValue(const Math::Vector3& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<Math::Vector3> ( new Helium::MemberProperty<Editor::Transform, Math::Vector3> (m_Transform, &Editor::Transform::GetTranslate, &Editor::Transform::SetTranslate), v );
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

            virtual Math::Matrix4 GetFrame(ManipulatorSpace space) HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetObjectMatrix() HELIUM_OVERRIDE;
            virtual Math::Matrix4 GetParentMatrix() HELIUM_OVERRIDE;

            virtual Math::Vector3 GetPivot() HELIUM_OVERRIDE
            {
                return Math::Vector3::Zero;
            }

            virtual Math::Vector3 GetValue() HELIUM_OVERRIDE
            {
                return m_Transform->GetTranslatePivot();
            }

            virtual Undo::CommandPtr SetValue(const Math::Vector3& v) HELIUM_OVERRIDE
            {
                return new Undo::PropertyCommand<Math::Vector3> ( new Helium::MemberProperty<Editor::Transform, Math::Vector3> (m_Transform, &Editor::Transform::GetTranslatePivot, &Editor::Transform::SetTranslatePivot), v );
            }
        };
    }
}