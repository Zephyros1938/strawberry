#ifndef TEXTURE
#define TEXTURE

#include "glad/glad.h"
#include <stb/stb_image.h>

#include <iostream>

class Texture {
public:
  unsigned int ID;

  Texture(const char *path);
  void bind(int target) const;
};

#endif // !TEXTURE
