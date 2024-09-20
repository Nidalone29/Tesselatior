#include "model_importer.h"

#include <filesystem>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "logger.h"
#include "utilities.h"
#include "mesh.h"

#include <glm/gtx/hash.hpp>

IMeshCreator ::~IMeshCreator() {
  //
}

StaticModel* StaticModelCreator::CreateMesh(
    const std::string& name, const std::filesystem::path& model_path_) {
  using Uint = unsigned int;
  Assimp::Importer importer;
  importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
                              aiPrimitiveType_LINE | aiPrimitiveType_POINT);

  // flags set to 0 for now, probably have to set them at some point if ASSIMP
  // doesn't deal with different file formats
  const aiScene* p_scene = importer.ReadFile(model_path_.string().c_str(), 0);

  if (!p_scene || p_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !p_scene->mRootNode) {
    LOG_ERROR("ASSIMP Error: {}", importer.GetErrorString());
    throw MeshImportException();
  }

  LOG_INFO("The number of embedded textures in the current file is: {}",
           p_scene->mNumTextures);

  std::vector<IMesh*> out_meshes;

  for (int i = 0; i < p_scene->mNumMeshes; i++) {
    const aiMesh* pai_mesh = p_scene->mMeshes[i];
    IMesh* my_mesh;

    // TODO Make the following if a lambda (or another function) and make
    // current_mesh_type const https://stackoverflow.com/a/3669353/11285697
    /* const */ MESH_TYPE current_mesh_type;

    // now we need to autodetect the mesh type
    if (pai_mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE) {
      current_mesh_type = MESH_TYPE::TRI;
    } else if (pai_mesh->mPrimitiveTypes == aiPrimitiveType_POLYGON) {
      // code to autodetect type
      const aiFace& firtst_ai_face = pai_mesh->mFaces[0];
      Uint current_index_num = firtst_ai_face.mNumIndices;
      for (Uint j = 1; j < pai_mesh->mNumFaces; j++) {
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
      throw;  // unsupported mesh type (lines and points are not supported)
    }

    // TODO support for meshes with different polygons
    // basically need to import it as is and then triangulate it for rendering
    if (current_mesh_type == MESH_TYPE::POLY) {
      // p_scene = importer.ApplyPostProcessing(aiProcess_SortByPType);
      // need to triangulate...
      throw;  // for now, will support later
    }

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

    for (Uint j = 0; j < pai_mesh->mNumVertices; j++) {
      const aiVector3D* p_pos = &(pai_mesh->mVertices[j]);
      const aiVector3D* p_normal = &(pai_mesh->mNormals[j]);
      const aiVector3D* p_tex_coord = pai_mesh->HasTextureCoords(0)
                                          ? &(pai_mesh->mTextureCoords[0][j])
                                          : &zero_3d;

      Vertex* x = new Vertex(glm::vec3(p_pos->x, p_pos->y, p_pos->z),
                             glm::vec3(p_normal->x, p_normal->y, p_normal->z),
                             glm::vec2(p_tex_coord->x, p_tex_coord->y));

      vertices->push_back(x);
    }

    // edges map
    std::map<std::pair<Uint, Uint>, HalfEdge*> edges_m;

    for (Uint j = 0; j < pai_mesh->mNumFaces; j++) {
      const aiFace& ai_face = pai_mesh->mFaces[j];

      std::pair<Uint, Uint> x;
      std::pair<Uint, Uint> s;  // swapped
      std::vector<Uint> current_indices;
      Face* f = new Face();
      faces->push_back(f);

      switch (current_mesh_type) {
        case MESH_TYPE::TRI:
          current_indices = {0, 1, 2};
          break;
        case MESH_TYPE::QUAD:
          // For some reason the first 2 indices need to be swapped otherwise
          // the hardware tessellator does not triangulate the quad properly
          current_indices = {1, 0, 2, 3};
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

    HalfEdgeData* curr_hfd =
        new HalfEdgeData(vertices, halfedges, faces, edges);

    // NOTE A mesh has one and only one material

    // NOTE If a material has not been found by Assimp, it loads a
    // "DefaultMaterial" (with ambient 0, diffuse 0.6 and specular 0.6... and
    // I think it even changes from format to format, for example a .blend
    // file has a different default material than a .obj file)
    const unsigned int material_i = pai_mesh->mMaterialIndex;
    const aiMaterial* material = p_scene->mMaterials[material_i];
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
              p_scene->GetEmbeddedTexture(diffuse_path.C_Str())) {
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
              p_scene->GetEmbeddedTexture(displacement_path.C_Str())) {
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
        //! For some reason ASSIMP does NOT find a displacement  texture in a
        //! obj material file (even if it is specified with disp as per
        //! https://en.wikipedia.org/wiki/Wavefront_.obj_file#Texture_maps)
        // The texture could be an external file, so we try to load it
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

    if (current_mesh_type == MESH_TYPE::TRI) {
      my_mesh = new TriMesh(curr_hfd, x);
    } else if (current_mesh_type == MESH_TYPE::TRI) {
      my_mesh = new QuadMesh(curr_hfd, x);
    } else {
      // TODO mixed meshes
    }
    out_meshes.push_back(my_mesh);
  }

  return new StaticModel(name, out_meshes,
                         ShaderManager::Instance().GetShader("TriangleShader"));
}

SubDivMesh* SubDivMeshCreator::CreateMesh(
    const std::string& name, const std::filesystem::path& model_path_) {
  using Uint = unsigned int;
  Assimp::Importer importer;
  importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
                              aiPrimitiveType_LINE | aiPrimitiveType_POINT);

  // flags set to 0 for now, probably have to set them at some point if ASSIMP
  // doesn't deal with different file formats
  const aiScene* p_scene = importer.ReadFile(model_path_.string().c_str(),
                                             aiProcess_JoinIdenticalVertices);

  if (!p_scene || p_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !p_scene->mRootNode) {
    LOG_ERROR("ASSIMP Error: {}", importer.GetErrorString());
    throw MeshImportException();
  }

  LOG_INFO("The number of embedded textures in the current file is: {}",
           p_scene->mNumTextures);

  if (p_scene->mNumMeshes > 1) {
    throw;  // we need a mesh not sn entire scene
  }

  const aiMesh* pai_mesh = p_scene->mMeshes[0];
  IMesh* my_mesh;

  // TODO Make the following if a lambda (or another function) and make
  // current_mesh_type const https://stackoverflow.com/a/3669353/11285697
  /* const */ MESH_TYPE current_mesh_type;

  // now we need to autodetect the mesh type
  if (pai_mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE) {
    current_mesh_type = MESH_TYPE::TRI;
  } else if (pai_mesh->mPrimitiveTypes == aiPrimitiveType_POLYGON) {
    // code to autodetect type
    const aiFace& firtst_ai_face = pai_mesh->mFaces[0];
    Uint current_index_num = firtst_ai_face.mNumIndices;
    for (Uint j = 1; j < pai_mesh->mNumFaces; j++) {
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
    throw;  // unsupported mesh type (lines and points are not supported)
  }

  // TODO support for meshes with different polygons
  // basically need to import it as is and then triangulate it for rendering
  if (current_mesh_type == MESH_TYPE::POLY) {
    // p_scene = importer.ApplyPostProcessing(aiProcess_SortByPType);
    // need to triangulate...
    throw;  // for now, will support later
  }

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

  bool to_do_compute_normals = !pai_mesh->HasNormals();

  for (Uint j = 0; j < pai_mesh->mNumVertices; j++) {
    const aiVector3D* p_pos = &(pai_mesh->mVertices[j]);
    const aiVector3D* p_normal =
        pai_mesh->HasNormals() ? &(pai_mesh->mNormals[j]) : &zero_3d;
    const aiVector3D* p_tex_coord = pai_mesh->HasTextureCoords(0)
                                        ? &(pai_mesh->mTextureCoords[0][j])
                                        : &zero_3d;

    Vertex* x = new Vertex(glm::vec3(p_pos->x, p_pos->y, p_pos->z),
                           glm::vec3(p_normal->x, p_normal->y, p_normal->z),
                           glm::vec2(p_tex_coord->x, p_tex_coord->y));

    vertices->push_back(x);
  }
  // edges map
  std::map<std::pair<Uint, Uint>, HalfEdge*> edges_m;

  for (Uint j = 0; j < pai_mesh->mNumFaces; j++) {
    const aiFace& ai_face = pai_mesh->mFaces[j];

    LOG_INFO("-------");
    std::pair<Uint, Uint> x;
    std::pair<Uint, Uint> s;  // swapped
    std::vector<Uint> current_indices;
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
      x = {ai_face.mIndices[current_indices[k]],
           ai_face.mIndices[current_indices[(k + 1) %
                                            to_underlying(current_mesh_type)]]};

      s = {x.second, x.first};

      LOG_INFO("{} - {}", x.first, x.second);

      current_hf = new HalfEdge(f);
      faces_halfedges.push_back(current_hf);
      f->halfedge = current_hf;

      // This works because I added the vertices in the same order of Assimp
      current_hf->vert = vertices->at(x.second);
      vertices->at(x.first)->halfedge = current_hf;

      // this doesn't work and I don't know why
      if (edges_m.find(s) != edges_m.end()) {
        current_hf->twin = edges_m[s];
        edges_m[s]->twin = current_hf;
      } else {
        edges_m[x] = current_hf;
      }

      halfedges->push_back(current_hf);
    }
    LOG_INFO("-----");
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

  HalfEdgeData* curr_hfd = new HalfEdgeData(vertices, halfedges, faces, edges);

  // NOTE A mesh has one and only one material

  // NOTE If a material has not been found by Assimp, it loads a
  // "DefaultMaterial" (with ambient 0, diffuse 0.6 and specular 0.6... and
  // I think it even changes from format to format, for example a .blend
  // file has a different default material than a .obj file)
  const unsigned int material_i = pai_mesh->mMaterialIndex;
  const aiMaterial* material = p_scene->mMaterials[material_i];
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
            p_scene->GetEmbeddedTexture(diffuse_path.C_Str())) {
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
            p_scene->GetEmbeddedTexture(displacement_path.C_Str())) {
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
      //! For some reason ASSIMP does NOT find a displacement  texture in a
      //! obj material file (even if it is specified with disp as per
      //! https://en.wikipedia.org/wiki/Wavefront_.obj_file#Texture_maps)
      // The texture could be an external file, so we try to load it
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

  const Shader* s;
  if (current_mesh_type == MESH_TYPE::TRI) {
    my_mesh = new TriMesh(curr_hfd, x);
    s = ShaderManager::Instance().GetShader("TriangleShader");
  } else if (current_mesh_type == MESH_TYPE::QUAD) {
    my_mesh = new QuadMesh(curr_hfd, x);
    s = ShaderManager::Instance().GetShader("TerrainShader");
  } else {
    // TODO mixed meshes
    s = ShaderManager::Instance().GetShader("TriangleShader");
  }

  if (to_do_compute_normals) {
    my_mesh->ApplySmoothNormals();
  }

  // TODO deal with shaders and shaders parameters
  return new SubDivMesh(name, my_mesh, s);
}

SubDivMesh* SubDivMeshCreator::CreateMesh(
    const std::string& name, const MESH_TYPE in_type,
    const std::vector<Vertex>& in_vertices,
    const std::vector<unsigned int>& in_indices) {
  using Uint = unsigned int;
  // the path should be empty, because in this case we are just creating the
  // model from code
  // I only care about the vertices for being contiguous since it is required
  // for OpenGL for rendering
  std::vector<Vertex*>* vertices = new std::vector<Vertex*>();
  std::vector<HalfEdge*>* halfedges = new std::vector<HalfEdge*>();
  std::vector<Face*>* faces = new std::vector<Face*>();
  std::vector<Edge*>* edges = new std::vector<Edge*>();

  // TODO REFACTOR
  const int indx_x_face = to_underlying(in_type);

  vertices->reserve(in_vertices.size());
  halfedges->reserve(in_indices.size());
  faces->reserve(in_indices.size() / indx_x_face);

  for (Uint i = 0; i < in_vertices.size(); i++) {
    Vertex* x = new Vertex(in_vertices[i].position, in_vertices[i].normal,
                           in_vertices[i].text_coords);
    vertices->push_back(x);
  }

  // edges map
  std::map<std::pair<Uint, Uint>, HalfEdge*> edges_m;

  for (int i = 0; i < in_indices.size(); i += indx_x_face) {
    std::pair<Uint, Uint> x;
    std::pair<Uint, Uint> s;  // swapped
    std::vector<Uint> current_indices;
    Face* f = new Face();
    faces->push_back(f);

    switch (in_type) {
      case MESH_TYPE::TRI:
        assert(indx_x_face == to_underlying(MESH_TYPE::TRI));
        current_indices = {0, 1, 2};
        break;
      case MESH_TYPE::QUAD:
        assert(indx_x_face == to_underlying(MESH_TYPE::QUAD));
        // For some reason the first 2 indices need to be swapped otherwise
        // the hardware tessellator does not triangulate the quad properly
        current_indices = {1, 0, 2, 3};
        break;
      default:
        // TODO unsupported polygon type
        throw;
        break;
    }

    HalfEdge* current_hf;
    std::vector<HalfEdge*> faces_halfedges;
    for (int k = 0; k < current_indices.size(); k++) {
      x = {in_indices[i + k],
           in_indices[((k + 1) % to_underlying(in_type)) + i]};
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
  // just a default material
  Material x;
  x.AddTexture(Texture(TEXTURE_TYPE::DIFFUSE));

  IMesh* my_mesh;
  HalfEdgeData* curr_hfd = new HalfEdgeData(vertices, halfedges, faces, edges);

  if (in_type == MESH_TYPE::TRI) {
    my_mesh = new TriMesh(curr_hfd, x);
  } else if (in_type == MESH_TYPE::QUAD) {
    my_mesh = new QuadMesh(curr_hfd, x);
  } else {
    // TODO mixed meshes
  }

  return new SubDivMesh(name, my_mesh,
                        ShaderManager::Instance().GetShader("TriangleShader"));
}