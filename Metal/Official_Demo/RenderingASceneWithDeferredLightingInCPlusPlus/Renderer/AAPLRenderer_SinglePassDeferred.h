/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for the renderer class which performs Metal setup and per frame rendering for a single pass
 deferred renderer used for iOS & tvOS devices in addition to macOS devices with Apple Silicon.
*/

#ifndef AAPLRenderer_SinglePassDeferred_h
#define AAPLRenderer_SinglePassDeferred_h

#include "AAPLRenderer.h"

class Renderer_SinglePassDeferred : public Renderer
{
public:

    explicit Renderer_SinglePassDeferred(MTK::View & view);

    void drawableSizeWillChange(MTK::View& view, const MTL::Size & size);

    void drawInView(MTK::View& view);

#if SUPPORT_BUFFER_EXAMINATION

    void validateBufferExaminationMode();

#endif

private:

    void loadMetal();

    void drawDirectionalLight(MTL::RenderCommandEncoder& renderEncoder);

    void drawPointLights(MTL::RenderCommandEncoder& renderEncoder);

    MTL::RenderPipelineState m_lightPipelineState;

    MTL::RenderPassDescriptor m_viewRenderPassDescriptor;

    MTL::StorageMode m_GBufferStorageMode;
};

#endif // AAPLRenderer_SinglePassDeferred_h
