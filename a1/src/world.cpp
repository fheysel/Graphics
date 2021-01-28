﻿// world.cpp


#include "world.h"
#include "lander.h"
#include "ll.h"
#include "gpuProgram.h"
#include "strokefont.h"

#include <sstream>
#include <iomanip>

//const float textAspect = 0.7;	// text width-to-height ratio (you can use this for more realistic text on the screen)
int x = 0;

void World::updateState(float elapsedTime)

{
    // See if any keys are pressed for thrust

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // right arrow
        lander->rotateCW(elapsedTime);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // left arrow
        lander->rotateCCW(elapsedTime);

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // down arrow
        lander->addThrust(elapsedTime);
    else
        lander->thrust = 0;

    // Update the position and velocity

    lander->updatePose(elapsedTime);

    // See if the lander has touched the terrain

    vec3 closestTerrainPoint = landscape->findClosestPoint(lander->centrePosition());
    float closestDistance = (closestTerrainPoint - lander->centrePosition()).length();

    // Find if the view should be zoomed

    zoomView = (closestDistance < ZOOM_RADIUS);

    // Check for landing or collision and let the user know
    //
    // Landing is successful if the vertical speed is less than 1 m/s and
    // the horizontal speed is less than 0.5 m/s.
    //
    // SHOULD ALSO CHECK THAT LANDING SURFACE IS HORIZONAL, BUT THIS IS
    // NOT REQUIRED IN THE ASSIGNMENT.
    //
    // SHOULD ALSO CHECK THAT THE LANDER IS VERTICAL, BUT THIS IS NOT
    // REQUIRED IN THE ASSIGNMENT.

    // YOUR CODE HERE

    if (closestDistance <= 6) { // Collision
        if (lander->velocity.x < 0.5 && lander->velocity.y < 1) {
            mission_success = true;
            lander->control_lock = TRUE;
        }
        else {
            mission_failure = true;
            lander->control_lock = TRUE;
        }
        lander->stop();
    }
}



void World::draw()

