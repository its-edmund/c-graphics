#include <GLFW/glfw3.h>
#include <stdio.h>

int main() {
  printf("Hello world\n");

  if (!glfwInit()) {
    printf("GLFW initialization failed");
    return 1;
  }
}
