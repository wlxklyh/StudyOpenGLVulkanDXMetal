/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of the renderer class which performs Metal setup and per frame rendering for a
 single pass deferred renderer used for iOS & tvOS devices in addition to macOS devices with
 Apple Silicon.
*/

#include "AAPLRenderer_SinglePassDeferred.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "Shaders/AAPLShaderTypes.h"

#include "AAPLUtilities.h"

Renderer_SinglePassDeferred::Renderer_SinglePassDeferred(MTK::View& view)
: Renderer( view )
{
    m_singlePassDeferred = true;

    m_GBufferStorageMode = MTL::StorageModeMemoryless;

    loadMetal();
    loadScene();
}

void Renderer_SinglePassDeferred::loadMetal()
{
    Renderer::loadMetal();
    CFErrorRef error = nullptr;

    #pragma mark Point light render pipeline setup
    {
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;

        renderPipelineDescriptor.label( "Light" );
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(m_view.colorPixelFormat());
        renderPipelineDescriptor.colorAttachments[RenderTargetAlbedo].pixelFormat(m_albedo_specular_GBufferFormat);
        renderPipelineDescriptor.colorAttachments[RenderTargetNormal].pixelFormat(m_normal_shadow_GBufferFormat);
        renderPipelineDescriptor.colorAttachments[RenderTargetDepth].pixelFormat(m_depth_GBufferFormat);
        renderPipelineDescriptor.depthAttachmentPixelFormat(m_view.depthStencilPixelFormat());
        renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view.depthStencilPixelFormat());

        MTL::Library shaderLibrary = makeShaderLibrary();

        MTL::Function lightVertexFunction = shaderLibrary.makeFunction( "deferred_point_lighting_vertex" );
        MTL::Function lightFragmentFunction = shaderLibrary.makeFunction( "deferred_point_lighting_fragment_single_pass" );

        renderPipelineDescriptor.vertexFunction( &lightVertexFunction );
        renderPipelineDescriptor.fragmentFunction( &lightFragmentFunction );

        m_lightPipelineState = m_device.makeRenderPipelineState( renderPipelineDescriptor, &error );

        AAPLAssert(error == nullptr, error, "Failed to create lighting render pipeline state");
    }

    #pragma mark GBuffer + View render pass descriptor setup
    m_viewRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].loadAction(MTL::LoadActionDontCare);
    m_viewRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].storeAction(MTL::StoreActionDontCare);
    m_viewRenderPassDescriptor.colorAttachments[RenderTargetNormal].loadAction(MTL::LoadActionDontCare);
    m_viewRenderPassDescriptor.colorAttachments[RenderTargetNormal].storeAction(MTL::StoreActionDontCare);
    m_viewRenderPassDescriptor.colorAttachments[RenderTargetDepth].loadAction(MTL::LoadActionDontCare);
    m_viewRenderPassDescriptor.colorAttachments[RenderTargetDepth].storeAction(MTL::StoreActionDontCare);
    m_viewRenderPassDescriptor.depthAttachment.loadAction( MTL::LoadActionClear );
    m_viewRenderPassDescriptor.depthAttachment.storeAction( MTL::StoreActionDontCare) ;
    m_viewRenderPassDescriptor.stencilAttachment.loadAction( MTL::LoadActionClear );
    m_viewRenderPassDescriptor.stencilAttachment.storeAction( MTL::StoreActionDontCare );
    m_viewRenderPassDescriptor.depthAttachment.clearDepth( 1.0 );
    m_viewRenderPassDescriptor.stencilAttachment.clearStencil( 0 );

}

/// Respond to view size change
void Renderer_SinglePassDeferred::drawableSizeWillChange(MTK::View& view, const MTL::Size & size)
{
    // The renderer base class allocates all GBuffers >except< lighting GBuffer (since with the
    // single-pass deferred renderer the lighting buffer is the same as the drawable)
    Renderer::drawableSizeWillChange(size, m_GBufferStorageMode);

    // Re-set GBuffer textures in the GBuffer render pass descriptor after they have been
    // reallocated by a resize
    m_viewRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].texture(m_albedo_specular_GBuffer);
    m_viewRenderPassDescriptor.colorAttachments[RenderTargetNormal].texture(m_normal_shadow_GBuffer);
    m_viewRenderPassDescriptor.colorAttachments[RenderTargetDepth].texture(m_depth_GBuffer);

    // Drawable resize will only occur after the view redraws.  If the view is paused, the renderer
    // must explicitly force the view to redraw otherwise CoraAnimation will just stretch or squish
    // the drawable as the view resizes.
    if(view.isPaused())
    {
        view.draw();
    }
}

