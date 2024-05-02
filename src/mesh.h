#ifndef MESH_H
#define MESH_H

#include <ostream>
#include <vector>
#include <cstring>

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

  /**
   * @brief Construct a new Mesh object
   *
   * @param vertices
   * @param indices
   * @param material
   */
  Mesh(const std::vector<Vertex>& vertices,
       const std::vector<unsigned int>& indices, const unsigned int num_indices,
       const Material& material);
  ~Mesh();

  const GLuint& vao() const;
  const unsigned int& num_indices() const;
  const unsigned int& num_vertices() const;
  const Material& material() const;
  void material(const Material& material);

 private:
  GLuint VAO_;  // Vertex Array Object
  GLuint VBO_;  // Vertex Buffer Object
  GLuint IBO_;  // Index Buffer Object

  std::vector<Vertex> vertices_;
  std::vector<HalfEdge> half_edges_;

  std::vector<unsigned int> indices_;
  unsigned int num_indices_;

  // Rendering properties
  Material material_;
  // the type of meshes that this model stores (quads or tris)
  MESH_TYPE type_;
};

#endif  // MESH_H
