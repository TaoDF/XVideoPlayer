#pragma once

#include <QOpenGLWidget>
#include <QOpenglFunctions>
#include <QGLShaderProgram>
#include <mutex>
struct AVFrame;
class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    void Init(int width, int height);

    //release frame space no matter success or not
    virtual void Repaint(AVFrame *frame);
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
    std::mutex mux;

    //shader
    QGLShaderProgram program;
    //yuv address from shader
    GLuint unis[3] = { 0 };
    //openGL texture address
    GLuint texs[3] = { 0 };

    //texture memory space
    unsigned char* data[3] = {0};

    int width = 1920;
    int height = 1080;
};
