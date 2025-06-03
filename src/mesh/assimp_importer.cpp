#include "assimp_importer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>

#include "../logger.h"
#include "../material.h"

AssimpImporter::AssimpImporter() {
  importer_ = new Assimp::Importer();
  importer_->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
                                aiPrimitiveType_LINE | aiPrimitiveType_POINT);
}

AssimpImporter::~AssimpImporter() {
  delete importer_;
}

std::vector<IMesh*> AssimpImporter::import(
    const std::filesystem::path& filepath, const Options& opts) {
  unsigned int flags = 0;
  if (opts.triangulate) {
    flags |= aiProcess_Triangulate;
  }
  if (opts.pre_transform) {
    flags |= aiProcess_PreTransformVertices;
  }

  const aiScene* p_scene =
      importer_->ReadFile(filepath.string().c_str(), flags);

  if (!p_scene || p_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !p_scene->mRootNode) {
    const std::string err = importer_->GetErrorString();
    LOG_ERROR("ASSIMP Error: {}", err);
    throw MeshImportException();
  }

  if (opts.require_single_mesh && p_scene->mNumMeshes > 1) {
    throw MeshImportException();
  }

  LOG_INFO("The number of embedded textures in the current file is: {}",
           p_scene->mNumTextures);

  std::vector<IMesh*> out_meshes;

  for (int i = 0; i < p_scene->mNumMeshes; i++) {
    const aiMesh* pai_mesh = p_scene->mMeshes[i];
    IMesh* my_mesh;

    const MESH_TYPE current_mesh_type = DetectMeshType(pai_mesh);
    bool to_do_compute_normals = !pai_mesh->HasNormals();

    HalfEdgeData* curr_hfd = GenerateHalfedgeData(pai_mesh, current_mesh_type);

    // NOTE A mesh has one and only one material

    // NOTE If a material has not been found by Assimp, it loads a
    // "DefaultMaterial" (with ambient 0, diffuse 0.6 and specular 0.6... and
    // I think it even changes from format to format, for example a .blend
    // file has a different default material than a .obj file)
    const unsigned int material_i = pai_mesh->mMaterialIndex;
    const aiMaterial* material = p_scene->mMaterials[material_i];
    LOG_INFO("Processing material: {}", material->GetName().C_Str());

    if (current_mesh_type == MESH_TYPE::TRI) {
      my_mesh =
          new TriMesh(curr_hfd, ProcessMaterial(material, p_scene, filepath));
    } else if (current_mesh_type == MESH_TYPE::QUAD) {
      my_mesh =
          new QuadMesh(curr_hfd, ProcessMaterial(material, p_scene, filepath));
    } else {
      // TODO mixed meshes
    }

    if (to_do_compute_normals) {
      my_mesh->ApplySmoothNormals();
    }
    out_meshes.push_back(my_mesh);
  }

  importer_->FreeScene();  // not really necessary because ReadFile() calls it
  return out_meshes;
  // return new StaticModel(name, out_meshes,
  // ShaderManager::Instance().GetShader("TriangleShader"));
}

MESH_TYPE AssimpImporter::DetectMeshType(const aiMesh* pai_mesh) {
  MESH_TYPE current_mesh_type = MESH_TYPE::TRI;
  if (pai_mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE) {
    current_mesh_type = MESH_TYPE::TRI;
  } else if (pai_mesh->mPrimitiveTypes == aiPrimitiveType_POLYGON) {
    // code to autodetect type
    const aiFace& first_ai_face = pai_mesh->mFaces[0];
    unsigned int current_index_num = first_ai_face.mNumIndices;
    for (unsigned int j = 1; j < pai_mesh->mNumFaces; j++) {
      const aiFace& ai_face = pai_mesh->mFaces[j];
      if (ai_face.mNumIndices != current_index_num) {
        current_mesh_type = MESH_TYPE::TRI;
        break;
      }
    }
    if (current_index_num == 4) {
      current_mesh_type = MESH_TYPE::QUAD;
    } else {
      current_mesh_type = MESH_TYPE::POLY;
    }
  } else {
    // TODO fix
    // throw;  // unsupported mesh type (lines and points are not supported)
  }
  return current_mesh_type;
}

