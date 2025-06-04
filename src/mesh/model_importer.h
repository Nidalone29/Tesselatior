#ifndef MODEL_H
#define MODEL_H

#include <filesystem>

#include "importer.h"
#include "object.h"
#include "mesh.h"

// factory method class
class IMeshCreator {
 public:
  virtual ~IMeshCreator() = 0;
};

class StaticModelCreator final : public IMeshCreator {
 public:
  ~StaticModelCreator() = default;
  // it creates a mesh of either triangles or quads or polys
  StaticModel* CreateMesh(const std::string& name,
                          const std::filesystem::path& model_path_,
                          const Options& opts = Options());

  // StaticModel* CreateMesh(std::string url) {}

 private:
};

class SubDivMeshCreator : public IMeshCreator {
 public:
  ~SubDivMeshCreator() = default;

  SubDivMesh* CreateMesh(const std::string& name,
                         const std::filesystem::path& model_path_,
                         Options opts = Options());

  SubDivMesh* CreateMesh(const std::string& name, const MESH_TYPE in_type,
                         const std::vector<Vertex>& in_vertices,
                         const std::vector<unsigned int>& in_indices);

 private:
};

#endif  // MODEL_H
