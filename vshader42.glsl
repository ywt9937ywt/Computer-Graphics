/***************************
 * File: vshader42.glsl:
 *   A simple vertex shader.
 *
 * - Vertex attributes (positions & colors) for all vertices are sent
 *   to the GPU via a vertex buffer object created in the OpenGL program.
 *
 * - This vertex shader uses the Model-View and Projection matrices passed
 *   on from the OpenGL program as uniform variables of type mat4.
 ***************************/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec3 vPosition;
in  vec4 vColor;
in vec4 vnormal;
in vec4 m_specular_color;
in vec4 m_diffuse_color;
in vec4 m_ambient_color;
in int v_fog_flag;
in vec2 v_tex_Coord;
//in vec4 view_direction;
out vec2 f_tex_Coord2;
out float f_tex_Coord1;
out vec4 color;
out float z_length;

vec4 L_pos = vec4(-14.0, 12.0, -3.0, 1.0);
vec4 p_specular_color = vec4(1.0, 1.0, 1.0, 1.0);
vec4 p_diffuse_color = vec4(1.0, 1.0, 1.0, 1.0);
vec4 p_ambient_color = vec4(0.0, 0.0, 0.0, 1.0);

vec4 spot_direction = vec4(-6.0, 0.0, -4.5, 1.0)-L_pos;
float PI = 3.14;

uniform mat4 lookat;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 g_specular_color;
uniform vec4 g_diffuse_color;
uniform vec4 g_ambient_color;
uniform vec4 light_direction;
uniform bool enable_light;
uniform bool is_point_light;
uniform int sphere_tex_coor;

void main() 
{
f_tex_Coord2 = v_tex_Coord;
if (sphere_tex_coor == 1) {
    f_tex_Coord1 = 1.0;// 2.5 * vPosition.x;
}
if (sphere_tex_coor == 2) {
    f_tex_Coord1 = 1.5 * (vPosition.x + vPosition.y + vPosition.z);
}
vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
vec4 vColor4 = vec4(vColor.r, vColor.g, vColor.b, vColor.a);
vec4 pos_view = model_view * vPosition4;
if (sphere_tex_coor == 3) {
    f_tex_Coord1 = 2.5 * pos_view.x;
}
if (sphere_tex_coor == 4) {
    f_tex_Coord1 = 1.5 * (pos_view.x + pos_view.y + pos_view.z);
}
z_length = length(pos_view);
gl_Position = projection * pos_view;
if (enable_light) {
    // YJC: Original, incorrect below:
    //      gl_Position = projection*model_view*vPosition/vPosition.w;
    

    L_pos = lookat * L_pos;
    vec4 L_dir = normalize(L_pos - pos_view);
    spot_direction = normalize(spot_direction);

    vec4 view_direction = -normalize(pos_view);
    vec4 vnormaled = normalize(model_view * vnormal);
    float phi = max(0., dot(vnormaled, normalize(-light_direction + view_direction)));
    //vec4 c = m_diffuse_color * max(0., dot(vnormaled, -light_direction)) * g_diffuse_color;
    vec4 distant_light = m_ambient_color * g_ambient_color + m_diffuse_color * max(0., dot(vnormaled, -light_direction)) * g_diffuse_color + m_specular_color * pow(phi, 125) * g_specular_color;

    float d = length(L_pos - pos_view);
    float atten = (1 / (2.0 + 0.01 * d + 0.001 * d * d));
    vec4 point_light = vec4(0.0, 0.0, 0.0, 0.0);
    phi = max(0., dot(vnormaled, normalize(L_dir + view_direction)));
    if (is_point_light) {
        point_light = atten * (m_ambient_color * p_ambient_color + m_diffuse_color * max(0., dot(vnormaled, L_dir)) * p_diffuse_color + m_specular_color * pow(phi, 125) * p_specular_color);
    }
    else {
        spot_direction = normalize(lookat * spot_direction);
        atten = atten *(cos(PI / 9) < dot(spot_direction, -L_dir) ? pow(dot(spot_direction, -L_dir), 15) : 0);
        point_light= atten * (m_ambient_color * p_ambient_color + m_diffuse_color * max(0., dot(vnormaled, L_dir)) * p_diffuse_color + m_specular_color * pow(phi, 125) * p_specular_color);
    }
    
    vec4 c = point_light+distant_light;
    //float phi = max(0., dot(vnormaled, normalize(L_dir + view_direction)));
    //vec4 c = m_diffuse_color * max(0., dot(vnormaled, -light_direction)) * g_diffuse_color;
    //c = c + m_ambient_color * g_ambient_color + m_diffuse_color * max(0., dot(vnormaled, L_dir)) * g_diffuse_color + m_specular_color * pow(phi, 125) * g_specular_color;
    color = vec4(c.rgb, 1.0);
    //color = vec4(1.0, 0.0, 0.0, 1.0);
}
else {
    //color = vec4(vColor4.rgb, 1.0);
    color = vColor4;
    //color = vec4(0.0, 1.0, 0.0, 1.0);
}
} 
