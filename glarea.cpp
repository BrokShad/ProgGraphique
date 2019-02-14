// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "glarea.h"
#include <GL/glu.h>
#include <QDebug>
#include <QSurfaceFormat>
#include <QtGui/QMatrix4x4>
#include <cmath>
#include <iostream>


#define X 0
#define Y 1

#ifndef PI
#define PI 3.1415f
#endif


std::vector<GLfloat> vertices;
std::vector<GLfloat> colors;
std::vector<GLfloat> vertices2;
std::vector<GLfloat> colors2;
static const QString vertexShaderFile   = ":/basic.vsh";
static const QString fragmentShaderFile = ":/basic.fsh";


GLArea::GLArea(QWidget *parent) :
    QOpenGLWidget(parent)
{
    qDebug() << "init GLArea" ;

    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
    sf.setSamples(16);  // nb de sample par pixels : suréchantillonnage por l'antialiasing, en décalant à chaque fois le sommet
    // cf https://www.khronos.org/opengl/wiki/Multisampling et https://stackoverflow.com/a/14474260
    setFormat(sf);
    qDebug() << "Depth is"<< format().depthBufferSize();

    setEnabled(true);  // événements clavier et souris
    setFocusPolicy(Qt::StrongFocus); // accepte focus
    setFocus();                      // donne le focus

    m_timer = new QTimer(this);
    m_timer->setInterval(50);  // msec
    connect (m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect (this, SIGNAL(radiusChanged(double)), this, SLOT(setRadius(double)));
}

GLArea::~GLArea()
{
    qDebug() << "destroy GLArea";

    delete m_timer;

    // Contrairement aux méthodes virtuelles initializeGL, resizeGL et repaintGL,
    // dans le destructeur le contexte GL n'est pas automatiquement rendu courant.
    makeCurrent();

    // ici destructions de ressources GL

    doneCurrent();
}


void GLArea::initializeGL()
{
    qDebug() << __FUNCTION__ ;
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    // shaders
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile);  // compile
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFile);
    if (! m_program->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << m_program->log();
    }

    // récupère identifiants de "variables" dans les shaders
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");
}

void GLArea::resizeGL(int w, int h)
{
    qDebug() << __FUNCTION__ << w << h;

    // C'est fait par défaut
    glViewport(0, 0, w, h);

    m_ratio = (double) w / h;
    // doProjection();
}


void GLArea::paintCyl(QMatrix4x4 matrix2, GLfloat ep_cyl, GLfloat r_cyl, int nb_fac, GLfloat coul_r, GLfloat coul_v, GLfloat coul_b)
{
    GLfloat alpha = 2*PI/nb_fac;
    for (int var = 0; var < nb_fac; ++var) {
        //        matrix.rotate(alpha, 0, 0, 1);

        vertices.push_back(cos(alpha*var)*r_cyl);
        vertices.push_back(sin(alpha*var)*r_cyl);
        vertices.push_back(ep_cyl/2);

        colors.push_back(coul_r*0.2);
        colors.push_back(coul_v*0.2);
        colors.push_back(coul_b*0.2);

        vertices.push_back(cos(alpha*var)*r_cyl);
        vertices.push_back(sin(alpha*var)*r_cyl);
        vertices.push_back(-ep_cyl/2);

        colors.push_back(coul_r*0.2);
        colors.push_back(coul_v*0.2);
        colors.push_back(coul_b*0.2);

        vertices.push_back(cos(alpha*var + alpha)*r_cyl);
        vertices.push_back(sin(alpha*var + alpha)*r_cyl);
        vertices.push_back(-ep_cyl/2);

        colors.push_back(coul_r*0.2);
        colors.push_back(coul_v*0.2);
        colors.push_back(coul_b*0.2);

        vertices.push_back(cos(alpha*var + alpha)*r_cyl);
        vertices.push_back(sin(alpha*var + alpha)*r_cyl);
        vertices.push_back(ep_cyl/2);

        colors.push_back(coul_r*0.2);
        colors.push_back(coul_v*0.2);
        colors.push_back(coul_b*0.2);
    }

    m_program->bind();

    m_program->setUniformValue(m_matrixUniform, matrix2);
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors.data());

    glEnableVertexAttribArray(m_posAttr);  // rend le VAA accessible pour glDrawArrays
    glEnableVertexAttribArray(m_colAttr);

    glDrawArrays(GL_QUADS, 0, nb_fac*4);

    glDisableVertexAttribArray(m_posAttr);
    glDisableVertexAttribArray(m_colAttr);

    m_program->release();

    vertices.clear();
    colors.clear();

    for(int i = -1 ; i < 2 ; i+=2) {
        for (int var = 0; var < nb_fac; ++var) {
            vertices2.push_back(cos(alpha*var)*r_cyl);
            vertices2.push_back(sin(alpha*var)*r_cyl);
            vertices2.push_back(i*ep_cyl/2);

            colors2.push_back(coul_r);
            colors2.push_back(coul_v);
            colors2.push_back(coul_b);
        }
        m_program->bind();

        m_program->setUniformValue(m_matrixUniform, matrix2);
        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, &vertices2[0]);
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, &colors2[0]);

        glEnableVertexAttribArray(m_posAttr);  // rend le VAA accessible pour glDrawArrays
        glEnableVertexAttribArray(m_colAttr);

        glDrawArrays(GL_POLYGON, 0, nb_fac);

        glDisableVertexAttribArray(m_posAttr);
        glDisableVertexAttribArray(m_colAttr);
        m_program->release();
        vertices2.clear();

        colors2.clear();
    }
    // on ne met pas glutSwapBuffers(); !
}


