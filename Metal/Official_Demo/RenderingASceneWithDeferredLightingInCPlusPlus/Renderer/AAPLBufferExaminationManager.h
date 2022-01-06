/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for renderer class which performs Metal setup and per frame rendering
*/
#ifndef AAPLRenderer_BufferExaminationManager_h
#define AAPLRenderer_BufferExaminationManager_h

#include "AAPLConfig.h"

#if SUPPORT_BUFFER_EXAMINATION

#include "CPPMetal.hpp"
#include <forward_list>

class Renderer;

typedef enum ExaminationMode
{
    ExaminationModeDisabled           = 0x00,
    ExaminationModeAlbedo             = 0x01,
    ExaminationModeNormals            = 0x02,
    ExaminationModeSpecular           = 0x04,
    ExaminationModeDepth              = 0x08,
    ExaminationModeShadowGBuffer      = 0x10,
    ExaminationModeShadowMap          = 0x20,
    ExaminationModeMaskedLightVolumes = 0x40,
    ExaminationModeFullLightVolumes   = 0x80,
    ExaminationModeAll                = 0xFF
} ExaminationMode;

class BufferExaminationManager
{
public:

    BufferExaminationManager(Renderer & renderer,
                             MTK::View albedoGBufferView,
                             MTK::View normalsGBufferView,
                             MTK::View depthGBufferView,
                             MTK::View shadowGBufferView,
                             MTK::View finalFrameView,
                             MTK::View specularGBufferView,
                             MTK::View shadowMapView,
                             MTK::View lightMaskView,
                             MTK::View lightCoverageView);

    BufferExaminationManager(const BufferExaminationManager & rhs) = delete;

    BufferExaminationManager & operator=(const BufferExaminationManager & rhs) = delete;

    virtual ~BufferExaminationManager();

    void updateDrawableSize(MTL::Size size);

    void drawAndPresentBuffersWithCommandBuffer(MTL::CommandBuffer & commandBuffer);

    MTL::Texture *offscreenDrawable() const;

    void mode(ExaminationMode mode);
    ExaminationMode mode() const;

private:

    void loadMetalState();

    void drawAlbedoGBufferWithCommandBuffer(MTL::CommandBuffer & commandBuffer);
    void drawNormalsGBufferWithCommandBuffer(MTL::CommandBuffer & commandBuffer);
    void drawDepthGBufferWithCommandBuffer(MTL::CommandBuffer & commandBuffer);
    void drawShadowGBufferWithCommandBuffer(MTL::CommandBuffer & commandBuffer);
    void drawFinalRenderWithCommandBuffer(MTL::CommandBuffer & commandBuffer);
    void drawSpecularGBufferWithCommandBuffer(MTL::CommandBuffer & commandBuffer);
    void drawShadowMapWithCommandBuffer(MTL::CommandBuffer & commandBuffer);
    void drawLightMaskWithCommandBuffer(MTL::CommandBuffer & commandBuffer);
    void drawLightVolumesWithCommandBuffer(MTL::CommandBuffer & commandBuffer);

    void renderLightVolumesExaminationWithCommandBuffer(MTL::CommandBuffer & commandBuffer,
                                                        bool fullVolumes);

    Renderer & m_renderer;

    MTL::Device m_device;

    ExaminationMode m_mode;

    MTK::View m_albedoGBufferView;
    MTK::View m_normalsGBufferView;
    MTK::View m_depthGBufferView;
    MTK::View m_shadowGBufferView;
    MTK::View m_finalFrameView;
    MTK::View m_specularGBufferView;
    MTK::View m_shadowMapView;
    MTK::View m_lightMaskView;
    MTK::View m_lightCoverageView;

    MTL::Texture *m_offscreenDrawable;
    MTL::Texture *m_lightVolumeTarget;

    std::forward_list<MTK::View*> m_allViews;

    MTL::RenderPipelineState m_textureDepthPipelineState;
    MTL::RenderPipelineState m_textureRGBPipelineState;
    MTL::RenderPipelineState m_textureAlphaPipelineState;

    // Render pipeline state used to visualize the point light volume coverage and stencil
    // culled light volume coverage
    MTL::RenderPipelineState m_lightVolumeVisualizationPipelineState;

    // Depth stencil state used to create point light volume coverage visualization buffer
    MTL::DepthStencilState m_depthTestOnlyDepthStencilState;
};

inline ExaminationMode BufferExaminationManager::mode() const
{
    return m_mode;
}

inline MTL::Texture *BufferExaminationManager::offscreenDrawable() const
{
    return m_offscreenDrawable;
}

#endif // SUPPORT_BUFFER_EXAMINATION

#endif // AAPLRenderer_BufferExaminationManager_h

