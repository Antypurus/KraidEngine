struct VS_OUTPUT
{
	float4 position:SV_POSITION;
    float3 normal:NORMAL;
    float3 color:COLOR;
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
    
    return output;
}

struct PS_OUTPUT
{
    float4 color: SV_TARGET;
};

PS_OUTPUT PSMain(VS_OUTPUT input)
{
    PS_OUTPUT ret;
    ret.color = float4(abs(input.color.xyz),1.0f);
    return ret;
};
