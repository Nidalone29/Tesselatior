#ifndef MODEL_H
#define MODEL_H

#include <filesystem>

#include <assimp/mesh.h>

#include "object.h"
#include "mesh.h"

// factory method class
// the way that you can create a renderable object are very different for each
// type of object. If I want to support a virtual variadic function I have to
// mess a lot with templates. So for now this is what it is (maybe use mixins
// with CRTP?)
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
                          unsigned int flags = 0);

  // StaticModel* CreateMesh(std::string url) {}

 private:
};

class SubDivMeshCreator : public IMeshCreator {
 public:
  ~SubDivMeshCreator() = default;

  SubDivMesh* CreateMesh(const std::string& name,
                         const std::filesystem::path& model_path_,
                         unsigned int flags = 0);

  SubDivMesh* CreateMesh(const std::string& name, const MESH_TYPE in_type,
                         const std::vector<Vertex>& in_vertices,
                         const std::vector<unsigned int>& in_indices);

 private:
};

#endif  // MODEL_H
