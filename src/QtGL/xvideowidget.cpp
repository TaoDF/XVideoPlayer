#include "xvideowidget.h"
#include <QDebug>

#define GET_STR(x) #x
#define A_VER 3
#define T_VER 4

//vertex shader
const char* vString = GET_STR(
	attribute vec4 vertexIn;
	attribute vec2 textureIn;
	varying vec2 textureOut;
	void main(void)
	{
		gl_Position = vertexIn;
		textureOut = textureIn;
	}
);

//frag shader
const char* tString = GET_STR(
	varying vec2 textureOut;
    uniform sampler2D tex_y;
	uniform sampler2D tex_u;
	uniform sampler2D tex_v;
	void main(void)
	{
		vec3 yuv;
		vec3 rgb;
		yuv.x = texture2D(tex_y, textureOut).r;
		yuv.y = texture2D(tex_u, textureOut).r - 0.5;
		yuv.z = texture2D(tex_v, textureOut).r - 0.5;
		rgb = mat3(1.0, 1.0, 1.0, 0.0, -0.39465, 2.03211, 1.13983, -0.58060, 0.0) * yuv;
		gl_FragColor = vec4(rgb, 1.0);
	}
);

XVideoWidget::XVideoWidget(QWidget *parent) : QOpenGLWidget(parent)
{

}

XVideoWidget::~XVideoWidget()
{

}

//init
void XVideoWidget::initializeGL()
{
	qDebug() << "initializeGL";
	//init opengl (QOpenglfunctions)
	initializeOpenGLFunctions();

	//program load shader(vertex and fragment) script
	qDebug() << program.addShaderFromSourceCode(QGLShader::Vertex, vString);
	qDebug() << program.addShaderFromSourceCode(QGLShader::Fragment, tString);

	//set vertex variable
	program.bindAttributeLocation("vertexIn", A_VER);

	//set frag var
	program.bindAttributeLocation("textureIn", T_VER);


	//compile shader and bind
	qDebug() << "program.link() = " << program.link();
	qDebug() << "program.bind() = " << program.bind();

	//pass vertex and texture coord
	//vertex
	static const GLfloat ver[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, 1.0f,
	};
	//frag
	static const GLfloat tex[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	//vertex
	glVertexAttribPointer(A_VER, 2, GL_FLOAT, 0, 0, ver);
	glEnableVertexAttribArray(A_VER);

	//frag
	glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, ver);
	glEnableVertexAttribArray(T_VER);


	//get texture from shader 
	unis[0] = program.uniformLocation("tex_y");
	unis[1] = program.uniformLocation("tex_u");
	unis[2] = program.uniformLocation("tex_v");

	//create texture
	glGenTextures(3, texs);

	//Y
	//magnify linear
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//create video card texture space
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	//U
	//magnify linear
	glBindTexture(GL_TEXTURE_2D, texs[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//create video card texture space
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width/2, height/2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	//V
	//magnify linear
	glBindTexture(GL_TEXTURE_2D, texs[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//create video card texture space
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width/2, height/2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
}

void XVideoWidget::paintGL()
{
	qDebug() << "paintGL";
}

//window size change
void XVideoWidget::resizeGL(int width, int height)
{
	qDebug() << "resizeGL " << width << ":" << height;
}