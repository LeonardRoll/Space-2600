
cbuffer CB0
{
	matrix WVPMatrix;					// 64 bytes
	//float4 DirLight_Vector;	// 16 bytes
	//float4 DirLight_Colour;	// 16 bytes
	//float4 PoiLight_Location;	// 16 bytes
	//float4 PoiLight_Colour;	// 16 bytes
	//float4 AmbLight_Colour;		// 16 bytes
	// float2 packing;		// 0 bytes
} // TOTAL SIZE = 64 bytes

Texture2D Tex;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord :TEXCOORD;
};

VOut ModelVS(float4 position : POSITION,  float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	float4 default_color = {1, 1, 1, 1};
	float4 ambient_color = {.4, .4, .4, .4};
	float4 light_vector = {1, 1, 1, 1};

	//float fixedlight = dot(normalize(light_vector), normal ); // basic non moving lighting to give basic lit object

	output.position = mul(WVPMatrix, position);

	output.texcoord = texcoord;

	output.color = default_color;
	//output.color = default_color*fixedlight + ambient_color;

	return output;
}


 float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET 
 {
	return Tex.Sample(sampler0, texcoord) * color;
 }