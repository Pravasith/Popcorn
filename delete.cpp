
void Renderer::Render(Scene *scene) {
  // Get or allocate a command buffer for the current frame
  VkCommandBuffer commandBuffer = GetCurrentFrameCommandBuffer();

  // Begin recording the command buffer
  BeginCommandBuffer(commandBuffer);

  // Shadow Pass
  BeginRenderPass(commandBuffer, shadowRenderPass); // Record render pass begin
  for (auto &mesh : scene->GetMeshes()) {
    Pipeline *pipeline =
        pipelineManager.GetPipeline(mesh->GetMaterial(), shadowRenderPass);
    BindPipeline(commandBuffer, pipeline); // Record pipeline binding
    DrawMesh(commandBuffer, mesh);         // Record draw call
  }
  EndRenderPass(commandBuffer); // Record render pass end

  // Geometry Pass
  BeginRenderPass(commandBuffer,
                  geometryRenderPass); // Record render pass begin
  for (auto &mesh : scene->GetMeshes()) {
    Pipeline *pipeline =
        pipelineManager.GetPipeline(mesh->GetMaterial(), geometryRenderPass);
    BindPipeline(commandBuffer, pipeline); // Record pipeline binding
    DrawMesh(commandBuffer, mesh);         // Record draw call
  }
  EndRenderPass(commandBuffer); // Record render pass end

  // Lighting Pass
  BeginRenderPass(commandBuffer,
                  lightingRenderPass);           // Record render pass begin
  BindPipeline(commandBuffer, lightingPipeline); // Record pipeline binding
  DrawFullScreenQuad(commandBuffer);             // Record draw call
  EndRenderPass(commandBuffer);                  // Record render pass end

  // Post-Processing Pass
  BeginRenderPass(commandBuffer,
                  postProcessingRenderPass); // Record render pass begin
  BindPipeline(commandBuffer,
               postProcessingPipeline); // Record pipeline binding
  DrawFullScreenQuad(commandBuffer);    // Record draw call
  EndRenderPass(commandBuffer);         // Record render pass end

  // End recording the command buffer
  EndCommandBuffer(commandBuffer);

  // Submit the command buffer to the GPU queue
  SubmitCommandBuffer(commandBuffer);
}
