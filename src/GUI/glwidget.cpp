#include "glwidget.h"

#include <QString>

namespace {

const char kVertexShaderSource[] =
    "attribute vec3 aPos;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "}\n";

const char kFragmentShaderSource[] =
    "#ifdef GL_ES\n"
    "precision mediump float;\n"
    "#endif\n"
    "uniform vec4 uColor;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = uColor;\n"
    "}\n";

const char kVertPath[] = ":/resources/shaders/shader.vert.glsl";
const char kFragPath[] = ":/resources/shaders/shader.frag.glsl";

const char * const kPositionAttrNames[] = {
    "aPos", "vertex", "position", "aPosition", "vPosition", "vertexPosition"
};

const char * const kColorUniformNames[] = {
    "uColor", "color", "uColour", "fragColor", "triangleColor"
};

const GLfloat kVertices[] = {
    -0.5f, 0.0f, 0.0f,
     0.5f, 0.0f, 0.0f,
     0.0f, 0.5f, 0.0f
};

} // namespace

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent),
      m_vbo(QGLBuffer::VertexBuffer),
      m_shader(0),
      m_btn(0),
      m_posLoc(-1),
      m_colorLoc(-1),
      m_shaderReady(false),
      m_useAltColor(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setTriangleColor(1.0f, 0.0f, 0.0f, 1.0f);

    // 如需按钮测试，取消注释；m_btn 已初始化为 0。
    // m_btn = new QPushButton(QString::fromLatin1("Test"), this);
    // connect(m_btn, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
}

GLWidget::~GLWidget()
{
    // OpenGL 资源必须在 QGLWidget 的上下文销毁前、且上下文为当前时释放。
    const bool hadValidContext = isValid();
    if (hadValidContext)
        makeCurrent();

    cleanupGL();

    if (hadValidContext)
        doneCurrent();
}

void GLWidget::cleanupGL()
{
    m_shaderReady = false;
    m_posLoc = -1;
    m_colorLoc = -1;

    // QGLBuffer / QGLShaderProgram 内部用共享资源 guard 管理上下文；
    // 即使 initializeGL 因新 QGLContext 被再次调用，这里也不会 delete 野指针。
    if (m_vbo.isCreated())
        m_vbo.destroy();

    delete m_shader;
    m_shader = 0;
}

void GLWidget::setTriangleColor(float r, float g, float b, float a)
{
    m_triangleColor[0] = r;
    m_triangleColor[1] = g;
    m_triangleColor[2] = b;
    m_triangleColor[3] = a;
}

void GLWidget::onBtnClicked()
{
    qDebug() << "Btn clicked, change color";

    m_useAltColor = !m_useAltColor;
    if (m_useAltColor)
        setTriangleColor(0.8f, 0.4f, 0.1f, 1.0f);
    else
        setTriangleColor(1.0f, 0.0f, 0.0f, 1.0f);

    updateGL();
}

void GLWidget::printOpenGLVersionInfo()
{
    const QGLContext *myContext = context();
    if (!myContext || !isValid()) {
        qWarning() << "GLWidget: no valid QGLContext, cannot query GL strings";
        return;
    }

    const bool needMakeCurrent = (QGLContext::currentContext() != myContext);
    if (needMakeCurrent)
        makeCurrent();

    const char *version  = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    const char *vendor   = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
    const char *renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    qDebug() << "GL Version:" << (version  ? version  : "<null>");
    qDebug() << "Vendor:"     << (vendor   ? vendor   : "<null>");
    qDebug() << "Renderer:"   << (renderer ? renderer : "<null>");

    if (needMakeCurrent)
        doneCurrent();
}

void GLWidget::initializeGL()
{
    const QGLContext *ctx = context();
    if (!ctx || !isValid()) {
        qWarning() << "GLWidget::initializeGL: invalid QGLContext";
        return;
    }

    // Qt 4.8 的 QGLFunctions 必须绑定到当前上下文；新上下文时会重新绑定。
    initializeGLFunctions(ctx);

    // initializeGL 可能在 QGLWidget 被分配新 QGLContext 时再次调用。
    cleanupGL();

    printOpenGLVersionInfo();

    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    if (!hasOpenGLFeature(QGLFunctions::Shaders) ||
        !hasOpenGLFeature(QGLFunctions::Buffers) ||
        !QGLShaderProgram::hasOpenGLShaderPrograms(ctx)) {
        qWarning() << "GLWidget: shaders or buffer objects are not supported";
        return;
    }

    // 显式绑定 QGLContext，且不把 this 设为 QObject parent，避免上下文销毁后才被父对象 delete。
    m_shader = new QGLShaderProgram(ctx, 0);

    if (!buildShaderProgram(true)) {
        cleanupGL();
        return;
    }

    if (!createVertexBuffer()) {
        cleanupGL();
        return;
    }

    m_shaderReady = true;
}

