//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4x4 g_viewproj : register(c0);  // World * View * Projection transformation
float4x4 g_world : register(c4);  // local to world

struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
    float4 Diffuse    : TEXCOORD0;  // vertex diffuse color
};


VS_OUTPUT main( in float4 position : POSITION, in float4 tangent : TEXCOORD1)
{
	VS_OUTPUT output;
	
	float4 ws_pos = mul( position, g_world );
	tangent.xyz = normalize(tangent.xyz);
	
	// transform the vertex into projection space. 
    output.Position = mul( ws_pos, g_viewproj );
    output.Diffuse = half4((tangent.xyz+1.0f)*0.5f,0);
    
    return output;
}

