//----------------------------------------------------------------------------------------------------------------------
// ScreenSpaceTexture.hlsl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

//! @systoggle TEXTURED

#include "Common.inl"

struct VertexOutput
{
    float4 color    : COLOR;
#if TEXTURED
    float2 texCoord : TEXCOORD0;
#endif
};

#if HELIUM_TYPE_VERTEX

struct VertexInput
{
    float4 position : POSITION;
    float4 color    : COLOR;
#if TEXTURED
    float2 texCoord : TEXCOORD0;
#endif
};

float4 main( VertexInput vIn, out VertexOutput vOut ) : POSITION
{
	vOut.color = vIn.color;
#if TEXTURED
	vOut.texCoord = vIn.texCoord;
#endif
	
	return vIn.position;
}

#endif  // HELIUM_TYPE_VERTEX

#if HELIUM_TYPE_PIXEL

#if TEXTURED
// Texture to render.
Texture2D DiffuseMap;
#endif

float4 main( VertexOutput vOut ) : SV_Target
{
	float4 color = vOut.color;
#if TEXTURED
	color *= DiffuseMap.Sample( DefaultSamplerState, vOut.texCoord.xy );
#endif
	
	return color;
}

#endif  // HELIUM_TYPE_PIXEL
