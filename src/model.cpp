#include "model.h"

#include <iostream>
#include <filesystem>

#include <assimp/Importer.hpp>

#include "logger.h"
#include "common.h"
#include "utilities.h"

Model::Model(const std::filesystem::path& path, unsigned int flags)
    : _model_path(path) {
  // ASSIMP imports the mesh
  LOG_TRACE("Model(const std::filesystem::path&, unsigned int)");
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

  if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !pScene->mRootNode) {
    LOG_ERROR("ASSIMP Error: {}", Importer.GetErrorString());
    throw MeshImportException();
  }

  // dealing with every mesh in the model
  for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
    // -- Dealing with a Mesh
    const aiMesh* paiMesh = pScene->mMeshes[i];

    const aiVector3D Zero3D(0.0F, 0.0F, 0.0F);

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    for (unsigned int j = 0; j < paiMesh->mNumVertices; j++) {
      const aiVector3D* pPos = &(paiMesh->mVertices[j]);
      const aiVector3D* pNormal = &(paiMesh->mNormals[j]);
      const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0)
                                        ? &(paiMesh->mTextureCoords[0][j])
                                        : &Zero3D;

      Vertices.emplace_back(glm::vec3(pPos->x, pPos->y, pPos->z),
                            glm::vec3(pNormal->x, pNormal->y, pNormal->z),
                            glm::vec2(pTexCoord->x, pTexCoord->y));
    }

    for (unsigned int j = 0; j < paiMesh->mNumFaces; j++) {
      const aiFace& Face = paiMesh->mFaces[j];
      // TODO pass triangulate by default
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
        Texture texture(texture_path, DIFFUSE_TEXTURE);
        x.addTexture(texture);
      }
    } else {
      // texture does not exist, load blank
      Texture texture("white.png", DIFFUSE_TEXTURE);
      x.addTexture(texture);
    }
    // add the mesh to the model
    _meshes.emplace_back(Vertices, Indices, _num_indices, x);
  }
}

const std::vector<Mesh>& Model::getMeshes() const {
  return _meshes;
}

const Transform& Model::getTransform() const {
  return _transform;
}

void Model::setTransform(const Transform& transform) {
  _transform = transform;
}
