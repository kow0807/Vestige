// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit

// OUT
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

Texture2D dissolveTex : register(t1);	// ディゾルブマスク

// 定数バッファ : スロット四番目
cbuffer cbParam : register(b4)
{
	float g_time;
	float g_scroll_speed;
	float2 dummy;
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{
	// 上方向にへスクロール
	PSInput.uv.y -= g_time * g_scroll_speed;

	// 繰り返し
	PSInput.uv.y = frac(PSInput.uv.y);

	float4 color;

	color = diffuseMapTexture.Sample(diffuseMapSampler, PSInput.uv);

	return color;
}