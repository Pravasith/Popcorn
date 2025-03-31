#pragma once

#include "BufferObjects.h"
#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "RenderPassVk.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class Pipelines { Base = 0, Deferred };

class PipelineFactoryVk {
public:
  [[nodiscard]] inline static PipelineFactoryVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new PipelineFactoryVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of PipelineFactoryVk")
    };
  };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  PipelineFactoryVk(const PipelineFactoryVk &) = delete;
  PipelineFactoryVk &operator=(const PipelineFactoryVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  PipelineFactoryVk(PipelineFactoryVk &&) = delete;
  PipelineFactoryVk &operator=(PipelineFactoryVk &&) = delete;

  template <Pipelines T>
  void CreatePipeline(const BufferDefs::Layout &vertexBufferLayout,
                      const RenderPassVk &basicRenderPass);

  template <Pipelines T>
  [[nodiscard]] constexpr const GfxPipelineVk &GetPipeline() const {
    if constexpr (T == Pipelines::Base) {
      return m_basePipeline;
    } else if constexpr (T == Pipelines::Deferred) {
      return m_deferredPipeline;
    }
  };

private:
  PipelineFactoryVk() {
    PC_PRINT("CREATED", TagType::Constr, "PipelineFactoryVk.h")
  };

  ~PipelineFactoryVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "PipelineFactoryVk.h")
  };

private:
  static PipelineFactoryVk *s_instance;

  GfxPipelineVk m_basePipeline;
  GfxPipelineVk m_deferredPipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
