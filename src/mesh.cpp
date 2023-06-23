
#include "mesh.h"

#include <assimp/Importer.hpp>  // Assimp Importer object
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Vertex& v) {
  os << "[" << v.position.x << ", " << v.position.y << ", " << v.position.z
     << "] ";
  os << "(" << v.normal.x << ", " << v.normal.y << ", " << v.normal.z << ") ";
  os << "<" << v.textcoord.x << ", " << v.textcoord.y << "> ";

  return os;
}

/*
Mesh::Vertex::Vertex() {}

Mesh::Vertex::Vertex(const glm::vec3& p, const glm::vec3& n,const glm::vec2& t)
: position(p), textcoord(t), normal(n) {
}
*/

Mesh::Mesh() : _VAO(-1), _VBO(-1), _IBO(-1), _num_indices(0) {}

Mesh::~Mesh() { clear(); }

void Mesh::clear() {
  glDeleteBuffers(1, &_VBO);
  _VBO = -1;
  glDeleteBuffers(1, &_IBO);
  _IBO = -1;
  glDeleteVertexArrays(1, &_VAO);
  _VAO = -1;
  _num_indices = 0;
}

bool Mesh::load_mesh(const std::string& Filename, unsigned int flags) {
  // Release the previously loaded mesh (if it exists)
  clear();

  bool Ret = false;
  Assimp::Importer Importer;

  std::cout << "Loading '" << Filename << "'" << std::endl;
  const aiScene* pScene = Importer.ReadFile(
      Filename.c_str(),
      flags);  // aiProcess_Triangulate | aiProcess_GenSmoothNormals);// |
               // aiProcess_FlipUVs);

  std::string Filepath = get_file_path(Filename);

  if (pScene) {
    Ret = init_from_scene(pScene, Filepath);
  } else {
    std::cout << "Error loading " << Filename << " : "
              << Importer.GetErrorString() << std::endl;
  }

  return Ret;
}

bool Mesh::init_from_scene(const aiScene* pScene, const std::string& Filepath) {
  // Copiamo i dati dal formato Assimp agli array di vertici e indici
  //
  // NOTA: CONSIDERIAMO SOLO *UNA* MESH E LA RELATIVA TEXTURE COLORE

  bool Ret = true;
  std::vector<Vertex> Vertices;
  std::vector<unsigned int> Indices;

  const aiMesh* paiMesh =
      pScene->mMeshes[0];  // consideriamo solo una mesh (mesh 0)

  const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

  for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
    const aiVector3D* pPos = &(paiMesh->mVertices[i]);
    const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
    const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0)
                                      ? &(paiMesh->mTextureCoords[0][i])
                                      : &Zero3D;

    Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
             glm::vec3(pNormal->x, pNormal->y, pNormal->z),
             glm::vec2(pTexCoord->x, pTexCoord->y));

    Vertices.push_back(v);
  }

  for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
    const aiFace& Face = paiMesh->mFaces[i];
    assert(Face.mNumIndices == 3);
    Indices.push_back(Face.mIndices[0]);
    Indices.push_back(Face.mIndices[1]);
    Indices.push_back(Face.mIndices[2]);
  }

  _num_indices = Indices.size();

  // Creiamo e bindiamo gli oggetti OpenGL

  glGenVertexArrays(1, &_VAO);
  glBindVertexArray(_VAO);

  glGenBuffers(1, &_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0],
               GL_STATIC_DRAW);

  glGenBuffers(1, &_IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(),
               &Indices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(ATTRIB_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(struct Vertex, position));
  glVertexAttribPointer(ATTRIB_NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(struct Vertex, normal));
  glVertexAttribPointer(ATTRIB_COLOR_TEXTURE_COORDS, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(struct Vertex, textcoord));

  glBindVertexArray(0);

  // Carichiamo la texture
  // Consideriamo solo un materiale che ha una texture diffusiva
  for (unsigned int i = 1; i < pScene->mNumMaterials; i++) {
    const aiMaterial* pMaterial = pScene->mMaterials[i];

    aiString materialName;

    pMaterial->Get(AI_MATKEY_NAME, materialName);

    // std::cout<< i << " " << materialName.data << std::endl;

    if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
      aiString Path;

      if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL,
                                NULL, NULL, NULL) == AI_SUCCESS) {
        std::string data = Path.data;

        std::string FullPath = Filepath + "/" + data;

        if (!_texture.load(FullPath.c_str())) {
          std::cout << "  Error loading texture '" << FullPath << "'"
                    << std::endl;
        } else {
          std::cout << "  Loaded texture '" << FullPath << "'" << std::endl;
          break;
        }
      }
    }
  }

  if (!_texture.is_valid()) {
    Ret = _texture.load("white.png");
    std::cout << "  Loaded blank texture." << std::endl;
  }

  return Ret;
}

std::string Mesh::get_file_path(const std::string& Filename) const {
  std::string::size_type SlashIndex = Filename.find_last_of("/");
  std::string Dir;

  if (SlashIndex == std::string::npos) {
    Dir = ".";
  } else if (SlashIndex == 0) {
    Dir = "/";
  } else {
    Dir = Filename.substr(0, SlashIndex);
  }

  return Dir;
}

void Mesh::render(void) {
  glBindVertexArray(_VAO);

  _texture.bind(TEXTURE_COLOR);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glDrawElements(GL_TRIANGLES, _num_indices, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
}
