
SamplerState point_sampler: register(s0);
SamplerState linear_sampler: register(s1);
SamplerState anisotropic_sampler: register(s2);
Texture2D mesh_texture: register(t0);

struct VS_OUTPUT
{
	float4 position:SV_POSITION;
    float3 normal:NORMAL;
    float3 color:COLOR;
    float2 uv:UV;
};

struct VS_INPUT
{
	float3 pos:POSITION;
    float3 normal:NORMAL;
    float3 color:COLOR;
    float2 uv:UV;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output;
    
	output.position = float4(input.pos,1.0f);
    output.normal = input.normal;
    output.color = input.color;
    output.uv = input.uv;
    
    return output;
}

struct PS_OUTPUT
{
    float4 color: SV_TARGET;
};

PS_OUTPUT PSMain(VS_OUTPUT input)
{
    PS_OUTPUT ret;
    float4 color = mesh_texture.Sample(linear_sampler, input.uv);
    ret.color = color;
    return ret;
};
