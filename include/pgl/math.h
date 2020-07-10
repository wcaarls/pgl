/** \file math.h
 *
 * PGL, a primitive OpenGL 3D primitive library.
 *
 * This file contains vector math functions.
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

#ifndef PGL_MATH_H_
#define PGL_MATH_H_

#include <ostream>

#include <string.h>
#include <math.h>

namespace pgl {

/** \brief 3-component vector.
 *
 * Individual components can be addressed through either
 * \code
 * vector.x
 * \endcode
 * or
 * \code
 * vector[0]
 * \endcode
 */
class Vector3
{
  public:
    union
    {
      double data[3];
      struct
      {
        double x, y, z;
      };
    };
    
  public:
    Vector3(float _data[3])
    {
      data[0] = (double)_data[0];
      data[1] = (double)_data[1];
      data[2] = (double)_data[2];
    }
    
    Vector3(double _data[3])
    {
      data[0] = _data[0];
      data[1] = _data[1];
      data[2] = _data[2];
    }
    
    Vector3() { }
    Vector3(double _x, double _y, double _z) : data{_x, _y, _z} { }
    Vector3(const Vector3 &rhs) : data{rhs.x, rhs.y, rhs.z} { }
    Vector3 &operator=(const Vector3 &rhs) = default;
    
    double &operator[](const unsigned int idx)
    {
      return data[idx];
    }
  
    double operator[](const unsigned int idx) const
    {
      return data[idx];
    }
  
    Vector3 operator-() const
    {
      return Vector3(-x, -y, -z);
    }

    Vector3 operator+(const Vector3 &rhs) const
    {
      return Vector3(x+rhs.x, y+rhs.y, z+rhs.z);
    }

    Vector3 operator-(const Vector3 &rhs) const
    {
      return Vector3(x-rhs.x, y-rhs.y, z-rhs.z);
    }

    Vector3 operator*(const double &rhs) const
    {
      return Vector3(x*rhs, y*rhs, z*rhs);
    }

    /// Elementwise product.
    Vector3 operator*(const Vector3 &rhs) const
    {
      return Vector3(x*rhs.x, y*rhs.y, z*rhs.z);
    }

    Vector3 operator/(const double &rhs) const
    {
      return Vector3(x/rhs, y/rhs, z/rhs);
    }

    Vector3 operator/(const Vector3 &rhs) const
    {
      return Vector3(x/rhs.x, y/rhs.y, z/rhs.z);
    }

    /// Power.
    Vector3 operator^(const double &rhs) const
    {
      return Vector3(pow(x, rhs), pow(y, rhs), pow(z, rhs));
    }
    
    Vector3 cross(const Vector3 &rhs) const
    {
      return Vector3(y*rhs.z - z*rhs.y, z*rhs.x-x*rhs.z, x*rhs.y - y*rhs.x);
    }
    
    double norm() const
    {
      return sqrt(normsq());
    }

    double normsq() const
    {
      return x*x+y*y+z*z;
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector3 &obj)
    {
      os << "[" << obj.x << ", " << obj.y << ", " << obj.z << "]";
      return os;
    }
};

/** \brief Homogeneous coordinate transform.
 *
 * Column-major storage order. The translation can be addressed through either
 * \code
 * transform.x
 * \endcode
 * or
 * \code
 * transform[12]
 * \endcode
 *
 * \note
 * Since we're working with 3-component vectors, a matrix-vector product
 * will not apply the last row of the matrix. It is, however, stored for
 * potential other uses.
 */
class Transform
{
  public:
    union
    {
      double data[16];
      struct
      {
        double __dummy[12];
        double x, y, z;
        double __dummy2;
      };
    };

  public:
    Transform() { }
  
    Transform(double _data[16])
    {
      set(_data);
    }
    
    /// Specifies transform through intrinsict roll-pitch-yaw and translation.
    Transform(const Vector3 &rotation, const Vector3 &translation)
    {
      set(rotation, translation);
    }
    
