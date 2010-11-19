#pragma once

#include "Pipeline/API.h"
#include "Foundation/Component/Component.h"

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API BoundingBoxComponent : public Component::ComponentBase
        {
        private:
            Vector3 m_Extents;
            Vector3 m_Offset;
            Vector3 m_Minima;
            Vector3 m_Maxima;

        public:

            REFLECT_DECLARE_CLASS( BoundingBoxComponent, Component::ComponentBase );

            static void EnumerateClass( Reflect::Compositor< BoundingBoxComponent >& comp );

        public:
            BoundingBoxComponent()
            {
            }

            virtual ~BoundingBoxComponent()
            {
            }

        public:
            virtual Component::ComponentUsage GetComponentUsage() const HELIUM_OVERRIDE;

        public:

            const Vector3& GetExtents() const
            {
                return m_Extents;
            }
            void SetExtents( const Vector3& extents )
            {
                m_Extents = extents;
            }

            const Vector3& GetOffset() const
            {
                return m_Offset;
            }
            void SetOffset( const Vector3& offset )
            {
                m_Offset = offset;
            }

            const Vector3& GetMinima() const
            {
                return m_Minima;
            }
            void SetMinima( const Vector3& minima )
            {
                m_Minima = minima;
            }

            const Vector3& GetMaxima() const
            {
                return m_Maxima;
            }
            void SetMaxima( const Vector3& maxima )
            {
                m_Maxima = maxima;
            }
        };

        typedef Helium::StrongPtr< BoundingBoxComponent > BoundingBoxComponentPtr;
    }
}