#include "model.h"

#include <assimp/Importer.hpp>

#include <iostream>

std::string get_file_path(const std::string& Filename) {
  std::string::size_type SlashIndex = Filename.find_last_of("/");
  std::string Dir;

  if (SlashIndex == std::string::npos) {
    Dir = ".";
  } else if (SlashIndex == 0) {
    Dir = "/";
  } else {
    Dir = Filename.substr(0, SlashIndex);
  }

  return Dir;
}

Model::Model(const std::string& path, unsigned int flags) {
  // ASSIMP imports the mesh
  load_meshes(path, flags);
  std::cout << "model created" << std::endl;
}

/**
 * @brief we load the meshes in the file
 * @param path 3D model file location
 */
void Model::load_meshes(const std::string& path, unsigned int flags) {
  Assimp::Importer Importer;

  // flags set to 0 for now, probably have to set them at some point if ASSIMP
  // doesnt deal with different file formats
  const aiScene* pScene = Importer.ReadFile(path.c_str(), flags);

  std::vector<Mesh> meshes;

  // dealing with every mesh in the model
  for (int i = 0; i < pScene->mNumMeshes; i++) {
    // -- Dealing with a Mesh
    const aiMesh* paiMesh = pScene->mMeshes[i];

    const aiVector3D Zero3D(0.0F, 0.0F, 0.0F);

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
      const aiVector3D* pPos = &(paiMesh->mVertices[i]);
      const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
      const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0)
                                        ? &(paiMesh->mTextureCoords[0][i])
                                        : &Zero3D;

      Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
               glm::vec3(pNormal->x, pNormal->y, pNormal->z),
               glm::vec2(pTexCoord->x, pTexCoord->y));

      Vertices.push_back(v);
    }

    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
      const aiFace& Face = paiMesh->mFaces[i];
      assert(Face.mNumIndices == 3);
      Indices.push_back(Face.mIndices[0]);
      Indices.push_back(Face.mIndices[1]);
      Indices.push_back(Face.mIndices[2]);
    }

    unsigned int _num_indices = Indices.size();

    // -- Deal with material

    // NOTE a mesh has one and only one material

    // for now i only consider the diffusive property (so the color for non
    // metal materials, because i don't support metallic at the moment)

    const unsigned int material_i = paiMesh->mMaterialIndex;
    const aiMaterial* material = pScene->mMaterials[material_i];
    // TODO deal with all material properties
    Material x;

    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
      aiString Path;

      if (material->GetTexture(aiTextureType_DIFFUSE, 0, &Path, nullptr,
                               nullptr, nullptr, nullptr,
                               nullptr) == AI_SUCCESS) {
        std::string data = Path.data;
        std::string Filepath = get_file_path(path);
        std::string FullPath = Filepath + "/" + data;
        // e mo si carica la texture con stb_image
        // TODO the type will become an enum eventually
        Texture texture(FullPath, "diffusive");
        x.addTexture(texture);
      }
    } else {
      // texture does not exist, load blank
      Texture texture("white.png", "diffusive");
      x.addTexture(texture);
    }
    // add the mesh to the model
    _meshes.push_back(Mesh(Vertices, Indices, _num_indices, x));
  }
}

std::vector<Mesh>& Model::getMeshes() {
  return _meshes;
}

const Transform& Model::getTransform() const {
  return _transform;
}

void Model::setTransform(const Transform& transform) {
  _transform = transform;
}
