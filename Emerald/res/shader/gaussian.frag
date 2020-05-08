#version 330
out vec4 FragColor;

in vec2 fsUv;

uniform sampler2D _Texture;

uniform bool _Horizontal;
const float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{             
     vec2 tex_offset = 1.0 / textureSize(_Texture, 0); // gets size of single texel
     vec3 result = texture(_Texture, fsUv).rgb * weight[0];
     if(_Horizontal)
     {
         for(int i = 1; i < 5; ++i)
         {
            result += texture(_Texture, fsUv + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(_Texture, fsUv - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
         }
     }
     else
     {
         for(int i = 1; i < 5; ++i)
         {
             result += texture(_Texture, fsUv + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
             result += texture(_Texture, fsUv - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
         }
     }
     FragColor = vec4(result, 1.0);
}

