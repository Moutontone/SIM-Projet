#include "viewer.h"
#include "meshLoader.h"

#include <math.h>
#include <iostream>
#include <QTime>

using namespace std;

Viewer::Viewer(char *,const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _light(glm::vec3(0,0,1)),
    _motion(glm::vec3(0,0,0)),
    _y(.0),
    temps(.0),
    _speed_y(.010),
    _camX(0),
    _camY(-1.001),
    _camZ(.1),
    _lookAtX(0),
    _t(.0),
    _mode(false),
    _ndResol(512) {

  setlocale(LC_ALL,"C");

  _tree = new Mesh("models/cloud.off");

  _grid = new Grid(_ndResol,-1.0f,1.0f);
  _cam  = new Camera(1.0f,glm::vec3(0.0f,0.0f,0.0f));

  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

void Viewer::QtTimerEvt(){
    updateGL();
    _t += 10.;
}
Viewer::~Viewer() {
  delete _timer;
  delete _grid;
  delete _cam;
  delete _tree;

  // delete all GPU objects
  deleteShaders();
  deleteTextures();
  deleteVAO();
}

void Viewer::createVAO() {
    // buffers des arbres
    glGenVertexArrays(1, &_vaoThrees);
    glGenBuffers(3, _buffers);

  // cree les buffers associés au terrain

  glGenBuffers(2,_terrain);
  glGenVertexArrays(1,&_vaoTerrain);

  // create the VBO associated with the grid (the terrain)
  glBindVertexArray(_vaoTerrain);
  glBindBuffer(GL_ARRAY_BUFFER,_terrain[0]); // vertices
  glBufferData(GL_ARRAY_BUFFER,_grid->nbVertices()*3*sizeof(float),_grid->vertices(),GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_terrain[1]); // indices 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_grid->nbFaces()*3*sizeof(int),_grid->faces(),GL_STATIC_DRAW);

}

void Viewer::deleteVAO() {
    glDeleteBuffers(3, _buffers);
    glDeleteVertexArrays(1, &_vaoThrees);
  glDeleteBuffers(2,_terrain);
  glDeleteVertexArrays(1,&_vaoTerrain);
}

void Viewer::loadMeshIntoVAO() { // Into GPU
    glBindVertexArray(_vaoThrees);

    // Store mesh positions into buffer 0
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, _tree->nb_vertices*3*sizeof(float), _tree->vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
    glEnableVertexAttribArray(0);

    // Store mesh indices into buffer 1
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, _tree->nb_vertices*3*sizeof(float), _tree->normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_TRUE,0,(void *)0);
    glEnableVertexAttribArray(1);

    // Store mesh normals into buffer 2
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _tree->nb_faces*3*sizeof(float), _tree->faces, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Viewer::createShaders() {
  _terrainShader = new Shader();
  _waterShader = new Shader();
  _treeShader = new Shader();

  _terrainShader->load("shaders/terrain.vert","shaders/terrain.frag");
  _waterShader->load("shaders/water.vert","shaders/water.frag");
  _treeShader->load("shaders/cloud.vert", "shaders/cloud.frag");
}

void Viewer::deleteShaders() {
  delete _terrainShader;
  delete _waterShader;
  delete _treeShader;

  _terrainShader = NULL;
  _waterShader = NULL;
  _treeShader = NULL;
}

void Viewer::createTextures(){
    QImage image;
    // enable the use of 2D textures
    glEnable(GL_TEXTURE_2D);
    // create one texture on the GPU
    glGenTextures(2, _texIds);
    // load an image (CPU side)
    image = QGLWidget::convertToGLFormat(QImage("textures/grass_grass_0124_01_tiled.jpg"));
     // activate this texture (the current one)
     glBindTexture(GL_TEXTURE_2D,_texIds[0]);
     // set texture parameters
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
     // transfer data from CPU to GPU memory
     glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,image.width(),image.height(),
                  0, GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image.bits());
     // generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Viewer::deleteTextures() {
    glDeleteTextures(1,_texIds);
}

void Viewer::reloadShaders() {
  if(_terrainShader)
    _terrainShader->reload("shaders/terrain.vert","shaders/terrain.frag");
  if (_waterShader)
      _waterShader->reload("shaders/water.vert","shaders/water.frag");
  if (_treeShader)
    _treeShader->reload("shaders/cloud.vert", "shaders/cloud.frag");
}

void Viewer::drawAThree(const glm::vec3 &pos) {

    const int id = _treeShader->id();
    // send uniform variables
    glm::mat4 mdv = glm::scale(_cam->mdvMatrix(), glm::vec3(0.0005));
    mdv = glm::rotate(mdv, (float) 90, glm::vec3(0,1,0));
    mdv = glm::rotate(mdv, (float) 15, glm::vec3(0,0,1));
    mdv = glm::translate(mdv,pos);

    glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(mdv[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_projMatrix[0][0]));
    glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));

    glUniform3fv(glGetUniformLocation(id,"light"),1,&(_light[0]));
    glUniform3fv(glGetUniformLocation(id,"motion"),1,&(_motion[0]));
    glUniform1f(glGetUniformLocation(id,"_y"),_y);

    glDrawElements(GL_TRIANGLES, 3*_tree->nb_faces, GL_UNSIGNED_INT, (void *) 0);

}

