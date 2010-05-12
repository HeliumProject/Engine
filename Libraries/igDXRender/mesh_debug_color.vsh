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


VS_OUTPUT main( in float4 position : POSITION, in float4 color : TEXCOORD3)
{
	VS_OUTPUT output;
	
	float4 ws_pos = mul( position, g_world );
	
	// transform the vertex into projection space. 
    output.Position = mul( ws_pos, g_viewproj );
    output.Diffuse = color;
    
    return output;
}

