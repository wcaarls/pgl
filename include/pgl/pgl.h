/** \file pgl.h
 *
 * PGL, a primitive OpenGL 3D primitive library.
 *
 * Main headerfile.
 *
 * (c) 2020, Wouter Caarls.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PGL_PGL_H_
#define PGL_PGL_H_

#include "math.h"

#include <GL/gl.h>

#include <vector>
#include <iostream>

// GLFW-compatible defines
#define PGL_RELEASE 0
#define PGL_PRESS   1
#define PGL_REPEAT  2

#define PGL_MOUSE_BUTTON_LEFT   0
#define PGL_MOUSE_BUTTON_RIGHT  1
#define PGL_MOUSE_BUTTON_MIDDLE 2

namespace pgl {

/**
 * \mainpage
 *
 * PGL, a primitive OpenGL 3D primitive library
 *
 * https://github.com/wcaarls/pgl
 *
 * \image html example.png "Example"
 * 
 * The library consists of the following main classes:
 *
 * * Object, the basic node structure of the scene graph.
 *   + Primitive, something that can be drawn.
 *     - Box, a box.
 *     - WireBox, a wireframe box.
 *     - Sphere, a sphere.
 *     - Cylinder, a generalized cylinder.
 *     - Cone, a cone,
 *     - Arrow, an arrow
 *     - Capsule, a cylinder with rounded encaps.
 *   + Scene, a collection of objects and information on how to draw them.
 * * Controller, which adjusts the camera viewpoint of an associated Scene.
 *
 * There are also utility Vector3 and matrix functions to specify points and
 * coordinate Transform%ations such as Rotation%s and Translation%s. They
 * mostly behave like you would expect them to.
 *
 * In general, the classes have public members that can be accessed directly,
 * such as a Primitive::transform or Scene::fovy. 
 *
 * The code to generate the example above is essentially
 * \code
 *   // Initialize our scene
 *   scene = new pgl::Scene();
 *   scene->attach(new pgl::Box({2, 2, 0.05}, {0, 0, -1.025}));
 *   scene->attach(new pgl::WireBox({2, 2, 2}));
 *   scene->attach(new pgl::Sphere(0.05));
 *   scene->attach(new pgl::Arrow({-1, -1, -1}, { 0, -1, -1}, 0.02))->color = {1, 0, 0};
 *   scene->attach(new pgl::Arrow({-1, -1, -1}, {-1,  0, -1}, 0.02))->color = {0, 1, 0};
 *   scene->attach(new pgl::Arrow({-1, -1, -1}, {-1, -1, 0}, 0.02))->color = {0, 0, 1};
 *   
 *   // Add a custom object
 *   scene->attach(object = new pgl::Object());
 *   object->attach(new pgl::Capsule({-0.3, 0, 0}, {0.3, 0, 0}, 0.02))->color = {1, 0, 0};
 *   object->attach(new pgl::Capsule({0, -0.3, 0}, {0, 0.3, 0}, 0.02))->color = {0, 1, 0};
 *
 *   // Initialize orbit controller
 *   controller = new pgl::OrbitController(scene);
 *   controller->view(0.5, 0.4, 4);
 * \endcode
 *
 * To enable mouse interaction, the Controller's callback functions must be
 * appropriately hooked into the window system. They are compatible with GLFW
 * constants.
 */

/**
 * \brief Object in a scene.
 *
 * Basically a transform specifying the position/orientation
 * and a list of sub-objects to draw relative to the current
 * one. Does not actually draw anything.
 */
class Object
{
  public:
    Transform transform;           ///< Position and orientation.
    std::vector<Object*> children; ///< Sub-objects to draw relative to this.
    
  public:
    /**
     * \brief Default constructor.
     *
     * Sets initial transform to the identity matrix.
     */
    Object() : transform({0, 0, 0}, {0, 0, 0}) { }

    virtual ~Object()
    {
      for (size_t ii=0; ii != children.size(); ++ii)
        delete children[ii];
      children.clear();
    }

    /// Draw sub-objects relative to this one.  
    virtual void draw()
    {
      glPushMatrix();
      glMultMatrixd(transform.data);
      for (size_t ii=0; ii != children.size(); ++ii)
        children[ii]->draw();
      glPopMatrix();
    }
    
    /** \brief Add child to list of sub-objects.
     *
     * \returns attached child. This allows code like
     * \code
     * object->attach(new SubObject())->color = {1, 0, 0};
     * \endcode
     *
     * \note
     * Transfers ownership.
     */
    template<class T>
    T* attach(T *child)
    {
      children.push_back(child);
      return child;
    }
};

/**
 * \brief Defines camera position and frustum.
 *
 * A scene is a special object that defines the initial
 * projection and modelview matrices. It does not make
 * sense to attach it to another object.
 */
class Scene : public Object
{
  public:
    Vector3 background; ///< Background color.
    double fovy;        ///< Vertical field of view.
  
  public:
    /**
     * \brief Default constructor.
     *
     * Sets background color to black and field of view such that
     * an object of size X fills the vertical field at distance X.
     */
    Scene() : background{0, 0, 0}, fovy(0.92) { }
  
    virtual void draw()
    {
      glClearColor(background.x, background.y, background.z, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      int dims[4];
      glGetIntegerv(GL_VIEWPORT, dims);
      double aspect = dims[2]/(double)dims[3];
      
      double f = 1/tan(fovy/2);
      double near = 1, far = 100;
      
      double matrix[] = {f/aspect, 0., 0., 0.,
                         0., f, 0., 0.,
                         0., 0., (far+near)/(near-far), -1.,
                         0., 0., 2*far*near/(near-far), 0.};
      
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixd(matrix);
      
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixd(transform.data);
      
      for (size_t ii=0; ii != children.size(); ++ii)
        children[ii]->draw();
    }
};

/**
 * \brief Camera controller.
 *
 * Allows user interaction to move a Scene's camera.
 */
class Controller
{
  public:
    Scene *scene; ///< Scene to move camera of.
    
  public:
    Controller(Scene *_scene) : scene(_scene) { }
    virtual ~Controller() { }
  
    /// Click handler.
    virtual void click(int button, int action, int mods, double xpos, double ypos) = 0;
    
    /// Scroll wheel handler.
    virtual void scroll(double xoffset, double yoffset) = 0;
    
    /// Mouse motion handler.
    virtual void motion(double xpos, double ypos) = 0;
};

}

#include "primitive.h"
#include "controller.h"

#endif // PGL_PGL_H_
