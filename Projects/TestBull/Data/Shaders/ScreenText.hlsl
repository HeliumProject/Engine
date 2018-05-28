//----------------------------------------------------------------------------------------------------------------------
// ScreenText.hlsl
//
// Copyright (C) 2011 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

//! @systoggle_v PROJECT

#include "Common.inl"

struct VertexOutput
{
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD0;
};

#if HELIUM_TYPE_VERTEX

struct VertexInput
{
    float4 position     : POSITION;
    float4 color        : COLOR;
    float2 texCoord     : TEXCOORD0;
#if PROJECT
    float2 screenOffset : TEXCOORD1;
#endif
};

cbuffer ViewportData
{
    // Scale to apply to position values in .xy, offset to apply in .zw.
    float4 PositionScaleOffset : register( c0 );

#if PROJECT
    matrix WorldInverseViewProjection : register( c1 );
#endif
}

float4 main( VertexInput vIn, out VertexOutput vOut ) : POSITION
{
    vOut.color = vIn.color;
    vOut.texCoord = vIn.texCoord;

#if PROJECT
    float2 screenPos = vIn.screenOffset;
#else
    float2 screenPos = vIn.position.xy;
#endif

    screenPos = screenPos * PositionScaleOffset.xy + PositionScaleOffset.zw;

#if PROJECT
    float4 position = mul( WorldInverseViewProjection, vIn.position );
    screenPos += round( position.xy / position.w - PositionScaleOffset.zw ) + PositionScaleOffset.zw;
#endif

    return float4(screenPos.xy, 0.0f, 1.0f);
}

#endif  // HELIUM_TYPE_VERTEX

#if HELIUM_TYPE_PIXEL

// Texture to render.
Texture2D DiffuseMap;

float4 main( VertexOutput vOut ) : SV_Target
{
    float4 color = vOut.color;
    color.a *= DiffuseMap.Sample( DefaultSamplerState, vOut.texCoord.xy ).r;

    return color;
}

#endif  // HELIUM_TYPE_PIXEL
