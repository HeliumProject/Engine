#pragma once

#include "PreprocessingPc/PreprocessingPc.h"
#include "Foundation/FilePath.h"

#pragma warning( push )
#pragma warning( disable:4005 )
#include <d3d10_1.h>
#pragma warning( pop )

namespace Helium
{
    /// Include handler to use when compiling Direct3D HLSL shaders.
    class D3DIncludeHandler : public ID3D10Include
    {
    public:
        /// @name Construction/Destruction
        //@{
        explicit D3DIncludeHandler( const FilePath& rShaderPath );
        virtual ~D3DIncludeHandler();
        //@}

        /// @name ID3D10Include Interface
        //@{
        STDMETHOD( Open )( D3D10_INCLUDE_TYPE includeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes );
        STDMETHOD( Close )( LPCVOID pData );
        //@}

    private:
        /// Directory containing the shader file being processed.
        FilePath m_shaderDirectory;
    };
}
