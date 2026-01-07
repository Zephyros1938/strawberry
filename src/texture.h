#ifndef TEXTURE
#define TEXTURE

#include "glad/glad.h"
#include <stb/stb_image.h>

#include <iostream>

class Texture {
public:
  unsigned int ID;

  Texture(const char *path) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else {
      std::cerr << "Failed to load texture <" << path << ">" << std::endl;
    }

    stbi_image_free(data);

    ID = tex;
  }
  void bind(int target = GL_TEXTURE0) {
    glActiveTexture(target);
    glBindTexture(GL_TEXTURE_2D, ID);
  }
};

#endif // !TEXTURE
