#ifndef ASSIMP_IMPORTER_H
#define ASSIMP_IMPORTER_H

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/material.h>

#include "mesh.h"
#include "importer.h"

class AssimpImporter : public IImporter {
 public:
  AssimpImporter();
  AssimpImporter(const AssimpImporter& other) = delete;
  AssimpImporter& operator=(const AssimpImporter& other) = delete;
  AssimpImporter(AssimpImporter&& other) = delete;
  AssimpImporter&& operator=(AssimpImporter&& other) = delete;
  ~AssimpImporter();

  std::vector<IMesh*> import(const std::filesystem::path& filepath,
                             const Options& opts = Options()) override;

 private:
  MESH_TYPE DetectMeshType(const aiMesh* pai_mesh);
  HalfEdgeData* GenerateHalfedgeData(const aiMesh* pai_mesh,
                                     const MESH_TYPE current_mesh_type);

  Material* ProcessMaterial(const aiMaterial* material, const aiScene* p_scene,
                            const std::filesystem::path filepath);

  Assimp::Importer* importer_;
};

#endif  // ASSIMP_IMPORTER_H
