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


QVector<GLfloat> vertices;
QVector<GLfloat> colors;
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

    tearGLObjects();
    // ici destructions de ressources GL

    doneCurrent();
}

void GLArea::makeGLObjects()
{

    GLfloat texCoords[] = {
        0, 0,
        1, 0,
        1, 1
    };

    paintCyl(matrix2,1, 0.15f, nb_fac, 0.4, 0.9, 0.9,false); //H
    paintCyl(matrix2,0.2, 0.25f, nb_fac, 0.4, 0.9, 0.9,false); //H2
    paintCyl(matrix2,0.9, 0.1f, nb_fac, 0.4, 0.9, 0.9,false); //HBIS
    paintCyl(matrix2,0.33f, 0.13f, nb_fac, 0.4, 0.9, 0.9,false); //J
    paintCyl(matrix2,0.8, 0.1f, nb_fac, 0.4, 0.9, 0.9,false); //JH
    paintCyl(matrix2, 1.2, 0.3, nb_fac, 0.4, 0.9, 0.9,false); //KJ
    paintCyl(matrix2, 1, 0.1f, nb_fac, 0.4, 0.9, 0.9,false); //KJ
    paintCyl(matrix2,1.3, 0.4, nb_fac, 0.9, 0.9, 0.4,false); //FIXE
    paintCyl(matrix2,1.3, 0.4, nb_fac, 0.9, 0.9, 0.4,true);

    QVector<GLfloat> vertData;
    for (int i = 0; i < vertices.size()/3; ++i) {
        // coordonnées sommets
        for (int j = 0; j < 3; j++)
            vertData.append(vertices[i*3+j]);
        for (int j = 0; j < 3; j++)
            vertData.append(colors[i*3+j]);
        for (int j = 0; j < 2; j++)
            vertData.append(texCoords[j]);
        // couleurs sommets
    }

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertData.constData(),vertData.count() * sizeof(GLfloat));

    for (int i = 0; i < 1; i++) {
        QString nom = QString(":/metal.jpg");
        QImage image(nom);
        if (image.isNull())
            qDebug() << "load image" << nom << "failed";
        else qDebug() << "load image" << nom << image.size();
        m_textures[i] = new QOpenGLTexture(image);
    }
}

void GLArea::tearGLObjects()
{
    delete m_textures[0];
    m_vbo.destroy();
}

void GLArea::initializeGL()
{
    qDebug() << __FUNCTION__ ;
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    makeGLObjects();
    qDebug() << "makeGlObj vectSize : " << vertices.size() << endl;
    // shaders
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile);  // compile
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFile);
    if (! m_program->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << m_program->log();
    }

    m_program->setUniformValue("texture", 0);
}

void GLArea::resizeGL(int w, int h)
{
    qDebug() << __FUNCTION__ << w << h;

    // C'est fait par défaut
    glViewport(0, 0, w, h);

    m_ratio = (double) w / h;
    // doProjection();
}


