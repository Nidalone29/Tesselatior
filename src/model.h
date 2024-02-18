#ifndef MODEL_H
#define MODEL_H

#include <filesystem>

#include "mesh.h"
#include "transform.h"

class Model {
 public:
  Model() = delete;
  explicit Model(const std::filesystem::path& path, unsigned int flags = 0);
  ~Model();
  Model(const Model& other) = default;
  Model& operator=(const Model& other) = default;
  Model(Model&& other) = default;
  Model& operator=(Model&& other) = default;

  const std::vector<Mesh>& meshes() const;
  const Transform& transform() const;
  void transform(const Transform& transform);

 private:
  void LoadMeshes(unsigned int flags);

  // 1 to n meshes
  std::vector<Mesh> meshes_;
  // be applied to all meshes of the model
  Transform transform_;

  std::filesystem::path model_path_;
};

#endif  // MODEL_H
