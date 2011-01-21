//----------------------------------------------------------------------------------------------------------------------
// StandardBase.hlsl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

//! @toggle_p EMISSIVE_MAP
//! @toggle_p NORMAL_MAP
//! @select SPECULAR NONE SPECULAR_DIFFUSE_ALPHA SPECULAR_MAP
//! @sysselect_v SKINNING NONE SKINNING_SMOOTH SKINNING_RIGID
//! @sysselect SHADOWS NONE SHADOWS_SIMPLE SHADOWS_PCF_DITHERED

#include "Common.inl"

struct VertexOutput
{
    half4  color              : COLOR;
    half4  texCoord0          : TEXCOORD0;

    half3  worldUp            : TEXCOORD1;

    half3  toDirectionalLight : TEXCOORD2;

#if SPECULAR
    half3  toEye              : TEXCOORD3;
#endif

#if SHADOWS
    float3 shadowPos          : TEXCOORD4;
#endif
#if SHADOWS_PCF_DITHERED
    float3 screenPos          : TEXCOORD5;
#endif
};

#if L_TYPE_VERTEX

struct VertexInput
{
    float4 position     : POSITION;
    float3 normal       : NORMAL;
    float4 tangent      : TANGENT;
#if SKINNING
#if SKINNING_SMOOTH
	float4 blendWeight  : BLENDWEIGHT;
#endif
	float4 blendIndices : BLENDINDICES;
#else
    float4 color        : COLOR;
#endif
    float4 texCoord0    : TEXCOORD0;
};

cbuffer ViewGlobalData
{
    ViewVertexConstantGlobalData ViewGlobalData : register( c0 );
}

cbuffer ViewPassData
{
    ViewVertexConstantBasePassData ViewPassData : register( c8 );
}

cbuffer InstanceGlobalData
{
    InstanceVertexConstantGlobalData InstanceGlobalData : register( c14 );
}

float4 main( VertexInput vIn, out VertexOutput vOut ) : POSITION
{
#if SKINNING
	vOut.color = half4( 1, 1, 1, 1 );
#else
    vOut.color = half4( vIn.color );
#endif
    vOut.texCoord0 = half4( vIn.texCoord0 );
    
    float4 localPosition = vIn.position;
    float3 normal = vIn.normal * 2 - 1;
    float4 tangentEx = vIn.tangent * 2 - 1;

#if SKINNING
#if SKINNING_SMOOTH
	float3x4 partialSkinningMatrix =
		InstanceGlobalData.bonePalette[ vIn.blendIndices.x ] * vIn.blendWeight.x +
		InstanceGlobalData.bonePalette[ vIn.blendIndices.y ] * vIn.blendWeight.y +
		InstanceGlobalData.bonePalette[ vIn.blendIndices.z ] * vIn.blendWeight.z +
		InstanceGlobalData.bonePalette[ vIn.blendIndices.w ] * vIn.blendWeight.w;
#else
	float3x4 partialSkinningMatrix = InstanceGlobalData.bonePalette[ vIn.blendIndices.x ];
#endif

	matrix worldMatrix = matrix( partialSkinningMatrix, float4( 0, 0, 0, 1 ) );
#else
    matrix worldMatrix = matrix( InstanceGlobalData.transform, float4( 0, 0, 0, 1 ) );
#endif
    
#if SHADOWS
    matrix shadowInvViewProj = mul( ViewPassData.shadowInverseViewProjection, worldMatrix );
    float4 shadowPosProj = mul( shadowInvViewProj, localPosition );
    vOut.shadowPos = shadowPosProj.xyz / shadowPosProj.w;
#endif

    matrix worldInvView = mul( ViewGlobalData.inverseView, worldMatrix );
    normal = normalize( mul( worldInvView, float4( normal, 0 ) ).xyz );
    float3 tangent = normalize( mul( worldInvView, float4( tangentEx.xyz, 0 ) ).xyz );
    float3 binormal = normalize( cross( normal, tangent ) * tangentEx.w );

    vOut.worldUp = half3( tangent.y, binormal.y, normal.y );

    vOut.toDirectionalLight = half3(
        dot( ViewPassData.toDirectionalLight.xyz, tangent ),
        dot( ViewPassData.toDirectionalLight.xyz, binormal ),
        dot( ViewPassData.toDirectionalLight.xyz, normal ) );

#if SPECULAR
    vOut.toEye = half3( normalize( -mul( worldInvView, localPosition ).xyz ) );
#endif

    matrix worldInvViewProjection = mul( ViewGlobalData.inverseViewProjection, worldMatrix );
    
    float4 outPosition = mul( worldInvViewProjection, localPosition );

#if SHADOWS_PCF_DITHERED
    vOut.screenPos =
		float3( ( outPosition.xy + outPosition.ww ) * ViewPassData.screenPointAdjustments.xy + 0.5 * outPosition.w, outPosition.w );
#endif
    
    return outPosition;
}

