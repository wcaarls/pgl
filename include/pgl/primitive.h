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

/// Box
class Box : public Primitive
{
  public:
    Box(const Vector3 &size)
    {
      make(size);
    }
    
    Box(const Vector3 &size, const Vector3 &offset)
    {
      make(size);
      transform = Translation(offset);
    }
    
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
    
      glNewList(list, GL_COMPILE);
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

/// Wireframe box
class WireBox : public Primitive
{
  public:
    WireBox(const Vector3 &size)
    {
      make(size);
    }
    
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
    
      glNewList(list, GL_COMPILE);
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

/// Sphere
class Sphere : public Primitive
{
  public:
    Sphere(double radius)
    {
      make(radius);
    }

    Sphere(double radius, const Vector3 &offset)
    {
      make(radius);
      transform = Translation(offset);
    }

  protected:
    void make(double radius)
    {
      glNewList(list, GL_COMPILE);
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

/// Cylinder, optionally with different start and end radii
class Cylinder : public Primitive
{
  public:
    Cylinder(double length, double radius, double endradius=-1)
    {
      make(length, radius, endradius);
    }
    
    Cylinder(const Vector3 &start, const Vector3 &end, double radius, double endradius=-1)
    {
      make(align(start, end), radius, endradius);
    }
    
  protected:
    void make(double length, double radius, double endradius)
    {
      if (endradius < 0)
        endradius = radius;
    
      glNewList(list, GL_COMPILE);
      
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

/// Cylinder ending in a point
class Cone : public Cylinder
{
  public:
    Cone(double length, double radius) : Cylinder(length, radius, 0) { }
    Cone(const Vector3 &start, const Vector3 &end, double radius) : Cylinder(start, end, radius, 0) { }
};

/// Arrow
class Arrow : public Primitive
{
  public:
    Arrow(double length, double radius, double headlength=-1, double headradius=-1)
    {
      make(length, radius, headlength, headradius);
    }
    
    Arrow(const Vector3 &start, const Vector3 &end, double radius, double headlength=-1, double headradius=-1)
    {
      make(align(start, end), radius, headlength, headradius);
    }
    
    void draw()
    {
      children[0]->color = color;
      children[1]->color = color;
      
      Primitive::draw();
    }
    
  protected:
    void make(double length, double radius, double headlength, double headradius)
    {
      if (headlength < 0)
        headlength = radius*6;
      if (headradius < 0)
        headradius = headlength/3;
        
      attach(new Cylinder(length, radius));
      attach(new Cone(headlength, headradius))->transform = Translation({0, 0, length/2});
    }
};

/// Cylinder with rounded endcaps
class Capsule : public Primitive
{
  public:
    Capsule(double length, double radius)
    {
      make(length, radius);
    }
    
    Capsule(const Vector3 &start, const Vector3 &end, double radius)
    {
      make(align(start, end), radius);
    }
    
  protected:
    void make(double length, double radius)
    {
      glNewList(list, GL_COMPILE);
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