/// Frame drawing routine
void Renderer_SinglePassDeferred::drawInView(MTK::View& view)
{
    MTL::CommandBuffer commandBuffer = beginFrame();
    commandBuffer.label("Shadow commands");

    drawShadow(commandBuffer);
    commandBuffer.commit();

    commandBuffer = beginDrawableCommands();
    commandBuffer.label("GBuffer & Lighting Commands");

    MTL::Texture* drawableTexture = currentDrawableTexture();
    if (drawableTexture)
    {
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetLighting].texture(*drawableTexture);
        m_viewRenderPassDescriptor.depthAttachment.texture(*m_view.depthStencilTexture());
        m_viewRenderPassDescriptor.stencilAttachment.texture(*m_view.depthStencilTexture());

        MTL::RenderCommandEncoder renderEncoder =
            commandBuffer.renderCommandEncoderWithDescriptor(m_viewRenderPassDescriptor);
        renderEncoder.label("Combined GBuffer & Lighting Pass");

        Renderer::drawGBuffer( renderEncoder );

        drawDirectionalLight( renderEncoder );

        Renderer::drawPointLightMask( renderEncoder );

        drawPointLights( renderEncoder );

        Renderer::drawSky( renderEncoder );

        Renderer::drawFairies( renderEncoder );

        renderEncoder.endEncoding();
    }

    endFrame( commandBuffer );
}

void Renderer_SinglePassDeferred::drawDirectionalLight(MTL::RenderCommandEncoder& renderEncoder)
{
    renderEncoder.pushDebugGroup("Draw Directional Light");

    Renderer::drawDirectionalLightCommon(renderEncoder);

    renderEncoder.popDebugGroup();
}

void Renderer_SinglePassDeferred::drawPointLights(MTL::RenderCommandEncoder& renderEncoder)
{
    renderEncoder.pushDebugGroup("Draw Point Lights");

    renderEncoder.setRenderPipelineState( m_lightPipelineState );

    // Call common base class method after setting state in the renderEncoder specific to the
    // single-pass deferred renderer
    Renderer::drawPointLightsCommon( renderEncoder );

    renderEncoder.popDebugGroup();
}

#if SUPPORT_BUFFER_EXAMINATION

/// Set up render targets for display when buffer examnination mode enabled.  Set up target for
/// optimal rendering when buffer examnination mode disabled.
void Renderer_SinglePassDeferred::validateBufferExaminationMode()
{
    // When in buffer examination mode, the renderer must allocate the GBuffers with
    // StorageModePrivate since the buffer examination manager needs the GBuffers written to main
    // memory to render them on screen later.
    // However, when a buffer examination mode is not enabled, the renderer only needs the GBuffers
    // in the GPU tile memory, so it can use StorageModeMemoryless to conserve memory.

    if( m_bufferExaminationManager->mode())
    {
        // Clear the background of the GBuffer when examining buffers.  When rendering normally
        // clearing is wasteful, but when examining the buffers, the backgrounds appear corrupt
        // making unclear what's actually rendered to the buffers
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].loadAction( MTL::LoadActionClear );
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetNormal].loadAction( MTL::LoadActionClear );
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetDepth].loadAction( MTL::LoadActionClear );

        // Store results of all buffers to examine them.  This is wasteful when rendering
        // normally, but necessary to present them on screen.
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].storeAction( MTL::StoreActionStore );
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetNormal].storeAction( MTL::StoreActionStore );
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetDepth].storeAction( MTL::StoreActionStore );
        m_viewRenderPassDescriptor.depthAttachment.storeAction( MTL::StoreActionStore );
        m_viewRenderPassDescriptor.stencilAttachment.storeAction( MTL::StoreActionStore );

        m_GBufferStorageMode = MTL::StorageModePrivate;
    }
    else
    {
        // When exiting buffer examination mode, return to efficient state settings
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].loadAction( MTL::LoadActionDontCare );
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetNormal].loadAction( MTL::LoadActionDontCare );
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetDepth].loadAction( MTL::LoadActionDontCare );
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].storeAction( MTL::StoreActionDontCare );
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetNormal].storeAction( MTL::StoreActionDontCare );
        m_viewRenderPassDescriptor.colorAttachments[RenderTargetDepth].storeAction( MTL::StoreActionDontCare );
        m_viewRenderPassDescriptor.depthAttachment.storeAction( MTL::StoreActionDontCare );
        m_viewRenderPassDescriptor.stencilAttachment.storeAction( MTL::StoreActionDontCare );

        m_GBufferStorageMode = MTL::StorageModeMemoryless;
    }

    // Force reallocation of GBuffers.
    drawableSizeWillChange( m_view, m_view.drawableSize() );
}

#endif // SUPPORT_BUFFER_EXAMINATION