bool GLWidget::buildShaderProgram(bool tryResourceFiles)
{
    m_shader->removeAllShaders();

    bool added = false;

    if (tryResourceFiles) {
        const QString vertPath = QString::fromLatin1(kVertPath);
        const QString fragPath = QString::fromLatin1(kFragPath);

        added = m_shader->addShaderFromSourceFile(QGLShader::Vertex, vertPath);
        if (added)
            added = m_shader->addShaderFromSourceFile(QGLShader::Fragment, fragPath);

        if (!added)
            qWarning() << "External shader files failed, using built-in shaders:" << m_shader->log();
    }

    if (!added) {
        m_shader->removeAllShaders();
        added = m_shader->addShaderFromSourceCode(QGLShader::Vertex, kVertexShaderSource);
        if (added)
            added = m_shader->addShaderFromSourceCode(QGLShader::Fragment, kFragmentShaderSource);

        if (!added) {
            qWarning() << "Built-in shaders failed:" << m_shader->log();
            return false;
        }
    }

    if (!m_shader->link()) {
        qWarning() << "Shader link failed:" << m_shader->log();
        if (tryResourceFiles) {
            m_shader->removeAllShaders();
            return buildShaderProgram(false);
        }
        return false;
    }

    if (!m_shader->isLinked()) {
        qWarning() << "Shader program is not linked";
        return false;
    }

    if (!resolveShaderLocations()) {
        if (tryResourceFiles) {
            qWarning() << "External shader interface mismatch, rebuilding with built-in shaders";
            return buildShaderProgram(false);
        }
        qWarning() << "Cannot resolve shader attribute/uniform locations";
        return false;
    }

    return true;
}

bool GLWidget::resolveShaderLocations()
{
    m_posLoc = -1;
    const int posNameCount = int(sizeof(kPositionAttrNames) / sizeof(kPositionAttrNames[0]));
    for (int i = 0; i < posNameCount; ++i) {
        const int loc = m_shader->attributeLocation(kPositionAttrNames[i]);
        if (loc >= 0) {
            m_posLoc = loc;
            break;
        }
    }

    m_colorLoc = -1;
    const int colorNameCount = int(sizeof(kColorUniformNames) / sizeof(kColorUniformNames[0]));
    for (int i = 0; i < colorNameCount; ++i) {
        const int loc = m_shader->uniformLocation(kColorUniformNames[i]);
        if (loc >= 0) {
            m_colorLoc = loc;
            break;
        }
    }

    if (m_posLoc < 0)
        qWarning() << "Vertex position attribute not found in linked program";
    if (m_colorLoc < 0)
        qWarning() << "Color uniform not found in linked program";

    return m_posLoc >= 0 && m_colorLoc >= 0;
}

bool GLWidget::createVertexBuffer()
{
    m_vbo.setUsagePattern(QGLBuffer::StaticDraw);

    if (!m_vbo.create()) {
        qWarning() << "QGLBuffer::create() failed";
        return false;
    }

    if (!m_vbo.bind()) {
        qWarning() << "QGLBuffer::bind() failed";
        m_vbo.destroy();
        return false;
    }

    m_vbo.allocate(kVertices, int(sizeof(kVertices)));
    m_vbo.release();
    return true;
}

void GLWidget::resizeGL(int w, int h)
{
    if (w <= 0 || h <= 0)
        return;

    glViewport(0, 0, GLsizei(w), GLsizei(h));
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_shaderReady || !m_shader || !m_shader->isLinked() ||
        !m_vbo.isCreated() || m_posLoc < 0 || m_colorLoc < 0) {
        return;
    }

    // 着色器未链接或 bind 失败时绝不继续调用 uniform / draw。
    if (!m_shader->bind())
        return;

    m_shader->setUniformValue(m_colorLoc,
                              m_triangleColor[0],
                              m_triangleColor[1],
                              m_triangleColor[2],
                              m_triangleColor[3]);

    if (!m_vbo.bind()) {
        m_shader->release();
        return;
    }

    m_shader->enableAttributeArray(m_posLoc);
    m_shader->setAttributeBuffer(m_posLoc, GL_FLOAT, 0, 3, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    m_shader->disableAttributeArray(m_posLoc);
    m_vbo.release();

    m_shader->release();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (!event)
        return;

    switch (event->key()) {
    case Qt::Key_W:
        qDebug() << "W";
        event->accept();
        break;
    case Qt::Key_S:
        qDebug() << "S";
        event->accept();
        break;
    case Qt::Key_A:
        qDebug() << "A";
        event->accept();
        break;
    case Qt::Key_D:
        qDebug() << "D";
        event->accept();
        break;
    case Qt::Key_Escape:
        setFocus();
        event->accept();
        break;
    default:
        QGLWidget::keyPressEvent(event);
        return;
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (!event)
        return;

    QGLWidget::keyReleaseEvent(event);
}
