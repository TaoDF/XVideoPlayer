#include "xvideowidget.h"
#include <QDebug>
#include <QTimer>
extern "C"
{
	#include <libavutil/frame.h> 
}

#define GET_STR(x) #x
#define A_VER 3
#define T_VER 4

FILE* fp = NULL;

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

XVideoWidget::XVideoWidget(QWidget* parent) : QOpenGLWidget(parent)
{

}

XVideoWidget::~XVideoWidget()
{

}

void XVideoWidget::Init(int inWidth, int inHeight)
{
	mux.lock();

	this->width = inWidth;
	this->height = inHeight;

	mux.unlock();
}

void XVideoWidget::Repaint(AVFrame* frame)
{
	if (!frame) return;

	//line alignment
	mux.lock();

	//make sure the size is correct
	if (!data[0] || frame->width != this->width || frame->height != this->height || frame->width * frame->height == 0)
	{
		av_frame_free(&frame);
		mux.unlock();
		return;
	}
	memcpy(data[0], frame->data[0], width * height);
	memcpy(data[1], frame->data[1], width * height / 4);
	memcpy(data[2], frame->data[2], width * height / 4);

	mux.unlock();

	//refresh display
	update();
}

//init
void XVideoWidget::initializeGL()
{
	initializeOpenGLFunctions();
	//////////////////////////////////
	//set texture memory space
	delete data[0];
	delete data[1];
	delete data[2];

	data[0] = new unsigned char[width * height]; //Y
	data[1] = new unsigned char[width * height / 4]; //U
	data[2] = new unsigned char[width * height / 4]; //V

	if (texs[0])
	{
		glDeleteTextures(3, texs);
	}
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	//V
	//magnify linear
	glBindTexture(GL_TEXTURE_2D, texs[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//create video card texture space
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	///////////////////////

	qDebug() << "initializeGL";

	mux.lock();

	//init opengl (QOpenglfunctions)
	//initializeOpenGLFunctions();

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
		1.0f, 1.0f
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
	glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, tex);
	glEnableVertexAttribArray(T_VER);


	//get texture from shader 
	unis[0] = program.uniformLocation("tex_y");
	unis[1] = program.uniformLocation("tex_u");
	unis[2] = program.uniformLocation("tex_v");

	mux.unlock();

	//fp = fopen("C:\\Users\\TaoDaFen\\Desktop\\workspace\\1\\temp\\src\\QtGL\\x64\\Debug\\out.yuv", "rb");
	//if (!fp)
	//{
	//	qDebug() << "open video failed!";
	//}

	////start Timer
	//QTimer* ti = new QTimer(this);
	//connect(ti, SIGNAL(timeout()), this, SLOT(update()));
	//ti->start(40);
}

void XVideoWidget::paintGL()
{
	//if (feof(fp))
	//{
	//	fseek(fp, 0, SEEK_SET);
	//}

	//fread(data[0], 1, width * height, fp);
	//fread(data[1], 1, width * height / 4, fp);
	//fread(data[2], 1, width * height / 4, fp);

	mux.lock();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texs[0]); //bind Y to 0 level
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, data[0]); //modify texture
	glUniform1i(unis[0], 0); //0 level

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texs[1]); //bind U to 1 level
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, data[1]); //modify texture
	glUniform1i(unis[1], 1); //1 level

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, texs[2]); //bind V to 2 level
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, data[2]); //modify texture
	glUniform1i(unis[2], 2); //2 level

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	qDebug() << "paintGL";

	mux.unlock();
}

//window size change
void XVideoWidget::resizeGL(int width, int height)
{
	mux.lock();
	qDebug() << "resizeGL " << width << ":" << height;
	mux.unlock();
}