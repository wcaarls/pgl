/*
 * PGL, a primitive OpenGL 3D primitive library
 * Main headerfile
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

#ifndef __PGL_PGL_H_
#define __PGL_PGL_H_

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

class Primitive
{
  public:
    Transform transform;
    Vector3 color;
    GLuint list;
    
    std::vector<Primitive*> children;
    
  public:
    Primitive() : transform({0, 0, 0}, {0, 0, 0}), color(1, 1, 1)
    {
      list = glGenLists(1);
    }

    ~Primitive()
    {
      glDeleteLists(list, 1);
      for (size_t ii=0; ii != children.size(); ++ii)
        delete children[ii];
      children.clear();
    }
  
    void draw()
    {
      glPushMatrix();
      glMultMatrixd(transform.data);
      
      glColor3d(color.x, color.y, color.z);
      
      glCallList(list);
      for (size_t ii=0; ii != children.size(); ++ii)
        children[ii]->draw();
      glPopMatrix();
    }
    
    // Transfers ownership
    void attach(Primitive *child)
    {
      children.push_back(child);
    }

  protected:
    void vertex(const Vector3 &v)
    {
      glVertex3d(v.x, v.y, v.z);
    }
    
    void quad(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &v4)
    {
      vertex(v1);
      vertex(v2);
      vertex(v3);
      vertex(v4);
    }
};

class Scene
{
  public:
    Transform transform;
    Vector3 background;
  
    std::vector<Primitive*> objects;
    
  public:
    Scene() : background{0, 0, 0}
    {
    }
  
    ~Scene()
    {
      for (size_t ii=0; ii != objects.size(); ++ii)
        delete objects[ii];
      objects.clear();
    }
    
    void draw()
    {
      glClearColor(background.x, background.y, background.z, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      int dims[4];
      glGetIntegerv(GL_VIEWPORT, dims);
      double aspect = dims[2]/(double)dims[3];
      
      // Angle of view such that at distance x our field is x vertically.
      double f = 1/tan(0.46);
      double near = 1, far = 100;
      
      // gluPerspective(45/aspect, aspect, near, far)
      double matrix[] = {f/aspect, 0., 0., 0.,
                         0., f, 0., 0.,
                         0., 0., (far+near)/(near-far), -1.,
                         0., 0., 2*far*near/(near-far), 0.};
      
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixd(matrix);
      
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixd(transform.data);
      
      for (size_t ii=0; ii != objects.size(); ++ii)
        objects[ii]->draw();
    }
    
    // Transfers ownership
    void attach(Primitive *object)
    {
      objects.push_back(object);
    }
};

class Controller
{
  public:
    Scene *scene;
    
  public:
    Controller(Scene *_scene) : scene(_scene)
    {
    }
  
    virtual void click(int button, int action, int mods, double xpos, double ypos) = 0;
    virtual void scroll(double xoffset, double yoffset) = 0;
    virtual void motion(double xpos, double ypos) = 0;
};

}

#include "primitive.h"
#include "controller.h"

#endif // __PGL_PGL_H_
