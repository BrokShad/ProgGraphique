// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#ifndef GLAREA_H
#define GLAREA_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QKeyEvent>
#include <QTimer>

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
    void paintPiston(QMatrix4x4 matrix2, float taille);
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void paintCyl(QMatrix4x4 matrix2, GLfloat ep_cyl, GLfloat r_cyl, int nb_fac, GLfloat coul_r, GLfloat coul_v, GLfloat coul_b);

private:
    double m_angle = 90;
    QTimer *m_timer = nullptr;
    double m_anim = 90;
    double m_radius = 0.05;
    double m_ratio = 50;
    double m_alpha = 0;
    int X = 0;
    int Y = 1;
    int Z = 2;
    QMatrix4x4 matrix;
    QMatrix4x4 matrix2;

    // Pour utiliser les shaders
    QOpenGLShaderProgram *m_program;
    int m_posAttr;
    int m_colAttr;
    int m_matrixUniform;
};

#endif // GLAREA_H
