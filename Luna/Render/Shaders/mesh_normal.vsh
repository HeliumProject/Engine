//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4x4 g_viewproj		: register(c0);  // World * View * Projection transformation
float4x4 g_world		: register(c4);  // local to world
float4   g_ws_eye_pos	: register(c9);  // world space pos of the eye

struct VS_OUTPUT_NORMAL
{
    float4 m_position		: POSITION;   // vertex position 
    float3 m_ws_tangent		: TEXCOORD0;
    float3 m_ws_binormal	: TEXCOORD1;
    float3 m_ws_normal		: TEXCOORD2;
    float2 m_uv				: TEXCOORD3;
    float4 m_color			: TEXCOORD4;
    float3 m_ws_view_vec	: TEXCOORD5;
    float3 m_ts_vert_to_eye : TEXCOORD6;
};


VS_OUTPUT_NORMAL main
( 
	in float4 position : POSITION, 
	in float3 normal   : TEXCOORD0,
	in float3 tangent  : TEXCOORD1,
	in float2 uv       : TEXCOORD2,
	in float4 color    : TEXCOORD3)
{
	VS_OUTPUT_NORMAL output;
	
	float4 ws_pos = mul( position, g_world );
	float3 ws_normal = mul( normal, g_world );
	float3 ws_tangent = mul( tangent, g_world );
	output.m_ws_normal = normalize(ws_normal);
	output.m_ws_tangent = normalize(ws_tangent);
	output.m_ws_binormal = cross(ws_tangent,ws_normal)*-1.0f;
	
	// transform the vertex into projection space. 
    output.m_position = mul( ws_pos, g_viewproj );

	output.m_uv = uv;
	output.m_color = color;

	float3 ws_eye_vec = ws_pos.xyz-g_ws_eye_pos.xyz; 
	output.m_ws_view_vec = ws_eye_vec;						// ws eye to vertex 
    
    // tangent space vertex to eye
    output.m_ts_vert_to_eye.x = dot(-ws_eye_vec,output.m_ws_tangent);
    output.m_ts_vert_to_eye.y = dot(-ws_eye_vec,output.m_ws_binormal);
    output.m_ts_vert_to_eye.z = dot(-ws_eye_vec,output.m_ws_normal);
    
    return output;
}

