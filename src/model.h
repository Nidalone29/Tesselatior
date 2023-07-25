#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "transform.h"

/**
 * @brief basically a Model?
 *
 */
class Model {
 public:
  Model() = delete;  // can't create an empty model
  Model(const std::string& path);
  const std::vector<Mesh>& getMesh() const;
  const Transform& getTransform() const;
  void setTransform(const Transform& transform);

 private:
  void load_meshes(const std::string& path);

  // 1 to n meshes
  std::vector<Mesh> _mesh;
  // i think this will be applied to all meshes
  Transform _transform;
  // they can be different types at the same time (color, normal...)
  std::vector<Texture> _textures;
};

#endif  // MODEL_H