static float G[2] = {0, 0};
static float H[2] = {-0.5f, 0};
static float I[2] = {-0.5f, 0};
static float J[2] = {0, 0};
static float beta = 0;

static float GH = sqrt(powf(H[X] - G[X], 2) + powf(H[Y] - G[Y], 2));
static float HJ = 0.8f;

const static float decalage = -1.5f;

void GLArea::paintGL()
{
    qDebug() << __FUNCTION__ ;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind(); // active le shader program
    matrix.setToIdentity();
    GLfloat hr = m_radius, wr = hr * m_ratio;            // = glFrustum
    matrix.frustum(-wr, wr, -hr, hr, 0.1, 5.0);
    //matrix.perspective(60.0f, m_radius, 0.1f, 100.0f);  // = gluPerspective

    // Remplace gluLookAt (0, 0, 3.0, 0, 0, 0, 0, 1, 0);
    matrix.translate(0, 0, -3.0);


    //    H[X] = G[X] + GH * cos(PI - m_alpha);
    //    H[Y] = G[Y] + GH * sin(PI - m_alpha);

    //    I[X] = H[X];

    // Rotation de la scène pour l'animation
    matrix.rotate(m_angle, 0, 1, 0);
    matrix.rotate(m_anim,0,0,1);

    QMatrix4x4 matrix2 = matrix;

    m_program->setUniformValue(m_matrixUniform, matrix);

    paintPiston(matrix2, 1.0);

    m_program->release();
}

