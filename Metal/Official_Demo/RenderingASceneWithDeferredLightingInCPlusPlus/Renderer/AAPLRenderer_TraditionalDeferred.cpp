/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of the renderer class which performs Metal setup and per frame rendering for a
 traditional deferred renderer used for macOS devices without Apple Silicon and the
 iOS & tvOS simulators.
*/

#include "AAPLRenderer_TraditionalDeferred.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "AAPLShaderTypes.h"

#include "AAPLUtilities.h"

Renderer_TraditionalDeferred::Renderer_TraditionalDeferred(MTK::View & view)
: Renderer(view)
{
    m_singlePassDeferred = false;
    loadMetal();
    loadScene();
}

Renderer_TraditionalDeferred::~Renderer_TraditionalDeferred()
{
}

/// Create traditional deferred renderer specific Metal state objects
void Renderer_TraditionalDeferred::loadMetal()
{
    Renderer::loadMetal();

    CFErrorRef error = nullptr;


    #pragma mark Point light render pipeline setup
    {
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;

        renderPipelineDescriptor.label( "Light" );
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat( m_view.colorPixelFormat() );

        // Enable additive blending
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].blendingEnabled( true );
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].rgbBlendOperation( MTL::BlendOperationAdd );
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].alphaBlendOperation( MTL::BlendOperationAdd );
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].destinationRGBBlendFactor( MTL::BlendFactorOne );
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].destinationAlphaBlendFactor( MTL::BlendFactorOne );
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].sourceRGBBlendFactor( MTL::BlendFactorOne );
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].sourceAlphaBlendFactor( MTL::BlendFactorOne );

        renderPipelineDescriptor.depthAttachmentPixelFormat( m_view.depthStencilPixelFormat() );
        renderPipelineDescriptor.stencilAttachmentPixelFormat( m_view.depthStencilPixelFormat() );

        MTL::Library shaderLibrary = makeShaderLibrary();

        MTL::Function lightVertexFunction = shaderLibrary.makeFunction( "deferred_point_lighting_vertex" );
        MTL::Function lightFragmentFunction = shaderLibrary.makeFunction( "deferred_point_lighting_fragment_traditional" );

        renderPipelineDescriptor.vertexFunction( &lightVertexFunction );
        renderPipelineDescriptor.fragmentFunction( &lightFragmentFunction );

        m_lightPipelineState = m_device.makeRenderPipelineState( renderPipelineDescriptor, &error );

        AAPLAssert(error == nullptr, error, "Failed to create lighting render pipeline state");
    }

    #pragma mark GBuffer render pass descriptor setup
    // Create a render pass descriptor to create an encoder for rendering to the GBuffers.
    // The encoder stores rendered data of each attachment when encoding ends.
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetLighting].loadAction( MTL::LoadActionDontCare );
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetLighting].storeAction( MTL::StoreActionDontCare );
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].loadAction( MTL::LoadActionDontCare );
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].storeAction( MTL::StoreActionStore );
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].loadAction( MTL::LoadActionDontCare );
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].storeAction( MTL::StoreActionStore );
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].loadAction( MTL::LoadActionDontCare );
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].storeAction( MTL::StoreActionStore );
    m_GBufferRenderPassDescriptor.depthAttachment.clearDepth( 1.0 );
    m_GBufferRenderPassDescriptor.depthAttachment.loadAction( MTL::LoadActionClear );
    m_GBufferRenderPassDescriptor.depthAttachment.storeAction( MTL::StoreActionStore );

    m_GBufferRenderPassDescriptor.stencilAttachment.clearStencil( 0 );
    m_GBufferRenderPassDescriptor.stencilAttachment.loadAction( MTL::LoadActionClear );
    m_GBufferRenderPassDescriptor.stencilAttachment.storeAction( MTL::StoreActionStore );

    // Create a render pass descriptor for thelighting and composition pass

    // Whatever rendered in the final pass needs to be stored so it can be displayed
    m_finalRenderPassDescriptor.colorAttachments[0].storeAction( MTL::StoreActionStore );
    m_finalRenderPassDescriptor.depthAttachment.loadAction( MTL::LoadActionLoad );
    m_finalRenderPassDescriptor.stencilAttachment.loadAction( MTL::LoadActionLoad );
}

/// Respond to view size change
void Renderer_TraditionalDeferred::drawableSizeWillChange(MTK::View & view, const MTL::Size & size)
{
    // The renderer base class allocates all GBuffers >except< lighting GBuffer (since with the
    // single-pass deferred renderer the lighting buffer is the same as the drawable)
    Renderer::drawableSizeWillChange( size, MTL::StorageModePrivate );

    // Re-set GBuffer textures in the GBuffer render pass descriptor after they have been
    // reallocated by a resize
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].texture( m_albedo_specular_GBuffer );
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].texture( m_normal_shadow_GBuffer );
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].texture( m_depth_GBuffer );

    // Drawable resize will only occur after the view redraws.  If the view is paused, the renderer
    // must explicitly force the view to redraw otherwise CoraAnimation will just stretch or squish
    // the drawable as the view resizes.
    if(view.isPaused())
    {
        view.draw();
    }
}

