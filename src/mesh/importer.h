#ifndef IMPORTER_H
#define IMPORTER_H

#include <filesystem>

#include "mesh.h"

struct Options {
  bool triangulate;
  bool pre_transform;
  bool require_single_mesh;

  Options()
      : triangulate(false), pre_transform(true), require_single_mesh(false) {
    //
  }
};

class IImporter {
 public:
  IImporter() = default;
  IImporter(const IImporter& other) = delete;
  IImporter& operator=(const IImporter& other) = delete;
  IImporter(IImporter&& other) = delete;
  IImporter&& operator=(IImporter&& other) = delete;

  virtual ~IImporter() = 0;

  virtual std::vector<IMesh*> import(const std::filesystem::path& filepath,
                                     const Options& opts = Options()) = 0;

 private:
};

#endif  // IMPORTER_H
