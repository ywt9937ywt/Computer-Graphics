#pragma once
#include <QVector>
#include <QtGui>
#include <cstring>
#include "Triangle.h"
#include "Texture.h"
#include "Light.h"

#define WIDTH  700
#define HEIGHT  700


class Rasterizer
{
struct boundingBox {
	float top_left_x;
	float top_left_y;
	float bottom_right_x;
	float bottom_left_y;
};

public:
	Rasterizer(int w, int h);
	void set_cameraM(QMatrix4x4 cameramatrix);
	void set_perspM(QMatrix4x4 perspmatrix);
	void set_viewM(QMatrix4x4 viewmatrix);
	void set_texture(Texture text) { texture = text; }
	void append_triangle(Triangle tria);
	void append_light(Light light);
	void draw();
	void clean_zbuffer();
	std::tuple<float, float, float> computeBarycentric2D(int x, int y, const QVector3D* v);
	QRgb** get_framebuffer();

private:
	void rasterize_triangle(const Triangle& t, QVector3D vert_pos[]);
	void draw_line(QVector3D begin, QVector3D end, QVector3D view_begin, QVector3D view_end);
	void super_sampling_aa();
	QVector3D Blinn_Phong(QVector3D point, QVector3D norm);
	QVector3D Blinn_Phong(QVector3D point, QVector3D norm, QVector3D tex_color);
	void set_color(int x, int y, const Triangle& t, QVector3D vert_pos[]);
	QVector3D interpolate_I(float alpha, float beta, float gamma, QVector3D vert1, QVector3D vert2, QVector3D vert3, QVector3D vert_pos[], float weight);
	QVector2D interpolate_I(float alpha, float beta, float gamma, QVector2D vert1, QVector2D vert2, QVector2D vert3, QVector3D vert_pos[], float weight);
	float interpolate(QVector2D begin, QVector2D end, QVector2D pos, float depth_begin, float depth_end);

private:
	QMatrix4x4 camera_matrix;
	QMatrix4x4 persp_matrix;
	QMatrix4x4 view_matrix;

	QVector<QVector3D> vertex_pool;
	QVector<Triangle> triangle_pool;
	QVector<Triangle> transformed_triangle_pool;
	QVector<Light> lights;

	int width, height;
	QRgb** array;
	float** z_buffer;

	Texture texture;
};

