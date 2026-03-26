Shader "Crescent/URP/Blinn-Phong"
{
    Properties
    {
        _Color ("Diffuse Color", Color) = (1,1,1,1)
        _MainTex ("Diffuse Texture", 2D) = "white" {}
        _AmbientColor ("Ambient Color", Color) = (0.2,0.2,0.2,1)
        _SpecularColor ("Specular Color", Color) = (1,1,1,1)
        _Shininess ("Shininess", Float) = 32
        _Transparency ("Transparency", Range(0,1)) = 1
        _Illumination ("Illumination Model", Float) = 2
        [NoScaleOffset] _SphereTex ("Reflection SphereMap", 2D) = "black" {}
        _SphereBlend ("SphereMap Blend", Range(0,1)) = 0
        [NoScaleOffset] _CubeTex ("Reflection Cubemap", Cube) = "" {}
        _CubeBlend ("Cubemap Blend", Range(0,1)) = 0
        [HideInInspector] _SrcBlend ("_SrcBlend", Float) = 1
        [HideInInspector] _DstBlend ("_DstBlend", Float) = 0
        [HideInInspector] _ZWrite ("_ZWrite", Float) = 1
    }

    SubShader
    {
        Tags { "RenderPipeline"="UniversalPipeline" "RenderType"="Opaque" "Queue"="Geometry" }
        LOD 300

        Pass
        {
            Name "UniversalForward"
            Tags { "LightMode"="UniversalForward" }

            Cull Back
            ZWrite [_ZWrite]
            Blend [_SrcBlend] [_DstBlend]

            HLSLPROGRAM
            #pragma target 3.0
            #pragma vertex vert
            #pragma fragment frag

            #pragma multi_compile _ _MAIN_LIGHT_SHADOWS
            #pragma multi_compile _ _MAIN_LIGHT_SHADOWS_CASCADE
            #pragma multi_compile _ _ADDITIONAL_LIGHTS
            #pragma multi_compile _ _ADDITIONAL_LIGHT_SHADOWS
            #pragma multi_compile _ _SHADOWS_SOFT
            #pragma multi_compile_fog

            #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Core.hlsl"
            #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Lighting.hlsl"

            TEXTURE2D(_MainTex);
            SAMPLER(sampler_MainTex);
            TEXTURE2D(_SphereTex);
            SAMPLER(sampler_SphereTex);
            TEXTURECUBE(_CubeTex);
            SAMPLER(sampler_CubeTex);

            CBUFFER_START(UnityPerMaterial)
                float4 _Color;
                float4 _AmbientColor;
                float4 _SpecularColor;
                float4 _MainTex_ST;
                float _Shininess;
                float _Transparency;
                float _Illumination;
                float _SphereBlend;
                float _CubeBlend;
            CBUFFER_END

            struct Attributes
            {
                float4 positionOS : POSITION;
                float3 normalOS : NORMAL;
                float2 uv : TEXCOORD0;
            };

            struct Varyings
            {
                float4 positionCS : SV_POSITION;
                float2 uv : TEXCOORD0;
                float3 normalWS : TEXCOORD1;
                float3 viewDirWS : TEXCOORD2;
                float3 positionWS : TEXCOORD3;
                float4 shadowCoord : TEXCOORD4;
                float fogCoord : TEXCOORD5;
            };

            Varyings vert(Attributes IN)
            {
                Varyings OUT;

                VertexPositionInputs positionInputs = GetVertexPositionInputs(IN.positionOS.xyz);
                VertexNormalInputs normalInputs = GetVertexNormalInputs(IN.normalOS);

                OUT.positionCS = positionInputs.positionCS;
                OUT.positionWS = positionInputs.positionWS;
                OUT.normalWS = NormalizeNormalPerVertex(normalInputs.normalWS);
                OUT.viewDirWS = GetWorldSpaceViewDir(positionInputs.positionWS);
                OUT.uv = TRANSFORM_TEX(IN.uv, _MainTex);
                OUT.shadowCoord = GetShadowCoord(positionInputs);
                OUT.fogCoord = ComputeFogFactor(positionInputs.positionCS.z);

                return OUT;
            }

            float3 ComputeDiffuseSpecular(float3 n, float3 v, float3 albedo, float illum, float4 shadowCoord, float3 positionWS)
            {
                // Keep a constant ambient baseline so back-facing sides are still visible.
                float3 ambient = max(SampleSH(n), 0.0) + _AmbientColor.rgb;
                float3 result = albedo;

                if (illum >= 1.0)
                {
                    Light mainLight = GetMainLight(shadowCoord);
                    float ndotl = max(0.0, dot(n, mainLight.direction));
                    float mainAtten = mainLight.distanceAttenuation * mainLight.shadowAttenuation;

                    result = (albedo * ambient) + (albedo * mainLight.color * ndotl * mainAtten);

                    #ifdef _ADDITIONAL_LIGHTS
                    uint additionalLightsCount = GetAdditionalLightsCount();
                    for (uint i = 0u; i < additionalLightsCount; ++i)
                    {
                        Light light = GetAdditionalLight(i, positionWS);
                        float ndotlAdd = max(0.0, dot(n, light.direction));
                        float atten = light.distanceAttenuation * light.shadowAttenuation;
                        result += albedo * light.color * ndotlAdd * atten;
                    }
                    #endif
                }

                if (illum >= 2.0)
                {
                    float specPow = max(1.0, _Shininess);

                    Light mainLight = GetMainLight(shadowCoord);
                    float3 h = normalize(mainLight.direction + v);
                    float ndoth = max(0.0, dot(n, h));
                    float spec = pow(ndoth, specPow);
                    float mainAtten = mainLight.distanceAttenuation * mainLight.shadowAttenuation;
                    result += _SpecularColor.rgb * mainLight.color * spec * mainAtten;

                    #ifdef _ADDITIONAL_LIGHTS
                    uint additionalLightsCount = GetAdditionalLightsCount();
                    for (uint i = 0u; i < additionalLightsCount; ++i)
                    {
                        Light light = GetAdditionalLight(i, positionWS);
                        float3 hAdd = normalize(light.direction + v);
                        float ndothAdd = max(0.0, dot(n, hAdd));
                        float specAdd = pow(ndothAdd, specPow);
                        float atten = light.distanceAttenuation * light.shadowAttenuation;
                        result += _SpecularColor.rgb * light.color * specAdd * atten;
                    }
                    #endif
                }

                return result;
            }

            half4 frag(Varyings IN) : SV_Target
            {
                float3 n = normalize(IN.normalWS);
                float3 v = normalize(IN.viewDirWS);

                float4 tex = SAMPLE_TEXTURE2D(_MainTex, sampler_MainTex, IN.uv);
                float3 albedo = tex.rgb * _Color.rgb;
                float alpha = saturate(tex.a * _Color.a * _Transparency);

                float illum = floor(_Illumination + 0.5);
                float3 result = ComputeDiffuseSpecular(n, v, albedo, illum, IN.shadowCoord, IN.positionWS);

                float3 sphereReflDir = reflect(-v, n);
                float sphereM = 2.0 * sqrt(
                    sphereReflDir.x * sphereReflDir.x +
                    sphereReflDir.y * sphereReflDir.y +
                    (sphereReflDir.z + 1.0) * (sphereReflDir.z + 1.0));
                float2 sphereUV = sphereReflDir.xy / max(sphereM, 1e-5) + 0.5;
                float3 sphereReflection = SAMPLE_TEXTURE2D(_SphereTex, sampler_SphereTex, saturate(sphereUV)).rgb;

                float3 reflDir = reflect(-v, n);
                float3 cubeReflection = SAMPLE_TEXTURECUBE(_CubeTex, sampler_CubeTex, reflDir).rgb;
                result += sphereReflection * saturate(_SphereBlend) + cubeReflection * saturate(_CubeBlend);

                result = MixFog(result, IN.fogCoord);

                return half4(result, alpha);
            }
            ENDHLSL
        }
    }

    Fallback Off
}