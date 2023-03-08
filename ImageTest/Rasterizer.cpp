#include "Rasterizer.h"


Rasterizer::Rasterizer(int w, int h)
{
	width = w;
	height = h;
    array = new QRgb* [width];
    for (int x = 0; x < width; x++) {
        array[x] = new QRgb[height]; // need to delete
    }

    z_buffer = new float * [width];
    for (int x = 0; x < width; x++) {
        z_buffer[x] = new float[height]; // need to delete
    }
}

void Rasterizer::set_cameraM(QMatrix4x4 cameramatrix)
{
	camera_matrix = cameramatrix;
}
void Rasterizer::set_perspM(QMatrix4x4 perspmatrix)
{
	persp_matrix = perspmatrix;
}

void Rasterizer::set_viewM(QMatrix4x4 viewmatrix)
{
	view_matrix = viewmatrix;
}

void Rasterizer::append_triangle(Triangle tria)
{
	triangle_pool.append(tria);
}

void Rasterizer::append_light(Light light)
{
    lights.append(light);
}

void Rasterizer::clean_zbuffer()
{
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            z_buffer[x][y] = -999999;
        }
    }
}

std::tuple<float, float, float> Rasterizer::computeBarycentric2D(int x, int y, const QVector3D* v)
{
    float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
    float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
    float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
    return { c1, c2, c3 };
}

QRgb** Rasterizer::get_framebuffer()
{
    return array;
}

void Rasterizer::rasterize_triangle(const Triangle& t, QVector3D vert_pos[]) {
    /*if (z_buffer[(int)t.v[0].x()][(int)t.v[0].y()] > vert_pos[0].z() && z_buffer[(int)t.v[1].x()][(int)t.v[1].y()] > vert_pos[1].z() && z_buffer[(int)t.v[2].x()][(int)t.v[2].y()] > vert_pos[2].z()) {
        return;
    }*/
	auto v = t.toVector4D();
	float minx = width, miny = height, maxx = 0, maxy = 0;
    for (int i = 0; i < 3; i++) {
        QVector4D p = t.toVector4D()[i];

        if (p.x() < minx) minx = p.x();
        if (p.x() > maxx) maxx = p.x();
        if (p.y() < miny) miny = p.y();
        if (p.y() > maxy) maxy = p.y();
    }
    //qDebug() << minx << " "<<maxx <<"   " << miny<< "   " << maxy;
	boundingBox box = { minx, miny, maxx, maxy };
    QVector<QVector2D> triangle_vec;


    QVector2D vec1 = QVector2D(t.v[1].x() - t.v[0].x(), t.v[1].y() - t.v[0].y());
    QVector2D vec2 = QVector2D(t.v[2].x() - t.v[1].x(), t.v[2].y() - t.v[1].y());
    QVector2D vec3 = QVector2D(t.v[0].x() - t.v[2].x(), t.v[0].y() - t.v[2].y());
    triangle_vec.append(vec1);
    triangle_vec.append(vec2);
    triangle_vec.append(vec3);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (x > minx && x<maxx && y>miny && y < maxy) {
                QVector<QVector2D> pixel_triangle_vec;
                for (int i = 0; i < sizeof(t.v); i++) {
                    QVector2D vec = QVector2D(x + 0.5, y + 0.5) - QVector2D(t.v[i].x(), t.v[i].y());
                    pixel_triangle_vec.append(vec);
                }

                float cross1 = pixel_triangle_vec[0].x() * triangle_vec[0].y() - pixel_triangle_vec[0].y() * triangle_vec[0].x();
                float cross2 = pixel_triangle_vec[1].x() * triangle_vec[1].y() - pixel_triangle_vec[1].y() * triangle_vec[1].x();
                float cross3 = pixel_triangle_vec[2].x() * triangle_vec[2].y() - pixel_triangle_vec[2].y() * triangle_vec[2].x();
                if (cross1 == 0) {
                    qDebug() << "1";
                    QVector2D p1 = QVector2D(t.v[0].x(), t.v[0].y());
                    QVector2D p2 = QVector2D(t.v[1].x(), t.v[1].y());
                    QVector2D p3 = QVector2D(t.v[2].x(), t.v[2].y());
                    double A1 = p1.y() - p2.y();
                    double B1 = p2.x() - p1.x();
                    double C1 = A1 * p1.x() + B1 * p1.y();
                    if (A1 * p3.x() + B1 * p3.y() + C1 > 0) {
                        set_color(x, y, t, vert_pos);
                    }
                }
                if (cross2 == 0) {
                    qDebug() << "2";
                    QVector2D p1 = QVector2D(t.v[0].x(), t.v[0].y());
                    QVector2D p2 = QVector2D(t.v[1].x(), t.v[1].y());
                    QVector2D p3 = QVector2D(t.v[2].x(), t.v[2].y());
                    double A1 = p2.y() - p3.y();
                    double B1 = p3.x() - p2.x();
                    double C1 = A1 * p2.x() + B1 * p2.y();
                    if (A1 * p1.x() + B1 * p1.y() + C1 > 0) {
                        set_color(x, y, t, vert_pos);
                    }
                }
                if (cross3 == 0) {
                    qDebug() << "3";
                    QVector2D p1 = QVector2D(t.v[0].x(), t.v[0].y());
                    QVector2D p2 = QVector2D(t.v[1].x(), t.v[1].y());
                    QVector2D p3 = QVector2D(t.v[2].x(), t.v[2].y());
                    double A1 = p3.y() - p1.y();
                    double B1 = p1.x() - p3.x();
                    double C1 = A1 * p3.x() + B1 * p3.y();
                    if (A1 * p2.x() + B1 * p2.y() + C1 > 0) {
                        set_color(x, y, t, vert_pos);
                    }
                }
                if ((cross1 < 0 && cross2 < 0 && cross3 < 0) || (cross1 > 0 && cross2 > 0 && cross3 > 0)) {
                    set_color(x, y, t, vert_pos);
                }
            }
        }

    }
}

