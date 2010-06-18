//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4x4 g_viewproj : register(c0);  // World * View * Projection transformation

struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
    float4 Diffuse    : TEXCOORD0;  // vertex diffuse color
};

VS_OUTPUT main( in float4 position : POSITION, in float4 color : TEXCOORD0)
{
	VS_OUTPUT output;
	
	// transform the vertex into projection space. 
    output.Position = mul( position, g_viewproj );
    output.Diffuse = color;
    
    return output;
}

