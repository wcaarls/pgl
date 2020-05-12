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

#include <memory>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>

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
 * \image html example.jpg "Example"
 * 
 * The library consists of the following main classes:
 *
 * * Node, the basic node structure of the scene graph.
 *   * Object, something that can be placed.
 *     * Primitive, something that can be drawn.
 *       - Box, a box.
 *       - WireBox, a wireframe box.
 *       - Sphere, a sphere.
 *       - Cylinder, a generalized cylinder.
 *       - Cone, a cone,
 *       - Arrow, an arrow
 *       - Capsule, a cylinder with rounded encaps.
 *       - Plane, a (possibly textured) plane.
 *       - Model, an STL model.
 *   * Scene, the root node of the scene graph.
 * * Camera, which defines the viewpoint for drawing a Scene.
 * * Controller, which adjusts the viewpoint of an associated Camera.
 *
 * There are also utility Vector3 and matrix functions to specify points and
 * coordinate Transform%ations such as Rotation%s and Translation%s. They
 * mostly behave like you would expect them to.
 *
 * In general, the classes have public members that can be accessed directly,
 * such as a Primitive::transform or Camera::fovy. 
 *
 * The code to generate the example above is essentially
 * \code
 * // Initialize our scene
 * scene = new pgl::Scene();
 * scene->attach(new pgl::Box({2, 2, 0.05}, {0, 0, -1}));
 * scene->attach(new pgl::WireBox({2, 2, 2}));
 * scene->attach(new pgl::Sphere(0.05));
 * scene->attach(new pgl::Arrow({-1, -1, -1}, { 0, -1, -1}, 0.02))->color = {1, 0, 0};
 * scene->attach(new pgl::Arrow({-1, -1, -1}, {-1,  0, -1}, 0.02))->color = {0, 1, 0};
 * scene->attach(new pgl::Arrow({-1, -1, -1}, {-1, -1, 0}, 0.02))->color = {0, 0, 1};

 * // Add a custom object that we'll animate
 * scene->attach(object = new pgl::Object());
 * object->attach(new pgl::Capsule({-0.3, 0, 0}, {0.3, 0, 0}, 0.02))->color = {1, 0, 0};
 * object->attach(new pgl::Capsule({0, -0.3, 0}, {0, 0.3, 0}, 0.02))->color = {0, 1, 0};
 *
 * // Add STL model
 * scene->attach(new pgl::Model("teapot.stl", {0, 0, -1}, 0.1))->color = {1, 1, 0};
 *
 * // Add some planes
 * scene->attach(new pgl::Plane({-100, 0, 0}, {0, 100, 0}, {0, 0, 10}))->color = {0.5, 0.5, 1};
 * scene->attach(new pgl::Plane({1, 0, 0}, {0, 1, 0}, {0, 0, -1}, pgl::Texture("ceramic-tiles.ppm"), 9));
 * scene->attach(new pgl::Plane({1, 0, 0}, {0, 0, 1}, {0, 1, 0}, pgl::Checkerboard4x4()))->color = {0, 1, 1};
 *
 * // Initialize camera
 * camera = new pgl::Camera(scene);
 *
 * // Initialize orbit controller
 * controller = new pgl::OrbitController(camera__);
 * controller->view(0.5, 0.4, 4);
 * \endcode
 *
 * The Camera's \link Camera::draw() draw \endlink function would then be called
 * in the window's refresh callback.
 * To enable mouse interaction, the Controller's callback functions must be
 * appropriately hooked into the window system. They are compatible with GLFW
 * constants.
 *
 * Note that it is not necessary to use the scene graph. You can draw the
 * primitives in your own code by directly calling their \link Primitive::draw()
 * draw \endlink function.
 */

/**
 * \brief Node in the scene graph.
 *
 * Mainly a list of sub-objects to draw. Note that the sub-objects
 * are owned by the node and deleted when the node itself is
 * deleted.
 */
class Node
{
  public:
    std::vector<Node*> children; ///< Sub-objects.
    
  public:
    virtual ~Node()
    {
      for (size_t ii=0; ii != children.size(); ++ii)
        delete children[ii];
      children.clear();
    }

