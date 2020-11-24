/********************************************************************************
** Form generated from reading UI file 'XPlay.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XPLAY_H
#define UI_XPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include "xvideowidget.h"

QT_BEGIN_NAMESPACE

class Ui_XPlayClass
{
public:
    XVideoWidget *video;

    void setupUi(QWidget *XPlayClass)
    {
        if (XPlayClass->objectName().isEmpty())
            XPlayClass->setObjectName(QString::fromUtf8("XPlayClass"));
        XPlayClass->resize(1034, 800);
        video = new XVideoWidget(XPlayClass);
        video->setObjectName(QString::fromUtf8("video"));
        video->setGeometry(QRect(40, 100, 800, 600));

        retranslateUi(XPlayClass);

        QMetaObject::connectSlotsByName(XPlayClass);
    } // setupUi

    void retranslateUi(QWidget *XPlayClass)
    {
        XPlayClass->setWindowTitle(QCoreApplication::translate("XPlayClass", "XPlay", nullptr));
    } // retranslateUi

};

namespace Ui {
    class XPlayClass: public Ui_XPlayClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XPLAY_H
