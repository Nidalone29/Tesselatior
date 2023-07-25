#include "model.h"

Model::Model(const std::string& path) : _mesh(mesh), _transform(transform) {
  // ASSIMP imports the mesh

  // TODO set transform to 0
}

void Model::load_meshes(const std::string& path) {
  Assimp::Importer Importer;

  // flags set to 0 for now, probabli have to set them at some point if ASSIMP
  // doesnt deal with different file formats
  const aiScene* pScene = Importer.ReadFile(path.c_str(), 0);

  std::vector<Mesh> meshes;

  for (int i = 0; i < pScene->mNumMeshes; i++) {
    // deal with mesh
    const aiMesh* paiMesh = pScene->mMeshes[i];

    // deal with material
    // NOTE a mesh has one and only one material
    const unsigned int material_i = paiMesh->mMaterialIndex;
    const aiMaterial* material = pScene->mMaterials[material_i];
    material->;

    // deal with textures
    // NOTE that not every file format embeds textures
    if (pScene->HasTextures()) {
      // great the textures are embedded and we can extract them with assimp
      // (maybe we are in GLTF binary format (.glb))
    } else {
      // if the texture are not embedded, then they must be linked somewhere in
      // the file... and loaded with stb image...
      material->GetTexture();
    }

    // add the mesh to the model
  }
}

const std::vector<Mesh>& Model::getModel() const {
  return _mesh;
}

const Transform& Model::getTransform() const {
  return _transform;
}

void setTransform(const Transform& transform) {
  _transform = transform;
}
