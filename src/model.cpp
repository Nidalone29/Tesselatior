#include "model.h"

#include <iostream>
#include <filesystem>

#include <assimp/Importer.hpp>

#include <assimp/scene.h>

#include "logger.h"
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

  LOG_INFO("The number of embedded textures in the current file is: {}",
           pScene->mNumTextures);

  for (int i = 0; i < pScene->mNumMeshes; i++) {
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

    // NOTE a mesh has one and only one material

    // for now i only import the diffusive property (so the color for non
    // metal materials, because i don't support metallic or else at the
    // moment)

    // NOTE: If a material has not been found by Assimp, it loads a
    // "DefaultMaterial" (with ambient 0, diffuse 0.6 and specular 0.6... and i
    // think it even changes from format to format, for example a .blend file
    // has a different default material than a .obj file)
    const unsigned int material_i = paiMesh->mMaterialIndex;
    const aiMaterial* material = pScene->mMaterials[material_i];
    LOG_INFO("Processing material: {}", material->GetName().C_Str());
    Material x;

    aiColor3D color;

    if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == aiReturn_SUCCESS) {
      x.setAmbientReflectivity(glm::vec3(color.r, color.g, color.b));
    }

    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS) {
      x.setSpecularReflectivity(glm::vec3(color.r, color.g, color.b));
    }

    float shininess = 0.0F;
    if (material->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
      x.setGlossinessExponent(shininess);
    }

    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
      x.setDiffuseReflectivity(glm::vec3(color.r, color.g, color.b));
    }

    aiString diffuse_path;
    if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0),
                      diffuse_path) == aiReturn_SUCCESS) {
      x.setDiffuseReflectivity(glm::vec3(1.0F, 1.0F, 1.0F));
      if (const auto* _texture =
              pScene->GetEmbeddedTexture(diffuse_path.C_Str())) {
        // returned pointer is not null, aka the texture is embedded
        // the texture is compressed (png, jpeg...), so we load it with stb
        if (_texture->mHeight == 0) {
          Texture texture(_texture, TEXTURE_TYPE::DIFFUSE);
          x.addTexture(texture);
        } else {
          // the texture is not compressed
          // _texture->mWidth;
          // _texture->mHeight;
          // _texture->pcData;
          LOG_ERROR("Unsupported uncompressed texture");
          x.addTexture(Texture(TEXTURE_TYPE::DIFFUSE));
        }
      } else {
        // the texture could be an external file, so we try to load it
        std::filesystem::path texture_path = _model_path;
        texture_path.replace_filename(diffuse_path.data);
        // if we can't find a texture with this path, then there will still be
        // a texture created automatically with the default "white.png"
        Texture texture(texture_path, TEXTURE_TYPE::DIFFUSE);
        x.addTexture(texture);
      }
    } else {
      // the default white texture that doesn't influence the base color
      // (because the base color gets multiplied by 1)
      x.addTexture(Texture(TEXTURE_TYPE::DIFFUSE));
    }

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
