#ifndef MODEL_H
#define MODEL_H

#include <filesystem>
#include <initializer_list>

#include "mesh.h"
#include "transform.h"

class Model {
 public:
  Model() = default;
  Model(const MESH_TYPE type, const std::filesystem::path& path,
        unsigned int flags = 0);

  Model(const MESH_TYPE in_type, const std::vector<Vertex>& in_vertices,
        const std::vector<unsigned int>& in_indices);

  ~Model();
  Model(const Model& other) = delete;
  Model& operator=(const Model& other) = delete;
  Model(Model&& other) = delete;
  Model& operator=(Model&& other) = delete;

  const std::vector<Mesh>& meshes() const;
  void meshes(const std::initializer_list<Mesh>& in);
  const MESH_TYPE& mesh_type() const;
  const Transform& transform() const;
  void transform(const Transform& transform);

 private:
  void LoadMeshes(const MESH_TYPE type, unsigned int flags);

  MESH_TYPE type_;
  // 1 to n meshes (for static meshes)
  // only 1 for progressive, subdiv and terrain
  std::vector<Mesh> meshes_;
  // be applied to all meshes of the model
  Transform transform_;

  std::filesystem::path model_path_;
};

#endif  // MODEL_H