HalfEdgeData* AssimpImporter::GenerateHalfedgeData(
    const aiMesh* pai_mesh, const MESH_TYPE current_mesh_type) {
  const aiVector3D zero_3d(0.0F, 0.0F, 0.0F);
  // I only care about the vertices for being contiguous since it is
  // required for OpenGL for rendering
  std::vector<Vertex*>* vertices = new std::vector<Vertex*>();
  std::vector<HalfEdge*>* halfedges = new std::vector<HalfEdge*>();
  std::vector<Face*>* faces = new std::vector<Face*>();
  std::vector<Edge*>* edges = new std::vector<Edge*>();

  vertices->reserve(pai_mesh->mNumVertices);
  halfedges->reserve(to_underlying(current_mesh_type) * pai_mesh->mNumFaces);
  faces->reserve(pai_mesh->mNumFaces);

  const bool has_normals = pai_mesh->HasNormals();

  for (unsigned int j = 0; j < pai_mesh->mNumVertices; j++) {
    const aiVector3D* p_pos = &(pai_mesh->mVertices[j]);
    const aiVector3D* p_normal =
        has_normals ? &(pai_mesh->mNormals[j]) : &zero_3d;
    const aiVector3D* p_tex_coord = pai_mesh->HasTextureCoords(0)
                                        ? &(pai_mesh->mTextureCoords[0][j])
                                        : &zero_3d;

    Vertex* x = new Vertex(glm::vec3(p_pos->x, p_pos->y, p_pos->z),
                           glm::vec3(p_normal->x, p_normal->y, p_normal->z),
                           glm::vec2(p_tex_coord->x, p_tex_coord->y));

    vertices->push_back(x);
  }

  // edges map
  std::map<std::pair<unsigned int, unsigned int>, HalfEdge*> edges_m;

  for (unsigned int j = 0; j < pai_mesh->mNumFaces; j++) {
    const aiFace& ai_face = pai_mesh->mFaces[j];

    std::pair<unsigned int, unsigned int> x;
    std::pair<unsigned int, unsigned int> s;  // swapped
    std::vector<unsigned int> current_indices;
    Face* f = new Face();
    faces->push_back(f);

    switch (current_mesh_type) {
      case MESH_TYPE::TRI:
        current_indices = {0, 1, 2};
        break;
      case MESH_TYPE::QUAD:
        // For some reason the first 2 indices need to be swapped otherwise
        // the hardware tessellator does not triangulate the quad properly
        current_indices = {0, 1, 2, 3};
        break;
      default:
        // TODO unsupported polygon type
        throw;
        break;
    }

    HalfEdge* current_hf;
    std::vector<HalfEdge*> faces_halfedges;
    for (int k = 0; k < current_indices.size(); k++) {
      x = {ai_face.mIndices[k],
           ai_face.mIndices[(k + 1) % to_underlying(current_mesh_type)]};
      s = {x.second, x.first};
      current_hf = new HalfEdge(f);
      faces_halfedges.push_back(current_hf);
      f->halfedge = current_hf;

      // This works because I added the vertices in the same order of Assimp
      current_hf->vert = vertices->at(x.second);
      vertices->at(x.first)->halfedge = current_hf;

      if (edges_m.find(s) != edges_m.end()) {
        current_hf->twin = edges_m[s];
        edges_m[s]->twin = current_hf;
      } else {
        edges_m[x] = current_hf;
      }

      halfedges->push_back(current_hf);
    }

    // setting the next
    for (int z = 0; z < faces_halfedges.size(); z++) {
      faces_halfedges[z]->next =
          faces_halfedges[(z + 1) % faces_halfedges.size()];
    }
  }

  edges->reserve(edges_m.size());
  for (const auto [_, he] : edges_m) {
    Edge* e = new Edge();
    e->halfedge = he;
    he->edge = e;
    if (!he->IsBoundary()) {
      he->twin->edge = e;
    }
    edges->push_back(e);
  }

  return new HalfEdgeData(vertices, halfedges, faces, edges);
}

