/*

  Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MESH_H
#define MESH_H

#include <ostream>
#include <vector>
#include <GL/glew.h>
#include "common.h"
#include "texture2D.h"
#include "glm/glm.hpp"
#include <cstring>
#include <assimp/scene.h>        // Assimp output data structure
#include <assimp/postprocess.h>  // Assimp post processing flags

/**
 * Classe che incapsula la gestione dei modelli 3d caricati da file.
 * La classe usa la lista dei vertici indicizzati.
 * Al momento la classe supporta modelli con una sola texture colore.
 * Se il modello non ha una texture associata, viene usata una texture
 * di default "white.png"
 */
class Mesh {
 public:
  Mesh();
  ~Mesh();

  /**
   * Funzione che carica il modello e lo prepara per il rendering.
   *
   * @param filename nome del file
   * @param flags assimp post processing flags
   *
   * @return true se il modello è stato caricato correttamente
   */
  bool load_mesh(const std::string& Filename, unsigned int flags = 0);

  /**
   * Renderizza l'oggetto in scena usando per la texture, la TextureUnit
   * indicata.
   *
   * @param TextureUnit TextureUnit usata per recuperare i pixel
   *
   */
  virtual void render();

 private:
  bool init_from_scene(const aiScene* pScene, const std::string& Filename);

  void clear();

  std::string get_file_path(const std::string& Filename) const;

  unsigned int _num_indices;
  Texture2D _texture;
  GLuint _VAO;
  GLuint _VBO;
  GLuint _IBO;
};

std::ostream& operator<<(std::ostream& os, const Vertex& v);

#endif  // MESH_H
