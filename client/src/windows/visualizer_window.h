#pragma once

#include <QPainter>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <stack>
#undef ERROR
#include <DCS_Utils/include/DCS_ModuleUtils.h>

class VisualizerWindow : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
	Q_OBJECT
public:
	VisualizerWindow(QWidget* parent = nullptr);
	~VisualizerWindow();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    // void resizeEvent(QResizeEvent* event) override;
    // void paintEvent(QPaintEvent* event) override;

private:
    QMatrix4x4 view_matrix;
    QMatrix4x4 proj_matrix;

    QOpenGLShader* vert;
    QOpenGLShader* frag;
    QOpenGLShaderProgram* program;

    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    static constexpr int CIRCLE_SEG = 32;
    static constexpr QVector3D DEF_COLOR = QVector3D(1.0f, 1.0f, 1.0f);
    enum class DrawShape : GLint
    {
        LINE,
        CIRCLE,
        RECT
    };

    static constexpr GLint OFFSETS[] = {
        0,
        2,
        2 + CIRCLE_SEG
    };

    static constexpr GLint COUNTS[] = {
        2,
        CIRCLE_SEG,
        5
    };

    // This will generate many draw calls but we shouldn't have that many primitives
    void drawCircle(float x, float y, float sx, float sy, QVector3D color = DEF_COLOR);
    void drawLine(float x, float y, float d, float a, QVector3D color = DEF_COLOR);
    void drawRect(float x, float y, float w, float h, float a, QVector3D color = DEF_COLOR);
    void render();
};