/// Draw directional lighting, which, with a tradition deferred renderer needs to set GBuffers as
/// textures before executing common rendering code to draw the light
void Renderer_TraditionalDeferred::drawDirectionalLight(MTL::RenderCommandEncoder & renderEncoder)
{
    renderEncoder.pushDebugGroup( "Draw Directional Light" );
    renderEncoder.setFragmentTexture( m_albedo_specular_GBuffer, RenderTargetAlbedo );
    renderEncoder.setFragmentTexture( m_normal_shadow_GBuffer, RenderTargetNormal );
    renderEncoder.setFragmentTexture( m_depth_GBuffer, RenderTargetDepth );

    Renderer::drawDirectionalLightCommon( renderEncoder );

    renderEncoder.popDebugGroup();
}

/// Setup traditional deferred rendering specific pipeline and set GBuffer textures.  Then call
/// common renderer code to apply the point lights
void Renderer_TraditionalDeferred::drawPointLights(MTL::RenderCommandEncoder & renderEncoder)
{
    renderEncoder.pushDebugGroup( "Draw Point Lights" );

    renderEncoder.setRenderPipelineState( m_lightPipelineState );

    renderEncoder.setFragmentTexture( m_albedo_specular_GBuffer, RenderTargetAlbedo );
    renderEncoder.setFragmentTexture( m_normal_shadow_GBuffer, RenderTargetNormal );
    renderEncoder.setFragmentTexture( m_depth_GBuffer, RenderTargetDepth );

    // Call common base class method after setting state in the renderEncoder specific to the
    // traditional deferred renderer
    Renderer::drawPointLightsCommon( renderEncoder );

    renderEncoder.popDebugGroup();
}

/// Frame drawing routine
void Renderer_TraditionalDeferred::drawInView(MTK::View & view)
{
    {
        MTL::CommandBuffer commandBuffer = Renderer::beginFrame();
        commandBuffer.label( "Shadow & GBuffer Commands" );

        Renderer::drawShadow( commandBuffer );

        m_GBufferRenderPassDescriptor.depthAttachment.texture( *view.depthStencilTexture() );
        m_GBufferRenderPassDescriptor.stencilAttachment.texture( *view.depthStencilTexture() );

        MTL::RenderCommandEncoder renderEncoder =
            commandBuffer.renderCommandEncoderWithDescriptor( m_GBufferRenderPassDescriptor );
        renderEncoder.label( "GBuffer Generation" );

        Renderer::drawGBuffer( renderEncoder );

        renderEncoder.endEncoding();

        // Commit commands so that Metal can begin working on non-drawable dependant work without
        // waiting for a drawable to become avaliable
        commandBuffer.commit();
    }

    {
        MTL::CommandBuffer commandBuffer = Renderer::beginDrawableCommands();

        commandBuffer.label( "Lighting Commands" );

        MTL::Texture *drawableTexture = Renderer::currentDrawableTexture();

        // The final pass can only render if a drawable is available, otherwise it needs to skip
        // rendering this frame.
        if(drawableTexture)
        {
            // Render the lighting and composition pass

            m_finalRenderPassDescriptor.colorAttachments[0].texture( *drawableTexture );
            m_finalRenderPassDescriptor.depthAttachment.texture( *m_view.depthStencilTexture() );
            m_finalRenderPassDescriptor.stencilAttachment.texture( *m_view.depthStencilTexture() );

            MTL::RenderCommandEncoder renderEncoder =
                commandBuffer.renderCommandEncoderWithDescriptor( m_finalRenderPassDescriptor );
            renderEncoder.label( "Lighting & Composition Pass" );

            drawDirectionalLight( renderEncoder );

            Renderer::drawPointLightMask( renderEncoder );

            drawPointLights( renderEncoder );

            Renderer::drawSky( renderEncoder );

            Renderer::drawFairies( renderEncoder );

            renderEncoder.endEncoding();
        }

        Renderer::endFrame( commandBuffer );
    }
}

#if SUPPORT_BUFFER_EXAMINATION

/// Set up render targets for display when buffer examnination mode enabled.  Set up target for
/// optimal rendering when buffer examnination mode disabled.
void Renderer_TraditionalDeferred::validateBufferExaminationMode()
{
    if( m_bufferExaminationManager->mode() )
    {
        // Clear the background of the GBuffer when examining buffers.  When rendering normally
        // clearing is wasteful, but when examining the buffers, the backgrounds appear corrupt
        // making unclear what's actually rendered to the buffers
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].loadAction( MTL::LoadActionClear );
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].loadAction( MTL::LoadActionClear );
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].loadAction( MTL::LoadActionClear );

        // Store depth and stencil buffers after filling them.  This is wasteful when rendering
        // normally, but necessary to present the light mask culling view.
        m_finalRenderPassDescriptor.stencilAttachment.storeAction( MTL::StoreActionStore );
        m_finalRenderPassDescriptor.depthAttachment.storeAction( MTL::StoreActionStore );
    }
    else
    {
        // When exiting buffer examination mode, return to efficient state settings
        m_finalRenderPassDescriptor.stencilAttachment.storeAction( MTL::StoreActionDontCare );
        m_finalRenderPassDescriptor.depthAttachment.storeAction( MTL::StoreActionDontCare );
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].loadAction( MTL::LoadActionDontCare );
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].loadAction( MTL::LoadActionDontCare );
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].loadAction( MTL::LoadActionDontCare );
    }
}

#endif // END SUPPORT_BUFFER_EXAMINATION 


