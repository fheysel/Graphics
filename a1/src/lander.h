// lander.h
//
// Unit of length is the meter


#ifndef LANDER_H
#define LANDER_H


#include "headers.h"


class Lander {

  static float landerVerts[];	// lander segments as vertex pairs
  int numSegments;		// number of line segments in the lander model
  
  GLuint VAO;			// VAO for lander geometry
  GLuint VBO;			// VBO for vertex buffer, in case it needs to be updated 

  vec3 position;		// position in world coordinates (m)

  float orientation;		// orientation (radians CCW)
  float angularVelocity;	// angular velocity (radians/second CCW)

  float worldMaxX, worldMaxY;	// world dimensions

 public:

  float width, height;		// lander dimensions in world coordinates
  vec3 velocity;		// velocity in world coordinates (m/s)
  vec3 velocityBeforeLanding; // velocity just before landing saved for display
  float fuel;
  bool control_lock;
  float thrust;

  Lander( float maxX, float maxY ) {
    worldMaxX = maxX;
    worldMaxY = maxY;
    reset();
    setupVAO();
    fuel = 5000;
    control_lock = FALSE;
    thrust = 0;
  };

  void setupVAO();  

  void draw( mat4 &worldToViewTransform );

  void updatePose( float deltaT );

  void reset() {

#if 1  // production				
    position = vec3( 0.05 * worldMaxX, 0.7 * worldMaxY, 0.0  );
    velocity = vec3( 30.0f, 0.0f, 0.0f );
    control_lock = false;
#else  // debug
    position = vec3( 762, 48, 0 );
    velocity = vec3( 0, -2, 0 );
#endif

    orientation = 0;
    angularVelocity = 0;
  }

  void rotateCW( float deltaT );
  void rotateCCW( float deltaT );
  void addThrust( float deltaT );

  vec3 centrePosition() { return position; }

  float speed() { return velocity.length(); }

  void stop() {
    angularVelocity = 0;
    velocity = vec3(0,0,0);
  }
};


#endif
