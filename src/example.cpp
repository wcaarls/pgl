#include "pgl/pgl.h"

#include <GLFW/glfw3.h>

#include <iostream>

#include <unistd.h>

using namespace pgl;

bool stop__ = false;
Scene *scene__;
Object *object__;
OrbitController *controller__;

void refresh(GLFWwindow* window)
{
  scene__->draw();
  
  object__->transform = Rotation({0, 0, 0.01}) * object__->transform;
  
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
  scene__ = new pgl::Scene();
  scene__->attach(new pgl::Box({2, 2, 0.05}, {0, 0, -1.025}));
  scene__->attach(new pgl::WireBox({2, 2, 2}));
  scene__->attach(new pgl::Sphere(0.05));
  scene__->attach(new pgl::Arrow({-1, -1, -1}, { 0, -1, -1}, 0.02))->color = {1, 0, 0};
  scene__->attach(new pgl::Arrow({-1, -1, -1}, {-1,  0, -1}, 0.02))->color = {0, 1, 0};
  scene__->attach(new pgl::Arrow({-1, -1, -1}, {-1, -1, 0}, 0.02))->color = {0, 0, 1};
  
  // Add a custom object
  scene__->attach(object__ = new Object());
  object__->attach(new Capsule({-0.3, 0, 0}, {0.3, 0, 0}, 0.02))->color = {1, 0, 0};
  object__->attach(new Capsule({0, -0.3, 0}, {0, 0.3, 0}, 0.02))->color = {0, 1, 0};
  
  // Initialize orbit controller
  controller__ = new OrbitController(scene__);
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
  delete scene__;

  return 0;
}
