#ifndef HJGWIDGET_H
#define HJGWIDGET_H

#include "OpenGLHeader.h"
#include <QOpenGLWidget>
#include "Scene.h"
#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <string>
class HJGWidget : public QOpenGLWidget, protected QtOpenGLBaseClass
{
  Q_OBJECT
public:
  HJGWidget()=default;
  explicit HJGWidget(QWidget* parent=nullptr);

  void switchScene(int index);

  void addScene(HJGraphics::Scene& scene);

  void setFPS(int f){fps=f;}

  int getFPS(){return fps;}
protected:
  std::vector<HJGraphics::Scene *> scenes;
  HJGraphics::Scene *currentScene;

  float fov;
  bool firstMouse;
  bool mouseDown;
  float mouseSensitivity;
  float lastX;
  float lastY;
  float yaw;
  float pitch;
  float moveSpeed;
  glm::vec3 originalDirection;

  int fps;

  //Inherited Funtions from QOpenGLWidget
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void wheelEvent(QWheelEvent* event) override;
};

#endif // HJGWIDGET_H