void Rasterizer::set_color(int x, int y, const Triangle& t, QVector3D vert_pos[])
{
    std::tuple<float, float, float> result = computeBarycentric2D(x, y, t.v);
    //qDebug() << std::get<0>(result) <<"  " << std::get<1>(result) << "  " << std::get<2>(result);
    float alpha = std::get<0>(result);
    float beta = std::get<1>(result);
    float gamma = std::get<2>(result);

    float _x = 1.0 / (alpha / vert_pos[0].x() + beta / vert_pos[1].x() + gamma / vert_pos[2].x());
    float _y = 1.0 / (alpha / vert_pos[0].y() + beta / vert_pos[1].y() + gamma / vert_pos[2].y());
    float z = 1.0 / (alpha / vert_pos[0].z() + beta / vert_pos[1].z() + gamma / vert_pos[2].z());
    /*float w_reciprocal = 1.0 / ( alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    z_interpolated *= w_reciprocal;*/
    if (z_buffer[x][y] < z) {
        z_buffer[x][y] = z;
        //QVector3D interpolated_color = interpolate_I(alpha, beta, gamma, t.color[0], t.color[1] , t.color[2] ,vert_pos, 1/z);
        //array[x][y] = qRgb((int)interpolated_color.x(), (int)interpolated_color.y(), (int)interpolated_color.z());

        // rock texture
        /*QVector2D interpolated_tex = interpolate_I(alpha, beta, gamma, t.tex_coord[0], t.tex_coord[1], t.tex_coord[2], vert_pos, 1 / z);
        QVector3D tex_color = texture.getColor(interpolated_tex.x(), interpolated_tex.y());
        array[x][y] = qRgb((int)tex_color.x(), (int)tex_color.y(), (int)tex_color.z());*/

        //blinn_phong spot
        QVector2D interpolated_tex = interpolate_I(alpha, beta, gamma, t.tex_coord[0], t.tex_coord[1], t.tex_coord[2], vert_pos, 1 / z);
        QVector3D interpolated_norm = interpolate_I(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], vert_pos, 1 / z);
        QVector3D tex_color = texture.getColor(interpolated_tex.x(), interpolated_tex.y());
        //QVector3D result_color = Blinn_Phong(QVector3D(_x, _y, z), interpolated_norm, tex_color);
        QVector3D result_color = Blinn_Phong(QVector3D(_x, _y, z), interpolated_norm);
        //array[x][y] = qRgb((int)result_color.x(), (int)result_color.y(), (int)result_color.z());

        // spot texture
        /*QVector2D interpolated_tex = interpolate_I(alpha, beta, gamma, t.tex_coord[0], t.tex_coord[1], t.tex_coord[2], vert_pos, 1 / z);
        QVector3D tex_color = texture.getColor(interpolated_tex.x(), interpolated_tex.y());
        array[x][y] = qRgb((int)tex_color.x(), (int)tex_color.y(), (int)tex_color.z());*/

        array[x][y] = qRgb(255, 0, 0);
        //qDebug() << interpolated_color.x() << "  " << interpolated_color.y() << "  " << interpolated_color.z();
        //return;
    }
}

