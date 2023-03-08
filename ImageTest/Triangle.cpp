#include "Triangle.h"
#include <QVector>
#include <QtGui>

Triangle::Triangle() {
	v[0] = { 0.0, 0.0, 0.0 };
	v[1] = { 0.0, 0.0, 0.0 };
	v[2] = { 0.0, 0.0, 0.0 };

	color[0] = { 0.0, 0.0, 0.0 };
	color[1] = { 0.0, 0.0, 0.0 };
	color[2] = { 0.0, 0.0, 0.0 };

	tex_coord[0] = { 0, 0 };
	tex_coord[1] = { 0, 0 };
	tex_coord[2] = { 0, 0 };
}

void Triangle::setVertex(int index, QVector3D vCoord) { v[index] = vCoord; }

void Triangle::setNormal(int index, QVector3D	norm) { normal[index] = norm; }

void Triangle::setColor(int index, QVector3D rgb) { color[index] = rgb; }

void Triangle::setTexCoord(int index, float s, float t) { tex_coord[index] = { s,t }; }

Triangle Triangle::transform_triangle(QMatrix4x4 mvp) 
{
	Triangle transformed_t;
	QVector4D p = mvp * QVector4D(v[0],1);
	QVector4D p1 = mvp * QVector4D(v[1], 1);
	QVector4D p2 = mvp * QVector4D(v[2], 1);
	if (p.w() != 0) {
		float positionx = p.x() / p.w();
		float positiony = p.y() / p.w();
		float positionz = p.z() / p.w();
		transformed_t.setVertex(0, QVector3D(positionx, positiony, positionz));
		//qDebug() << "Pixel:" << p.x() / p.w() << "   " << p.y() / p.w() << " p.w(): " << p.w();
	}
	else {
		qDebug() << "p.w() is 0. error!!";
	}

	if (p1.w() != 0) {
		float positionx = p1.x() / p1.w();
		float positiony = p1.y() / p1.w();
		float positionz = p1.z() / p1.w();
		transformed_t.setVertex(1, QVector3D(positionx, positiony, positionz));
		//qDebug() << "Pixel:" << p1.x() / p1.w() << "   " << p1.y() / p1.w() << " p.w(): " << p1.w();
	}
	else {
		qDebug() << "p.w() is 0. error!!";
	}

	if (p2.w() != 0) {
		float positionx = p2.x() / p2.w();
		float positiony = p2.y() / p2.w();
		float positionz = p2.z() / p2.w();
		transformed_t.setVertex(2, QVector3D(positionx, positiony, positionz));
		//qDebug() << "Pixel:" << p2.x() / p2.w() << "   " << p2.y() / p2.w() << " p.w(): " << p2.w();
	}
	else {
		qDebug() << "p.w() is 0. error!!";
	}

	transformed_t.setColor(0, color[0]);
	transformed_t.setColor(1, color[1]);
	transformed_t.setColor(2, color[2]);

	transformed_t.setNormal(0, normal[0]);
	transformed_t.setNormal(1, normal[1]);
	transformed_t.setNormal(2, normal[2]);
	
	transformed_t.setTexCoord(0, tex_coord[0].x(), tex_coord[0].y());
	transformed_t.setTexCoord(1, tex_coord[1].x(), tex_coord[1].y());
	transformed_t.setTexCoord(2, tex_coord[2].x(), tex_coord[2].y());

	return transformed_t;
}

QVector<QVector4D> Triangle::toVector4D() const
{
	QVector<QVector4D> res(3);
	for (int i = 0; i < 3; i++) {
		res[i]=QVector4D( v[i].x(), v[i].y(), v[i].z(), (float)1.0 );
	}
	return res;
}
