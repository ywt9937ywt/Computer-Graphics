#include "ImageTest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageTest w;
    w.resize(720,720);
    w.show();
    return a.exec();
}


