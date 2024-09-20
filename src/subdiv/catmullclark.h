#ifndef CATMULLCLARK_H
#define CATMULLCLARK_H

#include "subdivision.h"

class CatmullClarkSubdiv final : public ISubdivision {
 public:
  // I can't specify the argument type because it violates the Liskov
  // Substitution Principle
  [[nodiscard]] QuadMesh* subdivide(IMesh* in, int n_steps) override {
    if (const QuadMesh* d = dynamic_cast<QuadMesh*>(in); d != nullptr) {
      // the architecture of the program
      // (AvailableSubdivAlgosFactory::GetAvailableAlgos()) should guarantee
      // that this works
      return subdivide(d, n_steps);
    }
    return nullptr;
  }

  [[nodiscard]] QuadMesh* subdivide(const QuadMesh* in, int n_steps);

 private:
};

#endif  // CATMULLCLARK_H