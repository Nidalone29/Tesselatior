#ifndef MESH_H
#define MESH_H

#include <unordered_map>

#include <GL/glew.h>
#include "halfedge.h"
#include "material.h"
#include "utilities.h"

// sa stands for subdivision algorithms
namespace sa {

enum class SubDiv {
  NONE,
  // --
  LOOP,
  CATMULL,
  SQRT3
};

// why inline and not static?
// https://www.youtube.com/watch?v=QVHwOOrSh3w
// https://www.youtube.com/watch?v=rQhBECyA6ew
// string for displaying algorithms name to UI
inline const std::unordered_map<SubDiv, std::string> kSubdivisions = {
    {SubDiv::NONE, "none"},
    {SubDiv::LOOP, "loop"},
    {SubDiv::CATMULL, "catmull"},
    {SubDiv::SQRT3, "sqrt3"},
};

};  // namespace sa

enum class ATTRIB_ID {
  POSITIONS = 0,
  NORMALS = 1,
  TEXTURE_COORDS = 2
};

enum class SHADING {
  FLAT = 0,
  SMOOTH = 1
};

// interface (all functions are pure and no data)
class IMesh {
 public:
  // opengl rendering
  [[nodiscard]] virtual const GLuint& vao() const = 0;
  [[nodiscard]] virtual unsigned int num_indices() const = 0;
  [[nodiscard]] virtual const Material& material() const = 0;
  [[nodiscard]] virtual int PatchNumVertices() const = 0;

  [[nodiscard]] virtual IMesh* clone() = 0;
  [[nodiscard]] virtual int num_vertices() const = 0;
  [[nodiscard]] virtual int num_edges() const = 0;
  [[nodiscard]] virtual int num_faces() const = 0;
  [[nodiscard]] virtual std::vector<sa::SubDiv> CompatibleSubdivs() = 0;
  virtual void ApplySmoothNormals() = 0;
  virtual void GenerateOpenGLBuffers(
      std::vector<Vertex>* vertices = nullptr,
      std::vector<unsigned int>* indices = nullptr) = 0;
  virtual void GenerateOpenGLBuffersWithSmoothShading() = 0;
  virtual void GenerateOpenGLBufferWithFlatShading() = 0;

  virtual ~IMesh() = default;

 protected:
};

// abstract base class (for sharing implementation across the derived classes
// aka implementation inheritance). I make this non instantiable with the pure
// virtual destructor trick
// The reason for why there is a virtual clone() method is:
// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c130-for-making-deep-copies-of-polymorphic-classes-prefer-a-virtual-clone-function-instead-of-public-copy-constructionassignment
// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c67-a-polymorphic-class-should-suppress-public-copymove
class AbstractMesh : public IMesh {
 public:
  AbstractMesh() = delete;  // can't create an empty mesh

  AbstractMesh(MESH_TYPE type, HalfEdgeData* hf_data, const Material& material);
  virtual ~AbstractMesh();

  AbstractMesh(const AbstractMesh& other) = delete;
  AbstractMesh& operator=(const AbstractMesh& other) = delete;
  AbstractMesh(AbstractMesh&& other) = delete;
  AbstractMesh& operator=(AbstractMesh&& other) = delete;

  [[nodiscard]] const GLuint& vao() const override {
    return VAO_;
  }

  [[nodiscard]] unsigned int num_indices() const override {
    return num_indices_;
  }

  [[nodiscard]] const HalfEdgeData* half_edge_data() const {
    return hf_data_;
  }

  [[nodiscard]] const Material& material() const {
    return material_;
  }

  void material(const Material& m) {
    material_ = m;
  }

  // This is additional debug info
  [[nodiscard]] int num_vertices() const override {
    return hf_data_->vertices()->size();
  }

  [[nodiscard]] int num_edges() const override {
    return hf_data_->edges()->size();
  }

  [[nodiscard]] int num_faces() const override {
    return hf_data_->faces()->size();
  }
  // it generates smooth normals for the mesh
  void ApplySmoothNormals() override;

  // we generate the buffers, as is, without any modification to the underlying
  // data, it overrides prev buffer
  void GenerateOpenGLBuffers(
      std::vector<Vertex>* vertices = nullptr,
      std::vector<unsigned int>* indices = nullptr) override;
  // computes smooth normals on a copy of the halfedge data then calls
  // generatebuffer
  void GenerateOpenGLBuffersWithSmoothShading() override;
  // has to duplicate vertices, but we still want the original unduped data, so
  // it just duplicates a copy of the vertices before binding the copy to the
  // GPU buffers
  void GenerateOpenGLBufferWithFlatShading() override;

 protected:
  void ClearOpenGLBuffers();

  // OpenGL specific
  GLuint VAO_;  // Vertex Array Object
  GLuint VBO_;  // Vertex Buffer Object
  GLuint IBO_;  // Index Buffer Object
  unsigned int num_indices_;

  // Actual mesh data
  HalfEdgeData* hf_data_;
  Material material_;
};

// we will have:
// - Triangular meshes
// - Quad meshes (rendered using tessellation shaders)
// - Polygonal meshes (all non trig primitives will be triangulated)
class TriMesh final : public AbstractMesh {
 public:
  TriMesh(HalfEdgeData* hf_data, const Material& material);
  [[nodiscard]] int PatchNumVertices() const override {
    return to_underlying(MESH_TYPE::TRI);
  }

  [[nosdiscard]] std::vector<sa::SubDiv> CompatibleSubdivs() override {
    return {sa::SubDiv::NONE, sa::SubDiv::LOOP, sa::SubDiv::SQRT3};
  }

  [[nodiscard]] IMesh* clone() override {
    return new TriMesh(new HalfEdgeData(*this->hf_data_), this->material_);
  }
};

class QuadMesh final : public AbstractMesh {
 public:
  QuadMesh(HalfEdgeData* hf_data, const Material& material);
  [[nodiscard]] int PatchNumVertices() const override {
    return to_underlying(MESH_TYPE::QUAD);
  }

  [[nosdiscard]] std::vector<sa::SubDiv> CompatibleSubdivs() override {
    return {sa::SubDiv::NONE, sa::SubDiv::CATMULL};
  }

  [[nodiscard]] IMesh* clone() override {
    return new QuadMesh(new HalfEdgeData(*this->hf_data_), this->material_);
  }
};

class PolyMesh final : public AbstractMesh {
 public:
  PolyMesh(HalfEdgeData* hf_data, const Material& material);
  [[nodiscard]] int PatchNumVertices() const override {
    // NOTE this mesh binds a triangulated version of itself to the gpu
    return to_underlying(MESH_TYPE::TRI);
  }

  [[nosdiscard]] std::vector<sa::SubDiv> CompatibleSubdivs() override {
    return {sa::SubDiv::NONE};
  }

  [[nodiscard]] IMesh* clone() override {
    return new PolyMesh(new HalfEdgeData(*this->hf_data_), this->material_);
  }
};

#endif  // MESH_H
