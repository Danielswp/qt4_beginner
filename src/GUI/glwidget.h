#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QGLBuffer>
#include <QtOpenGL/QGLContext>

#include <QKeyEvent>
#include <QPushButton>
#include <QDebug>

class GLWidget : public QGLWidget, protected QGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

    void printOpenGLVersionInfo();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

protected slots:
    void onBtnClicked();

private:
    Q_DISABLE_COPY(GLWidget)

    void cleanupGL();
    bool buildShaderProgram(bool tryResourceFiles);
    bool resolveShaderLocations();
    bool createVertexBuffer();
    void setTriangleColor(float r, float g, float b, float a);

private:
    QGLBuffer m_vbo;                 // 用 QGLBuffer 管理 VBO，避免裸 GLuint 跨上下文失效
    QGLShaderProgram *m_shader;      // 不设 QObject parent，显式在当前上下文有效时 delete
    QPushButton *m_btn;

    int m_posLoc;
    int m_colorLoc;
    bool m_shaderReady;
    bool m_useAltColor;

    float m_triangleColor[4];        // r, g, b, a
};

#endif // GLWIDGET_H