Material* AssimpImporter::ProcessMaterial(
    const aiMaterial* material, const aiScene* p_scene,
    const std::filesystem::path filepath) {
  Material* x = new Material();

  aiColor3D color;

  if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == aiReturn_SUCCESS) {
    x->ambient_reflectivity(glm::vec3(color.r, color.g, color.b));
  }

  if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS) {
    x->specular_reflectivity(glm::vec3(color.r, color.g, color.b));
  }

  float shininess = 0.0F;
  if (material->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
    x->shininess(shininess);
  }

  if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
    x->diffuse_reflectivity(glm::vec3(color.r, color.g, color.b));
  }

  aiString diffuse_path;
  if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0),
                    diffuse_path) == aiReturn_SUCCESS) {
    x->diffuse_reflectivity(glm::vec3(1.0F, 1.0F, 1.0F));
    if (const auto* _texture =
            p_scene->GetEmbeddedTexture(diffuse_path.C_Str())) {
      // returned pointer is not null, aka the texture is embedded
      // the texture is compressed (png, jpeg...), so we load it with stb
      if (_texture->mHeight == 0) {
        Texture texture(_texture, TEXTURE_TYPE::DIFFUSE);
        x->AddTexture(texture);
      } else {
        // the texture is not compressed
        // _texture->mWidth;
        // _texture->mHeight;
        // _texture->pcData;
        LOG_ERROR("Unsupported uncompressed texture");
        x->AddTexture(Texture(TEXTURE_TYPE::DIFFUSE));
      }
    } else {
      // the texture could be an external file, so we try to load it
      std::filesystem::path texture_path = filepath;
      texture_path.replace_filename(diffuse_path.data);
      // if we can't find a texture with this path, then there will still be
      // a texture created automatically with the default "white.png"
      Texture texture(texture_path, TEXTURE_TYPE::DIFFUSE);
      x->AddTexture(texture);
    }
  } else {
    // the default white texture that doesn't influence the base color
    // (because the base color gets multiplied by 1)
    x->AddTexture(Texture(TEXTURE_TYPE::DIFFUSE));
  }

  aiString displacement_path;
  if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_DISPLACEMENT, 0),
                    displacement_path) == aiReturn_SUCCESS) {
    if (const auto* _texture =
            p_scene->GetEmbeddedTexture(displacement_path.C_Str())) {
      // returned pointer is not null, aka the texture is embedded
      // the texture is compressed (png, jpeg...), so we load it with stb
      if (_texture->mHeight == 0) {
        Texture texture(_texture, TEXTURE_TYPE::DISPLACEMENT);
        x->AddTexture(texture);
      } else {
        // the texture is not compressed
        // _texture->mWidth;
        // _texture->mHeight;
        // _texture->pcData;
        LOG_ERROR("Unsupported uncompressed texture");
        // x.AddTexture(Texture(TEXTURE_TYPE::DISPLACEMENT));
      }
    } else {
      //! For some reason ASSIMP does NOT find a displacement  texture in a
      //! obj material file (even if it is specified with disp as per
      //! https://en.wikipedia.org/wiki/Wavefront_.obj_file#Texture_maps)
      // The texture could be an external file, so we try to load it
      std::filesystem::path texture_path = filepath;
      texture_path.replace_filename(displacement_path.data);
      // if we can't find a texture with this path, then there will still be
      // a texture created automatically with the default "black.png"
      Texture texture(texture_path, TEXTURE_TYPE::DISPLACEMENT);
      x->AddTexture(texture);
    }
  } else {
    // Texture texture(Texture(TEXTURE_TYPE::DISPLACEMENT));
  }

  return x;
}
