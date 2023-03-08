#pragma once
#include <QVector>
#include <QtGui>


class Light
{
public:
	QVector3D pos;
	QVector3D intensity;


public:
	Light() {

	};
	Light(QVector3D position, QVector3D intens);
};

