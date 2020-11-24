/********************************************************************************
** Form generated from reading UI file 'QtGL.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTGL_H
#define UI_QTGL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include "xvideowidget.h"

QT_BEGIN_NAMESPACE

class Ui_QtGLClass
{
public:
    XVideoWidget *openGLWidget;

    void setupUi(QWidget *QtGLClass)
    {
        if (QtGLClass->objectName().isEmpty())
            QtGLClass->setObjectName(QString::fromUtf8("QtGLClass"));
        QtGLClass->resize(1025, 805);
        openGLWidget = new XVideoWidget(QtGLClass);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));
        openGLWidget->setGeometry(QRect(60, 50, 800, 600));

        retranslateUi(QtGLClass);

        QMetaObject::connectSlotsByName(QtGLClass);
    } // setupUi

    void retranslateUi(QWidget *QtGLClass)
    {
        QtGLClass->setWindowTitle(QCoreApplication::translate("QtGLClass", "QtGL", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtGLClass: public Ui_QtGLClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTGL_H
