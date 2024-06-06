#ifndef MESH_H
#define MESH_H

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>        // Assimp output data structure
#include <assimp/postprocess.h>  // Assimp post processing flags

#include "vertex.h"
#include "texture.h"
#include "material.h"

enum class ATTRIB_ID {
  POSITIONS = 0,
  NORMALS = 1,
  TEXTURE_COORDS = 2
};

enum class MESH_TYPE {
  TRIANGLES = 3,
  QUADS = 4
};

/**
 * @brief A mesh is saved in a renederable state for opengl
 *
 */
class Mesh {
 public:
  Mesh() = delete;  // can't create an empty mesh

  Mesh(const MESH_TYPE type, std::vector<Vertex*>* vertices,
       std::vector<HalfEdge*>* halfedges, std::vector<Face*>* faces,
       std::vector<Edge*>* edges, const Material& material);
  ~Mesh();

  Mesh(const Mesh& other);
  Mesh& operator=(const Mesh& other);
  Mesh(Mesh&& other) = default;
  Mesh& operator=(Mesh&& other) = default;

  const GLuint& vao() const;
  unsigned int num_indices() const;
  unsigned int num_vertices() const;

  std::vector<Vertex*>* vertices();
  std::vector<HalfEdge*>* half_edges();
  std::vector<Face*>* faces();
  std::vector<Edge*>* edges();

  const Material& material() const;
  void material(const Material& material);

  void split(Edge* e);
  void flip(const Edge* e);

  void GenerateOpenGLBuffers();

 private:
  [[nodiscard]] std::vector<unsigned int>* CreateIndexBuffer() const;
  [[nodiscard]] std::vector<Vertex>* CreateVertexBuffer() const;

  void ClearOpenGLBuffers();

  GLuint VAO_;  // Vertex Array Object
  GLuint VBO_;  // Vertex Buffer Object
  GLuint IBO_;  // Index Buffer Object

  std::vector<Vertex*>* vertices_;
  std::vector<HalfEdge*>* half_edges_;
  std::vector<Face*>* faces_;
  std::vector<Edge*>* edges_;

  // Rendering properties
  Material material_;
  // the type of meshes that this model stores (quads or tris)
  MESH_TYPE type_;
};

#endif  // MESH_H
