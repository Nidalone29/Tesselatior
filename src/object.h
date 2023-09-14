#ifndef OBJECT_H
#define OBJECT_H

#include "model.h"

class Object {
 public:
  Object() = delete;  // TODO TEMP
  Object(const Model& model);
  ~Object();
  Object(const Object& other) = default;
  Object& operator=(const Object& other) = default;
  Object(Object&& other) = default;
  Object& operator=(Object&& other) = default;

  const Model& getModel() const;
  void setModel(const Model& model);
  Model& getModModel();

 private:
  Model _model;
};

#endif  // OBJECT_H