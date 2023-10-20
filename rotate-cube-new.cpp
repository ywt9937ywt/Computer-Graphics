/************************************************************
 * Handout: rotate-cube-new.cpp (A Sample Code for Shader-Based OpenGL ---
                                 for OpenGL version 3.1 and later)
 * Originally from Ed Angel's textbook "Interactive Computer Graphics" 6th Ed
              sample code "example3.cpp" of Chapter 4.
 * Moodified by Yi-Jen Chiang to include the use of a general rotation function
   Rotate(angle, x, y, z), where the vector (x, y, z) can have length != 1.0,
   and also to include the use of the function NormalMatrix(mv) to return the
   normal matrix (mat3) of a given model-view matrix mv (mat4).

   (The functions Rotate() and NormalMatrix() are added to the file "mat-yjc-new.h"
   by Yi-Jen Chiang, where a new and correct transpose function "transpose1()" and
   other related functions such as inverse(m) for the inverse of 3x3 matrix m are
   also added; see the file "mat-yjc-new.h".)

 * Extensively modified by Yi-Jen Chiang for the program structure and user
   interactions. See the function keyboard() for the keyboard actions.
   Also extensively re-structured by Yi-Jen Chiang to create and use the new
   function drawObj() so that it is easier to draw multiple objects. Now a floor
   and a rotating cube are drawn.

** Perspective view of a color cube using LookAt() and Perspective()

** Colors are assigned to each vertex and then the rasterizer interpolates
   those colors across the triangles.
**************************************************************/
#include "Angel-yjc.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#define PI 3.1415
#define ImageWidth  32
#define ImageHeight 32
#define	stripeImageWidth 32

typedef Angel::vec3  color3;
typedef Angel::vec3  point3;

GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */
GLuint cube_buffer;   /* vertex buffer object id for cube */
GLuint floor_buffer;  /* vertex buffer object id for floor */
GLuint axis_buffer;  /* vertex buffer object id for axis */
GLuint sphere_buffer;  /* vertex buffer object id for sphere */
GLuint shadow_buffer;  /* vertex buffer object id for sphere */
int demo_menu_ID, shadow_menu_ID, light_menu_ID, shading_menu_ID, light_type_ID, fog_ID;
int blend_ID, tm_ground;
GLint a[4]{ 0,1,2,3 };
int input_type[4]{ 8, 128, 256, 1024 };
int sphere_1d_coor = 1;
int t1or2 = 1;
int ground_map = 1;

int animationFlag = 0; // 1: animation; 0: non-animation. Toggled by key 'a' or 'A'
int leftmouse_animationFlag = 1;
int draw_shadow_Flag = 1;

int cubeFlag = 1;   // 1: solid cube; 0: wireframe cube. Toggled by key 'c' or 'C'
int floorFlag = 1;  // 1: solid floor; 0: wireframe floor. Toggled by key 'f' or 'F'
int sphereFlag = 1;  // 1: solid sphere; 0: wireframe sphere. Toggled by key 'f' or 'F'
int shadowFlag = 1;
int shadingFlag = 0;  // 1; flat shading 0: smooth shading
int light_Flag = 1;
int texture_app_flag = 1;  // 0: no texture application: obj color
                           // 1: texutre color
                           // 2: (obj color) * (texture color)
int need_blend = 1;
GLuint enable_light, is_point_light, fog_mode; // 1; lighting 0: no lighting
static GLuint texName;
// Projection transformation parameters
GLfloat  fovy = 60.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect = 1;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 18.0;

mat4 rotate_m(vec4(1.0, 0.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0), vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 1.0));
GLfloat rotate_speed = 0.5f;
GLfloat angle = 0.0; // rotation angle in degrees
vec4 init_eye(7.0, 3.0, -10.0, 1.0);
vec4 L_pos(-14.0, 12.0, -3.0, 1.0);
mat4 shadow_m(vec4(L_pos.y, -L_pos.x, 0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), vec4(0.0, -L_pos.z, L_pos.y, 0.0),vec4(0.0, -1.0, 0.0, L_pos.y));
vec4 shadow_color(0.25, 0.25, 0.25, 0.65);
vec4 floor_ambient_light_color(0.2, 0.2, 0.2, 1.0);
vec4 floor_diffuse_color(0.0, 1.0, 0.0, 1.0);
vec4 floor_specular_color(0.0, 0.0, 0.0, 1.0);
vec4 sphere_ambient_light_color(0.2, 0.2, 0.2, 1.0);
vec4 sphere_diffuse_color(1.0, 0.84, 0.0, 1.0);
vec4 sphere_specular_color(1.0, 0.84, 0.0, 1.0);
vec4 directional_light_color(0.0, 0.0, 0.0, 1.0);
vec4 eye = init_eye;               // current viewer position

