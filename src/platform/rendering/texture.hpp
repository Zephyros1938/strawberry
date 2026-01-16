#pragma once

#include "glad/glad.h"
#include <stb/stb_image.h>

class Texture {
public:
  unsigned int ID;
  int width, height;

  Texture(const char *path);
  Texture() : ID(0) {};
  /*~Texture() {
    if (ID != 0)
      glDeleteTextures(1, &ID);
  }*/
  void bind(int target) const;
  void unbind(int target) const;
};