QVector3D Rasterizer::Blinn_Phong(QVector3D point, QVector3D norm)
{
    QVector3D result_color(0, 0, 0);
    QVector3D eye_pos(50, 0, 0);
    int p = 8;
    for (auto& light : lights)
    {
        QVector3D ka(0.005, 0.005, 0.005);
        QVector3D kd(0.05, 0.05, 0.05);
        QVector3D ks(0.7937, 0.7937, 0.7937);
        QVector3D amb_light_I(10, 10, 10);
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        float r = (light.pos - point).length()/10;
        auto l = (light.pos - point).normalized();
        auto n = norm.normalized();
        auto v = (eye_pos - point).normalized();

        auto ambient = QVector3D(amb_light_I.x()*ka.x(), amb_light_I.y() * ka.y(), amb_light_I.z() * ka.z());
        auto diffuse = QVector3D(light.intensity.x() / (r * r)* kd.x(), light.intensity.y() / (r * r) * kd.y(), light.intensity.z() / (r * r) * kd.z()) * std::max(0.0, (double)QVector3D::dotProduct(l,n));
        auto h = (v + l).normalized();
        auto specular = QVector3D(light.intensity.x() / (r * r)*ks.x(), light.intensity.y() / (r * r) * ks.y(), light.intensity.z() / (r * r) * ks.z()) * pow(std::max(0.0, (double)QVector3D::dotProduct(n,h)), p);
        
        QVector3D inter_color =  ambient + diffuse + specular;
        result_color += inter_color;
    }
    return result_color * 255.f;
}

QVector3D Rasterizer::Blinn_Phong(QVector3D point, QVector3D norm, QVector3D tex_color)
{
    QVector3D result_color(0, 0, 0);
    QVector3D eye_pos(50, 0, 0);
    int p = 8;
    for (auto& light : lights)
    {
        QVector3D ka(0.005, 0.005, 0.005);
        QVector3D kd = tex_color / 255.f;
        QVector3D ks(0.7937, 0.7937, 0.7937);
        QVector3D amb_light_I(10, 10, 10);
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        float r = (light.pos - point).length()/10;
        auto l = (light.pos - point).normalized();
        auto n = norm.normalized();
        auto v = (eye_pos - point).normalized();

        auto ambient = QVector3D(amb_light_I.x() * ka.x(), amb_light_I.y() * ka.y(), amb_light_I.z() * ka.z());
        auto diffuse = QVector3D(light.intensity.x() / (r * r) * kd.x(), light.intensity.y() / (r * r) * kd.y(), light.intensity.z() / (r * r) * kd.z()) * std::max(0.0, (double)QVector3D::dotProduct(l, n));
        auto h = (v + l).normalized();
        auto specular = QVector3D(light.intensity.x() / (r * r) * ks.x(), light.intensity.y() / (r * r) * ks.y(), light.intensity.z() / (r * r) * ks.z()) * pow(std::max(0.0, (double)QVector3D::dotProduct(n, h)), p);
        

        QVector3D inter_color = ambient + diffuse + specular;
        result_color += inter_color;
    }
    return result_color * 255.f;
}

