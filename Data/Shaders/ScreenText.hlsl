//----------------------------------------------------------------------------------------------------------------------
// ScreenText.hlsl
//
// Copyright (C) 2011 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "Common.inl"

struct VertexOutput
{
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD0;
};

#if L_TYPE_VERTEX

struct VertexInput
{
    float4 position : POSITION;
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD0;
};

cbuffer ViewportData
{
	// Scale to apply to position values in .xy, offset to apply in .zw.
	float4 PositionScaleOffset : register( c0 );
}

float4 main( VertexInput vIn, out VertexOutput vOut ) : POSITION
{
	vOut.color = vIn.color;
	vOut.texCoord = vIn.texCoord;
	
	return float4( vIn.position.xy * PositionScaleOffset.xy + PositionScaleOffset.zw, 0.0f, 1.0f );
}

#endif  // L_TYPE_VERTEX

#if L_TYPE_PIXEL

// Texture to render.
Texture2D DiffuseMap;

float4 main( VertexOutput vOut ) : SV_Target
{
	float4 color = vOut.color;
	color.a *= DiffuseMap.Sample( DefaultSamplerState, vOut.texCoord.xy ).r;

	return color;
}

#endif  // L_TYPE_PIXEL