void GLArea::paintCyl(QMatrix4x4 matrix2, GLfloat ep_cyl, GLfloat r_cyl, int nb_fac, GLfloat coul_r, GLfloat coul_v, GLfloat coul_b, bool coupe)
{
    GLfloat alpha = 2*PI/nb_fac;
    int nb_facTmp = nb_fac;
    if (coupe)
        nb_facTmp = nb_fac/2;
    for (int var = 0; var < nb_facTmp; ++var) {
        //        matrix.rotate(alpha, 0, 0, 1);

        //A
        vertices.push_back(cos(alpha*var)*r_cyl);
        vertices.push_back(sin(alpha*var)*r_cyl);
        vertices.push_back(ep_cyl/2);

        colors.push_back(coul_r*0.2);
        colors.push_back(coul_v*0.2);
        colors.push_back(coul_b*0.2);

        //B
        vertices.push_back(cos(alpha*var)*r_cyl);
        vertices.push_back(sin(alpha*var)*r_cyl);
        vertices.push_back(-ep_cyl/2);

        colors.push_back(coul_r*0.2);
        colors.push_back(coul_v*0.2);
        colors.push_back(coul_b*0.2);

        //C
        vertices.push_back(cos(alpha*var + alpha)*r_cyl);
        vertices.push_back(sin(alpha*var + alpha)*r_cyl);
        vertices.push_back(-ep_cyl/2);

        colors.push_back(coul_r*0.2);
        colors.push_back(coul_v*0.2);
        colors.push_back(coul_b*0.2);

        //C
        vertices.push_back(cos(alpha*var + alpha)*r_cyl);
        vertices.push_back(sin(alpha*var + alpha)*r_cyl);
        vertices.push_back(-ep_cyl/2);

        colors.push_back(coul_r*0.2);
        colors.push_back(coul_v*0.2);
        colors.push_back(coul_b*0.2);

        //D
        vertices.push_back(cos(alpha*var + alpha)*r_cyl);
        vertices.push_back(sin(alpha*var + alpha)*r_cyl);
        vertices.push_back(ep_cyl/2);

        colors.push_back(coul_r*0.2);
        colors.push_back(coul_v*0.2);
        colors.push_back(coul_b*0.2);

        //A
        vertices.push_back(cos(alpha*var)*r_cyl);
        vertices.push_back(sin(alpha*var)*r_cyl);
        vertices.push_back(ep_cyl/2);

        colors.push_back(coul_r*0.2);
        colors.push_back(coul_v*0.2);
        colors.push_back(coul_b*0.2);
    }
    for(int i = -1 ; i < 2 ; i+=2) {
        for (int var = 0; var < nb_facTmp; ++var) {
            vertices.push_back(cos(alpha*var)*r_cyl);
            vertices.push_back(sin(alpha*var)*r_cyl);
            vertices.push_back(i*ep_cyl/2);

            colors.push_back(coul_r);
            colors.push_back(coul_v);
            colors.push_back(coul_b);

            vertices.push_back(cos(alpha*(var-1))*r_cyl);
            vertices.push_back(sin(alpha*(var-1))*r_cyl);
            vertices.push_back(i*ep_cyl/2);

            colors.push_back(coul_r);
            colors.push_back(coul_v);
            colors.push_back(coul_b);

            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(i*ep_cyl/2);

            colors.push_back(coul_r);
            colors.push_back(coul_v);
            colors.push_back(coul_b);

        }
    }

    //    vertices.clear();
    //    colors.clear();

}


void GLArea::setCoupe()
{
    coupe = !coupe;
    update();
}

