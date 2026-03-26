Shader "Crescent/Blinn-Phong"
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
        Tags { "RenderType"="Opaque" "Queue"="Geometry" }
        LOD 300
        Cull Back
        ZWrite [_ZWrite]
        Blend [_SrcBlend] [_DstBlend]

        CGPROGRAM
        #pragma surface surf ObjBlinnPhong fullforwardshadows keepalpha
        #pragma target 3.0

        sampler2D _MainTex;
        sampler2D _SphereTex;
        samplerCUBE _CubeTex;
        fixed4 _Color;
        fixed4 _AmbientColor;
        fixed4 _SpecularColor;
        float _Shininess;
        float _Transparency;
        float _Illumination;
        float _SphereBlend;
        float _CubeBlend;

        struct Input
        {
            float2 uv_MainTex;
            float3 worldNormal;
            float3 viewDir;
        };

        void surf(Input IN, inout SurfaceOutput o)
        {
            fixed4 tex = tex2D(_MainTex, IN.uv_MainTex);
            o.Albedo = tex.rgb * _Color.rgb;
            o.Alpha = saturate(tex.a * _Color.a * _Transparency);

            float3 n = normalize(IN.worldNormal);
            float3 v = normalize(IN.viewDir);

            float3 sphereReflDir = reflect(-v, n);
            float sphereM = 2.0 * sqrt(
                sphereReflDir.x * sphereReflDir.x +
                sphereReflDir.y * sphereReflDir.y +
                (sphereReflDir.z + 1.0) * (sphereReflDir.z + 1.0));
            float2 sphereUV = sphereReflDir.xy / max(sphereM, 1e-5) + 0.5;
            fixed3 sphereReflection = tex2D(_SphereTex, saturate(sphereUV)).rgb;

            float3 reflDir = reflect(-v, n);
            fixed3 reflection = texCUBE(_CubeTex, reflDir).rgb;
            o.Emission =
                sphereReflection * saturate(_SphereBlend) +
                reflection * saturate(_CubeBlend);
        }

        inline fixed4 LightingObjBlinnPhong(SurfaceOutput s, fixed3 lightDir, fixed3 viewDir, fixed atten)
        {
            fixed3 n = normalize(s.Normal);
            fixed3 l = normalize(lightDir);
            fixed3 v = normalize(viewDir);
            fixed3 h = normalize(l + v);

            fixed illum = floor(_Illumination + 0.5);
            fixed3 ambient = UNITY_LIGHTMODEL_AMBIENT.rgb * _AmbientColor.rgb;
            fixed3 result = s.Albedo;

            if (illum >= 1.0)
            {
                fixed ndotl = max(0.0, dot(n, l));
                result = ambient + (s.Albedo * _LightColor0.rgb * ndotl * atten);
            }

            if (illum >= 2.0)
            {
                float specPow = max(1.0, _Shininess);
                fixed ndoth = max(0.0, dot(n, h));
                fixed spec = pow(ndoth, specPow);
                result += _SpecularColor.rgb * _LightColor0.rgb * spec * atten;
            }

            result += s.Emission;

            return fixed4(result, s.Alpha);
        }

        ENDCG
    }

    Fallback "Legacy Shaders/Transparent/Diffuse"
}