void Rasterizer::super_sampling_aa()
{
    QRgb** ssaa_array;
    ssaa_array = new QRgb * [width];
    for (int x = 0; x < width; x++) {
        ssaa_array[x] = new QRgb[height]; // need to delete
    }
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int num_valid_pixel = 0;
            int r = 0;
            int g = 0;
            int b = 0;
            if (x-1>=0 ) {
                QColor clrCurrent(array[x-1][y]);
                num_valid_pixel++;
                r += clrCurrent.red();
                g += clrCurrent.green();
                b += clrCurrent.blue();
            }
            if (y-1 >= 0) {
                QColor clrCurrent(array[x][y-1]);
                num_valid_pixel++;
                r += clrCurrent.red();
                g += clrCurrent.green();
                b += clrCurrent.blue();
            }
            if (x+1 <= 199) {
                QColor clrCurrent(array[x+1][y]);
                num_valid_pixel++;
                r += clrCurrent.red();
                g += clrCurrent.green();
                b += clrCurrent.blue();
            }
            if (y + 1 <= 199) {
                QColor clrCurrent(array[x][y+1]);
                num_valid_pixel++;
                r += clrCurrent.red();
                g += clrCurrent.green();
                b += clrCurrent.blue();
            }
            ssaa_array[x][y] = qRgb(r/num_valid_pixel, g/num_valid_pixel, b/num_valid_pixel);
        }
    }
    for (int i = 0; i < height; i++)
        delete []array[i];
    delete []array;
    array = ssaa_array;
}

QVector3D Rasterizer::interpolate_I(float alpha, float beta, float gamma, QVector3D vert1, QVector3D vert2, QVector3D vert3, QVector3D vert_pos[], float weight)
{
    QVector3D up = alpha * vert1 / vert_pos[0].z() + beta * vert2 / vert_pos[1].z() + gamma * vert3 / vert_pos[2].z();
    QVector3D interpolated_I = up / weight;
    return interpolated_I;
}

QVector2D Rasterizer::interpolate_I(float alpha, float beta, float gamma, QVector2D vert1, QVector2D vert2, QVector2D vert3, QVector3D vert_pos[], float weight)
{
    QVector2D up = alpha * vert1 / vert_pos[0].z() + beta * vert2 / vert_pos[1].z() + gamma * vert3 / vert_pos[2].z();
    QVector2D interpolated_I = up / weight;
    return interpolated_I;
}

float Rasterizer::interpolate(QVector2D begin, QVector2D end, QVector2D pos, float depth_begin, float depth_end)
{
    if (begin == end) return -99999;
    float b2x = 0, b2e = 0;
    if (begin.x() - end.x() != 0) {
        b2x = fabs(pos.x() - begin.x());
        b2e = fabs(end.x() - begin.x());
    }
    else {
        b2x = fabs(pos.y() - begin.y());
        b2e = fabs(end.y() - begin.y());
    }

    
    float s = b2x / b2e;
    return 1/(1/depth_begin * s + (1 - s) * 1/depth_end);
}