{
  mat4 worldToViewTransform;

  if (!zoomView) {

    // Find the world-to-view transform that transforms the world
    // to the [-1,1]x[-1,1] viewing coordinate system, with the
    // left edge of the landscape at the left edge of the screen, and
    // the bottom of the landscape BOTTOM_SPACE above the bottom edge
    // of the screen (BOTTOM_SPACE is in viewing coordinates).

    float s = 2.0 / (landscape->maxX() - landscape->minX());

    worldToViewTransform
      = translate( -1, -1 + BOTTOM_SPACE, 0 )
      * scale( s, s, 1 )
      * translate( -landscape->minX(), -landscape->minY(), 0 );

    arrowXPosition = vec3(825, 780, 0);
    arrowYPosition = vec3(825, 730, 0);
    arrowScale = 11;

  } else {

    // Find the world-to-view transform that is centred on the lander
    // and is 2*ZOOM_RADIUS wide (in world coordinates).

    // YOUR CODE HERE
    float s = 2.0 / ((lander->centrePosition().x +  ZOOM_RADIUS) - (lander->centrePosition().x - ZOOM_RADIUS));

    worldToViewTransform
        = translate(-1, -1 + BOTTOM_SPACE, 0)
        * scale(s, s, 1)
        * translate(-(lander->centrePosition().x - ZOOM_RADIUS), -(lander->centrePosition().y - ZOOM_RADIUS), 0);

    arrowXPosition = vec3(825/2, 780/2, 0);
    arrowYPosition = vec3(825/2, 730/2, 0);
    arrowScale = 1;
  }

  // Draw the landscape and lander, passing in the worldToViewTransform
  // so that they can append their own transforms before passing the
  // complete transform to the vertex shader.

  landscape->draw( worldToViewTransform );
  lander->draw( worldToViewTransform );

  // Draw the heads-up display (i.e. all text).

  stringstream ss;

  drawStrokeString( "LUNAR LANDER", -0.2, 0.85, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  ss.setf( ios::fixed, ios::floatfield );
  ss.precision(1);

  ss << "FUEL: " << lander->fuel << " L";
  drawStrokeString( ss.str(), -0.95, 0.75, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  // YOUR CODE HERE (modify the above code, too)

  vec3 closestTerrainPoint = landscape->findClosestPoint(lander->centrePosition());
  float closestDistance = (closestTerrainPoint - lander->centrePosition()).length();
  ss.str(std::string()); // Clear stream
  ss << "ALTITUDE:             " << abs(closestDistance - 6) << " m";
  drawStrokeString(ss.str(), 0.18, 0.75, 0.04, glGetUniformLocation(myGPUProgram->id(), "MVP"));

  ss.str(std::string()); // Clear stream
  ss << "HORIZONTAL SPEED     " << abs(lander->velocity.x) << " m/s";
  drawStrokeString(ss.str(), 0.18, 0.65, 0.04, glGetUniformLocation(myGPUProgram->id(), "MVP"));

  ss.str(std::string()); // Clear stream
  ss << "VERTICAL SPEED       " << abs(lander->velocity.y) << " m/s";
  drawStrokeString(ss.str(), 0.18, 0.55, 0.04, glGetUniformLocation(myGPUProgram->id(), "MVP"));

  // Draw velocity arrows in HUD
  glBindVertexArray(arrowVAO);
  mat4 MVP_new;
  
  // Horizontal arrow
  if (lander->velocity.x != 0) {
      if (lander->velocity.x > 0) {
          arrowOrientation = 3 * PI / 2;
      }
      else {
          arrowOrientation = PI / 2;
      }
      MVP_new = worldToViewTransform * translate(arrowXPosition) 
                                     * scale(arrowScale, arrowScale, 0) 
                                     * rotate(arrowOrientation, vec3(0, 0, 1));
      glUniformMatrix4fv(glGetUniformLocation(myGPUProgram->id(), "MVP"), 1, GL_TRUE, &MVP_new[0][0]);
      glDrawArrays(GL_LINES, 0, numArrowVerts * 2);
  }
  

  // Vertical arrow
  if (lander->velocity.y != 0) {
      if (lander->velocity.y > 0) {
          arrowOrientation = 0;
      }
      else {
          arrowOrientation = PI;
      }
      MVP_new = worldToViewTransform * translate(arrowYPosition) 
                                     * scale(arrowScale, arrowScale, 0) 
                                     * rotate(arrowOrientation, vec3(0, 0, 1));
      glUniformMatrix4fv(glGetUniformLocation(myGPUProgram->id(), "MVP"), 1, GL_TRUE, &MVP_new[0][0]);
      glDrawArrays(GL_LINES, 0, numArrowVerts * 2);
  }

  // Display success or failure message on landing
  if (mission_success && !mission_failure) {
      ss.str(std::string()); // Clear stream
      ss << "SUCCESS!";
      drawStrokeString(ss.str(), -0.3, 0.3, 0.15, glGetUniformLocation(myGPUProgram->id(), "MVP"));
  }

  if (mission_failure) {
      ss.str(std::string()); // Clear stream
      ss << "FAILURE!";
      drawStrokeString(ss.str(), -0.3, 0.3, 0.15, glGetUniformLocation(myGPUProgram->id(), "MVP"));
  }
}

// Create an arrow VAO
//
// This needs 'numArrowVerts' and 'arrowVAO' defined in world.h

void World::setupArrowVAO()

{
    // create an arrow from line segments

    GLfloat arrowVerts[] = {
      0,    1,    0,    -1,
      0,    1,    0.5, 0.25,
      0,    1,   -0.5, 0.25,
      0.5, 0.25, -0.5, 0.25
    };

    numArrowVerts = sizeof(arrowVerts) / sizeof(GLfloat);

    // ---- Create a VAO for this object ----

    glGenVertexArrays(1, &arrowVAO);
    glBindVertexArray(arrowVAO);

    // Store the vertices

    GLuint VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numArrowVerts * sizeof(GLfloat), arrowVerts, GL_STATIC_DRAW);

    // define the position attribute

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Done

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}