point3 A(-4, 1, 4);
point3 B(-1, 1, -4);
point3 C(3, 1, 5);
vec4 AB_vec = vec4(3.0, 0.0, -8.0, 0.0);
vec4 BC_vec = vec4(4.0, 0.0, 9.0, 0.0);
vec4 CA_vec = vec4(-7.0, 0.0, -1.0, 0.0);
vec4 AB = normalize(vec4(3.0, 0.0, -8.0, 0.0));
vec4 BC = normalize(vec4(4.0, 0.0, 9.0, 0.0));
vec4 CA = normalize(vec4(-7.0, 0.0, -1.0, 0.0));
float move_dist = 0.0f;
float first_round = length(vec4(3.0, 0.0, -8.0, 0.0));
float second_round = length(vec4(4.0, 0.0, 9.0, 0.0));
float third_round = length(vec4(-7.0, 0.0, -1.0, 0.0));

const int cube_NumVertices = 36; //(6 faces)*(2 triangles/face)*(3 vertices/triangle)
#if 0
point3 cube_points[cube_NumVertices]; // positions for all vertices
color3 cube_colors[cube_NumVertices]; // colors for all vertices
#endif
#if 1
point3 cube_points[100]; 
color3 cube_colors[100];
#endif

int sphere_ts = 128;
point3 sphere_points[1024 * 3];
color3 sphere_colors[1024 * 3];
color3 sphere_light_term[1024 * 9];
color3 floor_light_term[6*3];
vec4 flat_sphere_normals[1024 * 3];
vec4 smooth_sphere_normals[1024 * 3];
point3 shadow_points[1024 * 3];
vec4 shadow_colors[1024 * 3];
GLubyte Image[ImageHeight][ImageWidth][4];
GLubyte stripeImage[4 * stripeImageWidth];

