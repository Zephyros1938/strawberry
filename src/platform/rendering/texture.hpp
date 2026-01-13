#pragma once

#include "glad/glad.h"
#include <stb/stb_image.h>

class Texture {
public:
  unsigned int ID;
  int width, height;

  Texture(const char *path);
  Texture();
  void bind(int target) const;
};