#endif  // L_TYPE_VERTEX

#if L_TYPE_PIXEL

// Diffuse map texture.
Texture2D DiffuseMap;

#if NORMAL_MAP
// Normal map texture.
Texture2D NormalMap;
#endif

#if EMISSIVE_MAP
// Emissive map texture.
Texture2D EmissiveMap;
#endif

#if SPECULAR_MAP
// Specular map texture.
Texture2D SpecularMap;
#endif

#if SHADOWS
// Directional light shadow map.
#if L_PROFILE_PC_SM4
Texture2D _ShadowMap;
SamplerComparisonState ShadowSamplerState
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;

    ComparisonFunc = LESS;
    ComparisonFilter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
};
#else  // L_PROFILE_PC_SM4
Texture2D _ShadowMapTexture;
sampler _ShadowMap = sampler_state
{
	Texture = <_ShadowMapTexture>;
	MipFilter = POINT;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};
#endif  // L_PROFILE_PC_SM4
#endif  // SHADOWS

cbuffer ViewPassData
{
    ViewPixelConstantBasePassData ViewPassData : register( c0 );
}

cbuffer MaterialParameters
{
#if NORMAL_MAP
    float NormalMapHeightScale : register( c4 );
#endif

#if SPECULAR
    float SpecularExponent : register( c5 );
#endif
}

#if SHADOWS_PCF_DITHERED
static const float4 PCF_BASE_KERNEL[] =
{
	float4( -1.5f,  0.5f, 0.5f, -1.5f ),
	float4( -1.5f, -1.5f, 0.5f,  0.5f )
};
#endif

