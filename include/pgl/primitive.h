/*
 * PGL, a primitive OpenGL 3D primitive library
 * This file contains various 3D primitives
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

#ifndef __PGL_PRIMITIVE_H_
#define __PGL_PRIMITIVE_H_

#include "pgl.h"

// Must be divisible by 4
#define FACETS 20

namespace pgl {

/**
 * \brief Basic 3D primitive.
 *
 * An object that actually draws something. Derived objects generally
 * create a display list upon construction that is drawn by this
 * superclass.
 *
 * \note
 * Objects will generally by aligned along the Z axis and centered
 * on the origin.
 */
class Primitive : public Object
{
  public:
    Vector3 color; ///< Primitive color.
    
  protected:
    GLuint list_;  ///< Display list identifier.
    
  public:
    /** \brief Default constructor.
     *
     * The default color is white.
     */
    Primitive() : color(1, 1, 1)
    {
      list_ = glGenLists(1);
    }

    virtual ~Primitive()
    {
      glDeleteLists(list_, 1);
    }
  
    virtual void draw()
    {
      glColor3d(color.x, color.y, color.z);

      glPushMatrix();
      glMultMatrixd(transform.data);
      
      glCallList(list_);
      for (size_t ii=0; ii != children.size(); ++ii)
        children[ii]->draw();
      glPopMatrix();
    }
    
  protected:
    /** \brief Align primitive along axis.
     *
     * Align a centered z-axis aligned primitive along end-start,
     * starting at start.
     *
     * \returns length of vector.
     */
    double align(const Vector3 &start, const Vector3 &end)
    {
      Vector3 vec = end-start;
      double len = vec.norm();
      vec = vec / len;
      
      double angle = acos(vec.z);
      Vector3 axis{-vec.y, vec.x, 0};
    
      transform = Transform(axis, angle, start)*Translation({0, 0, len/2});
      
      return len;
    }
  
    /// Sets norm for subsequent vertices.
    virtual void normal(const Vector3 &v)
    {
      double norm = v.norm();
    
      glNormal3d(v.x/norm, v.y/norm, v.z/norm);
    }
    
    /// Enters vertex into display list.
    virtual void vertex(const Vector3 &v)
    {
      glVertex3d(v.x, v.y, v.z);
    }
    
    /** \brief Draws a four-sided polygon using two triangles.
     *
     * Assuming the polygon is in counter-clockwise order, the triangles
     * are as well.
     */
    virtual void quad(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &v4)
    {
      triangle(v1, v2, v3);
      triangle(v3, v4, v1);
    }

    /// Draws triangle.
    virtual void triangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
    {
      vertex(v1);
      vertex(v2);
      vertex(v3);
    }
};

/// Box primitive.
class Box : public Primitive
{
  public:
    /// Specfies box size.
    Box(const Vector3 &size)
    {
      make(size);
    }
    
    /// Specfies box size and offset.
    Box(const Vector3 &size, const Vector3 &offset)
    {
      make(size);
      transform = Translation(offset);
    }
    
    /** \brief Specfies box start and end coordinates, as well as thickness.
     */
    Box(const Vector3 &start, const Vector3 &end, double thickness)
    {
      make({thickness, thickness, align(start, end)});
    }
    
  protected:
    void make(const Vector3 &size)
    {
      Vector3 s2 = size/2;
      
      Vector3 vppp( s2.x,  s2.y,  s2.z), vnpp(-s2.x,  s2.y,  s2.z), vnnp(-s2.x, -s2.y,  s2.z), vpnp( s2.x, -s2.y,  s2.z),
              vppn( s2.x,  s2.y, -s2.z), vnpn(-s2.x,  s2.y, -s2.z), vnnn(-s2.x, -s2.y, -s2.z), vpnn( s2.x, -s2.y, -s2.z);
    
      glNewList(list_, GL_COMPILE);
      glBegin(GL_TRIANGLES);
        glNormal3d(0, 0, 1);
        quad(vnnp, vpnp, vppp, vnpp); // Z+
        glNormal3d(0, 0, -1);
        quad(vnnn, vnpn, vppn, vpnn); // Z-
        glNormal3d(1, 0, 0);
        quad(vpnn, vppn, vppp, vpnp); // X+
        glNormal3d(-1, 0, 0);
        quad(vnnn, vnnp, vnpp, vnpn); // X-
        glNormal3d(0, 1, 0);
        quad(vnpn, vnpp, vppp, vppn); // Y+
        glNormal3d(0, -1, 0);
        quad(vnnn, vpnn, vpnp, vnnp); // Y-
      glEnd();
      glEndList();
    }
};

/// Wireframe box primitive.
class WireBox : public Primitive
{
  public:
    /// Specfies box size.
    WireBox(const Vector3 &size)
    {
      make(size);
    }
    
