#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class MaterialHandler {
public:
  [[nodiscard]] inline static MaterialHandler *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new MaterialHandler();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of MaterialHandler")
    };
  };

  void CreateVulkanResources();

private:
  MaterialHandler() {
    PC_PRINT("CREATED", TagType::Constr, "MaterialHandler.h")
  };
  ~MaterialHandler() {
    PC_PRINT("DESTROYED", TagType::Destr, "MaterialHandler.h")
  };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  MaterialHandler(const MaterialHandler &) = delete;
  MaterialHandler &operator=(const MaterialHandler &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  MaterialHandler(MaterialHandler &&) = delete;
  MaterialHandler &operator=(MaterialHandler &&) = delete;

private:
  static MaterialHandler *s_instance;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
