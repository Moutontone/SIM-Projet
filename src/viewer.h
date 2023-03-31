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

  void createShaders();
  void deleteShaders();
  void reloadShaders();
  
  // drawing functions 
  void drawScene(GLuint id);

  QTimer        *_timer;    // timer that controls the animation

  Grid   *_grid;   // the grid
  Camera *_cam;    // the camera

  glm::vec3 _light;  // light direction
  glm::vec3 _motion; // motion offset for the noise texture 
  bool      _mode;   // camera motion or light motion

  // les shaders 
  Shader *_terrainShader;
  
  // vbo/vao ids 
  GLuint _vaoTerrain;
  GLuint _terrain[2];

  unsigned int _ndResol;
};

#endif // VIEWER_H