    /// Specfies box size and offset.
    WireBox(const Vector3 &size, const Vector3 &offset)
    {
      make(size);
      transform = Translation(offset);
    }
    
  protected:
    void make(const Vector3 &size)
    {
      Vector3 s2 = size/2;
      
      Vector3 vppp( s2.x,  s2.y,  s2.z), vnpp(-s2.x,  s2.y,  s2.z), vnnp(-s2.x, -s2.y,  s2.z), vpnp( s2.x, -s2.y,  s2.z),
              vppn( s2.x,  s2.y, -s2.z), vnpn(-s2.x,  s2.y, -s2.z), vnnn(-s2.x, -s2.y, -s2.z), vpnn( s2.x, -s2.y, -s2.z);
    
      glNewList(list_, GL_COMPILE);
      glBegin(GL_LINES);
        vertex(vnnp); vertex(vpnp);
        vertex(vnnn); vertex(vpnn);
        vertex(vnnp); vertex(vnpp);
        vertex(vnnn); vertex(vnpn);
        vertex(vppp); vertex(vnpp);
        vertex(vppn); vertex(vnpn);
        vertex(vppp); vertex(vpnp);
        vertex(vppn); vertex(vpnn);
        vertex(vnnn); vertex(vnnp);
        vertex(vpnn); vertex(vpnp);
        vertex(vnpn); vertex(vnpp);
        vertex(vppn); vertex(vppp);
      glEnd();
      glEndList();
    }
};

/// Sphere primitive.
class Sphere : public Primitive
{
  public:
    /// Specifies sphere radius.
    Sphere(double radius)
    {
      make(radius);
    }

    /// Specifies sphere radius and offset.
    Sphere(double radius, const Vector3 &offset)
    {
      make(radius);
      transform = Translation(offset);
    }

  protected:
    void make(double radius)
    {
      glNewList(list_, GL_COMPILE);
      glBegin(GL_TRIANGLES);
        for (size_t jj=0; jj != FACETS/2; ++jj)
        {
          double phi1 = jj*2.*M_PI/FACETS, phi2 = (jj+1)*2.*M_PI/FACETS;
          double r1 = radius*sin(phi1), r2 = radius*sin(phi2);
          double z1 = -radius*cos(phi1), z2 = -radius*cos(phi2);
          
          for (size_t ii=0; ii != FACETS; ++ii)
          {
            double theta1 = ii*2.*M_PI/FACETS, theta2 = (ii+1)*2.*M_PI/FACETS;
                        
            quad({r1*cos(theta1), r1*sin(theta1), z1},
                 {r1*cos(theta2), r1*sin(theta2), z1},
                 {r2*cos(theta2), r2*sin(theta2), z2},
                 {r2*cos(theta1), r2*sin(theta1), z2});
          }
        }
      glEnd();
      glEndList();
    }

    void vertex(const Vector3 &v)
    {
      normal(v);
      Primitive::vertex(v);
    }
};

/** \brief Generalized cylinder.
 *
 * The cylinder can have different start and end radii.
 */
class Cylinder : public Primitive
{
  public:
    /** \brief Specifies length, radius, and end radius.
     *
     * When not specified, the end radius is equal to the radius.
     */
    Cylinder(double length, double radius, double endradius=-1)
    {
      make(length, radius, endradius);
    }
    
    /** \brief Specifies start and end coordinates, as well as radius and end radius.
     *
     * When not specified, the end radius is equal to the radius.
     */
    Cylinder(const Vector3 &start, const Vector3 &end, double radius, double endradius=-1)
    {
      make(align(start, end), radius, endradius);
    }
    
  protected:
    void make(double length, double radius, double endradius)
    {
      if (endradius < 0)
        endradius = radius;
    
      glNewList(list_, GL_COMPILE);
      
      // Body
      glBegin(GL_TRIANGLES);
        for (size_t ii=0; ii != FACETS; ++ii)
        {
          double theta1 = ii*2*M_PI/FACETS, theta2 = (ii+1.)*2*M_PI/FACETS;
          
          quad({   radius*cos(theta1),    radius*sin(theta1), -length/2},
               {   radius*cos(theta2),    radius*sin(theta2), -length/2},
               {endradius*cos(theta2), endradius*sin(theta2),  length/2},
               {endradius*cos(theta1), endradius*sin(theta1),  length/2});
        }
      glEnd();
      
      // Top
      glBegin(GL_TRIANGLE_FAN);
        normal({0, 0, 1});
        for (size_t ii=0; ii != FACETS; ++ii)
        {
          double theta = ii*2*M_PI/FACETS;
          vertex({endradius*cos(theta), endradius*sin(theta), length/2});
        }
      glEnd();
      
      // Bottom
      glBegin(GL_TRIANGLE_FAN);
        normal({0, 0, -1});
        for (size_t ii=0; ii != FACETS; ++ii)
        {
          double theta = ii*2.*M_PI/-FACETS;
          vertex({radius*cos(theta), radius*sin(theta), -length/2});
        }
      glEnd();
      glEndList();
    }
    
