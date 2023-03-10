Shader "Custom/FlatShader"
{
    Properties
    {
        //_Color ("Color", Color) = (1,1,1,1)
        _MainTex ("Albedo (RGB)", 2D) = "white" {}
        //_Glossiness ("Smoothness", Range(0,1)) = 0.5
        //_Metallic ("Metallic", Range(0,1)) = 0.0
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        //LOD 200
        pass {
            CGPROGRAM
            // Physically based Standard lighting model, and enable shadows on all light types
            #include "UnityCG.cginc"
            #pragma vertex vert
            #pragma geometry geom
            #pragma fragment frag

            // Use shader model 3.0 target, to get nicer looking lighting
            #pragma target 3.0

            struct v2g
            {
                float4 pos: SV_POSITION;
                //float2 uv : TEXCOORD0;
                float3 vertex : TEXCOORD1;

            };

            struct g2f
            {
                //float2 uv : TEXCOORD0;
                float4 pos : SV_POSITION;
                float light : TEXCOORD1;
            };

            //half _Glossiness;
            //half _Metallic;
            //fixed4 _Color;
            sampler2D _MainTex;

            // Add instancing support for this shader. You need to check 'Enable Instancing' on materials that use the shader.
            // See https://docs.unity3d.com/Manual/GPUInstancing.html for more information about instancing.
            // #pragma instancing_options assumeuniformscaling
            UNITY_INSTANCING_BUFFER_START(Props)
                // put more per-instance properties here
            UNITY_INSTANCING_BUFFER_END(Props)

            v2g vert (appdata_base v)
            {
                v2g output;
                output.vertex = v.vertex;
                //output.uv = v.texcoord;
                output.pos = UnityObjectToClipPos(v.vertex);
                return output;
            }

            [maxvertexcount(3)]
            void geom(triangle v2g IN[3], inout TriangleStream<g2f> tristream)
            {
                g2f o;

                float3 vec1 = IN[1].vertex - IN[0].vertex;
                float3 vec2 = IN[2].vertex - IN[0].vertex;
                float3 normal = cross(vec1, vec2);
                normal = normalize(normal);
                //normal = normalize(mul(normal, (float3x3) unity_WorldToObject));
            
                float3 lightDir = normalize(_WorldSpaceLightPos0.xyz);
                o.light = max(0., dot(normal, lightDir));

                //o.uv = (IN[0].uv + IN[1].uv + IN[2].uv) / 3;

                for (int i = 0; i < 3; i++)
                {
                    o.pos = IN[i].pos;
                    tristream.Append(o);
                }
            }

            fixed4 frag(g2f i) : SV_Target
            {
                float4 col = i.light;
                return col;
            }

            ENDCG
        }  
        
    }
    FallBack "Diffuse"
}