void Viewer::drawThrees(GLuint id) {
    glBindVertexArray(_vaoThrees);

    // We draw some threes
    const float r = _tree->radius*2.5;

//    int nuages = 10;
//    for (int i=-nuages; i<nuages; i++){
//        drawAThree(glm::vec3(r*10,r*1.5,i*r));
//    }

    drawAThree(glm::vec3(r*1.5,r*1.5,r*1.5));
    drawAThree(glm::vec3(r,r*1.35,r*2.6));
    drawAThree(glm::vec3(r*2,r*0.8,r*-0.5));
    drawAThree(glm::vec3(r*5,r*1,r*0.3));
    drawAThree(glm::vec3(r*15,r*0.6,r*5.4));
    drawAThree(glm::vec3(r*15,r*-0.4,r*-9.4));

    drawAThree(glm::vec3(-r*2,r*1.55,r*-1.4));
    drawAThree(glm::vec3(-r*1.3,r*1.30,r*-1));
    drawAThree(glm::vec3(-r*1.7,r*1.2,r*1.8));

    glBindVertexArray(0);
}

void Viewer::drawScene(GLuint id) {
  // send uniform variables

  glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(_viewMatrix[0][0]));
  //glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));
  glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_projMatrix[0][0]));
  glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));
  glUniform3fv(glGetUniformLocation(id,"light"),1,&(_light[0]));
  glUniform3fv(glGetUniformLocation(id,"motion"),1,&(_motion[0]));
  if (id == _terrainShader->id()) {
      glUniform1f(glGetUniformLocation(id,"_y"),_y);
  } else if (id == _waterShader->id()) {
      glUniform1f(glGetUniformLocation(id,"_y"),_y);
      glUniform1f(glGetUniformLocation(id,"_t"),_t);
  }
    // send textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,_texIds[0]);
    glUniform1i(glGetUniformLocation(id,"grassmap"),0);
  // draw faces
    glBindVertexArray(_vaoTerrain);
    glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);
    glBindVertexArray(0);
}

void Viewer::paintGL() {

    _t += .001;
  // allow opengl depth test 
  glEnable(GL_DEPTH_TEST);

//
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // screen viewport
  glViewport(0,0,width(),height());

  // clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // move camera
  //glm::vec3 camPos(r,-2, 1);
    //glm::vec3 center(r/2,.5,0);
    float r = riverFlow(_y + -1);
    glm::vec3 camPos(r,_camY, _camZ);
    float lookahead = .5;
  glm::vec3 center(_lookAtX + riverFlow(_y -1 +lookahead),0,0);
//  glm::vec3 center(_lookAtX + r,0,0);
  glm::vec3 up(0, 0, 1);
  _viewMatrix = glm::lookAt(camPos, center, up);
//	float fovy = 20.0;
	float fovy = 45.0;
	float aspect = (float)width()/(float)height();
	float near = 0.1;
	float far = 500.0;
	_projMatrix = glm::perspective(fovy, aspect, near, far);

    // trees
    glUseProgram(_treeShader->id());
    drawThrees(_treeShader->id());
    // terrain
    glUseProgram(_terrainShader->id());
    drawScene(_terrainShader->id());
    // water
    glUseProgram(_waterShader->id());
    drawScene(_waterShader->id());


//  drawScene(_waterShader->id());

    // disable depth test
  glDisable(GL_DEPTH_TEST);

  // disable shader 
  glUseProgram(0);
}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  updateGL();
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    _mode = false;
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  } 

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));
 
  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
  }

  updateGL();
}

