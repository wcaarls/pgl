/** \file example.cpp
 *
 * PGL, a primitive OpenGL 3D primitive library.
 *
 * Example for basic PGL usage.
 *
 * (c) 2020, Wouter Caarls.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <pgl/pgl.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <unistd.h>

bool stop__ = false;
pgl::Camera *camera__;
pgl::Object *object__;
pgl::OrbitController *controller__;

void refresh(GLFWwindow* window)
{
  camera__->draw();
  
  object__->transform = pgl::Rotation({0, 0, 0.01}) * object__->transform;
  
  glfwSwapBuffers(window);
}

void reshape(GLFWwindow* /*window*/, int width, int height)
{
  glViewport(0, 0, width, height);
}

void click(GLFWwindow* window, int button, int action, int mods)
{
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  controller__->click(button, action, mods, xpos, ypos);
}

void scroll(GLFWwindow* /*window*/, double xoffset, double yoffset)
{
  controller__->scroll(xoffset, yoffset);
}

void motion(GLFWwindow* /*window*/, double xpos, double ypos)
{
  controller__->motion(xpos, ypos);
}

void close(GLFWwindow* /*window*/)
{
  stop__ = true;
}

int main(void)
{
  // Initialize GLFW
  if (glfwInit() != GL_TRUE)
  {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return 1;
  }
  
  GLFWwindow *window = glfwCreateWindow(512, 512, "PGL example", NULL, NULL);
  if (window == nullptr)
  {
    std::cerr << "Failed to create window" << std::endl;
    return 1;
  }
  
  glfwMakeContextCurrent(window);
  
  // Initialize our scene
  auto scene = new pgl::Scene();
  scene->attach(new pgl::Box({2, 2, 0.05}, {0, 0, -1}));
  scene->attach(new pgl::WireBox({2, 2, 2}));
  scene->attach(new pgl::Sphere(0.05));
  scene->attach(new pgl::Arrow({-1, -1, -1}, { 0, -1, -1}, 0.02))->color = {1, 0, 0};
  scene->attach(new pgl::Arrow({-1, -1, -1}, {-1,  0, -1}, 0.02))->color = {0, 1, 0};
  scene->attach(new pgl::Arrow({-1, -1, -1}, {-1, -1, 0}, 0.02))->color = {0, 0, 1};
  
  // Add a custom object that we'll animate
  scene->attach(object__ = new pgl::Object());
  object__->attach(new pgl::Capsule({-0.3, 0, 0}, {0.3, 0, 0}, 0.02))->color = {1, 0, 0};
  object__->attach(new pgl::Capsule({0, -0.3, 0}, {0, 0.3, 0}, 0.02))->color = {0, 1, 0};
  
  // Add STL model
  scene->attach(new pgl::Model("teapot.stl", {0, 0, -1}, 0.1))->color = {1, 1, 0};

  // Add some planes
  scene->attach(new pgl::Plane({-100, 0, 0}, {0, 100, 0}, {0, 0, 10}))->color = {0.5, 0.5, 1};
  scene->attach(new pgl::Plane({1, 0, 0}, {0, 1, 0}, {0, 0, -1}, pgl::Texture("ceramic-tiles.ppm"), 9));
  scene->attach(new pgl::Plane({1, 0, 0}, {0, 0, 1}, {0, 1, 0}, pgl::Checkerboard4x4()))->color = {0, 1, 1};
  
  // Initialize camera
  camera__ = new pgl::Camera(scene);
  
  // Initialize orbit controller
  controller__ = new pgl::OrbitController(camera__);
  controller__->view(0.5, 0.4, 4);

  // Register callbacks for orbit controller
  glfwSetWindowRefreshCallback(window, refresh);
  glfwSetFramebufferSizeCallback(window, reshape);
  glfwSetMouseButtonCallback(window, click);
  glfwSetScrollCallback(window, scroll);
  glfwSetCursorPosCallback(window, motion);
  glfwSetWindowCloseCallback(window, close);

  // Setup OpenGL
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glCullFace(GL_BACK);

  // Main loop  
  stop__ = false;
  while (!stop__)
  {
    glfwPollEvents();
    refresh(window);
    usleep(10000);
  }
  
  // Clean up
  glfwTerminate();
  
  delete controller__;
  delete camera__;
  delete scene;

  return 0;
}
