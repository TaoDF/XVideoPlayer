#pragma once

#include <QOpenGLWidget>
#include <QOpenglFunctions>
#include <QGLShaderProgram>

class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    XVideoWidget(QWidget* parent);
    ~XVideoWidget();

protected:
    //refresh
    void paintGL();

    //init
    void initializeGL();

    //window size change
    void resizeGL(int width, int height);

private:
    //shader
    QGLShaderProgram program;
    //yuv address from shader
    GLuint unis[3] = { 0 };
    //openGL texture address
    GLuint texs[3] = { 0 };

    int width = 240;
    int height = 180;
};

