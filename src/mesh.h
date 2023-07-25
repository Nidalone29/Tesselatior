#ifndef MESH_H
#define MESH_H

#include <ostream>
#include <vector>
#include <GL/glew.h>
#include "common.h"
#include "vertex.h"
#include "texture.h"
#include "material.h"
#include <glm/glm.hpp>
#include <cstring>
#include <assimp/scene.h>        // Assimp output data structure
#include <assimp/postprocess.h>  // Assimp post processing flags

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
   * @param path path to the file of the mesh (obj file) from the current
   * directory (of the executable) so models/etc...
   * @param flags ASSIMP flags
   */
  Mesh(const std::string& path, unsigned int flags = 0);
  ~Mesh() = default;

  /**
   * Funzione che carica il modello e lo prepara per il rendering.
   *
   * @param filename nome del file
   * @param flags assimp post processing flags
   * (https://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410)
   *
   * @return true se il modello è stato caricato correttamente
   */
  void load_mesh(const std::string& Filename, unsigned int flags = 0);

  /**
   * Renderizza l'oggetto in scena usando per la texture, la TextureUnit
   * indicata.
   *
   * @param TextureUnit TextureUnit usata per recuperare i pixel
   *
   */
  virtual void render();

 private:
  void init_from_scene(const aiScene* pScene, const std::string& Filename);

  void clear();

  std::string get_file_path(const std::string& Filename) const;

  unsigned int _num_indices;
  GLuint _VAO;  // vertex Array Object
  GLuint _VBO;  // vertex Buffer Object
  GLuint _IBO;  // index Buffer Object

  std::vector<Vertex> _vertices;
  std::vector<unsigned int> _indices;
  Material _material;
};

std::ostream& operator<<(std::ostream& os, const Vertex& v);

#endif  // MESH_H
