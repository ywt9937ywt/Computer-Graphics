#pragma once
#include <QVector>
#include <QtGui>
class Texture
{
private:
	QImage texture_data;

public:
	int width, height;

public:
	Texture() {

	}
	Texture(const QString name) 
	{
		bool a = texture_data.load(name);
		if (!a) return;
		if (a)  qDebug() << "Texture is loaded successfully";
		width = texture_data.width();
		height = texture_data.height();
	}

	QVector3D getColor(float u, float v)
	{
		auto u_img = u * width;
		auto v_img = (1 - v) * height;
		auto color = texture_data.pixel(u_img, v_img);
		QColor c = color;
		return QVector3D(c.red(), c.green(), c.blue());
	}
};

