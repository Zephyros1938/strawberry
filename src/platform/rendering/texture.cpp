#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb/stb_image.h>

Texture::Texture(const char *path) {
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

  unsigned int format;
  switch (nrChannels) {
  case 1:
    format = GL_RED;
    break;
  case 4:
    format = GL_RGBA;
    break;
  default:
    format = GL_RGB;
    break;
  }

  if (data) {
    // Calculate the number of bytes in one row
    int bytesPerRow = width * nrChannels;

    // If the row size is not a multiple of 4, tell OpenGL to use 1-byte
    // alignment
    if (bytesPerRow % 4 != 0) {
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    } else {
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Reset to default
    }
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "Failed to load texture <" << path << ">" << std::endl;
  }

  stbi_image_free(data);

  ID = tex;
}

Texture::Texture() : ID(0) {}

void Texture::bind(int target = GL_TEXTURE0) const {
  glActiveTexture(target);
  glBindTexture(GL_TEXTURE_2D, ID);
}