    /// Specifies transform through axis-angle and translation.
    Transform(const Vector3 &axis, const double &angle, const Vector3 &translation)
    {
      set(axis, angle, translation);
    }
    
    void set(double _data[16])
    {
      memcpy(data, _data, 16*sizeof(double));
    }
    
    void set(const Vector3 &axis, const double &angle, const Vector3 &translation)
    {
      double s = sin(angle), c = cos(angle), c1 = 1-c;
      double x = axis.x, y = axis.y, z = axis.z;
      
      data[0] = c      + x*x*c1; data[4] = x*y*c1 - z*s;    data[ 8] = x*z*c1 + y*s;
      data[1] = y*x*c1 + z*s;    data[5] = c      + y*y*c1; data[ 9] = y*z*c1 - x*s;
      data[2] = z*x*c1 - y*s;    data[6] = z*y*c1 + x*s;    data[10] = c      + z*z*c1;
      data[3] = 0;               data[7] = 0;               data[11] = 0;
      
      this->x = translation.x;
      this->y = translation.y;
      this->z = translation.z;
      data[15] = 1;
    }
    
    void set(const Vector3 &rotation, const Vector3 &translation)
    {
      double sa = sin(rotation[2]), ca = cos(rotation[2]);
      double sb = sin(rotation[1]), cb = cos(rotation[1]);
      double sg = sin(rotation[0]), cg = cos(rotation[0]);

      // Intrinsic roll-pitch-yaw
      data[0] = ca*cb; data[4] = ca*sb*sg - sa*cg; data[8] = ca*sb*cg + sa*sg;
      data[1] = sa*cb; data[5] = sa*sb*sg + ca*cg; data[9] = sa*sb*cg - ca*sg;
      data[2] = -sb;   data[6] = cb*sg;            data[10] = cb*cg;
      data[3] = 0;     data[7] = 0;                data[11] = 0;
      
      x = translation.x;
      y = translation.y;
      z = translation.z;
      data[15] = 1;
    }
    
    double &operator[](const unsigned int idx)
    {
      return data[idx];
    }
  
    double operator[](const unsigned int idx) const
    {
      return data[idx];
    }
  
    Transform operator*(const Transform &rhs) const
    {
      Transform result;
    
      for (unsigned char ii = 0; ii < 4; ++ii)
        for (unsigned char jj = 0; jj < 4; ++jj)
        {
          double sum = 0;
          for (unsigned char kk = 0; kk < 4; ++kk)
            sum += data[ii+kk*4]*rhs.data[jj*4+kk];
          result[ii+jj*4] = sum;
        }
      
      return result;
    }
    
    Vector3 operator*(const Vector3 &rhs) const
    {
      return Vector3(data[0]*rhs.x + data[1]*rhs.y + data[ 2]*rhs.z,
                     data[4]*rhs.x + data[5]*rhs.y + data[ 6]*rhs.z,
                     data[8]*rhs.x + data[9]*rhs.y + data[10]*rhs.z);
    }

    friend std::ostream &operator<<(std::ostream &os, const Transform &obj)
    {
      os << "[" << obj.data[0] << ", " << obj.data[4] << ", " << obj.data[8] << ", " << obj.data[12] << std::endl
         << " " << obj.data[1] << ", " << obj.data[5] << ", " << obj.data[9] << ", " << obj.data[13] << std::endl
         << " " << obj.data[2] << ", " << obj.data[6] << ", " << obj.data[10] << ", " << obj.data[14] << std::endl
         << " " << obj.data[3] << ", " << obj.data[7] << ", " << obj.data[11] << ", " << obj.data[15] << "]" << std::endl;
      return os;
    }

};

/// Transform with zero translation.
class Rotation : public Transform
{
  public:
    Rotation(const Vector3 &rotation) : Transform(rotation, {0, 0, 0}) { }
};

/// Transform with identity rotation.
class Translation : public Transform
{
  public:
    Translation(const Vector3 &translation) : Transform({0, 0, 0}, translation) { }
};

}

#endif // PGL_MATH_H_
