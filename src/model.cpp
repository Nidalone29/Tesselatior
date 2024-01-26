#include "model.h"

#include <iostream>
#include <filesystem>

#include <assimp/Importer.hpp>

#include "logger.h"

Model::Model(const std::filesystem::path& path, unsigned int flags)
    : _model_path(path) {
  // ASSIMP imports the mesh
  LOG_TRACE("Model()");
  load_meshes(flags);
  LOG_INFO("Model created from \"{}\"", path.string());
}

Model::~Model() {
  LOG_TRACE("~Model()");
  LOG_TRACE("Destroying model \"{}\"", _model_path.string());
}

/**
 * @brief we load the meshes in the file
 * @param flags ASSIMP flags
 * https://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410
 */
void Model::load_meshes(unsigned int flags) {
  Assimp::Importer Importer;

  // flags set to 0 for now, probably have to set them at some point if ASSIMP
  // doesnt deal with different file formats
  const aiScene* pScene =
      Importer.ReadFile(_model_path.string().c_str(), flags);

  std::vector<Mesh> meshes;

  // dealing with every mesh in the model
  for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
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
        // e mo si carica la texture con stb_image
        // TODO the type will become an enum eventually
        std::filesystem::path texture_path = _model_path;
        texture_path.replace_filename(Path.data);  // Path.data
        Texture texture(texture_path, "diffusive");
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
