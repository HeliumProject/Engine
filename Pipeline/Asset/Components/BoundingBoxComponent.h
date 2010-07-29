#pragma once

#include "Pipeline/API.h"
#include "Foundation/Component/Component.h"

namespace Asset
{
    class PIPELINE_API BoundingBoxComponent : public Component::ComponentBase
    {
    private:
        Math::Vector3 m_Extents;
        Math::Vector3 m_Offset;
        Math::Vector3 m_Minima;
        Math::Vector3 m_Maxima;

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

        const Math::Vector3& GetExtents() const
        {
            return m_Extents;
        }
        void SetExtents( const Math::Vector3& extents )
        {
            m_Extents = extents;
        }

        const Math::Vector3& GetOffset() const
        {
            return m_Offset;
        }
        void SetOffset( const Math::Vector3& offset )
        {
            m_Offset = offset;
        }

        const Math::Vector3& GetMinima() const
        {
            return m_Minima;
        }
        void SetMinima( const Math::Vector3& minima )
        {
            m_Minima = minima;
        }

        const Math::Vector3& GetMaxima() const
        {
            return m_Maxima;
        }
        void SetMaxima( const Math::Vector3& maxima )
        {
            m_Maxima = maxima;
        }
    };

    typedef Helium::SmartPtr< BoundingBoxComponent > BoundingBoxComponentPtr;
}
