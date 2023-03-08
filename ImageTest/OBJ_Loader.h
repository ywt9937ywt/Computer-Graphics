#pragma once
#include <QVector>
#include <QtGui>
#include "Triangle.h"

class OBJ_Loader
{
public:
	OBJ_Loader();

	bool load(QString fileName, QVector<Triangle>& tri_pool);
};

