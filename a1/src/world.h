// world.h


#ifndef WORLD_H
#define WORLD_H


#include "headers.h"
#include "landscape.h"
#include "lander.h"
#include "ll.h"


#define BOTTOM_SPACE 0.1f // amount of blank space below terrain (in viewing coordinates) 


class World {

  Landscape *landscape;
  Lander    *lander;
  bool       zoomView; // show zoomed view when lander is close to landscape
  GLFWwindow *window;

  char *showMsg;
  int  score;
  bool mission_success;
  bool mission_failure;

  float PI;
  GLuint arrowVAO; // VAO for velocity arrows
  int numArrowVerts;
  float arrowScale;
  float arrowOrientation;
  vec3 arrowXPosition;
  vec3 arrowYPosition;
  

 public:

  World( GLFWwindow *w ) {
    landscape = new Landscape();
    lander    = new Lander( maxX(), maxY() ); // provide world size to help position lander
    zoomView  = false;
    window    = w;
    showMsg   = NULL;
    score     = 0;
    mission_success = false;
    mission_failure = false;
    setupArrowVAO();
    arrowXPosition = vec3(0.67, 0.66, 0);
    arrowYPosition = vec3(0.6, 0.56, 0);
    arrowScale = 0.02;
    PI = 3.14159;
  }

  void draw();

  void setupArrowVAO();

  void updateState( float elapsedTime );

  void resetLander() {
    lander->reset();
  }

  // World extremes (in world coordinates)

  float minX() { return 0; }
  float maxX() { return landscape->maxX(); }

  float minY() { return 0; }
  float maxY() { return (landscape->maxX() - landscape->minX()) / screenAspect * (2 - BOTTOM_SPACE) / 2; }
};

#endif
