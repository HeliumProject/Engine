//----------------------------------------------------------------------------------------------------------------------
// Common.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#if !L_PROFILE_PC_SM4
#define SV_Target COLOR
#endif

/// Maximum number of bones influencing vertices per skinned mesh (must match the same constant in
/// Dev/Engine/Include/GraphicsTypes/VertexTypes.h).
#define BONE_COUNT_MAX 75

/// Per-view vertex shader constant data for all passes.
struct ViewVertexConstantGlobalData
{
    /// Inverse view/projection matrix.
    matrix inverseViewProjection;
    /// Inverse view matrix.
    matrix inverseView;
};

/// Per-view vertex shader constant data for base-pass rendering.
struct ViewVertexConstantBasePassData
{
    /// Directional light shadow inverse view/projection matrix, with offsetting to map to UV space.
    matrix shadowInverseViewProjection;

    /// Directional light direction (pre-transformed to view space).
    float4 toDirectionalLight;

    /// x: ( Display width ) / 2
    /// y: ( Display height ) / 2
    /// z & w: Unused
    float4 screenPointAdjustments;
};

/// Per-view pixel shader constant data for base-pass rendering.
struct ViewPixelConstantBasePassData
{
    /// Ambient light top color.
    float4 ambientTopColor;
    /// Ambient light bottom color.
    float4 ambientBottomColor;

    /// Directional light color.
    float4 directionalLightColor;

    /// Inverse shadow map resolution (z & w components are unused).
    float4 inverseShadowMapResolution;
};

/// Per-instance vertex shader constant data for all passes.
struct InstanceVertexConstantGlobalData
{
#if SKINNING
    /// Bone palette.
    float3x4 bonePalette[ BONE_COUNT_MAX ];
#else
    /// World transform matrix.
    float3x4 transform;
#endif
};

/// Default sampler state slot.  This will be set by the engine to a sampler state using linear filtering (bilinear,
/// trilinear, or anisotropic based on the graphics configuration settings) and wrapping texture coordinates.
///
/// Note that this name must match the name returned by GraphicsScene::GetDefaultSamplerStateName(), as it is matched by
/// name during graphics scene updating and rendering.
SamplerState DefaultSamplerState
{
};

/// Normal map sample unpacking.
///
/// @param[in] color        Normal map color sample.
/// @param[in] heightScale  Amount by which to scale normal heights (values closer to zero cause darker edges).
///
/// @return  Normal value.
half3 UnpackNormalMapSample( half4 sample, half heightScale )
{
    half2 xy = half2( sample.r * sample.a, sample.g ) * 2 - 1;

    half3 normal = half3( xy.x, xy.y, sqrt( 1 - dot( xy, xy ) ) * heightScale );

    return half3( normalize( normal ) );
}
