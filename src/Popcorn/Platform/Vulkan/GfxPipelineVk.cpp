#include "GfxPipelineVk.h"
#include "Global_Macros.h"
#include "Sources.h"

ENGINE_NAMESPACE_BEGIN

void GfxPipelineVk::CreateGfxPipeline() {
  auto shVert = m_ShdrVk.LoadFile(PC_src_map[VkTriVert]);
  auto shFrag = m_ShdrVk.LoadFile(PC_src_map[VkTriFrag]);
};

ENGINE_NAMESPACE_END