const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 floor_points[floor_NumVertices]; // positions for all vertices
color3 floor_colors[floor_NumVertices]; // colors for all vertices
vec2 floor_texCoord[6];
vec4 floor_normals[floor_NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point3 vertices[8] = {
    point3(-5, 0,  8),
    point3(-5, 12,  8),
    point3(5, 12,  8),
    point3(5, 0,  8),
    point3(-5, 0, -4),
    point3(-5, 12, -4),
    point3(5, 12, -4),
    point3(5, 0, -4)
};
// RGBA colors
color3 vertex_colors[8] = {
    color3( 0.0, 0.0, 0.0),  // black
    color3( 1.0, 0.0, 0.0),  // red
    color3( 1.0, 1.0, 0.0),  // yellow
    color3( 0.2, 1.2, 0.2),  // green
    color3( 0.0, 0.0, 1.0),  // blue
    color3( 1.0, 0.0, 1.0),  // magenta
    color3( 1.0, 1.0, 1.0),  // white
    color3( 0.0, 1.0, 1.0)   // cyan
};

point3 axis_vertices[6] = {
    color3(0.0, 0.02, 0.0),
    color3(10.0, 0.02, 0.0),
    color3(0.0, 0.0, 0.0),
    color3(0.0, 10.0, 0.0),
    color3(0.0, 0.02, 0.0),
    color3(0.0, 0.02, 10.0),
};
color3 axis_colors[6] = {
    color3(1.0, 0.0, 0.0),
    color3(1.0, 0.0, 0.0),
    color3(1.0, 0.0, 1.0),
    color3(1.0, 0.0, 1.0),
    color3(0.0, 0.0, 1.0),
    color3(0.0, 0.0, 1.0),
};
//----------------------------------------------------------------------------
int Index = 0; // YJC: This must be a global variable since quad() is called
               //      multiple times and Index should then go up to 36 for
               //      the 36 vertices and colors

// quad(): generate two triangles for each face and assign colors to the vertices
void quad( int a, int b, int c, int d )
{
    cube_colors[Index] = vertex_colors[a]; cube_points[Index] = vertices[a]; Index++;
    cube_colors[Index] = vertex_colors[b]; cube_points[Index] = vertices[b]; Index++;
    cube_colors[Index] = vertex_colors[c]; cube_points[Index] = vertices[c]; Index++;

    cube_colors[Index] = vertex_colors[c]; cube_points[Index] = vertices[c]; Index++;
    cube_colors[Index] = vertex_colors[d]; cube_points[Index] = vertices[d]; Index++;
    cube_colors[Index] = vertex_colors[a]; cube_points[Index] = vertices[a]; Index++;
}
//----------------------------------------------------------------------------
// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}
//-------------------------------
// generate 2 triangles: 6 vertices and 6 colors
void floor()
{
    floor_colors[0] = vertex_colors[3]; floor_points[0] = vertices[3]; floor_normals[0] = vec4(0.0, 1.0, 0.0, 0.0);
    floor_colors[1] = vertex_colors[3]; floor_points[1] = vertices[0]; floor_normals[1] = vec4(0.0, 1.0, 0.0, 0.0);
    floor_colors[2] = vertex_colors[3]; floor_points[2] = vertices[4]; floor_normals[2] = vec4(0.0, 1.0, 0.0, 0.0);

    floor_colors[3] = vertex_colors[3]; floor_points[3] = vertices[4]; floor_normals[3] = vec4(0.0, 1.0, 0.0, 0.0);
    floor_colors[4] = vertex_colors[3]; floor_points[4] = vertices[7]; floor_normals[4] = vec4(0.0, 1.0, 0.0, 0.0);
    floor_colors[5] = vertex_colors[3]; floor_points[5] = vertices[3]; floor_normals[5] = vec4(0.0, 1.0, 0.0, 0.0);

    floor_texCoord[0] = vec2(0.0, 0.0); floor_texCoord[1] = vec2(5.0, 0.0); floor_texCoord[2] = vec2(5.0, 6.0);
    floor_texCoord[3] = vec2(5.0, 6.0); floor_texCoord[4] = vec2(0.0, 6.0); floor_texCoord[5] = vec2(0.0, 0.0);
    
    for (int i = 0; i < 6; i++) {
        floor_light_term[i] = color3(0.2, 0.2, 0.2);
        floor_light_term[6+ i] = color3(0.0, 1.0, 0.0);
        floor_light_term[12+ i] = color3(0.0, 0.0, 0.0);
    }
}
//----------------------------------------------------------------------------
void file_select()
{
    std::cout << "two files are available: " << std::endl;
    std::cout << "1. 8" << std::endl;
    std::cout << "2. 128" << std::endl;
    std::cout << "3. 256" << std::endl;
    std::cout << "4. 1024" << std::endl;
    std::cout << "Please input the index of the file you prefer: ";
    std::cin >> sphere_ts;
    if (sphere_ts - 1 < 0 && sphere_ts - 1 > 3) sphere_ts = 128;
    else sphere_ts = input_type[sphere_ts - 1];
    std::cout << sphere_ts << std::endl;
    std::string dir = std::string("C:/Users/10178/Desktop/study/CS6533/Rotate_sphere/sphere.") + std::to_string(sphere_ts);
    std::ifstream infile(dir);
    std::string line;
    std::getline(infile, line);
    for (int i = 0; i < sphere_ts; i++) {
        std::getline(infile, line);
        for (int j = 0; j < 3; j++) {
            std::getline(infile, line);
            //std::vector<float> point(3);
            std::istringstream s(line);
            float a, b, c;
            while ((s >> a >> b >> c)) {
                //std::cout << a << " " << b << " " << c << std::endl;
                sphere_points[i * 3 + j] = point3(a, b, c);
                sphere_colors[i * 3 + j] = color3(1.0, 0.84, 0);
                //vec4 p = shadow_m*point3(a, b, c);
                //shadow_points[i * 3 + j] = point3(p.x, p.y, p.z) / p.w;
                shadow_points[i * 3 + j] = point3(a, b, c);
                shadow_colors[i * 3 + j] = shadow_color;
                sphere_light_term[i * 3 + j] = color3(0.2, 0.2, 0.2);
                sphere_light_term[sphere_ts *3 + i * 3 + j] = color3(1.0, 0.84, 0.0);
                sphere_light_term[sphere_ts *6 + i * 3 + j] = color3(1.0, 0.84, 0.0);
                smooth_sphere_normals[i * 3 + j] = vec4(point3(a, b, c), 0);
                if (j == 2) {
                    vec4 A = sphere_points[i * 3 + j];
                    vec4 B = sphere_points[i * 3 + j - 1];
                    vec4 C = sphere_points[i * 3 + j - 2];
                    vec4 cb = B - C;
                    vec4 ca = A - C;
                    flat_sphere_normals[i * 3 + j] = vec4(normalize(cross(cb, ca)),0.0);
                    flat_sphere_normals[i * 3 + j -1] = vec4(normalize(cross(cb, ca)), 0.0);
                    flat_sphere_normals[i * 3 + j -2] = vec4(normalize(cross(cb, ca)), 0.0);
                }
            }
        }
    }
}
// OpenGL initialization
void rebind() {
    glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * sphere_ts * (sizeof(point3) + sizeof(point3) + sizeof(vec4) + 3 * sizeof(color3)),
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sphere_ts * sizeof(point3), sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * sphere_ts * sizeof(point3), 3 * sphere_ts * sizeof(point3),
        sphere_colors);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * sphere_ts * sizeof(point3) + 3 * sphere_ts * sizeof(point3), 3 * sphere_ts * sizeof(vec4),
        shadingFlag ? flat_sphere_normals : smooth_sphere_normals);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * sphere_ts * sizeof(point3) + 3 * sphere_ts * sizeof(point3) + 3 * sphere_ts * sizeof(vec4), 9 * sphere_ts * sizeof(color3),
        sphere_light_term);
}

