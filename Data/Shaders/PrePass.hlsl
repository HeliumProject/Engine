//----------------------------------------------------------------------------------------------------------------------
// PrePass.hlsl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

//! @sysselect_v SKINNING NONE SKINNING_SMOOTH SKINNING_RIGID

#include "Common.inl"

#if L_TYPE_VERTEX

struct VertexInput
{
    float4 position     : POSITION;
#if SKINNING
#if SKINNING_SMOOTH
	float4 blendWeight  : BLENDWEIGHT;
#endif
	float4 blendIndices : BLENDINDICES;
#endif
};

cbuffer ViewGlobalData
{
	ViewVertexConstantGlobalData ViewGlobalData : register( c0 );
}

cbuffer InstanceGlobalData
{
	InstanceVertexConstantGlobalData InstanceGlobalData : register( c8 );
}

float4 main( VertexInput vIn ) : POSITION
{
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

	matrix worldInvViewProjection = mul( ViewGlobalData.inverseViewProjection, worldMatrix );

	return mul( worldInvViewProjection, vIn.position );
}

#endif  // L_TYPE_VERTEX

#if L_TYPE_PIXEL

float4 main() : SV_Target
{
	return float4( 0, 0, 0, 0 );
}

#endif  // L_TYPE_PIXEL
