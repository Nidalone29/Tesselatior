#include "model_importer.h"

#include <filesystem>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../logger.h"
#include "../utilities.h"
#include "mesh.h"
#include "importer.h"
#include "assimp_importer.h"

#include <glm/gtx/hash.hpp>

IMeshCreator ::~IMeshCreator() {
  //
}

StaticModel* StaticModelCreator::CreateMesh(
    const std::string& name, const std::filesystem::path& model_path_,
    const Options& opts) {
  AssimpImporter import_;
  std::vector<IMesh*> result = import_.import(model_path_, opts);
  // do checks for the mesh (primitive type, mesh count...)
  // add shaders accordingly or throw

  return new StaticModel(name, result,
                         ShaderManager::Instance().GetShader("TriangleShader"));
}

SubDivMesh* SubDivMeshCreator::CreateMesh(
    const std::string& name, const std::filesystem::path& model_path_,
    Options opts) {
  opts.require_single_mesh = true;

  // For now this will by default use ASSIMP
  // TODO in the near future i want to add support for tinyobj because i don't
  // like how assimp imports obj files
  AssimpImporter a;
  IMesh* result = a.import(model_path_, opts).at(0);

  const Shader* s;
  if (TriMesh* d = dynamic_cast<TriMesh*>(result); d != nullptr) {
    s = ShaderManager::Instance().GetShader("TriangleShader");
  } else if (QuadMesh* d = dynamic_cast<QuadMesh*>(result); d != nullptr) {
    s = ShaderManager::Instance().GetShader("QuadsShader");
  } else {
    // TODO mixed meshes
    s = ShaderManager::Instance().GetShader("TriangleShader");
  }

  // TODO deal with shaders and shaders parameters
  return new SubDivMesh(name, result, s);
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
  Material* x = new Material();
  x->AddTexture(Texture(TEXTURE_TYPE::DIFFUSE));

  IMesh* my_mesh;
  HalfEdgeData* curr_hfd = new HalfEdgeData(vertices, halfedges, faces, edges);

  const Shader* s;
  if (in_type == MESH_TYPE::TRI) {
    my_mesh = new TriMesh(curr_hfd, x);
    s = ShaderManager::Instance().GetShader("TriangleShader");
  } else if (in_type == MESH_TYPE::QUAD) {
    my_mesh = new QuadMesh(curr_hfd, x);
    s = ShaderManager::Instance().GetShader("QuadsShader");
  } else {
    // TODO mixed meshes
    throw;  // unsupported
    // s = ShaderManager::Instance().GetShader("TriangleShader");
  }

  return new SubDivMesh(name, my_mesh, s);
}