#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>

class Texture {
 public:
  Texture();
  ~Texture() = default;
  Texture(const Texture& other) = default;
  Texture& operator=(const Texture& other) = default;
  Texture(Texture&& other) = default;
  Texture& operator=(Texture&& other) = default;

 private:
  unsigned int id;
  std::string type;
};

#endif  // TEXTURE_H