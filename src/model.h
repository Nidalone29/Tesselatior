#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "transform.h"

class Model {
 public:
  Model() = delete;  // can't create an empty model
  Model(const std::string& path, unsigned int flags = 0);
  Model(const Model& other) = default;
  Model& operator=(const Model& other) = default;
  Model(Model&& other) = default;
  Model& operator=(Model&& other) = default;

  std::vector<Mesh>& getMeshes();
  const Transform& getTransform() const;
  void setTransform(const Transform& transform);

 private:
  void load_meshes(const std::string& path, unsigned int flags);

  // 1 to n meshes
  std::vector<Mesh> _meshes;
  // i think this will be applied to all meshes
  Transform _transform;
};

#endif  // MODEL_H
