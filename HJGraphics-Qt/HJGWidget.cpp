#include "HJGWidget.h"

using namespace HJGraphics;
using namespace std;
using namespace glm;
HJGWidget::HJGWidget(QWidget *parent):QOpenGLWidget(parent){
  fov = 45.0f;
  firstMouse = true;
  mouseDown = false;
  mouseSensitivity = 0.2;
  lastX = static_cast<float>(width()) / 2;
  lastY = static_cast<float>(height()) / 2;
  yaw = 0.0f;
  pitch = 0.0f;
  moveSpeed=0.1;
  fps=60;
}
void HJGWidget::initializeGL(){
  initializeOpenGLFunctions();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_CULL_FACE);
  glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);

  qDebug()<<"default fbo: "<<this->defaultFramebufferObject();
  Coordinate* coord=new Coordinate;
  Skybox* skybox=new Skybox(15,string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_rt.tga"),
                            string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_lf.tga"),
                            string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_up.tga"),
                            string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_dn.tga"),
                            string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_bk.tga"),
                            string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_ft.tga"));

  Cylinder* cylinder=new Cylinder(0.25,3,100);
  cylinder->model=translate(cylinder->model,vec3(0.0f,0.0f,1.0f));
  cylinder->model=rotate(cylinder->model,radians(90.0f),vec3(1.0f,0.0f,0.0f));

  Box* box=new Box(2,2,2);
  box->model=translate(box->model,vec3(0.0f,0.0f,-2.5f));

  Plane* plane=new Plane(8,8,"/Users/hezhenbang/Documents/HJGraphics/texture/chessboard.jpg");

  //-----------------------Direction------------------------Position-----------------------Color
  SpotLight* spotLight=new SpotLight(glm::vec3(1.0f,-1.0f,-1.0f),glm::vec3(-5.0f,5.0f,3.0f),glm::vec3(0.0f,0.6f,0.6f));
  PointLight* pointLight=new PointLight(glm::vec3(0.0f,4.0f,1.0f));

  glm::vec3 cameraPos=glm::vec3(5.0f,5.0f,10.0f);
  glm::vec3 cameraDirection=glm::vec3(0.0f, 0.0f, 0.0f)-cameraPos;
  Camera* camera=new Camera(cameraPos,cameraDirection);

  Scene* scene=new Scene(this->width()*2,this->height()*2,0.2f,glm::vec3(0.0f,0.0f,1.0f));
  scene->addCamera(*camera);
  scene->addObject(*coord);
  scene->addObject(*cylinder);
  scene->addObject(*plane);
  scene->addObject(*box);
  scene->addObject(*skybox);
  scene->addLight(*pointLight);
  addScene(*scene);
}
void HJGWidget::resizeGL(int w, int h){
  glViewport(0,0,w,h);
}
void HJGWidget::paintGL(){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0, 0.5, 0.5, 1);
  if(currentScene==nullptr)return;
  currentScene->writeSharedUBOData();
  currentScene->setDefaultFramebuffer(this->defaultFramebufferObject());
  currentScene->draw();
}
void HJGWidget::mousePressEvent(QMouseEvent *event){
  this->setFocus();
  mouseDown = true;
  originalDirection=currentScene->getMainCamera()->direction;
  update();
}

void HJGWidget::mouseReleaseEvent(QMouseEvent *event){
  mouseDown = false;
  firstMouse = true;
  yaw=0;
  pitch=0;
  update();
}

void HJGWidget::mouseMoveEvent(QMouseEvent *event){
  double xpos=event->x();
  double ypos=event->y();
  if (!mouseDown)return;
  if (firstMouse) {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;
  xoffset *= mouseSensitivity;
  yoffset *= mouseSensitivity;

  yaw += xoffset;
  pitch += yoffset;

  glm::mat4 yawMat(1.0f);
  glm::mat4 pitchMat(1.0f);
  glm::vec3 cameraRight=glm::normalize(glm::cross(originalDirection,glm::vec3(0.0f,1.0f,0.0f)));
  glm::vec3 cameraUp=glm::normalize(glm::cross(cameraRight,originalDirection));
  yawMat=glm::rotate(yawMat,glm::radians(yaw),-cameraUp);
  pitchMat=glm::rotate(pitchMat,glm::radians(pitch),cameraRight);
  glm::vec4 newDir=yawMat*glm::vec4(originalDirection,0.0f);
  newDir=pitchMat*newDir;
  currentScene->getMainCamera()->direction=newDir;
  update();
}
void HJGWidget::wheelEvent(QWheelEvent *event){
  if (fov >= 1 && fov <= 60)fov -= event->delta()*0.01;
  if (fov <= 1)fov = 1.0f;
  else if (fov >= 60)fov = 60.0f;
  currentScene->getMainCamera()->fov=fov;
  update();
}

void HJGWidget::keyPressEvent(QKeyEvent *event){
  if(event->key()==Qt::Key_A){
      //left
      glm::vec3 cameraRight=glm::normalize(glm::cross(currentScene->getMainCamera()->direction,glm::vec3(0.0f,1.0f,0.0f)));
      currentScene->getMainCamera()->position-=cameraRight*moveSpeed;
    }
  if(event->key()==Qt::Key_D){
      //right
      glm::vec3 cameraRight=glm::normalize(glm::cross(currentScene->getMainCamera()->direction,glm::vec3(0.0f,1.0f,0.0f)));
      currentScene->getMainCamera()->position+=cameraRight*moveSpeed;
    }
  if(event->key()==Qt::Key_W){
      //front
      glm::vec3 cameraRight=glm::normalize(glm::cross(currentScene->getMainCamera()->direction,glm::vec3(0.0f,1.0f,0.0f)));
      glm::vec3 cameraFront=glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f),cameraRight));
      currentScene->getMainCamera()->position+=cameraFront*moveSpeed;
    }
  if(event->key()==Qt::Key_S){
      //back
      glm::vec3 cameraRight=glm::normalize(glm::cross(currentScene->getMainCamera()->direction,glm::vec3(0.0f,1.0f,0.0f)));
      glm::vec3 cameraFront=glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f),cameraRight));
      currentScene->getMainCamera()->position-=cameraFront*moveSpeed;
    }
  if(event->key()==Qt::Key_Q){
      //up
      currentScene->getMainCamera()->position+=glm::vec3(0,1,0)*moveSpeed;
    }
  if(event->key()==Qt::Key_E){
      //down
      currentScene->getMainCamera()->position+=glm::vec3(0,-1,0)*moveSpeed;
    }
  update();
}
void HJGWidget::switchScene(int index) {
  if(index<0){
      std::cout<<"WARNING @ Window::switchScene(int) : index is less than 0"<<std::endl;
      return;
    }
  if(index>=scenes.size()){
      std::cout<<"WARNING @ Window::switchScene(int) : index is more than scenes size"<<std::endl;
      return;
    }
  currentScene=scenes.at(index);
}
void HJGWidget::addScene(Scene &_scene) {
  scenes.push_back(&_scene);
  if(scenes.size() == 1)currentScene=&_scene;
}