void GLArea::paintGL()
{
    qDebug() << __FUNCTION__ ;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //    m_program->bind(); // active le shader program
    m_program->bind();

    matrix.setToIdentity();
    GLfloat hr = m_radius, wr = hr * m_ratio;            // = glFrustum
    //matrix.frustum(-wr, wr, -hr, hr, 0.1, 5.0);
    matrix.perspective(60.0f, m_radius, 0.1f, 100.0f);  // = gluPerspective

    // Remplace gluLookAt (0, 0, 3.0, 0, 0, 0, 0, 1, 0);
    matrix.translate(0, 0, -10.0);
    matrix.rotate(-100,1,0,0);

    // Rotation de la scène pour l'animation
    matrix.rotate(rotate,0.3,1,0.7);

    m_program->setUniformValue("matrix", matrix);
    m_program->setAttributeBuffer("posAttr", GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
    m_program->setAttributeBuffer("colAttr", GL_FLOAT, 3 * sizeof(GLfloat), 3, 8 * sizeof(GLfloat));
    m_program->setAttributeBuffer("texAttr", GL_FLOAT, 6 * sizeof(GLfloat), 2, 7 * sizeof(GLfloat));
    //    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices.data());
    //    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors.data());

    //    glEnableVertexAttribArray(m_posAttr);  // rend le VAA accessible pour glDrawArrays
    //    glEnableVertexAttribArray(m_colAttr);
    m_program->enableAttributeArray("posAttr");
    m_program->enableAttributeArray("texAttr");
    m_program->enableAttributeArray("colAttr");

    paintMoteur(matrix);

    //    glDisableVertexAttribArray(m_posAttr);
    //    glDisableVertexAttribArray(m_colAttr);
    m_program->disableAttributeArray("posAttr");
    m_program->disableAttributeArray("texAttr");
    m_program->disableAttributeArray("colAttr");

    m_program->release();

}

static float HJ = 0.8f;

void GLArea::paintPiston(QMatrix4x4 matrix2, int i)
{

    QMatrix4x4 matrix3 = matrix2;
    static float G[2] = {0, 0};
    static float H[2] = {-0.5f, 0};
    static float Hbis[2] = {-0.5f, 0};
    static float I[2] = {-0.5f, 0};
    static float J[2] = {0, 0};
    static float beta = 0;

    static float GH = sqrt(powf(H[X] - G[X], 2) + powf(H[Y] - G[Y], 2));
    static float HJ = 0.8f;


    H[X] = G[X] + GH * cos(PI - m_anim)*i;
    H[Y] = G[Y] + GH * sin(PI - m_anim)*i;

    I[X] = H[X];

    J[X] = I[X] - sqrt(pow(HJ, 2) - pow(GH*sin(-m_anim), 2));
    beta = atan(H[Y]/(abs(J[X] - H[X])));

    //KJ
    //    matrix2.rotate(m_angle, 0, 1.0f, 0.1f);
    //    matrix2.translate(0, 0, 0.2f);
    //    matrix2.translate(J[X]-1.8/2, J[Y], 0);
    //    matrix2.rotate(90, 0, 1.0f, 0);
    //    paintCyl(matrix2,1.8f, 0.1f, nb_fac, 0.9, 0.4, 0.9);
    //    matrix2 = matrix;

    //H
    matrix2.translate(H[X], H[Y], 0);
    m_program->setUniformValue("matrix", matrix2);
    m_textures[0]->bind();
    glDrawArrays(GL_TRIANGLES, 0, nb_fac*12);
    m_textures[0]->release();
    matrix2 = matrix3;

    //H2
    matrix2.translate(H[X], H[Y], 0.5);
    m_program->setUniformValue("matrix", matrix2);
    m_textures[0]->bind();
    glDrawArrays(GL_TRIANGLES, nb_fac*12, nb_fac*12);
    m_textures[0]->release();
    matrix2 = matrix3;


    //H2
    matrix2.translate(H[X], H[Y], -0.5);
    m_program->setUniformValue("matrix", matrix2);
    m_textures[0]->bind();
    glDrawArrays(GL_TRIANGLES, nb_fac*12, nb_fac*12);
    m_textures[0]->release();
    matrix2 = matrix3;

    Hbis[X] = G[X] + GH * -cos(PI - m_anim)*i;
    Hbis[Y] = G[Y] + GH * -sin(PI - m_anim)*i;

    if (i != 1) {

        //Hbis
        matrix2.translate(Hbis[X]/2, Hbis[Y]/2, -0.5);
        matrix2.rotate(90,0,1,0);
        matrix2.rotate(-(PI-m_anim)*180/PI,1,0,0);
        matrix2.translate(0,0,-0.1);
        m_program->setUniformValue("matrix", matrix2);
        m_textures[0]->bind();
        glDrawArrays(GL_TRIANGLES, 2*nb_fac*12, nb_fac*12);
        m_textures[0]->release();
        matrix2 = matrix3;

        //Hbis
        matrix2.translate(Hbis[X]/2, Hbis[Y]/2, 0.5);
        matrix2.rotate(90,0,1,0);
        matrix2.rotate(-(PI-m_anim)*180/PI,1,0,0);
        matrix2.translate(0,0,-0.1);
        m_program->setUniformValue("matrix", matrix2);
        m_textures[0]->bind();
        glDrawArrays(GL_TRIANGLES, 2*nb_fac*12, nb_fac*12);
        m_textures[0]->release();
        matrix2 = matrix3;
    }

    //(J)
    matrix2.translate(J[X], J[Y], 0);
    m_program->setUniformValue("matrix", matrix2);
    m_textures[0]->bind();
    glDrawArrays(GL_TRIANGLES, 3*nb_fac*12, nb_fac*12);
    m_textures[0]->release();
    matrix2 = matrix3;

    //JH
    matrix2.translate(J[X], 0, 0);
    matrix2.rotate(beta*180/PI, 0, 0, 1);
    matrix2.translate(HJ/2.0f, 0, 0);
    matrix2.rotate(90, 0, 1.0f, 0);
    m_program->setUniformValue("matrix", matrix2);
    m_textures[0]->bind();
    glDrawArrays(GL_TRIANGLES, 4*nb_fac*12, nb_fac*12);
    m_textures[0]->release();
    matrix2 = matrix3;

    //(KJ)
    matrix2.translate(J[X]-1.5, J[Y], 0);
    matrix2.rotate(90, 0, 1.0f, 0);
    m_program->setUniformValue("matrix", matrix2);
    m_textures[0]->bind();
    glDrawArrays(GL_TRIANGLES, 5*nb_fac*12, nb_fac*12);
    m_textures[0]->release();
    matrix2 = matrix3;

    //(KJ)
    matrix2.translate(J[X]-0.5, J[Y], 0);
    matrix2.rotate(90, 0, 1.0f, 0);
    m_program->setUniformValue("matrix", matrix2);
    m_textures[0]->bind();
    glDrawArrays(GL_TRIANGLES, 6*nb_fac*12, nb_fac*12);
    m_textures[0]->release();
    matrix2 = matrix3;

    //PISTON fixe
    matrix2.translate(-3, 0, 0);
    matrix2.rotate(90,0,1,0);
    m_program->setUniformValue("matrix", matrix2);
    if (coupe) {
        m_textures[0]->bind();
        glDrawArrays(GL_TRIANGLES, 8*nb_fac*12, nb_fac*6);
        m_textures[0]->release();
    } else {
        m_textures[0]->bind();
        glDrawArrays(GL_TRIANGLES, 7*nb_fac*12, nb_fac*12);
        m_textures[0]->release();
    }
    matrix2 = matrix3;



}

void GLArea::paintMoteur(QMatrix4x4 matrix2 )
{
    QMatrix4x4 matrix3 = matrix2;
    paintPiston(matrix3,1);
    matrix3 = matrix2;

    matrix3.translate(0,0,1);
    paintPiston(matrix3,1);
    matrix3 = matrix2;

    matrix3.translate(0,0,2);
    paintPiston(matrix3,-1);
    matrix3 = matrix2;

    matrix3.translate(0,0,-1);
    paintPiston(matrix3,-1);
    matrix3 = matrix2;

    //    matrix3.translate(2,0,0);
    //    matrix3.rotate(90,0,1,0);
    //    paintPiston(matrix3);
    //    matrix3 = matrix2;
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
    case Qt::Key_D :
        acceleration = 0.05;
        m_timer->start();
        break;
    case Qt::Key_S :
        m_timer->stop();
        break;
    case Qt::Key_A :
        if (ev->text() == "a"){
            if (acceleration > 0){
                acceleration-=0.05;
            } else if (acceleration < 0.001){
                acceleration = 0;
            }
        }
        else if (acceleration < 0.65) {
            acceleration += 0.05;
        }
        break;


    case Qt::Key_R :
        rotate += 3;
        if (rotate >= 360) rotate -= 360;
        update();
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
    m_anim += acceleration;
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




