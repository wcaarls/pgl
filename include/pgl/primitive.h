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

namespace pgl {

class Box : public Primitive
{
  public:
    Vector3 size;
    
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
      Vector3 vec = end-start;
      double len = vec.norm();
      vec = vec / len;
      
      double angle = acos(vec.x);
      Vector3 axis{0, -vec.z, vec.y};
    
      make({len, thickness, thickness});
      
      transform = Transform(axis, angle, start)*Translation({len/2, 0, 0});
    }
    
  protected:
    void make(const Vector3 &size)
    {
      Vector3 s2 = size/2;
      
      Vector3 vppp( s2.x,  s2.y,  s2.z), vnpp(-s2.x,  s2.y,  s2.z), vnnp(-s2.x, -s2.y,  s2.z), vpnp( s2.x, -s2.y,  s2.z),
              vppn( s2.x,  s2.y, -s2.z), vnpn(-s2.x,  s2.y, -s2.z), vnnn(-s2.x, -s2.y, -s2.z), vpnn( s2.x, -s2.y, -s2.z);
    
      glNewList(list, GL_COMPILE);
      glBegin(GL_QUADS);
        glNormal3d(0, 0, -1);
        quad(vnnn, vpnn, vppn, vnpn); // Z-
        glNormal3d(0, 0, 1);
        quad(vnnp, vnpp, vppp, vpnp); // Z+
        glNormal3d(-1, 0, 0);
        quad(vnnn, vnpn, vnpp, vnnp); // X-
        glNormal3d(1, 0, 0);
        quad(vpnn, vpnp, vppp, vppn); // X+
        glNormal3d(0, -1, 0);
        quad(vnnn, vnnp, vpnp, vpnn); // Y-
        glNormal3d(0, 1, 0);
        quad(vnpn, vppn, vppp, vnpp); // Y+
      glEnd();
      glEndList();
    }
};

}

#endif // __PGL_PRIMITIVE_H_
