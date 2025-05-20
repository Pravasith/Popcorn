#pragma once

#include "BufferObjects.h"
#include "CompositePipelinesVk.h"
#include "GlobalMacros.h"
#include "LightingPipelinesVk.h"
#include "MaterialPipelinesVk.h"
#include "Popcorn/Core/Base.h"
#include "RenderPassVk.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class Pipelines {
  GBuffer = 1, // G-Buffer
  Lighting,    // Lighting
  Composite    // Composition
};

template <Pipelines T> struct DeriveGfxPipelineType;

template <> struct DeriveGfxPipelineType<Pipelines::GBuffer> {
  using type = BasicMatPipelineVk;
};

template <> struct DeriveGfxPipelineType<Pipelines::Lighting> {
  using type = LightingPipelineVk;
};

template <> struct DeriveGfxPipelineType<Pipelines::Composite> {
  using type = CompositePipelineVk;
};

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

  // Potentially takes in PipelineLayout and spits out the appropriate Pipeline
  // ref
  template <Pipelines T>
  static void CreatePipeline(const BufferDefs::Layout &vertexBufferLayout,
                             const RenderPassVk &basicRenderPass);

  template <Pipelines U>
  [[nodiscard]] constexpr DeriveGfxPipelineType<U>::type &GetGfxPipeline() {
    if constexpr (U == Pipelines::GBuffer) {
      return m_basicMatPipeline;
    } else if constexpr (U == Pipelines::Lighting) {
      return m_lightingPipeline;
    } else if constexpr (U == Pipelines::Composite) {
      return m_compositePipeline;
    }
    // Other pipelines
  };

  void CleanUp();

private:
  PipelineFactoryVk() {
    PC_PRINT("CREATED", TagType::Constr, "PipelineFactoryVk")
  };

  ~PipelineFactoryVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "PipelineFactoryVk")
  };

private:
  static PipelineFactoryVk *s_instance;

  BasicMatPipelineVk m_basicMatPipeline;
  // PbrMatPipelineVk m_pbrMatPipeline;
  LightingPipelineVk m_lightingPipeline;
  CompositePipelineVk m_compositePipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
