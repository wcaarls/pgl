#include "pgl/pgl.h"

#include <GLFW/glfw3.h>

#include <iostream>

#include <unistd.h>

#if (GLFW_VERSION_MAJOR >= 4) || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3)
  #define GLFW_THROW_IF(x, _str) \
    do { \
      if (x) \
      { \
        std::string str(_str); \
        const char *glfwstr; \
        glfwGetError(&glfwstr); \
        throw std::runtime_error(str + ": " + glfwstr); \
      } \
    } while (false)
#else
  #define GLFW_THROW_IF(x, _str) \
    do { \
      if (x) \
      { \
        throw std::runtime_error(_str); \
      } \
    } while (false)
#endif

using namespace pgl;

bool stop__ = false;
Primitive *box__;
Scene *scene__;
OrbitController *controller__;

void refresh(GLFWwindow* window)
{
  scene__->draw();
  
  box__->transform = box__->transform * Rotation({0.01, 0, 0});
  
  glfwSwapBuffers(window);
}

void reshape(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void click(GLFWwindow* window, int button, int action, int mods)
{
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  controller__->click(button, action, mods, xpos, ypos);
}

void scroll(GLFWwindow* window, double xoffset, double yoffset)
{
  controller__->scroll(xoffset, yoffset);
}

void motion(GLFWwindow* window, double xpos, double ypos)
{
  controller__->motion(xpos, ypos);
}

void close(GLFWwindow* window)
{
  stop__ = true;
}

int main(void)
{
  // Initialize GLFW
  GLFW_THROW_IF(glfwInit() != GL_TRUE, "Failed to initialize GLFW");
  
  GLFWwindow *window = glfwCreateWindow(512, 512, "PGL example", NULL, NULL);
  GLFW_THROW_IF(window == nullptr, "Failed to create window");
  glfwMakeContextCurrent(window);
  
  // Initialize our scene
  scene__ = new pgl::Scene();
  scene__->attach(new pgl::Box({10, 10, 0.1}));
  scene__->attach(new pgl::Sphere(1));
  scene__->attach(box__ = new pgl::Box({2, 1, 1}, {0, 0, 1}));
  scene__->attach(new pgl::Capsule({0, 0, 0}, {5, 0, 0}, 0.2))->color = {1, 0, 0};
  scene__->attach(new pgl::Capsule({0, 0, 0}, {0, 5, 0}, 0.2))->color = {0, 1, 0};
  scene__->attach(new pgl::Capsule({0, 0, 0}, {0, 0, 5}, 0.2))->color = {0, 0, 1};
  
  controller__ = new OrbitController(scene__);
  controller__->view(1, 0.5, 10);

  // Register callbacks  
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
