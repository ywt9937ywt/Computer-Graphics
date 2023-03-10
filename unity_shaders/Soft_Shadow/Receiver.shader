Shader "Custom/Receiver"
{
    Properties
    {
        _MainTex("Texture", 2D) = "white" {}
    }
        SubShader
    {
        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #pragma multi_compile_fog

            #include "UnityCG.cginc"

            sampler2D _MainTex;
            float4 _MainTex_ST;

            struct v2f {
                float4 pos:SV_POSITION;
                float2 uv:TEXCOORD0;
                float2 uv2:TEXCOORD1;
                float4 proj : TEXCOORD3;
                float2 depth : TEXCOORD4;
            };


            float4x4 my_ProjectionMatrix;
            sampler2D my_DepthTexture;

            v2f vert(appdata_full v)
            {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);

                o.depth = o.pos.zw;
                my_ProjectionMatrix = mul(my_ProjectionMatrix, unity_ObjectToWorld);
                o.proj = mul(my_ProjectionMatrix, v.vertex);
                //--------------------------------------------------
                o.uv = TRANSFORM_TEX(v.texcoord, _MainTex);
                //o.uv2 = v.texcoord1.xy * unity_LightmapST.xy + unity_LightmapST.zw;
                //UNITY_TRANSFER_FOG(o, o.pos);

                return o;
            }

            fixed4 frag(v2f v) : COLOR
            {
                fixed4 col = tex2D(_MainTex, v.uv);

                float receiver_depth = v.proj.z / v.proj.w;
                float blocker_depth = DecodeFloatRGBA(tex2D(my_DepthTexture, v.proj.xy));
                float shadow_sum = 0;
                //fixed4 dcol = tex2D(my_DepthTexture, v.proj.xy);
                int n = 4;
                int pcss_size = 2 * n + 1;
                for (int i = -n; i <= n; i++) {
                    for (int j = -n; j <= n; j++) {
                        float2 sample_p = float2(v.proj.x + 0.001*i, v.proj.y + 0.001*j);
                        fixed4 dcol = tex2D(my_DepthTexture, sample_p);
                        float d = DecodeFloatRGBA(dcol);
                        if (receiver_depth > d) {
                            shadow_sum += d;
                        }
                    }
                }
                shadow_sum = shadow_sum / (pcss_size * pcss_size);
                int kernelSize = 2*(receiver_depth - shadow_sum) / shadow_sum;
                //int kernelSize = min(4 * (1 - shadow_sum / receiver_depth), 4);
                int kernel = 2 * kernelSize + 1;
                int kernel_sum = 0;
                /*for (int a = -kernelSize; a <= kernelSize; a++) {
                    for (int b = -kernelSize; b <= kernelSize; b++) {
                        float2 sample_p = float2(v.proj.x + 0.001 * a, v.proj.y + 0.001 * b);
                        fixed4 dcol = tex2D(my_DepthTexture, sample_p);
                        float d = DecodeFloatRGBA(dcol);
                        if (receiver_depth > d) {
                            kernel_sum++;
                        }
                    }
                }
                kernel_sum = kernel_sum / (kernel * kernel);*/

                //fixed4 dcol = tex2D(my_DepthTexture, v.uv);
                /*float shadowScale = 1;
                if (depth > d)
                {
                    shadowScale = 0.55;
                }*/
                //return fixed4(col.x * shadowScale, col.y * shadowScale, col.z * shadowScale, 1);
                //if(shadow_sum == 1) return fixed4(0, 0, 0, 0);
                //return fixed4(kernel_sum, kernel_sum, kernel_sum, 1);
                return fixed4(kernelSize, 0, 0, 1);
                //return col * shadowScale;
                //return col * (1- kernel_sum);

            }
            ENDCG
        }
    }
}