#include "ImageTest.h"
#include "Triangle.h"
#include <QtGui>
#include <QWidget>
#include <QDebug>

ImageTest::ImageTest(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

void ImageTest::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    QImage image(WIDTH, HEIGHT, QImage::Format_RGB32);
    //painter.drawLine(QPoint(100, 100), QPoint(10, 100));
    init();

    array = raster.get_framebuffer();
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            image.setPixel(x, HEIGHT-1-y, array[x][y]);
        }
    }
    /*for (int x = 0; x < 200; x++) {
        for (int y = 50; y < 60; y++) {
            image.setPixel(x, y, qRgb(0, 0, 0));
        }
    }*/
    
    painter.drawImage(10, 10, image);
}

QMatrix4x4 ImageTest::get_model_matrix(double angle)
{
    angle = angle * PI / 180;
    QMatrix4x4 rotation(cos(angle), 0, sin(angle), 0,
        0, 1, 0, 0,
        -sin(angle), 0, cos(angle), 0,
        0, 0, 0, 1);

    QMatrix4x4 scale(30, 0, 0, 0,
        0, 30, 0, 0,
        0, 0, 30, 0,
        0, 0, 0, 1);

    QMatrix4x4 translate(1, 0, 0, 50,
        0, 1, 0, 0,
        0, 0, 1, -50,
        0, 0, 0, 1);

    return translate * rotation * scale;
}

QMatrix4x4 ImageTest::get_camera_matrix(QVector3D position, QMatrix4x4 camera_coords) 
{
    QMatrix4x4 camera_matrix1 = camera_coords;
    QMatrix4x4 camera_matrix2 (1, 0, 0, -position.x(),
        0, 1, 0, -position.y(),
        0, 0, 1, -position.z(),
        0, 0, 0, 1);

    return camera_matrix1 * camera_matrix2;
}

QMatrix4x4 ImageTest::get_persp_matrix(float n, float f, double vertical_fov_radian, double width_height_ratio)
{
    float b, t, l, r;
    t = tan(vertical_fov_radian/2) * abs(n);
    b = -tan(vertical_fov_radian/2) * abs(n);
    r = t * width_height_ratio;
    l = -t * width_height_ratio;
    QMatrix4x4 persp_matrix(n, 0, 0, 0,
        0, n, 0, 0,
        0, 0, n + f, -n * f,
        0, 0, 1, 0);
    QMatrix4x4 orth_matrix(2 / (r - l), 0, 0, -(r + l) / (r - l),
        0, 2 / (t - b), 0, -(t + b) / (t - b),
        0, 0, 2 / (n - f), -(n + f) / (n - f),
        0, 0, 0, 1);

    return orth_matrix * persp_matrix;
}

QMatrix4x4 ImageTest::get_viewport_matrix(float width, float height)
{
    return QMatrix4x4(width / 2, 0, 0, (width - 1) / 2,
        0, height / 2, 0, (height - 1) / 2,
        0, 0, 1, 0,
        0, 0, 0, 1);
}

void ImageTest::init()

{
    Light light1(QVector3D(-80, 80, 0), QVector3D(500, 500, 500));
    raster.append_light(light1);
    QString obj_path_pre = "C:/Users/HP/Desktop/graphics/Assignment3/Code/models/";
    QString obj_path_pre1 = "C:/Users/HP/Desktop/graphics/Assignment3/Code/models/";
    QString obj_path = "rock/rock.obj";
    QString obj_path1 = "spot/spot_triangulated_good.obj";
    QString texture_path = "rock/rock.png";
    QString texture_path1 = "spot/spot_texture";
    raster.set_texture(Texture(obj_path_pre1 + texture_path1));
    obj_buffer.load(obj_path_pre1+obj_path1, tri_pool);
    auto model_Matrix = get_model_matrix(160);
    for (auto t : tri_pool) {
        Triangle new_t = t.transform_triangle(model_Matrix);
        raster.append_triangle(new_t);
    }

    /*Triangle t;
    t.setVertex(0, QVector3D(0, 0, -60));
    t.setVertex(1, QVector3D(100, 0, -60));
    t.setVertex(2, QVector3D(50, 50, -60));
    t.setColor(0, QVector3D(255, 0, 0));
    t.setColor(1, QVector3D(0, 255, 0));
    t.setColor(2, QVector3D(0, 0, 255));
    raster.append_triangle(t);

    Triangle t1;
    t1.setVertex(0, QVector3D(50, 25, -60));
    t1.setVertex(1, QVector3D(50, -35, -60));
    t1.setVertex(2, QVector3D(90, -10, -70));
    t1.setColor(0, QVector3D(0, 255, 0));
    t1.setColor(1, QVector3D(0, 255, 0));
    t1.setColor(2, QVector3D(0, 255, 0));
    raster.append_triangle(t1);*/

    QVector3D camera_position(50,0,0);
    QMatrix4x4 camM1(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    
    raster.set_cameraM(get_camera_matrix(camera_position, camM1));
    raster.set_perspM(get_persp_matrix(-20, -70, PI / 2, 1));
    raster.set_viewM(get_viewport_matrix(WIDTH, HEIGHT));
    raster.draw();
    qDebug() << "rasterization finish";
}

