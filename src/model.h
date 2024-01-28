#ifndef MODEL_H
#define MODEL_H

#include <filesystem>

#include "mesh.h"
#include "transform.h"

class Model {
 public:
  Model() = delete;  // can't create an empty model
  explicit Model(const std::filesystem::path& path, unsigned int flags = 0);
  ~Model();
  Model(const Model& other) = default;
  Model& operator=(const Model& other) = default;
  Model(Model&& other) = default;
  Model& operator=(Model&& other) = default;

  const std::vector<Mesh>& getMeshes() const;
  const Transform& getTransform() const;
  void setTransform(const Transform& transform);

 private:
  void load_meshes(unsigned int flags);

  // 1 to n meshes
  std::vector<Mesh> _meshes;
  // be applied to all meshes of the model
  Transform _transform;

  std::filesystem::path _model_path;
};

#endif  // MODEL_H