void image_set_up(void)
{
    int i, j, c;

    /* --- Generate checkerboard image to the image array ---*/
    for (i = 0; i < ImageHeight; i++)
        for (j = 0; j < ImageWidth; j++)
        {
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));

            if (c == 1) /* white */
            {
                c = 255;
                Image[i][j][0] = (GLubyte)c;
                Image[i][j][1] = (GLubyte)c;
                Image[i][j][2] = (GLubyte)c;
            }
            else  /* green */
            {
                Image[i][j][0] = (GLubyte)0;
                Image[i][j][1] = (GLubyte)150;
                Image[i][j][2] = (GLubyte)0;
            }

            Image[i][j][3] = (GLubyte)255;
        }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /*--- Generate 1D stripe image to array stripeImage[] ---*/
    for (j = 0; j < stripeImageWidth; j++) {
        /* When j <= 4, the color is (255, 0, 0),   i.e., red stripe/line.
           When j > 4,  the color is (255, 255, 0), i.e., yellow remaining texture
         */
        stripeImage[4 * j] = (GLubyte)255;
        stripeImage[4 * j + 1] = (GLubyte)((j > 4) ? 255 : 0);
        stripeImage[4 * j + 2] = (GLubyte)0;
        stripeImage[4 * j + 3] = (GLubyte)255;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    /*----------- End 1D stripe image ----------------*/

    /*--- texture mapping set-up is to be done in
          init() (set up texture objects),
          display() (activate the texture object to be used, etc.)
          and in shaders.
     ---*/

}

void init()
{
    program = InitShader("vshader42.glsl", "fshader42.glsl");
    glUseProgram(program);

    image_set_up();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(2, &texName);
    //glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
    glActiveTexture(GL_TEXTURE0);  // Set the active texture unit to be 0 
    glBindTexture(GL_TEXTURE_2D, 0); // Bind the texture to this texture unit
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight,
        0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, 1);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 4 * stripeImageWidth,
        0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage); 
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //colorcube();
#if 0 //YJC: The following is not needed
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
#endif

 // Create and initialize a vertex buffer object for cube, to be used in display()
    glGenBuffers(1, &cube_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, cube_buffer);



#if 0
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_points) + sizeof(cube_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_points), cube_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_points), sizeof(cube_colors),
                    cube_colors);
#endif
#if 0
    glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(point3)*cube_NumVertices + sizeof(color3)*cube_NumVertices,
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
                    sizeof(point3) * cube_NumVertices, cube_points);
    glBufferSubData(GL_ARRAY_BUFFER, 
                    sizeof(point3) * cube_NumVertices, 
                    sizeof(color3) * cube_NumVertices,
                    cube_colors);
