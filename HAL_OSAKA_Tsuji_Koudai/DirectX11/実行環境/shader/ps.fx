//--------------------------------------------------------------------------------------
// ps.fx
//--------------------------------------------------------------------------------------
#include	"psvscommon.fx"

//--------------------------------------------------------------------------------------
//  ピクセルシェーダー
//--------------------------------------------------------------------------------------

float4 main(VS_OUTPUT input) : SV_Target
{
	float4 col;
	float4 texcol = g_Tex.Sample(g_SamplerLinear, input.Tex);

	float4 N = input.Normal;
	N = normalize(N);

	float4 L = LightDirection;
	L = normalize(L);
	L.w = 0.0f;

	float d;
	d = max(0.0f, dot(L, N));

	float4 diffuse;
	diffuse = d * diffuseMaterial;

	//スペキュラの計算
	float s;

	float4 V;
	V = EyePos - input.WPos;
	V = normalize(V);

	float4 H;
	H = L + V;
	H = normalize(H);

	s = pow(max(0.0f, dot(N, H)), 300);

	float4 specular;
	specular = s;

	col = texcol * diffuse + specular;
	col.a = 1.0f;
	return col;
}