void Viewer::keyPressEvent(QKeyEvent *ke) {
  const float step = 0.05;
  if(ke->key()==Qt::Key_Space) {
      _y += _speed_y;
  }
  if(ke->key()==Qt::Key_Z) {
    glm::vec2 v = glm::vec2(glm::transpose(_cam->normalMatrix())*glm::vec3(0,0,-1))*step;
    if(v[0]!=0.0 && v[1]!=0.0) v = glm::normalize(v)*step;
    else v = glm::vec2(0,1)*step;
    _motion[0] += v[0];
    _motion[1] += v[1];
  }

  if(ke->key()==Qt::Key_S) {
    glm::vec2 v = glm::vec2(glm::transpose(_cam->normalMatrix())*glm::vec3(0,0,-1))*step;
    if(v[0]!=0.0 && v[1]!=0.0) v = glm::normalize(v)*step;
    else v = glm::vec2(0,1)*step;
    _motion[0] -= v[0];
    _motion[1] -= v[1];
  }

  if(ke->key()==Qt::Key_Q) {
    _motion[2] += step;
  }

  if(ke->key()==Qt::Key_D) {
    _motion[2] -= step;
  }

  //camera motion
    if(ke->key()==Qt::Key_Z) {
        _camZ += .1;
    }
    if(ke->key()==Qt::Key_S) {
        _camZ -= .1;
    }
    if(ke->key()==Qt::Key_A) {
        _camY -= .1;
    }
    if(ke->key()==Qt::Key_E) {
        _camY += .1;
    }
    if(ke->key()==Qt::Key_Q) {
        _lookAtX -= .1;
    }
    if(ke->key()==Qt::Key_D) {
        _lookAtX += .1;
    }
    if(ke->key()==Qt::Key_I) {
        //info
        printf("Info :\n");

        float r = riverFlow(_y + _camY);
        printf("\tCamera (r,_CamY, _CamZ) = (%f,%f,%f)\n",r ,_camY, _camZ);
        printf("\tCamera lookAtX = %f\n",_lookAtX);
        printf("\t_y = %f and riverflow(_y) = %f \n",_y, riverFlow(_y));
        printf("\ttime _t : %f \n", _t);

    }




  // key a: play/stop animation
  if(ke->key()==Qt::Key_A) {
    if(_timer->isActive()) 
      _timer->stop();
    else 
      _timer->start();
  }

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }
  
  // // key f: compute FPS
  // if(ke->key()==Qt::Key_F) {
  //   int elapsed;
  //   QTime timer;
  //   timer.start();
  //   unsigned int nb = 500;
  //   for(unsigned int i=0;i<nb;++i) {
  //     paintGL();
  //   }
  //   elapsed = timer.elapsed();
  //   double t = (double)nb/((double)elapsed);
  //   cout << "FPS : " << t*1000.0 << endl;
  // }

  // key r: reload shaders 
  if(ke->key()==Qt::Key_R) {
    reloadShaders();
  }

  updateGL();
}

void Viewer::initializeGL() {
  // make this window the current one
  makeCurrent();

  // init and chack glew
  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }

  // init OpenGL settings
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());

  // initialize camera
  _cam->initialize(width(),height(),true);

  // init shaders 
  createShaders();

  // init VAO/VBO
  createVAO();
  loadMeshIntoVAO();
  createTextures();

  // starts the timer 
  _timer->start();
}

float Viewer::riverFlow(float t){
    //return .5*sin(t*3);
    float l = .2;
    return .5*sin(t*3*l) + .2*sin(t*8*l) + 2*sin(t*0.2*l);
}