    /// Draw children.
    virtual void draw()
    {
      for (size_t ii=0; ii != children.size(); ++ii)
        children[ii]->draw();
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
 * \brief Object in a scene.
 *
 * Basically a transform specifying the position/orientation of the object.
 * Its children will be drawn relative to that transform. Derived classes
 * must implement the actual drawing.
 */
class Object : public Node
{
  public:
    Transform transform; ///< Position and orientation.
    
  public:
    /**
     * \brief Default constructor.
     *
     * Sets initial transform to the identity matrix.
     */
    Object() : transform({0, 0, 0}, {0, 0, 0}) { }

    /// Draw children relative to this object.
    virtual void draw()
    {
      glPushMatrix();
      glMultMatrixd(transform.data);
      Node::draw();
      glPopMatrix();
    }
};

/**
 * \brief Root node of the scene graph.
 *
 * A Scene is a special Node that clears the image and starts the
 * drawing process. It makes little sense to attach it to another Node.
 */
class Scene : public Node
{
  public:
    Vector3 color; ///< Background color.

  public:
    Scene() : color(0, 0, 0) { }
  
    /// Draw scene.
    virtual void draw()
    {
      glClearColor(color.x, color.y, color.z, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      Node::draw();
    }
};

/**
 * \brief Defines camera position and frustum.
 */
class Camera
{
  public:
    Transform transform; ///< Camera position.
    Scene *scene;        ///< Scene to draw.
    double fovy;         ///< Vertical field of view.
  
  public:
    /**
     * \brief Specifies the Scene to draw, as well as the vertical field of
     * view.
     * 
     * By default, sets field of view such that an object of size X fills the
     * vertical field at distance X.
     */
    Camera(Scene *_scene, double _fovy = 0.92) : scene(_scene), fovy(_fovy) { }
  
    /// Draw Scene from this camera's perspective.
    void draw()
    {
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
      
      GLfloat pos[] = {0, 0, 1, 0};
      glLightfv(GL_LIGHT0, GL_POSITION, pos);
      
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixd(transform.data);
      
      scene->draw();
    }
};

/**
 * \brief Camera controller.
 *
 * Allows user interaction to move a Camera.
 */
class Controller
{
  public:
    Camera *camera; ///< Camera to move.
    
  public:
    Controller(Camera *_camera) : camera(_camera) { }
    virtual ~Controller() { }
  
    /// Click handler.
    virtual void click(int button, int action, int mods, double xpos, double ypos) = 0;
    
    /// Scroll wheel handler.
    virtual void scroll(double xoffset, double yoffset) = 0;
    
    /// Mouse motion handler.
    virtual void motion(double xpos, double ypos) = 0;
};

/**
 * \brief Texture.
 *
 * Loads data into an OpenGL texture. This is a lightweight class that can be
 * copied at will.
 */
class Texture
{
  protected:
    typedef std::shared_ptr<GLuint> GLuintPtr;

  public:
    int width = 0,          ///< Texture width. Do not modify.
        height = 0;         ///< Texture height. Do not modify.
    
  protected:
    GLuintPtr texture_ = 0; ///< OpenGL texture identifier.
    
  public:
    Texture() { }
  
    /// Loads texture data from existing RGB array. Data can be discarded afterwards.
    Texture(int width, int height, unsigned char *data, bool interpolate=false)
    {
      make(width, height, data, interpolate);
    }
    
    /// Loads texture from Portable Pixmap (PPM) file.
    Texture(const std::string &file, bool interpolate=true)
    {
      std::ifstream ifs(file, std::ios::binary);
      std::string magic;
      
      ifs >> magic;
      if (magic != "P6")
      {
        std::cerr << file << " is not a valid binary PPM" << std::endl;
        return;
      }
      
      int _width = readint(ifs);
      int _height = readint(ifs);
      int maxval  = readint(ifs);
      
      if (maxval != 255)
      {
        std::cerr << file << ": pixel format not supported" << std::endl;
        return;
      }
      
      unsigned char *data = new unsigned char[_width*_height*3];

      ifs.read((char*)data, _width*_height*3);
      if (!ifs.good())
      {
        std::cerr << file << " is truncated" << std::endl;
        return;
      }
      
      make(_width, _height, data, interpolate);
      
      delete[] data;
    }
    
    ~Texture()
    {
      // Delete OpenGL texture when last reference goes out of scope.
      if (texture_.use_count() == 1)
        glDeleteTextures(1, texture_.get());
    }
    
    /// Returns whether texture is valid.
    operator bool() const
    {
      return texture_ != 0;
    }
    
    /// Use this texture as the current OpenGL 2D texture.
    void bind() const
    {
      glBindTexture(GL_TEXTURE_2D, *texture_);
    }
    
  protected:
    void make(int _width, int _height, unsigned char *data, bool interpolate)
    {
      width = _width;
      height = _height;
      texture_ = GLuintPtr(new GLuint);
    
      glGenTextures(1, texture_.get());
      glBindTexture(GL_TEXTURE_2D, *texture_);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolate?GL_LINEAR:GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolate?GL_LINEAR:GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)data);
    }
    
    void eatcomments(std::ifstream &ifs)
    {
      unsigned char c;
      do
      {
        c = ifs.get();
        if (c == '#')
        {
          char buffer[256];
          ifs.getline(buffer, 256);
        }
      } while (isspace(c) || c == '#');

      ifs.putback(c);
    }
    
    /// Read integer value from PPM file, ignoring comments and whitespace.
    int readint(std::ifstream &ifs)
    {
      int val;
      
      eatcomments(ifs);
      ifs >> val;
      eatcomments(ifs);
      
      return val;
    }
};

/// Checkerboard texture.
class Checkerboard4x4 : public Texture
{
  public:
    Checkerboard4x4()
    {
      const unsigned char m = 255;
      unsigned char data[] = {0,0,0,m,m,m,0,0,0,m,m,m,
                              m,m,m,0,0,0,m,m,m,0,0,0,
                              0,0,0,m,m,m,0,0,0,m,m,m,
                              m,m,m,0,0,0,m,m,m,0,0,0};
                              
      make(4, 4, data, false);
    }
};

}

#include "primitive.h"
#include "controller.h"

#endif // PGL_PGL_H_