#endif

    floor();     
 // Create and initialize a vertex buffer object for floor, to be used in display()
    glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors) + sizeof(floor_normals) + sizeof(floor_light_term) + sizeof(floor_texCoord),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
                    floor_colors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors), sizeof(floor_normals),
        floor_normals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors)+ sizeof(floor_normals), sizeof(floor_light_term),
        floor_light_term);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors) + sizeof(floor_normals) + sizeof(floor_light_term), sizeof(floor_texCoord),
        floor_texCoord);

    // Create buffer for Axis
    glGenBuffers(1, &axis_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices) + sizeof(axis_colors),
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(axis_vertices), axis_vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(axis_vertices), sizeof(axis_colors),
        axis_colors);

    // Create buffer for sphere
    glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * sphere_ts * (sizeof(point3) + sizeof(point3) + sizeof(vec4) + 3*sizeof(color3)),
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*sphere_ts*sizeof(point3), sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * sphere_ts * sizeof(point3), 3 * sphere_ts * sizeof(point3),
        sphere_colors);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * sphere_ts * sizeof(point3) + 3 * sphere_ts * sizeof(point3), 3 * sphere_ts * sizeof(vec4),
        shadingFlag ? flat_sphere_normals : smooth_sphere_normals);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * sphere_ts * sizeof(point3) + 3 * sphere_ts * sizeof(point3)+ 3 * sphere_ts * sizeof(vec4), 9 * sphere_ts * sizeof(color3), 
        sphere_light_term);

    // Create buffer for shadow
    glGenBuffers(1, &shadow_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, shadow_buffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * sphere_ts * (sizeof(point3) + sizeof(vec4)),
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sphere_ts * sizeof(point3), shadow_points);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * sphere_ts * sizeof(point3), 3 * sphere_ts * sizeof(vec4),
        shadow_colors);

 // Load shaders and create a shader program (to be used in display())
    
    //init_light();
    float g_ambient_light_color[4]{ 1.0, 1.0, 1.0, 1.0 };
    float g_diffuse_color[4]{ 0.8, 0.8, 0.8, 1.0 };
    float g_specular_color[4]{ 0.2, 0.2, 0.2, 1.0 };
    float dlight_eye_system[4]{ 0.1, 0.0, -1.0, 0.0 };
    GLuint gac = glGetUniformLocation(program, "g_ambient_color");
    GLuint gdc = glGetUniformLocation(program, "g_diffuse_color");
    GLuint gsc = glGetUniformLocation(program, "g_specular_color");
    GLuint ld = glGetUniformLocation(program, "light_direction");
    glUniform4fv(gac, 1, g_ambient_light_color);
    glUniform4fv(gdc, 1, g_diffuse_color);
    glUniform4fv(gsc, 1, g_specular_color);
    glUniform4fv(ld, 1, dlight_eye_system);
    enable_light = glGetUniformLocation(program, "enable_light");
    glUniform1iv(enable_light, 1, &a[1]);
    is_point_light = glGetUniformLocation(program, "is_point_light");
    glUniform1iv(is_point_light, 1, &a[0]);
    fog_mode = glGetUniformLocation(program, "f_fog_flag");
    glUniform1iv(fog_mode, 1, &a[0]);
    glUniform1i(glGetUniformLocation(program, "t1or2"), a[2]);

    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.529, 0.807, 0.92, 0.0 ); 
    glLineWidth(2.0);
}
//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices.
//
void drawObj(GLuint buffer, int num_vertices, int isP3, GLuint mode = GL_TRIANGLES)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation(program, "vColor"); 
    glEnableVertexAttribArray(vColor);
    unsigned int mysize = sizeof(point3) * num_vertices;
    glVertexAttribPointer(vColor, isP3*3 + (1 - isP3)*4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(mysize));

    GLuint vnor = glGetAttribLocation(program, "vnormal");
    glEnableVertexAttribArray(vnor);
    mysize = mysize + (isP3 ? sizeof(point3) * num_vertices : sizeof(vec4) * num_vertices);
    glVertexAttribPointer(vnor, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(mysize));

    GLuint m_ambient_color = glGetAttribLocation(program, "m_ambient_color");
    glEnableVertexAttribArray(m_ambient_color);
    mysize = mysize + num_vertices * sizeof(vec4);
    glVertexAttribPointer(m_ambient_color, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(mysize));

    GLuint m_diffuse_color = glGetAttribLocation(program, "m_diffuse_color");
    glEnableVertexAttribArray(m_diffuse_color);
    mysize = mysize + num_vertices * sizeof(color3);
    glVertexAttribPointer(m_diffuse_color, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(mysize));

    GLuint m_specular_color = glGetAttribLocation(program, "m_specular_color");
    glEnableVertexAttribArray(m_specular_color);
    mysize = mysize + num_vertices * sizeof(color3);
    glVertexAttribPointer(m_specular_color, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(mysize));

    GLuint tex_texCoord = glGetAttribLocation(program, "v_tex_Coord");
    glEnableVertexAttribArray(tex_texCoord);
    mysize = mysize + num_vertices * sizeof(color3);
    glVertexAttribPointer(tex_texCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(mysize));


      // the offset is the (total) size of the previous vertex attribute array(s)

    /* Draw a sequence of geometric objs (triangles) from the vertex buffer
       (using the attributes specified in each enabled vertex attribute array) */
    glDrawArrays(mode, 0, num_vertices);

    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);
}
//----------------------------------------------------------------------------
void display( void )
{
  GLuint  model_view;  // model-view matrix uniform shader variable location
  GLuint  projection;  // projection matrix uniform shader variable location
  GLuint  lookat;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram(program); // Use the shader program

    glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
    glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
    //glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
    // Pass on the value of texture_app_flag to the fragment shader
    glUniform1i(glGetUniformLocation(program, "Texture_app_flag"),
        texture_app_flag);
    model_view = glGetUniformLocation(program, "model_view" );
    projection = glGetUniformLocation(program, "projection" );
    lookat = glGetUniformLocation(program, "lookat");
/*---  Set up and pass on Projection matrix to the shader ---*/
    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

/*---  Set up and pass on Model-View matrix to the shader ---*/
    // eye is a global variable of vec4 set to init_eye and updated by keyboard()
    vec4    at(0.0, 0.0, 0.0, 1.0);
    vec4    up(0.0, 1.0, 0.0, 0.0);

    mat4  mv = LookAt(eye, at, up);
    glUniformMatrix4fv(lookat, 1, GL_TRUE, mv);
/*----- Set Up the Model-View matrix for the cube -----*/
#if 0 // The following is to verify the correctness of the function NormalMatrix():
      // Commenting out Rotate() and un-commenting mat4WithUpperLeftMat3() 
      // gives the same result.
      mv = mv * Translate(0.0f, 0.5f, 0.0f) * Scale (1.4f, 1.4f, 1.4f) 
              * Rotate(angle, 0.0f, 0.0f, 2.0f); 
           // * mat4WithUpperLeftMat3(NormalMatrix(Rotate(angle, 0.0f, 0.0f, 2.0f), 1));
#endif
#if 1 // The following is to verify that Rotate() about (0,2,0) is RotateY():
      // Commenting out Rotate() and un-commenting RotateY()
      // gives the same result.
  //
  // The set-up below gives a new scene (scene 2), using Correct LookAt().
      mv = mv * Translate(0.3f, 0.0f, 0.0f) * Scale (1.4f, 1.4f, 1.4f)
              * Rotate(angle, 0.0f, 2.0f, 0.0f);
	   // * RotateY(angle);
  //
  // The set-up below gives the original scene (scene 1), using Correct LookAt().
  //  mv = Translate(0.0f, 0.5f, 0.0f) * mv * Scale (1.4f, 1.4f, 1.4f) 
  //               * Rotate(angle, 0.0f, 2.0f, 0.0f);
	        // * RotateY(angle); 
  //
  // The set-up below gives the original scene (scene 1), when using previously 
  //     Incorrect LookAt() (= Translate(1.0f, 1.0f, 0.0f) * correct LookAt() )
  //  mv = Translate(-1.0f, -0.5f, 0.0f) * mv * Scale (1.4f, 1.4f, 1.4f) 
  //               * Rotate(angle, 0.0f, 2.0f, 0.0f);
	        // * RotateY(angle);
  //
#endif
#if 0  // The following is to verify that Rotate() about (3,0,0) is RotateX():
       // Commenting out Rotate() and un-commenting RotateX()
       // gives the same result.
      mv = mv * Translate(0.0f, 0.5f, 0.0f) * Scale (1.4f, 1.4f, 1.4f)
                    * Rotate(angle, 3.0f, 0.0f, 0.0f);
                 // * RotateX(angle);
#endif
#if 0
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    if (cubeFlag == 1) // Filled cube
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe cube
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawObj(cube_buffer, cube_NumVertices);  // draw the cube
#endif
/*----- Set up the Mode-View matrix for the floor -----*/
 // The set-up below gives a new scene (scene 2), using Correct LookAt() function
    mv = LookAt(eye, at, up);// *Translate(0.3f, 0.0f, 0.0f)* Scale(1.6f, 1.5f, 3.3f);
 //
 // The set-up below gives the original scene (scene 1), using Correct LookAt()
 //    mv = Translate(0.0f, 0.0f, 0.3f) * LookAt(eye, at, up) * Scale (1.6f, 1.5f, 3.3f);
 //
 // The set-up below gives the original scene (scene 1), when using previously 
 //       Incorrect LookAt() (= Translate(1.0f, 1.0f, 0.0f) * correct LookAt() ) 
 //    mv = Translate(-1.0f, -1.0f, 0.3f) * LookAt(eye, at, up) * Scale (1.6f, 1.5f, 3.3f);
 //
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
    glUniform1iv(enable_light, 1, a);
    glUniform1i(glGetUniformLocation(program, "Texture_app_flag"), 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawObj(axis_buffer, 6, 1,GL_LINES);
    glUniform1i(glGetUniformLocation(program, "Texture_app_flag"), texture_app_flag);
    glUniform1iv(enable_light, 1, &a[light_Flag]);


    /*----- Set Up sphere animation -----*/
    //glTexImage1D(GL_TEXTURE_1D, 1, GL_RGBA, 4* stripeImageWidth,
        //0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);
    glUniform1i(glGetUniformLocation(program, "t1or2"), t1or2);
    glUniform1i(glGetUniformLocation(program, "sphere_tex_coor"), sphere_1d_coor);
    int first_round_flag = 0;
    int second_round_flag = 0;
    vec4 direction = AB;
    vec4 plane_nor(0.0, 1.0, 0.0, 0.0);
    if (move_dist > first_round) {
        direction = BC;
        first_round_flag = 1;
    }
    if (move_dist > first_round + second_round) {
        direction = CA;
        second_round_flag = 1;
    }
    if (move_dist > first_round + second_round + third_round) {
        direction = AB;
        move_dist -= first_round + second_round + third_round;
        first_round_flag = 0;
        second_round_flag = 0;
    }
    
    vec4 trans = (move_dist - first_round_flag * first_round - second_round_flag * second_round) * direction;
    vec4 rotate_axis = cross(plane_nor, direction);

    rotate_m =  Rotate(rotate_speed, rotate_axis.x, rotate_axis.y, rotate_axis.z) * rotate_m;

    mv = LookAt(eye, at, up) * Translate(first_round_flag * AB_vec + second_round_flag * BC_vec + trans)
         * Translate(-4.0f, 1.0f, 4.0f) * rotate_m;
    //mv = LookAt(eye, at, up) * Translate(-4.0f, 1.0f, 4.0f) * rotate_m;
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
    if (sphereFlag == 1) // Filled sphere
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe sphere
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawObj(sphere_buffer, 3*sphere_ts, 1);
    glUniform1i(glGetUniformLocation(program, "sphere_tex_coor"), 0);
    glUniform1i(glGetUniformLocation(program, "t1or2"), a[2]);

    glDepthMask(GL_FALSE);
    glUniform1i(glGetUniformLocation(program, "Texture_app_flag"), ground_map);
    mv = LookAt(eye, at, up) * Translate(0.3f, 0.0f, 0.0f);// *Scale(1.6f, 1.5f, 3.3f);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    if (floorFlag == 1) // Filled floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawObj(floor_buffer, floor_NumVertices, 1);  // draw the floor
    glUniform1i(glGetUniformLocation(program, "Texture_app_flag"), texture_app_flag);
    glDepthMask(GL_TRUE);
   
    if (draw_shadow_Flag) {
        glUniform1iv(enable_light, 1, a);
        glUniform1i(glGetUniformLocation(program, "Texture_app_flag"), a[0]);
        if(need_blend == 1)
            glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        mv = LookAt(eye, at, up) * shadow_m * Translate(first_round_flag * AB_vec + second_round_flag * BC_vec + trans)  * Translate(-4.0f, 1.0f, 4.0f)* rotate_m;
        glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
        if (shadowFlag == 1) // Filled shadow
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else              // Wireframe shadow
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawObj(shadow_buffer, 3 * sphere_ts, 0);
        glDisable(GL_BLEND);
        glUniform1iv(enable_light, 1, &a[light_Flag]);
        glUniform1i(glGetUniformLocation(program, "Texture_app_flag"), texture_app_flag);
    }

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    mv = LookAt(eye, at, up) * Translate(0.3f, 0.0f, 0.0f);// *Scale(1.6f, 1.5f, 3.3f);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    if (floorFlag == 1) // Filled floor
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawObj(floor_buffer, floor_NumVertices, 1);  // draw the floor
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glutSwapBuffers();
}
//---------------------------------------------------------------------------
void idle (void)
{
      angle += rotate_speed;
      move_dist += (rotate_speed / 360.0) * 2 * PI;

      // angle += 1.0f;    //YJC: change this value to adjust the cube rotation speed.
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
void main_menu(int id) {
    switch (id)
    {
        case 1:
            eye = init_eye;
            break;
        case 2:
            draw_shadow_Flag = 0;
            break;
        case 3:
            draw_shadow_Flag = 1;
            break;
        case 4:
            glUniform1iv(enable_light, 1, a);
            light_Flag = 0;
            break;
        case 5:
            glUniform1iv(enable_light, 1, &a[1]);
            light_Flag = 1;
            break;
        case 6:
            glUniform1iv(enable_light, 1, a);
            sphereFlag = 1 - sphereFlag;
            break;
        case 7:
            shadingFlag = 1;
            rebind();
            break;
        case 8:
            shadingFlag = 0;
            rebind();
            break;
        case 9:
            glUniform1iv(is_point_light, 1, a);
            break;
        case 10:
            glUniform1iv(is_point_light, 1, &a[1]);
            break;
        case 11:
            glUniform1iv(fog_mode, 1, &a[0]);
            break;
        case 12:
            glUniform1iv(fog_mode, 1, &a[1]);
            break;
        case 13:
            glUniform1iv(fog_mode, 1, &a[2]);
            break;
        case 14:
            glUniform1iv(fog_mode, 1, &a[3]);
            break;
        case 15:
            need_blend = 1;
            break;
        case 16:
            need_blend = 0;
            break;
        case 17:
            ground_map = 1;
            break;
        case 18:
            ground_map = 0;
            break;
        case 50:
            exit(0);
            break;
    }
    glutPostRedisplay();
}
void OnMouse(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (animationFlag == 1) {
            leftmouse_animationFlag = 1 - leftmouse_animationFlag;
            if (leftmouse_animationFlag == 1) glutIdleFunc(idle);
            else                    glutIdleFunc(NULL);
        }
    }
}
void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;

        case 'X': eye[0] += 1.0; break;
	case 'x': eye[0] -= 1.0; break;
        case 'Y': eye[1] += 1.0; break;
	case 'y': eye[1] -= 1.0; break;
        case 'Z': eye[2] += 1.0; break;
	case 'z': eye[2] -= 1.0; break;

        case 'b': case 'B': // Toggle between animation and non-animation
	    animationFlag = 1 -  animationFlag;
            if (animationFlag == 1) glutIdleFunc(idle);
            else                    glutIdleFunc(NULL);
            break;
	   
        case 'c': case 'C': // Toggle between filled and wireframe cube
	    cubeFlag = 1 -  cubeFlag;   
            break;

        case 'f': case 'F': // Toggle between filled and wireframe floor
	    floorFlag = 1 -  floorFlag; 
            break;

	case ' ':  // reset to initial viewer/eye position
	    eye = init_eye;
	    break;
    }
    glutPostRedisplay();
}

