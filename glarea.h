// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#ifndef GLAREA_H
#define GLAREA_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QKeyEvent>
#include <QTimer>
#include <QOpenGLBuffer>

// Pour utiliser les shaders
#include <QtGui/QOpenGLShaderProgram>

class GLArea : public QOpenGLWidget,
               protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLArea(QWidget *parent = 0);
    ~GLArea();

public slots:
    void setRadius(double radius);
    void setCoupe();

signals:  // On ne les implémente pas, elles seront générées par MOC ;
          // les paramètres seront passés aux slots connectés.
    void radiusChanged(double newRadius);

protected slots:
    void onTimeout();

protected:
    void initializeGL() override;
    void doProjection();
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void paintPiston(QMatrix4x4 matrix2, int i);
    void paintMoteur (QMatrix4x4 matrix2);
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void paintCyl(QMatrix4x4 matrix2, GLfloat ep_cyl, GLfloat r_cyl, int nb_fac, GLfloat coul_r, GLfloat coul_v, GLfloat coul_b, bool coupe);
private:
    double m_angle = 0;
    QTimer *m_timer = nullptr;
    double m_anim = 90;
    double m_radius = 2.7;
    double m_ratio = 50;
    double m_alpha = 0;
    double rotate = 0;
    double deplacementZ = -3;
    double deplacementY = -3;
    double deplacementX = -3;
    int X = 0;
    int Y = 1;
    int Z = 2;
    bool coupe = false;
    QMatrix4x4 matrix;
    QMatrix4x4 matrix2;

    void makeGLObjects();
    void tearGLObjects(); // tear = démolir
    QOpenGLBuffer m_vbo;

    int nb_fac = 30;
    // Pour utiliser les shaders
    QOpenGLShaderProgram *m_program;
    int m_posAttr;
    int m_colAttr;
    int m_matrixUniform;
};

#endif // GLAREA_H
