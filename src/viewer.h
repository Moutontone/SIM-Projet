#ifndef VIEWER_H
#define VIEWER_H

// GLEW lib: needs to be included first!!
#include <GL/glew.h> 

// OpenGL library 
#include <GL/gl.h>

// OpenGL Utility library
#include <GL/glu.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QGLFormat>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <stack>

#include "camera.h"
#include "shader.h"
#include "grid.h"
#include "meshLoader.h"

class Viewer : public QGLWidget {
 public:
  Viewer(char *filename,const QGLFormat &format=QGLFormat::defaultFormat());
  ~Viewer();
  
 protected :
  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int width,int height);
  virtual void keyPressEvent(QKeyEvent *ke);
  virtual void mousePressEvent(QMouseEvent *me);
  virtual void mouseMoveEvent(QMouseEvent *me);

 private:
  // OpenGL objects creation
  void createVAO();
  void deleteVAO();

  void createTextures();
   void deleteTextures();
  GLuint _texIds[3];
  GLuint _texIdsBis[3];

  void loadMeshIntoVAO();

  void createShaders();
  void deleteShaders();
  void reloadShaders();

  // drawing functions
  void drawScene(GLuint id);
  void drawThrees(GLuint id);
  void drawAThree(const glm::vec3 &pos);

  QTimer        *_timer;    // timer that controls the animation
  void QtTimerEvt();

  Grid   *_grid;   // the grid
  Camera *_cam;    // the camera

  glm::vec3 _light;  // light direction
  glm::vec3 _motion; // motion offset for the noise texture

  float _y;
  float temps;
  float _speed_y;
  float riverFlow(float t);

  float _camX;
  float _camY;
  float _camZ;
  float _lookAtX;
  float _t;
  glm::mat4 _viewMatrix; // view matrix
  glm::mat4 _projMatrix; // projection matrix 
  

  bool      _mode;   // camera motion or light motion

  // les shaders 
  Shader *_terrainShader;
  Shader *_waterShader;
  Shader *_treeShader;

  // vbo/vao ids 
  GLuint _vaoTerrain;
  GLuint _terrain[2];

  Mesh *_tree;
  GLuint _vaoThrees;
  GLuint _buffers[3];

  unsigned int _ndResol;
};

#endif // VIEWER_H
