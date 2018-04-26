#pragma once

#include "Rendering/RShader.h"

namespace Helium
{
    class Renderer;

    HELIUM_DECLARE_RPTR( RVertexDescription );
    HELIUM_DECLARE_RPTR( RVertexInputLayout );

    /// Vertex shader interface.
    class HELIUM_RENDERING_API RVertexShader : public RShader
    {
    public:
        /// @name Type Information
        //@{
        EType GetType() const;
        //@}

        /// @name Vertex Input Layout Caching
        //@{
        void CacheDescription( Renderer* pRenderer, RVertexDescription* pDescription );
        RVertexInputLayout* GetCachedInputLayout() const;
        //@}

    protected:
        /// Most recently used vertex description.
        RVertexDescriptionPtr m_spCachedDescription;
        /// Input layout associated with the most recently used vertex description.
        RVertexInputLayoutPtr m_spCachedInputLayout;

        /// @name Construction/Destruction
        //@{
        RVertexShader();
        virtual ~RVertexShader() = 0;
        //@}
    };
}
