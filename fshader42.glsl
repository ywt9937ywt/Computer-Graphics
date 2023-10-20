/*****************************
 * File: fshader42.glsl
 *       A simple fragment shader
 *****************************/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec4 color;
in float z_length;
in vec2 f_tex_Coord2;
in float f_tex_Coord1;
uniform int f_fog_flag;
uniform sampler2D texture_2D; 
uniform sampler1D texture_1D;
uniform int Texture_app_flag; // 0: no texture application: obj color
                              // 1: texutre color
                              // 2: (obj color) * (texture color)
uniform int t1or2;
out vec4 fColor;

float fog_density = 0.09;
float f = 1.0;
vec4 fog_color;
void main() 
{
    fog_color = vec4(0.7, 0.7, 0.7, 0.5);
    vec4 c = color;
    
    if (Texture_app_flag == 0)
        c = color;
    else if (Texture_app_flag == 1) {
        if (t1or2 == 2)
            c = texture(texture_2D, f_tex_Coord2);
        if (t1or2 == 1) {
            c = texture(texture_1D, f_tex_Coord1);
        }
    }
    else {
        // Texture_app_flag == 2
        if (t1or2 == 2)
            c = texture(texture_2D, f_tex_Coord2);
        if (t1or2 == 1)
            c = texture(texture_1D, f_tex_Coord1);
    }

    switch (f_fog_flag) {
    case 1:
        //Linear
        f = (18 - z_length) / (18 - 0);
        break;
    case 2:
        //Exponential
        f = exp(-fog_density*z_length);
        break;
    case 3:
        //Exponential square
        f = exp(-(fog_density * z_length)* (fog_density * z_length));
        break;
    }
    f = clamp(f, 0.0, 1.0);
    //c = mix(fog_color, c, f);
    c = f * c + (1 - f) * fog_color;
    // fColor = vec4(c.w, c.w, c.w, 1);
    fColor = c;
} 