// Bresenham's line drawing algorithm
void Rasterizer::draw_line(QVector3D begin, QVector3D end, QVector3D view_begin, QVector3D view_end)
{
    auto x1 = begin.x();
    auto y1 = begin.y();
    auto x2 = end.x();
    auto y2 = end.y();

    QRgb line_color = qRgb( 255, 0, 0 );

    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

    dx = x2 - x1;
    dy = y2 - y1;
    dx1 = fabs(dx);
    dy1 = fabs(dy);
    px = 2 * dy1 - dx1;
    py = 2 * dx1 - dy1;

    if (dy1 <= dx1)
    {
        //draw from left to right
        if (dx >= 0)
        {
            x = x1;
            y = y1;
            xe = x2;
        }
        else
        {
            x = x2;
            y = y2;
            xe = x1;
        }
        QVector2D point = QVector2D(x, y);
        if (x >= 0 && x < WIDTH && y >= 0 && y <= HEIGHT) {
            float z = interpolate(QVector2D(begin.x(), begin.y()), QVector2D(end.x(), end.y()), QVector2D(x, y), view_begin.z(), view_end.z());
            if (z_buffer[x][y] < z) {
                array[x][y] = line_color;
                z_buffer[x][y] = z;
            }
        }
        //set_pixel(point, line_color);
        for (i = 0; x < xe; i++)
        {
            x = x + 1;
            if (px < 0)
            {
                px = px + 2 * dy1;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                {
                    y = y + 1;
                }
                else
                {
                    y = y - 1;
                }
                px = px + 2 * (dy1 - dx1);
            }
            //            delay(0);
            QVector2D point = QVector2D(x, y);
            if (x >= 0 && x < WIDTH && y >= 0 && y <= HEIGHT) {
                float z = interpolate(QVector2D(begin.x(), begin.y()), QVector2D(end.x(), end.y()), QVector2D(x, y), view_begin.z(), view_end.z());
                if (z_buffer[x][y] < z ) {
                    array[x][y] = line_color;
                    z_buffer[x][y] = z;
                }
            }
            
            //set_pixel(point, line_color);
        }
    }
    else
    {
        if (dy >= 0)
        {
            x = x1;
            y = y1;
            ye = y2;
        }
        else
        {
            x = x2;
            y = y2;
            ye = y1;
        }
        QVector2D point = QVector2D(x, y);
        if (x >= 0 && x < WIDTH && y >= 0 && y <= HEIGHT) {
            float z = interpolate(QVector2D(begin.x(), begin.y()), QVector2D(end.x(), end.y()), QVector2D(x, y), view_begin.z(), view_end.z());
            if (z_buffer[x][y] < z) {
                array[x][y] = line_color;
                z_buffer[x][y] = z;
            }
        }
        
        //set_pixel(point, line_color);
        for (i = 0; y < ye; i++)
        {
            y = y + 1;
            if (py <= 0)
            {
                py = py + 2 * dx1;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                {
                    x = x + 1;
                }
                else
                {
                    x = x - 1;
                }
                py = py + 2 * (dx1 - dy1);
            }
            //            delay(0);
            QVector2D point = QVector2D(x, y);
            //
            if(x >= 0 && x < WIDTH&& y >= 0 && y <= HEIGHT) {
                float z = interpolate(QVector2D(begin.x(), begin.y()), QVector2D(end.x(), end.y()), QVector2D(x, y), view_begin.z(), view_end.z());
                if (z_buffer[x][y] < z) {
                    array[x][y] = line_color;
                    z_buffer[x][y] = z;
                }
            }
            //set_pixel(point, line_color);
        }
    }
}


void Rasterizer::draw()
{
    clean_zbuffer();
	QMatrix4x4 mvp = view_matrix *persp_matrix *camera_matrix;
	for (auto t : triangle_pool) {
		Triangle new_t = t.transform_triangle(mvp);
        /*draw_line(new_t.v[0], new_t.v[1], t.v[0], t.v[1]);
        draw_line(new_t.v[1], new_t.v[2], t.v[1], t.v[2]);
        draw_line(new_t.v[2], new_t.v[0], t.v[2], t.v[0]);*/
		rasterize_triangle(new_t, t.v);
	}
    //super_sampling_aa();
}