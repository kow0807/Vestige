struct PS_INPUT
{
    float4 svPos    : SV_POSITION;
    float4 diffuse  : COLOR0;
    float4 specular : COLOR1;
    float2 uv       : TEXCOORD0;
    float2 suv      : TEXCOORD1;
};

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

// 雷演出用 定数バッファ（b4にバインド）
cbuffer cbLightning : register(b4)
{
    float lightningTime;     // 経過時間（秒）など（ディストーション用）
    float lightningStrength; // 雷の強さ（0.0～1.0）
    float2 pad;
}

// 疑似ノイズ生成関数
float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{
    float2 uv = PSInput.uv;

    float power = saturate(lightningStrength);
    float time = lightningTime;

    // ==== ディストーション（強調） ====
    float distortion = 0.5 * power; // 強調
    float noise = rand(uv * 20.0 + time);
    uv += (noise - 0.5) * distortion;

    // ==== RGBずれ（強調） ====
    float2 offset = float2(0.9, 0.0) * power; // 強調

    float r = tex.Sample(texSampler, uv + offset).r;
    float g = tex.Sample(texSampler, uv).g;
    float b = tex.Sample(texSampler, uv - offset).b;

    float4 col = float4(r, g, b, 1.0);

    // ==== フラッシュ ====
    float flash = power * 0.9;
    col.rgb = lerp(col.rgb, float3(1.0, 1.0, 1.0), flash);

    return col;
}