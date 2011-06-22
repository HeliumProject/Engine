//----------------------------------------------------------------------------------------------------------------------
// D3DIncludeHandler.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_PREPROCESSING_PC_D3D_INCLUDE_HANDLER_H
#define LUNAR_PREPROCESSING_PC_D3D_INCLUDE_HANDLER_H

#include "PreprocessingPc/PreprocessingPc.h"
#include "Foundation/File/Path.h"

#include <d3d10_1.h>

namespace Helium
{
    /// Include handler to use when compiling Direct3D HLSL shaders.
    class D3DIncludeHandler : public ID3D10Include
    {
    public:
        /// @name Construction/Destruction
        //@{
        explicit D3DIncludeHandler( const Path& rShaderPath );
        virtual ~D3DIncludeHandler();
        //@}

        /// @name ID3D10Include Interface
        //@{
        STDMETHOD( Open )(
            D3D10_INCLUDE_TYPE includeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes );
        STDMETHOD( Close )( LPCVOID pData );
        //@}

    private:
        /// Directory containing the shader file being processed.
        Path m_shaderDirectory;
    };
}

#endif  // LUNAR_PREPROCESSING_PC_D3D_INCLUDE_HANDLER_H
