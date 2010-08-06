#pragma once

#include "Editor/Scene/Camera.h"
#include "VertexResource.h"
#include "Render.h"

#include "Foundation/Math/AlignedBox.h"
#include "Foundation/Math/Color3.h"

namespace Helium
{
    namespace Editor
    {
        class PickVisitor;

        class Primitive : public VertexResource
        {
        protected:
            // the bounds of the primitive
            Math::AlignedBox m_Bounds;

            // are we filled or wireframe?
            bool m_IsSolid;

            // are we solid or transparent
            bool m_IsTransparent;

            // should we use the camera's shading mode for our render mode?
            bool m_IsUsingCameraShadingMode;

        public:

            //
            // RTTI
            //

        public:
            EDITOR_DECLARE_TYPE(Editor::Primitive, Object);
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            Primitive(ResourceTracker* tracker);
            virtual ~Primitive();

            const Math::AlignedBox& GetBounds() const
            {
                return m_Bounds;
            }

            bool IsSolid(Editor::Camera* camera) const;
            bool IsSolid() const
            {
                return m_IsSolid;
            }
            void SetSolid(bool value)
            {
                m_IsSolid = value;
            }

            bool IsTransparent() const
            {
                return m_IsTransparent;
            }
            void SetTransparent(bool value)
            {
                m_IsTransparent = value;
            }

            bool IsUsingCameraShadingMode() const
            {
                return m_IsUsingCameraShadingMode;
            }
            void SetUsingCameraShadingMode(bool use)
            {
                m_IsUsingCameraShadingMode = use;
            }

            virtual void Populate( PopulateArgs* ) = 0;
            virtual void Draw( DrawArgs*, const bool* solid = NULL, const bool* transparent = NULL ) const = 0;
            virtual bool Pick( PickVisitor*, const bool* solid = NULL ) const = 0;
        };


        //
        // Primitive Template is a base class for a standard primitive class
        //

        template <class T>
        class PrimitiveTemplate : public Editor::Primitive
        {
        protected:
            //
            // System Copy of Resources
            //

            std::vector<T> m_Vertices;


            //
            // Constructor
            //

            PrimitiveTemplate(ResourceTracker* tracker)
                : Editor::Primitive (tracker)
            {

            }

            virtual void Populate(PopulateArgs* args) HELIUM_OVERRIDE
            {
                switch (args->m_Type)
                {
                case ResourceTypes::Vertex:
                    {
                        HELIUM_ASSERT(m_Vertices.size() == GetElementCount());
                        HELIUM_ASSERT(sizeof(T) == ElementSizes[ GetElementType() ]);

                        memcpy(args->m_Buffer + args->m_Offset, &(*m_Vertices.begin()), m_Vertices.size() * sizeof(T));
                        args->m_Offset += (u32)m_Vertices.size() * sizeof(T);

                        break;
                    }
                }
            }
        };
    }
}