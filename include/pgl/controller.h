/*
 * PGL, a primitive OpenGL 3D primitive library
 * This file contains various scene controllers
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

#ifndef __PGL_CONTROLLER_H_
#define __PGL_CONTROLLER_H_

#include "pgl.h"

namespace pgl {

class OrbitController : public Controller
{
  public:
    Vector3 center;
    double azimuth, elevation, distance;
    
  protected:
    Vector3 old_center_;
    double old_azimuth_, old_elevation_, old_distance_, old_xpos_, old_ypos_;
    
    enum {modeNone, modeAngle, modeDistance, modeCenter} mode_;
    
  public:
    OrbitController(Scene *_scene) : Controller(_scene), center{0, 0, 0}, azimuth(60*M_PI/180), elevation(35*M_PI/180), distance(2), mode_(modeNone)
    {
      apply();
    }
    
    void view(double _azimuth, double _elevation, double _distance)
    {
      azimuth = _azimuth;
      elevation = _elevation;
      distance = _distance;
      
      apply();
    }
    
    void click(int button, int action, int mods, double xpos, double ypos)
    {
      if (action == PGL_PRESS)
      {
        old_xpos_      = xpos;
        old_ypos_      = ypos;
        old_center_    = center;
        old_azimuth_   = azimuth;
        old_elevation_ = elevation;
        old_distance_  = distance;
        
        if (button == PGL_MOUSE_BUTTON_LEFT)   mode_ = modeAngle;
        if (button == PGL_MOUSE_BUTTON_MIDDLE) mode_ = modeDistance;
        if (button == PGL_MOUSE_BUTTON_RIGHT)  mode_ = modeCenter;
      }
      else
        mode_ = modeNone;
    }
    
    void scroll(double xoffset, double yoffset)
    {
      distance *= pow(sqrt(2), -yoffset);
      apply();
    }
    
    void motion(double xpos, double ypos)
    {
      switch (mode_)
      {
        case modeAngle:
          azimuth = old_azimuth_ - 0.005*(xpos-old_xpos_);
          elevation = old_elevation_ + 0.005*(ypos-old_ypos_);
          break;
        case modeDistance:
          distance = old_distance_ + 0.02*(ypos-old_ypos_);
          break;
        case modeCenter:
          center = old_center_ + Rotation({0, 0, -azimuth})*(Vector3({old_xpos_-xpos, ypos-old_ypos_, 0})*0.02);
          break;
        default:
          break;
      }
      
      apply();
    }
    
    void apply()
    {
      scene->transform = Transform({-0.5*M_PI+elevation, 0, 0}, {0, 0, -distance})*Rotation({0, 0, -azimuth})*Translation(-center);
    }
};

}

#endif // __PGL_CONTROLLER_H_
