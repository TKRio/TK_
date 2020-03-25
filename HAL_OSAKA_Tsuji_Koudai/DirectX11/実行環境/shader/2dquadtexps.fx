#include	"psvscommon.fx"

//--------------------------------------------------------------------------------------
//  ピクセルシェーダー
//--------------------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_Target
{
	float4 texcol = g_Tex.Sample(g_SamplerLinear, input.Tex);
	float gray;
	return texcol;
	/*gray = 0.299*texcol.r + 0.587*texcol.g + 0.114*texcol.b;

	const float sRedScale = 1.07;
	const float sGreenScale = 0.74;
	const float sBlueScale = 0.43;

	float red;
	float green;
	float blue;

	red = gray * sRedScale;
	green = gray * sGreenScale;
	blue = gray * sBlueScale;

	return float4(red,green,blue, 1.0f);*/
}