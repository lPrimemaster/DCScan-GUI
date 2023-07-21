#include "visualizer_window.h"
#include <QPaintEvent>
#include <QPainterPath>
#include <QResizeEvent>
#include <QApplication>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEG2RAD M_PI * 2 / 360.0

VisualizerWindow::VisualizerWindow(QWidget* parent) : QOpenGLWidget(parent)
{

}

VisualizerWindow::~VisualizerWindow()
{
    vao.destroy();
    vbo.destroy();
}

void VisualizerWindow::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    proj_matrix.setToIdentity();
    proj_matrix.ortho(-100.0f, 100.0f, -100.0f, 100.0f, -1.0f, 1.0f);
    view_matrix.setToIdentity();

    // Vertex shader source code
    vert = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vert->compileSourceCode(
        R"""(
            #version 330 core
            layout(location = 0) in vec2 pos;
            out vec4 ocolor;

            uniform mat4 VP;
            uniform mat4 M;
            uniform vec3 color;
            
            void main()
            {
                ocolor = vec4(color, 1.0);
                gl_Position = VP * M * vec4(pos, 0.0, 1.0);
            }
        )"""
    );

    if(!vert->isCompiled())
    {
        LOG_ERROR("Error: %s", vert->log().toStdString().c_str());
    }

    // Fragment shader source code
    frag = new QOpenGLShader(QOpenGLShader::Fragment, this);
    frag->compileSourceCode(
        R"""(
            #version 330 core
            in vec4 ocolor;
            out vec4 fragcol;
            
            void main()
            {
                fragcol = ocolor;
            }
        )""" 
    );

    if(!frag->isCompiled())
    {
        LOG_ERROR("Error: %s", frag->log().toStdString().c_str());
    }

    // Setup the program
    program = new QOpenGLShaderProgram(this);
    program->addShader(vert);
    program->addShader(frag);
    if(!program->link())
    {
        LOG_ERROR("%s", program->log().toStdString().c_str());
    }

    // Setup data
    vao.create();
    vao.bind();
    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    std::vector<float> vtx;

    // Lines
    vtx.push_back(0.0f);
    vtx.push_back(0.0f);
    vtx.push_back(1.0f);
    vtx.push_back(0.0f);

    // Circles
    for(int i = 0; i < CIRCLE_SEG; i++)
    {
        float a = ((float)i / (CIRCLE_SEG-1)) * 2 * M_PI;
        float x = std::cosf(a);
        float y = std::sinf(a);
        vtx.push_back(x);
        vtx.push_back(y);
    }

    // Rects
    vtx.push_back( 1.0f);
    vtx.push_back( 1.0f);

    vtx.push_back(-1.0f);
    vtx.push_back( 1.0f);

    vtx.push_back(-1.0f);
    vtx.push_back(-1.0f);

    vtx.push_back( 1.0f);
    vtx.push_back(-1.0f);

    vtx.push_back( 1.0f);
    vtx.push_back( 1.0f);

    vbo.allocate(vtx.data(), sizeof(float) * (GLint)vtx.size());
    glVertexAttribPointer(0,  2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    vao.release();
}

void VisualizerWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    proj_matrix.setToIdentity();
    proj_matrix.ortho(-100.0f*(float)w/h, 100.0f*(float)w/h, -100.0f, 100.0f, -1.0f, 1.0f);
}

void VisualizerWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    program->bind();
    program->setUniformValue("VP", proj_matrix * view_matrix);
    vao.bind();
    render();
    vao.release();
    program->release();
}

void VisualizerWindow::render()
{
    drawCircle(0, 0, 15, 15);
    drawLine(0, 0, 15, 45);
    drawRect(0, 0, 50, 5, 45);
}

void VisualizerWindow::drawLine(float x, float y, float d, float a, QVector3D color)
{
    static constexpr GLint type = static_cast<std::underlying_type_t<DrawShape>>(DrawShape::LINE);
    QMatrix4x4 model;
    model.setToIdentity();
    model.translate(x, y);
    model.scale(d, d);
    model.rotate(QQuaternion::fromEulerAngles(0, 0, a));

    program->setUniformValue("M", model);
    program->setUniformValue("color", color);
    glDrawArrays(GL_LINE_STRIP, OFFSETS[type], COUNTS[type]);
}

void VisualizerWindow::drawCircle(float x, float y, float sx, float sy, QVector3D color)
{
    static constexpr GLint type = static_cast<std::underlying_type_t<DrawShape>>(DrawShape::CIRCLE);
    QMatrix4x4 model;
    model.setToIdentity();
    model.translate(x, y);
    model.scale(sx, sy);

    program->setUniformValue("M", model);
    program->setUniformValue("color", color);
    glDrawArrays(GL_LINE_STRIP, OFFSETS[type], COUNTS[type]);
}

void VisualizerWindow::drawRect(float x, float y, float w, float h, float a, QVector3D color)
{
    static constexpr GLint type = static_cast<std::underlying_type_t<DrawShape>>(DrawShape::RECT);
    QMatrix4x4 model;
    model.setToIdentity();
    model.translate(x, y);
    model.rotate(QQuaternion::fromEulerAngles(0, 0, a));
    model.scale(w, h);

    program->setUniformValue("M", model);
    program->setUniformValue("color", color);
    glDrawArrays(GL_LINE_STRIP, OFFSETS[type], COUNTS[type]);
}
