//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

struct VS_OUTPUT
{
    float4 pos        : POSITION;   // vertex position 
    float4 t0         : TEXCOORD0;
    float4 t1         : TEXCOORD1;
    float4 t2         : TEXCOORD2;
};

float4 g_screen_info : register (c8);

VS_OUTPUT main( in float4 position : POSITION, 
                in float4 tex0 : TEXCOORD0,
                in float4 tex1 : TEXCOORD1,
                in float4 tex2 : TEXCOORD2)
{
	VS_OUTPUT output;

	output.pos        = position;
	output.pos.x      = (output.pos.x - g_screen_info.x) /  g_screen_info.x;
	output.pos.y      = (output.pos.y - g_screen_info.y) /  (-g_screen_info.y);	  
	
	output.t0 = tex0;
	output.t1 = tex1;
	output.t2 = tex2;
       
    return output;
}

