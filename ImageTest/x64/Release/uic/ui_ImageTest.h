/********************************************************************************
** Form generated from reading UI file 'ImageTest.ui'
**
** Created by: Qt User Interface Compiler version 5.12.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGETEST_H
#define UI_IMAGETEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImageTestClass
{
public:

    void setupUi(QWidget *ImageTestClass)
    {
        if (ImageTestClass->objectName().isEmpty())
            ImageTestClass->setObjectName(QString::fromUtf8("ImageTestClass"));
        ImageTestClass->resize(600, 400);

        retranslateUi(ImageTestClass);

        QMetaObject::connectSlotsByName(ImageTestClass);
    } // setupUi

    void retranslateUi(QWidget *ImageTestClass)
    {
        ImageTestClass->setWindowTitle(QApplication::translate("ImageTestClass", "ImageTest", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ImageTestClass: public Ui_ImageTestClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGETEST_H
