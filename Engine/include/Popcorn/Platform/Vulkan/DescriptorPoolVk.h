#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class DescriptorSetLayoutsVk {
public:
  DescriptorSetLayoutsVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorSetLayoutsVk")
  };
  ~DescriptorSetLayoutsVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorSetLayoutsVk")
  };

private:
};

class DescriptorPoolVk {
public:
  DescriptorPoolVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorPoolVk")
  };
  ~DescriptorPoolVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorPoolVk")
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
