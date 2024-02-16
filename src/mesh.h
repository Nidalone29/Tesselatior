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
  COLOR_TEXTURE_COORDS = 2
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

  /**
   * @brief
   *
   */
  void load();

  const GLuint& getVAO() const;
  const unsigned int& get_num_indices() const;
  const Material& getMaterial() const;

 private:
  void clear();

  GLuint _VAO;  // Vertex Array Object
  GLuint _VBO;  // Vertex Buffer Object
  GLuint _IBO;  // Index Buffer Object

  std::vector<Vertex> _vertices;
  std::vector<unsigned int> _indices;
  unsigned int _num_indices;
  Material _material;
};

std::ostream& operator<<(std::ostream& os, const Vertex& v);

#endif  // MESH_H
