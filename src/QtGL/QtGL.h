#pragma once

#include <QtWidgets/QWidget>
#include "ui_QtGL.h"

class QtGL : public QWidget
{
    Q_OBJECT

public:
    QtGL(QWidget *parent = Q_NULLPTR);

private:
    Ui::QtGLClass ui;
};
