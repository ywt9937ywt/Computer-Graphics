Shader "Custom/PhongShader"
{
    Properties{
      _Color("Diffuse Material Color", Color) = (1,1,1,1)
      _SpecColor("Specular Material Color", Color) = (1,1,1,1)
      _Shininess("Shininess", Float) = 10
    }
        SubShader{
           Pass {
               Tags { "LightMode" = "ForwardBase" }
               CGPROGRAM

               #pragma vertex vert  
               #pragma fragment frag 

               #include "UnityCG.cginc"
               uniform float4 _LightColor0;

                uniform float4 _Color;
                uniform float4 _SpecColor;
                uniform float _Shininess;

                struct vertexInput {
                    float4 vertex : POSITION;
                    float3 normal : NORMAL;
                };

                struct v2f {
                    float4 pos : SV_POSITION;
                    float4 posWorld : TEXCOORD0;
                    float3 normalDir : TEXCOORD1;
                };

                v2f vert(vertexInput input)
                {
                    v2f output;

                    float4x4 modelMatrix = unity_ObjectToWorld;
                    //float4x4 modelMatrixInverse = unity_WorldToObject;

                    output.posWorld = mul(modelMatrix, input.vertex);
                    //output.normalDir = normalize(mul(float4(input.normal, 0.0), modelMatrixInverse).xyz);
                    output.normalDir = normalize(input.normal);
                    output.pos = UnityObjectToClipPos(input.vertex);
                    return output;
                }

                float4 frag(v2f input) : COLOR
                {
                    float3 normalDirection = normalize(input.normalDir);

                    float3 viewDirection = normalize(_WorldSpaceCameraPos - input.posWorld.xyz);
                    float3 lightDirection = normalize(_WorldSpaceLightPos0.xyz);;
                    float attenuation=  1.0;

                    float3 ambientLighting = UNITY_LIGHTMODEL_AMBIENT.rgb;// *_Color.rgb;

                    float3 diffuseReflection =max(0.0, dot(normalDirection, lightDirection));
                        //attenuation * _LightColor0.rgb * _Color.rgb * 

                    float3 specularReflection = pow(max(0.0, dot(reflect(-lightDirection, normalDirection),viewDirection)), _Shininess);
                       // attenuation * _LightColor0.rgb * _SpecColor.rgb * 

                    return float4(ambientLighting + diffuseReflection
                        + specularReflection, 1.0);
                }
                ENDCG
            }
        }
        FallBack "Diffuse"
}
