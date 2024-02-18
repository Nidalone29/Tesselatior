#ifndef OBJECT_H
#define OBJECT_H

#include "model.h"

class Object {
 public:
  Object() = delete;  // TODO TEMP
  explicit Object(const Model& model);
  ~Object();
  Object(const Object& other) = default;
  Object& operator=(const Object& other) = default;
  Object(Object&& other) = default;
  Object& operator=(Object&& other) = default;

  const Model& model() const;
  void model(const Model& model);

 private:
  Model model_;
};

#endif  // OBJECT_H