    void vertex(const Vector3 &v)
    {
      normal({v.x, v.y, 0});
      Primitive::vertex(v);
    }
};

/// Cylinder ending in a point.
class Cone : public Cylinder
{
  public:
    /// Specifies length and radius.
    Cone(double length, double radius) : Cylinder(length, radius, 0) { }
    
    /// Specifies start and end coordinates, as well as radius.
    Cone(const Vector3 &start, const Vector3 &end, double radius) : Cylinder(start, end, radius, 0) { }
};

/** \brief Arrow primitive.
 *
 * Consists of a Cylinder body and a Cone head.
 */
class Arrow : public Primitive
{
  protected:
    Cylinder *body_; ///< Arrow body.
    Cone *head_;     ///< Arrow head.

  public:
    /** \brief Specifies arrow length, radius, head length, and head radius.
     *
     * When not specified, head length is radius*6, while head radius is head length/2.
     */
    Arrow(double length, double radius, double headlength=-1, double headradius=-1)
    {
      make(length, radius, headlength, headradius);
    }
    
    /** \brief Specifies start and end coordinates, as well as radius, head length, and head radius.
     *
     * When not specified, head length is radius*6, while head radius is head length/2.
     */
    Arrow(const Vector3 &start, const Vector3 &end, double radius, double headlength=-1, double headradius=-1)
    {
      make(align(start, end), radius, headlength, headradius);
    }
    
    void draw()
    {
      body_->color = color;
      head_->color = color;
      
      Object::draw();
    }
    
  protected:
    void make(double length, double radius, double headlength, double headradius)
    {
      if (headlength < 0)
        headlength = radius*6;
      if (headradius < 0)
        headradius = headlength/3;
        
      attach(body_ = new Cylinder(length, radius));
      attach(head_ = new Cone(headlength, headradius))->transform = Translation({0, 0, length/2});
    }
};

/** \brief Capsule primitive.
 *
 * A capsule is a cylinder with rounded endcaps.
 */
class Capsule : public Primitive
{
  public:
    /// Specifies length and radius.
    Capsule(double length, double radius)
    {
      make(length, radius);
    }
    
    /// Specifies start and end coordinates, as well as radius.
    Capsule(const Vector3 &start, const Vector3 &end, double radius)
    {
      make(align(start, end), radius);
    }
    
  protected:
    void make(double length, double radius)
    {
      glNewList(list_, GL_COMPILE);
      glBegin(GL_TRIANGLES);
      
      // Start at bottom cap
      int jadj1=0, jadj2=1;
      double zadj1 = -length/2, zadj2 = -length/2;
      
      for (size_t jj=0; jj != FACETS/2+1; ++jj)
      {
        if (jj == FACETS/4)
        {
          // Move to body
          jadj2--;
          zadj2 += length;
        }
        else if (jj == FACETS/4 + 1)
        {
          // Move to top cap
          jadj1--;
          zadj1 += length;
        }
        
        double phi1 = (jj+jadj1)*2.*M_PI/FACETS, phi2 = (jj+jadj2)*2.*M_PI/FACETS;
        double r1 = radius*sin(phi1), r2 = radius*sin(phi2);
        double z1 = -radius*cos(phi1)+zadj1, z2 = -radius*cos(phi2)+zadj2;
        
        for (size_t ii=0; ii != FACETS; ++ii)
        {
          double theta1 = ii*2.*M_PI/FACETS, theta2 = (ii+1)*2.*M_PI/FACETS;

          quad({r1*cos(theta1), r1*sin(theta1), z1}, zadj1,
               {r1*cos(theta2), r1*sin(theta2), z1}, zadj1,
               {r2*cos(theta2), r2*sin(theta2), z2}, zadj2,
               {r2*cos(theta1), r2*sin(theta1), z2}, zadj2);
        }
      }
      glEnd();
      glEndList();
    }

    void quad(const Vector3 &v1, double z1, const Vector3 &v2, double z2, const Vector3 &v3, double z3, const Vector3 &v4, double z4)
    {
      triangle(v1, z1, v2, z2, v3, z3);
      triangle(v3, z3, v4, z4, v1, z1);
    }
    
    void triangle(const Vector3 &v1, double z1, const Vector3 &v2, double z2, const Vector3 &v3, double z3)
    {
      vertex(v1, z1);
      vertex(v2, z2);
      vertex(v3, z3);
    }
    
    void vertex(const Vector3 &v, double z)
    {
      normal({v.x, v.y, v.z-z});
      Primitive::vertex(v);
    }
};

}

#endif // __PGL_PRIMITIVE_H_
