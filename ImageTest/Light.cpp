#include <QVector>
#include <QtGui>
#include "Light.h"

Light::Light(QVector3D position, QVector3D intens)
{
	pos = position;
	intensity = intens;
}


