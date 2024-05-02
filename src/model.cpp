#include "model.h"

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "logger.h"
#include "utilities.h"

Model::Model(const MESH_TYPE type, const std::filesystem::path& path,
             unsigned int flags)
    : type_(type), model_path_(path) {
  // ASSIMP imports the mesh
  LOG_TRACE("Model(const std::filesystem::path&, unsigned int)");
  LoadMeshes(type, flags);
  LOG_INFO("Model created from \"{}\"", path.string());
}

Model::~Model() {
  LOG_TRACE("~Model()");
  LOG_TRACE("Destroying model \"{}\"", model_path_.string());
}

/**
 * @brief we load the meshes in the file
 * @param flags ASSIMP flags
 * https://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410
 */
void Model::LoadMeshes(const MESH_TYPE type, unsigned int flags) {
  Assimp::Importer Importer;

  // flags set to 0 for now, probably have to set them at some point if ASSIMP
  // doesn't deal with different file formats
  const aiScene* pScene =
      Importer.ReadFile(model_path_.string().c_str(), flags);

  if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !pScene->mRootNode) {
    LOG_ERROR("ASSIMP Error: {}", Importer.GetErrorString());
    throw MeshImportException();
  }

  LOG_INFO("The number of embedded textures in the current file is: {}",
           pScene->mNumTextures);

  for (int i = 0; i < pScene->mNumMeshes; i++) {
    const aiMesh* paiMesh = pScene->mMeshes[i];

    const aiVector3D zero_3d(0.0F, 0.0F, 0.0F);

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    for (unsigned int j = 0; j < paiMesh->mNumVertices; j++) {
      const aiVector3D* pPos = &(paiMesh->mVertices[j]);
      const aiVector3D* pNormal = &(paiMesh->mNormals[j]);
      const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0)
                                        ? &(paiMesh->mTextureCoords[0][j])
                                        : &zero_3d;

      Vertices.emplace_back(glm::vec3(pPos->x, pPos->y, pPos->z),
                            glm::vec3(pNormal->x, pNormal->y, pNormal->z),
                            glm::vec2(pTexCoord->x, pTexCoord->y));
    }

    for (unsigned int j = 0; j < paiMesh->mNumFaces; j++) {
      const aiFace& Face = paiMesh->mFaces[j];

      switch (type) {
        case MESH_TYPE::TRIANGLES:
          assert(Face.mNumIndices == to_underlying(MESH_TYPE::TRIANGLES));
          Indices.push_back(Face.mIndices[0]);
          Indices.push_back(Face.mIndices[1]);
          Indices.push_back(Face.mIndices[2]);
          break;
        case MESH_TYPE::QUADS:
          assert(Face.mNumIndices == to_underlying(MESH_TYPE::QUADS));
          // For some reason the first 2 indices need to be swapped otherwise
          // the hardware tessellator does not triangulate the quad properly
          Indices.push_back(Face.mIndices[1]);
          Indices.push_back(Face.mIndices[0]);
          Indices.push_back(Face.mIndices[2]);
          Indices.push_back(Face.mIndices[3]);
          break;
        default:
          throw;
          break;
      }
    }

    unsigned int num_indices_ = Indices.size();

    // NOTE A mesh has one and only one material

    // NOTE If a material has not been found by Assimp, it loads a
    // "DefaultMaterial" (with ambient 0, diffuse 0.6 and specular 0.6... and i
    // think it even changes from format to format, for example a .blend file
    // has a different default material than a .obj file)
    const unsigned int material_i = paiMesh->mMaterialIndex;
    const aiMaterial* material = pScene->mMaterials[material_i];
    LOG_INFO("Processing material: {}", material->GetName().C_Str());
    Material x;

    aiColor3D color;

    if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == aiReturn_SUCCESS) {
      x.ambient_reflectivity(glm::vec3(color.r, color.g, color.b));
    }

    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS) {
      x.specular_reflectivity(glm::vec3(color.r, color.g, color.b));
    }

    float shininess = 0.0F;
    if (material->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
      x.shininess(shininess);
    }

    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
      x.diffuse_reflectivity(glm::vec3(color.r, color.g, color.b));
    }

    aiString diffuse_path;
    if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0),
                      diffuse_path) == aiReturn_SUCCESS) {
      x.diffuse_reflectivity(glm::vec3(1.0F, 1.0F, 1.0F));
      if (const auto* _texture =
              pScene->GetEmbeddedTexture(diffuse_path.C_Str())) {
        // returned pointer is not null, aka the texture is embedded
        // the texture is compressed (png, jpeg...), so we load it with stb
        if (_texture->mHeight == 0) {
          Texture texture(_texture, TEXTURE_TYPE::DIFFUSE);
          x.AddTexture(texture);
        } else {
          // the texture is not compressed
          // _texture->mWidth;
          // _texture->mHeight;
          // _texture->pcData;
          LOG_ERROR("Unsupported uncompressed texture");
          x.AddTexture(Texture(TEXTURE_TYPE::DIFFUSE));
        }
      } else {
        // the texture could be an external file, so we try to load it
        std::filesystem::path texture_path = model_path_;
        texture_path.replace_filename(diffuse_path.data);
        // if we can't find a texture with this path, then there will still be
        // a texture created automatically with the default "white.png"
        Texture texture(texture_path, TEXTURE_TYPE::DIFFUSE);
        x.AddTexture(texture);
      }
    } else {
      // the default white texture that doesn't influence the base color
      // (because the base color gets multiplied by 1)
      x.AddTexture(Texture(TEXTURE_TYPE::DIFFUSE));
    }

    aiString displacement_path;
    if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_DISPLACEMENT, 0),
                      displacement_path) == aiReturn_SUCCESS) {
      if (const auto* _texture =
              pScene->GetEmbeddedTexture(displacement_path.C_Str())) {
        // returned pointer is not null, aka the texture is embedded
        // the texture is compressed (png, jpeg...), so we load it with stb
        if (_texture->mHeight == 0) {
          Texture texture(_texture, TEXTURE_TYPE::DISPLACEMENT);
          x.AddTexture(texture);
        } else {
          // the texture is not compressed
          // _texture->mWidth;
          // _texture->mHeight;
          // _texture->pcData;
          LOG_ERROR("Unsupported uncompressed texture");
          // x.AddTexture(Texture(TEXTURE_TYPE::DISPLACEMENT));
        }
      } else {
        //! For some reason ASSIMP does NOT find a diffuse texture in a obj
        //! material file (even if specified with disp as per
        //! https://en.wikipedia.org/wiki/Wavefront_.obj_file#Texture_maps)
        // the texture could be an external file, so we try to load it
        std::filesystem::path texture_path = model_path_;
        texture_path.replace_filename(displacement_path.data);
        // if we can't find a texture with this path, then there will still be
        // a texture created automatically with the default "black.png"
        Texture texture(texture_path, TEXTURE_TYPE::DISPLACEMENT);
        x.AddTexture(texture);
      }
    } else {
      // Texture texture(Texture(TEXTURE_TYPE::DISPLACEMENT));
    }

    meshes_.emplace_back(Vertices, Indices, num_indices_, x);
  }
}

const std::vector<Mesh>& Model::meshes() const {
  return meshes_;
}

const MESH_TYPE& Model::mesh_type() const {
  return type_;
}

const Transform& Model::transform() const {
  return transform_;
}

void Model::transform(const Transform& transform) {
  transform_ = transform;
}