float4 main( VertexOutput vOut ) : SV_Target
{
    half2 texCoord0 = half2( vOut.texCoord0.xy );

    half3 normal = half3( 0, 0, 1 );
#if NORMAL_MAP
    normal = UnpackNormalMapSample(
        half4( NormalMap.Sample( DefaultSamplerState, texCoord0 ) ),
        half( NormalMapHeightScale ) );
#endif

    half4 diffuseSample = half4( DiffuseMap.Sample( DefaultSamplerState, texCoord0 ) );

    half4 color = half4( 0, 0, 0, diffuseSample.a );
    
#if EMISSIVE_MAP
    color.rgb = half3( EmissiveMap.Sample( DefaultSamplerState, texCoord0 ).rgb );
#endif

    half3 worldUp = half3( normalize( half3( vOut.worldUp ) ) );
    half ambientBlend = half( saturate( dot( normal, worldUp ) * 0.5 + 0.5 ) );
    half3 diffuse = half3( lerp(
        half3( ViewPassData.ambientBottomColor.rgb ),
        half3( ViewPassData.ambientTopColor.rgb ),
        ambientBlend ) );

	half shadow = 1.0;
#if SHADOWS_SIMPLE
#if L_PROFILE_PC_SM4
	shadow = half( _ShadowMap.SampleCmpLevelZero( ShadowSamplerState, vOut.shadowPos.xy, vOut.shadowPos.z ) );
#else
	shadow = half( tex2Dproj( _ShadowMap, half4( vOut.shadowPos.xyz, 1 ) ).r );
#endif
#elif SHADOWS_PCF_DITHERED
	float2 screenPos = vOut.screenPos.xy / vOut.screenPos.z;
	float2 pcfDitherOffset = float2( frac( screenPos * 0.5 ) > 0.5 );
	pcfDitherOffset.y = float( frac( dot( pcfDitherOffset, float2( 0.5, 0.5 ) ) ) > 0.25 );

	float4 pcfOffsets[] =
	{
		PCF_BASE_KERNEL[ 0 ] + pcfDitherOffset.xyxy,
		PCF_BASE_KERNEL[ 1 ] + pcfDitherOffset.xyxy
	};

	float4 invShadowMapResSplat = ViewPassData.inverseShadowMapResolution.xyxy;
	pcfOffsets[ 0 ] *= invShadowMapResSplat;
	pcfOffsets[ 1 ] *= invShadowMapResSplat;

#if L_PROFILE_PC_SM4
	half4 shadowComponents = half4(
		half( _ShadowMap.SampleCmpLevelZero( ShadowSamplerState, vOut.shadowPos.xy + pcfOffsets[ 0 ].xy, vOut.shadowPos.z ) ),
		half( _ShadowMap.SampleCmpLevelZero( ShadowSamplerState, vOut.shadowPos.xy + pcfOffsets[ 0 ].zw, vOut.shadowPos.z ) ),
		half( _ShadowMap.SampleCmpLevelZero( ShadowSamplerState, vOut.shadowPos.xy + pcfOffsets[ 1 ].xy, vOut.shadowPos.z ) ),
		half( _ShadowMap.SampleCmpLevelZero( ShadowSamplerState, vOut.shadowPos.xy + pcfOffsets[ 1 ].zw, vOut.shadowPos.z ) ) );
#else
	half4 shadowComponents = half4(
		half( tex2Dproj( _ShadowMap, half4( vOut.shadowPos.xy + pcfOffsets[ 0 ].xy, vOut.shadowPos.z, 1 ) ).r ),
		half( tex2Dproj( _ShadowMap, half4( vOut.shadowPos.xy + pcfOffsets[ 0 ].zw, vOut.shadowPos.z, 1 ) ).r ),
		half( tex2Dproj( _ShadowMap, half4( vOut.shadowPos.xy + pcfOffsets[ 1 ].xy, vOut.shadowPos.z, 1 ) ).r ),
		half( tex2Dproj( _ShadowMap, half4( vOut.shadowPos.xy + pcfOffsets[ 1 ].zw, vOut.shadowPos.z, 1 ) ).r ) );
#endif
	shadow = dot( shadowComponents, half4( 0.25, 0.25, 0.25, 0.25 ) );
#endif

    half3 toDirectionalLight = half3( normalize( vOut.toDirectionalLight ) );
    half3 directionalLightColor = half3( ViewPassData.directionalLightColor.rgb );
    diffuse += directionalLightColor * half( saturate( dot( normal, toDirectionalLight ) ) ) * shadow;

    color.rgb += diffuse * diffuseSample.rgb;

#if SPECULAR
    half specularExponent = half( SpecularExponent );

#if SPECULAR_MAP
    half3 specularSample = half3( SpecularMap.Sample( DefaultSamplerState, texCoord0 ).rgb );
#else
    half3 specularSample = diffuseSample.aaa;
#endif

    half3 toEye = half3( normalize( vOut.toEye ) );

    half directionalSpecularAtten =
        half( pow( saturate( dot( toEye, reflect( toDirectionalLight, normal ) ) ), specularExponent ) );

    half3 specular = directionalLightColor * directionalSpecularAtten * shadow;

    color.rgb += specular * specularSample;
#endif

    return float4( color );
}

#endif  // L_TYPE_PIXEL
