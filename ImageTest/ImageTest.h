#pragma once

#include <QtWidgets/QWidget>
#include "ui_ImageTest.h"
#include "Rasterizer.h"
#include "OBJ_Loader.h"

#define PI 3.14159265f
#define WIDTH  700
#define HEIGHT  700

class ImageTest : public QWidget
{
    Q_OBJECT

struct boundingBox{
        float top_left_x;
        float top_left_y;
        float bottom_right_x;
        float bottom_left_y;
};

public:
    ImageTest(QWidget *parent = Q_NULLPTR);

protected:
    void paintEvent(QPaintEvent*);

private:
    void init();
    QMatrix4x4 get_model_matrix(double angle);
    QMatrix4x4 get_camera_matrix(QVector3D position, QMatrix4x4 camera_coords);
    QMatrix4x4 get_persp_matrix(float n, float f, double vertical_fov_radian, double width_height_ratio);
    QMatrix4x4 get_viewport_matrix(float width, float height);
    void updateDepthBuffer(const Triangle t);

    Ui::ImageTestClass ui;
    
private:
    Rasterizer raster = Rasterizer(WIDTH, HEIGHT);
    OBJ_Loader obj_buffer = OBJ_Loader();
    QRgb** array;
    QVector<Triangle> tri_pool;
};