void GLArea::paintPiston(QMatrix4x4 matrix2, float taille)
{
    int nb_fac = 30;

    // CYL 1
    float e_cyl1 = 0.2*taille;
    float l_cyl1 = 1.0*taille;
    paintCyl(matrix2, e_cyl1,l_cyl1,nb_fac,1.0,0.5,1.0);

    // CYL 2
    float e_cyl2 = e_cyl1*5;
    float l_cyl2 = l_cyl1/7;
    paintCyl(matrix2, e_cyl2,l_cyl2,nb_fac,0.9,0.4,0.9);

    //CYL 3
    float e_cyl3 = e_cyl2/2;
    float l_cyl3 = l_cyl2;
    float cyl3[3];
    cyl3[X] = e_cyl3/2;
    cyl3[Z] = -e_cyl2/1.65;

    matrix2.translate(0,0,cyl3[Z]);
    matrix2.translate(cyl3[X],0,0);
    matrix2.rotate(90,0,1,0);
    matrix2.rotate(45,0,0,1);
    paintCyl(matrix2, e_cyl3,l_cyl3,4,1.0,1.0,0.5);
    matrix2 = matrix;

    // CYL 4
    float cyl4[3];
    cyl4[X] = cyl3[X]+e_cyl3/2;
    cyl4[Z] = -e_cyl2/2-l_cyl3*2.3;
    float e_cyl4 = e_cyl3/2;
    float l_cyl4 = l_cyl3;

    matrix2.translate(cyl4[X],0,0);
    matrix2.translate(0,0,cyl4[Z]);
    matrix2.rotate(90,0,0,1);
    paintCyl(matrix2, e_cyl4,l_cyl4,nb_fac,1.0,0.5,1.0);
    matrix2 = matrix;

    // CYL 5
    float e_cyl5 = e_cyl3;
    float l_cyl5 = l_cyl3;
    float cyl5[3];
    cyl5[X] = cyl3[X];
    cyl5[Z] = cyl3[Z]-l_cyl4*3.1;

    matrix2.translate(0,0,cyl5[Z]);
    matrix2.translate(cyl5[X],0,0);
    matrix2.rotate(90,0,1,0);
    matrix2.rotate(45,0,0,1);
    paintCyl(matrix2, e_cyl5,l_cyl5,4,1.0,1.0,0.5);
    matrix2 = matrix;

    //CYL 6
    float cyl6[3];
    cyl6[Z] = cyl5[Z]-l_cyl5*1.6;

    matrix2.translate(0,0,cyl6[Z]);
    float e_cyl6 = e_cyl4;
    float l_cyl6 = l_cyl4;
    paintCyl(matrix2, e_cyl6,l_cyl6,nb_fac,0.9,0.4,0.9);
    matrix2 = matrix;

    // CYL 7
    float e_cyl7 = e_cyl3;
    float l_cyl7 = l_cyl3;
    float cyl7[3];
    cyl7[X] = -cyl5[X];
    cyl7[Z] = cyl5[Z]-e_cyl6*1.8;

    matrix2.translate(0,0,cyl7[Z]);
    matrix2.translate(cyl7[X],0,0);
    matrix2.rotate(90,0,1,0);
    matrix2.rotate(45,0,0,1);
    paintCyl(matrix2, e_cyl7,l_cyl7,4,1.0,1.0,0.5);
    matrix2 = matrix;

    // CYL 8
    float cyl8[3];
    cyl8[X] = -cyl4[X];
    cyl8[Z] = cyl7[Z]-e_cyl7*0.45;
    float e_cyl8 = e_cyl6;
    float l_cyl8 = l_cyl6;

    matrix2.translate(cyl8[X],0,0);
    matrix2.translate(0,0,cyl8[Z]);
    matrix2.rotate(90,0,0,1);
    paintCyl(matrix2, e_cyl8,l_cyl8,nb_fac,1.0,0.5,1.0);
    matrix2 = matrix;

    // CYL 9
    float e_cyl9 = e_cyl7;
    float l_cyl9 = l_cyl7;
    float cyl9[3];
    cyl9[X] = cyl7[X];
    cyl9[Z] = cyl7[Z]-e_cyl8*1.8;

    matrix2.translate(0,0,cyl9[Z]);
    matrix2.translate(cyl9[X],0,0);
    matrix2.rotate(90,0,1,0);
    matrix2.rotate(45,0,0,1);
    paintCyl(matrix2, e_cyl9,l_cyl9,4,1.0,1.0,0.5);
    matrix2 = matrix;

    //CYL 10
    float cyl10[3];
    cyl10[Z] = cyl9[Z]-l_cyl9*1.6;

    matrix2.translate(0,0,cyl10[Z]);
    float e_cyl10 = e_cyl4;
    float l_cyl10 = l_cyl4;
    paintCyl(matrix2, e_cyl10,l_cyl10,nb_fac,0.9,0.4,0.9);
    matrix2 = matrix;

    // CYL 11
    float e_cyl11 = e_cyl7;
    float l_cyl11 = l_cyl7;
    float cyl11[3];
    cyl11[X] = cyl9[X];
    cyl11[Z] = cyl9[Z]-e_cyl10*1.8;

    matrix2.translate(0,0,cyl11[Z]);
    matrix2.translate(cyl11[X],0,0);
    matrix2.rotate(90,0,1,0);
    matrix2.rotate(45,0,0,1);
    paintCyl(matrix2, e_cyl11,l_cyl11,4,1.0,1.0,0.5);
    matrix2 = matrix;

    // CYL 12
    float cyl12[3];
    cyl12[X] = cyl8[X];
    cyl12[Z] = cyl11[Z]-l_cyl11*1.5;
    float e_cyl12 = e_cyl6;
    float l_cyl12 = l_cyl6;

    matrix2.translate(cyl12[X],0,0);
    matrix2.translate(0,0,cyl12[Z]);
    matrix2.rotate(90,0,0,1);
    paintCyl(matrix2, e_cyl12,l_cyl12,nb_fac,1.0,0.5,1.0);
    matrix2 = matrix;

    // CYL 13
    float e_cyl13 = e_cyl7;
    float l_cyl13 = l_cyl7;
    float cyl13[3];
    cyl13[X] = cyl9[X];
    cyl13[Z] = cyl12[Z]-e_cyl12*0.9;

    matrix2.translate(0,0,cyl13[Z]);
    matrix2.translate(cyl13[X],0,0);
    matrix2.rotate(90,0,1,0);
    matrix2.rotate(45,0,0,1);
    paintCyl(matrix2, e_cyl13,l_cyl13,4,1.0,1.0,0.5);
    matrix2 = matrix;

    //CYL 14
    float cyl14[3];
    cyl14[Z] = cyl13[Z]-l_cyl13*1.59;

    matrix2.translate(0,0,cyl14[Z]);
    float e_cyl14 = e_cyl4;
    float l_cyl14 = l_cyl4;
    paintCyl(matrix2, e_cyl14,l_cyl14,nb_fac,0.9,0.4,0.9);
    matrix2 = matrix;

    // CYL 15
    float e_cyl15 = e_cyl3;
    float l_cyl15 = l_cyl3;
    float cyl15[3];
    cyl15[X] = cyl3[X];
    cyl15[Z] = cyl14[Z]-l_cyl14*1.5;

    matrix2.translate(0,0,cyl15[Z]);
    matrix2.translate(cyl15[X],0,0);
    matrix2.rotate(90,0,1,0);
    matrix2.rotate(45,0,0,1);
    paintCyl(matrix2, e_cyl15,l_cyl15,4,1.0,1.0,0.5);
    matrix2 = matrix;
}



void GLArea::keyPressEvent(QKeyEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->text();

    switch(ev->key()) {
    case Qt::Key_Space :
        m_angle += 3;
        if (m_angle >= 360) m_angle -= 360;
        update();
        break;
    case Qt::Key_A :
        if (m_timer->isActive())
            m_timer->stop();
        else m_timer->start();
        break;
    case Qt::Key_R :
        if (ev->text() == "r")
            setRadius(m_radius-0.05);
        else setRadius(m_radius+0.05);
        break;
    }
}

void GLArea::keyReleaseEvent(QKeyEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->text();
}

void GLArea::mousePressEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y() << ev->button();
}

void GLArea::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y() << ev->button();
}

void GLArea::mouseMoveEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y();
}

void GLArea::onTimeout()
{
    qDebug() << __FUNCTION__ << "ok";
    m_anim += 3;
    if (m_anim > 360) m_anim = 0;
    update();
}

void GLArea::setRadius(double radius)
{
    qDebug() << __FUNCTION__ << radius << sender();
    if (radius != m_radius && radius > 0.01 && radius <= 10) {
        m_radius = radius;
        qDebug() << "  emit radiusChanged()";
        emit radiusChanged(radius);
        update();
    }
}




