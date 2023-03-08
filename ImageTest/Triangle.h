#pragma once
#include <QVector>
#include <QtGui>
#include "Triangle.h"

class Triangle
{
public:
	Triangle();

	QVector3D v[3]; // original coordinates
	QVector3D color[3];
	QVector2D tex_coord[3];
	QVector3D normal[3];

	QVector3D a() const { return v[0]; }
	QVector3D b() const { return v[1]; }
	QVector3D c() const { return v[2]; }

	void setVertex(int index, QVector3D vCoord);
	void setNormal(int index, QVector3D	norm);
	void setColor(int index, QVector3D rgb);
	void setTexCoord(int index, float s, float t);
	Triangle transform_triangle(QMatrix4x4 mvp);

	QVector<QVector4D> toVector4D() const;
};

