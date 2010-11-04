//----------------------------------------------------------------------------------------------------------------------
// VectorConversion.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_VECTOR_CONVERSION_H
#define LUNAR_CORE_VECTOR_CONVERSION_H

#include "Platform/Math/Vector3.h"
#include "Platform/Math/Vector4.h"

namespace Helium
{
    /// @defgroup vectorconversion Vector3/Vector4 Conversion Support
    //@{
    inline Vector4 Vector3ToVector4( const Vector3& rVector );
    inline Vector4 PointToVector4( const Vector3& rVector );
    inline Vector4 RayToVector4( const Vector3& rVector );

    inline Vector3 Vector4ToVector3( const Vector4& rVector );
    //@}
}

#if HELIUM_SIMD_LRBNI
#include "Platform/Math/VectorConversionLrbni.inl"
#elif HELIUM_SIMD_SSE
#include "Platform/Math/VectorConversionSse.inl"
#endif

#endif  // LUNAR_CORE_VECTOR_CONVERSION_H
