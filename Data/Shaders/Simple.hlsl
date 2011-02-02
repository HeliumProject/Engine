//----------------------------------------------------------------------------------------------------------------------
// Simple.hlsl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

//! @sysselect TEXTURING NONE TEXTURING_BLEND TEXTURING_ALPHA

#include "Common.inl"

struct VertexOutput
{
    float4 color    : COLOR;
#if TEXTURING
    float2 texCoord : TEXCOORD0;
#endif
};

#if L_TYPE_VERTEX

struct VertexInput
{
    float4 position : POSITION;
    float4 color    : COLOR;
#if TEXTURING
    float2 texCoord : TEXCOORD0;
#endif
};

cbuffer ViewGlobalData
{
    ViewVertexConstantGlobalData ViewGlobalData : register( c0 );
}

float4 main( VertexInput vIn, out VertexOutput vOut ) : POSITION
{
	vOut.color = vIn.color;
#if TEXTURING
	vOut.texCoord = vIn.texCoord;
#endif
	
	return mul( ViewGlobalData.inverseViewProjection, vIn.position );
}

#endif  // L_TYPE_VERTEX

#if L_TYPE_PIXEL

#if TEXTURING
// Texture to render.
Texture2D DiffuseMap;
#endif

cbuffer InstanceData
{
	// Color to blend with each pixel color.
	float4 BlendColor : register( c0 );
}

float4 main( VertexOutput vOut ) : SV_Target
{
	float4 color = BlendColor * vOut.color;
#if TEXTURING_BLEND
	color *= DiffuseMap.Sample( DefaultSamplerState, vOut.texCoord.xy );
#elif TEXTURING_ALPHA
	color.a *= DiffuseMap.Sample( DefaultSamplerState, vOut.texCoord.xy ).r;
#endif

	return color;
}

#endif  // L_TYPE_PIXEL
