#pragma once

#include "Math/SimdMatrix44.h"

#include "Math/FpuAlignedBox.h"
#include "Math/FpuColor3.h"

#include "Pipeline/SceneGraph/Color.h"
#include "Pipeline/SceneGraph/Camera.h"
#include "Pipeline/SceneGraph/Render.h"
#include "Pipeline/SceneGraph/VertexResource.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PickVisitor;

        class Primitive : public VertexResource
        {
        protected:
            // the bounds of the primitive
            AlignedBox m_Bounds;

            // are we filled or wireframe?
            bool m_IsSolid;

            // are we solid or transparent
            bool m_IsTransparent;

        public:
            REFLECT_DECLARE_ABSTRACT( Primitive, Reflect::Object );
            static void InitializeType();
            static void CleanupType();

        public:
            Primitive();
            virtual ~Primitive();

            const AlignedBox& GetBounds() const
            {
                return m_Bounds;
            }

            bool IsSolid() const
            {
                return m_IsSolid;
            }
            void SetSolid( bool value )
            {
                m_IsSolid = value;
            }

            bool IsTransparent() const
            {
                return m_IsTransparent;
            }
            void SetTransparent( bool value )
            {
                m_IsTransparent = value;
            }

            virtual void Populate( PopulateArgs* args ) = 0;

            virtual void Draw( Helium::BufferedDrawer*,
                               DrawArgs*,
                               Helium::Color materialColor = Color::WHITE,
                               const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY,
                               const bool* solid = NULL,
                               const bool* transparent = NULL ) const = 0;

            virtual bool Pick( PickVisitor*,
                               const bool* solid = NULL ) const = 0;
        };


        //
        // Primitive Template is a base class for a standard primitive class
        //

        template <class T>
        class PrimitiveTemplate : public Primitive
        {
        protected:
            std::vector<T> m_Vertices;

            PrimitiveTemplate()
            {

            }

            virtual void Populate(PopulateArgs* args) HELIUM_OVERRIDE
            {
                switch (args->m_Type)
                {
                case ResourceTypes::Vertex:
                    {
                        HELIUM_ASSERT(m_Vertices.size() == GetElementCount());
                        HELIUM_ASSERT(sizeof(T) == VertexElementSizes[ GetElementType() ]);

                        memcpy(args->m_Buffer + args->m_Offset, &(*m_Vertices.begin()), m_Vertices.size() * sizeof(T));
                        args->m_Offset += (uint32_t)m_Vertices.size() * sizeof(T);

                        break;
                    }
                }
            }
        };
    }
}