//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (GLfloat) width  / (GLfloat) height;
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
int main( int argc, char **argv )
{
    glutInit(&argc, argv);
#ifdef __APPLE__ // Enable core profile of OpenGL 3.2 on macOS.
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
    glutInitWindowSize(512, 512);
    glutCreateWindow("Color Cube");

#ifdef __APPLE__ // on macOS
    // Core profile requires to create a Vertex Array Object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
#else           // on Linux or Windows, we still need glew
    /* Call glewInit() and error checking */
    int err = glewInit();
    if (GLEW_OK != err)
    { 
        printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err)); 
        exit(1);
    }
#endif

    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    //glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(OnMouse);

    file_select();
    init();

    std::cout << "create menu" << std::endl;
    shadow_menu_ID = glutCreateMenu(main_menu);
    glutAddMenuEntry("No", 2);
    glutAddMenuEntry("Yes", 3);
    light_menu_ID = glutCreateMenu(main_menu);
    glutAddMenuEntry("No", 4);
    glutAddMenuEntry("Yes", 5);
    shading_menu_ID = glutCreateMenu(main_menu);
    glutAddMenuEntry("flat shading", 7);
    glutAddMenuEntry("smooth shading", 8);
    light_type_ID = glutCreateMenu(main_menu);
    glutAddMenuEntry("spot light", 9);
    glutAddMenuEntry("point source", 10);
    fog_ID = glutCreateMenu(main_menu);
    glutAddMenuEntry("No Fog", 11);
    glutAddMenuEntry("Linear", 12);
    glutAddMenuEntry("Exponential", 13);
    glutAddMenuEntry("exponential Square", 14);
    blend_ID = glutCreateMenu(main_menu);
    glutAddMenuEntry("Yes", 15);
    glutAddMenuEntry("No", 16);
    tm_ground = glutCreateMenu(main_menu);
    glutAddMenuEntry("Yes", 17);
    glutAddMenuEntry("No", 18);
    demo_menu_ID = glutCreateMenu(main_menu);
    glutSetMenuFont(demo_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry("Default View Point", 1);
    glutAddSubMenu("Shadow", shadow_menu_ID);
    glutAddSubMenu("Enable Lighting", light_menu_ID);
    glutAddMenuEntry("Wire Frame Sphere", 6);
    glutAddSubMenu("Shading", shading_menu_ID);
    glutAddSubMenu("light type", light_type_ID);
    glutAddSubMenu("Fog Option", fog_ID);
    glutAddSubMenu("Blend shadow", blend_ID);
    glutAddSubMenu("Texture Mapped Ground", tm_ground);
    glutAddMenuEntry("quit", 50);
    glutAttachMenu(GLUT_LEFT_BUTTON);

    glutMainLoop();
    return 0;
}
