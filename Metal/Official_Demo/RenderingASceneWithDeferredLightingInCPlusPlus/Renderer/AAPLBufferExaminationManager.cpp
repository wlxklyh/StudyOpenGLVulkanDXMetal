  /*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of renderer class which performs Metal setup and per frame rendering
*/
#include "AAPLBufferExaminationManager.h"

#if SUPPORT_BUFFER_EXAMINATION

#include "AAPLShaderTypes.h"
#include "AAPLUtilities.h"
#include "AAPLRenderer.h"

#include <simd/simd.h>

#if TARGET_MACOS
#define ColorClass NSColor
#define MakeRect NSMakeRect
#else
#define ColorClass UIColor
#define MakeRect CGRectMake
#endif

BufferExaminationManager::BufferExaminationManager(Renderer & renderer,
                                                   MTK::View albedoGBufferView,
                                                   MTK::View normalsGBufferView,
                                                   MTK::View depthGBufferView,
                                                   MTK::View shadowGBufferView,
                                                   MTK::View finalFrameView,
                                                   MTK::View specularGBufferView,
                                                   MTK::View shadowMapView,
                                                   MTK::View lightMaskView,
                                                   MTK::View lightCoverageView)
: m_renderer            ( renderer )
, m_device              ( renderer.device() )
, m_mode                ( ExaminationModeDisabled)
, m_albedoGBufferView   ( albedoGBufferView )
, m_normalsGBufferView  ( normalsGBufferView )
, m_depthGBufferView    ( depthGBufferView )
, m_shadowGBufferView   ( shadowGBufferView )
, m_finalFrameView      ( finalFrameView )
, m_specularGBufferView ( specularGBufferView )
, m_shadowMapView       ( shadowMapView )
, m_lightMaskView       ( lightMaskView )
, m_lightCoverageView   ( lightCoverageView )
, m_offscreenDrawable   ( nullptr )
, m_lightVolumeTarget   ( nullptr )
{
    m_allViews.emplace_front( &m_albedoGBufferView  );
    m_allViews.emplace_front( &m_normalsGBufferView );
    m_allViews.emplace_front( &m_depthGBufferView   );
    m_allViews.emplace_front( &m_shadowGBufferView  );
    m_allViews.emplace_front( &m_finalFrameView     );
    m_allViews.emplace_front( &m_specularGBufferView);
    m_allViews.emplace_front( &m_shadowMapView      );
    m_allViews.emplace_front( &m_lightMaskView      );
    m_allViews.emplace_front( &m_lightCoverageView  );

    for(auto view : m_allViews)
    {
        // "Pause" the view since the BufferExaminationManager explicitly trigger's redraw in
        //  BufferExaminationManager::drawAndPresentBuffersWithCommandBuffer()
        view->paused ( true );

        // Initialize other properties
        view->colorPixelFormat( m_renderer.colorTargetPixelFormat() );
        view->hidden ( true );
    }

    loadMetalState();
}

BufferExaminationManager::~BufferExaminationManager()
{
    delete m_offscreenDrawable;
    m_offscreenDrawable = nullptr;

    delete m_lightVolumeTarget;
    m_lightVolumeTarget = nullptr;
}

void BufferExaminationManager::loadMetalState()
{
    CFErrorRef error = nullptr;

    MTL::Library shaderLibrary = m_renderer.makeShaderLibrary();

    #pragma mark Light volume visulalization render pipeline setup
    {
        MTL::Function vertexFunction   = shaderLibrary.makeFunction( "light_volume_visualization_vertex" );
        MTL::Function fragmentFunction = shaderLibrary.makeFunction( "light_volume_visualization_fragment" );

        MTL::RenderPipelineDescriptor renderPipelineDescriptor;

        renderPipelineDescriptor.label( "Light Volume Visualization" );
        renderPipelineDescriptor.vertexDescriptor( nullptr );
        renderPipelineDescriptor.vertexFunction( &vertexFunction );
        renderPipelineDescriptor.fragmentFunction( &fragmentFunction );
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat( m_renderer.colorTargetPixelFormat() );
        renderPipelineDescriptor.depthAttachmentPixelFormat( m_renderer.depthStencilTargetPixelFormat() );
        renderPipelineDescriptor.stencilAttachmentPixelFormat( m_renderer.depthStencilTargetPixelFormat() );

        m_lightVolumeVisualizationPipelineState = m_device.makeRenderPipelineState( renderPipelineDescriptor, &error);

        AAPLAssert(error == nullptr, error, "Failed to create light volume visualization render pipeline state");
    }

    #pragma mark Raw GBuffer visualization pipeline setup
    {
        MTL::Function vertexFunction = shaderLibrary.makeFunction( "texture_values_vertex" );
        MTL::Function fragmentFunction = shaderLibrary.makeFunction( "texture_rgb_fragment" );

        // Create simple pipelines that either render RGB or Alpha component of a texture
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;

        renderPipelineDescriptor.label( "Light Volume Visualization" );
        renderPipelineDescriptor.vertexDescriptor( nullptr );
        renderPipelineDescriptor.vertexFunction( &vertexFunction );
        renderPipelineDescriptor.fragmentFunction( &fragmentFunction );
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting] .pixelFormat( m_renderer.colorTargetPixelFormat() );


        // Pipeline to render RGB components of a texture
        m_textureRGBPipelineState = m_device.makeRenderPipelineState( renderPipelineDescriptor, &error );

        AAPLAssert(error == nullptr, error, "Failed to create texture RGB render pipeline state" );

        // Pipeline to render Alpha components of a texture (in RGB as grayscale)
        fragmentFunction = shaderLibrary.makeFunction( "texture_alpha_fragment" );
        renderPipelineDescriptor.fragmentFunction( &fragmentFunction );
        m_textureAlphaPipelineState = m_device.makeRenderPipelineState( renderPipelineDescriptor, &error );

        AAPLAssert(error == nullptr, error, "Failed to create texture alpha render pipeline state" );

        // Pipeline to render Alpha components of a texture (in RGB as grayscale), but with the
        // ability to apply a range with which to divide the alpha value by so that grayscale value
        // is normalized from 0-1
        fragmentFunction = shaderLibrary.makeFunction( "texture_depth_fragment" );
        renderPipelineDescriptor.fragmentFunction( &fragmentFunction );
        m_textureDepthPipelineState = m_device.makeRenderPipelineState( renderPipelineDescriptor, &error );

        AAPLAssert(error == nullptr, error, "Failed to create depth texture render pipeline state");
    }

    #pragma mark Light volume visulalization depth state setup
    {
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.depthWriteEnabled( false );
        depthStencilDesc.depthCompareFunction( MTL::CompareFunctionLessEqual );
        depthStencilDesc.label( "Depth Test Only" );

        m_depthTestOnlyDepthStencilState = m_device.makeDepthStencilState( depthStencilDesc );
    }
}

void BufferExaminationManager::updateDrawableSize(MTL::Size size)
{
    MTL::TextureDescriptor finalTextureDesc;

    finalTextureDesc.pixelFormat( m_renderer.colorTargetPixelFormat() );
    finalTextureDesc.width( size.width );
    finalTextureDesc.height( size.height );
    finalTextureDesc.usage( MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead );

    if(m_mode)
    {
        m_offscreenDrawable = m_device.newTextureWithDescriptor( finalTextureDesc );
        m_offscreenDrawable->label( "Offscreen Drawable" );
    }
    else
    {
        delete m_offscreenDrawable;
        m_offscreenDrawable = nullptr;
    }

    if(m_mode & (ExaminationModeMaskedLightVolumes | ExaminationModeFullLightVolumes))
    {
        m_lightVolumeTarget = m_device.newTextureWithDescriptor( finalTextureDesc );
        m_lightVolumeTarget->label( "Light Volume Drawable" );
    }
    else
    {
        delete m_lightVolumeTarget;
        m_lightVolumeTarget = nullptr;
    }
}


/// Draws icosahedrons encapsulating the pointLight volumes in *red* when 'fullVolumes' is set
/// This shows the fragments the point light fragment shader would need to execute if culling were
/// not enabled.  If light culling is enabled, the fragments drawn when culling enabled are colored
/// *green* allowing user to compare the coverage
void BufferExaminationManager::renderLightVolumesExaminationWithCommandBuffer(MTL::CommandBuffer & commandBuffer,
                                                                              bool fullVolumes)
{
    MTL::RenderPassDescriptor renderPassDescriptor;
    renderPassDescriptor.colorAttachments[0].clearColor( MTL::ClearColorMake(0, 0, 0, 1) );
    renderPassDescriptor.colorAttachments[0].loadAction( MTL::LoadActionClear );
    renderPassDescriptor.colorAttachments[0].texture( *m_lightVolumeTarget );
    renderPassDescriptor.colorAttachments[0].storeAction( MTL::StoreActionStore );

    {
        MTL::RenderCommandEncoder renderEncoder =
            commandBuffer.renderCommandEncoderWithDescriptor( renderPassDescriptor );
        renderEncoder.label( "Stenciled light volumes background" );

        // First draw the final fully composited scene as the background
        renderEncoder.setRenderPipelineState( m_textureRGBPipelineState );
        renderEncoder.setVertexBuffer( m_renderer.quadVertexBuffer(), 0, BufferIndexMeshPositions );
        renderEncoder.setFragmentTexture( *m_offscreenDrawable, TextureIndexBaseColor );
        renderEncoder.drawPrimitives( MTL::PrimitiveTypeTriangle, 0, 6 );

        renderEncoder.endEncoding();
    }

    renderPassDescriptor.depthAttachment.texture( m_renderer.depthStencilTexture() );
    renderPassDescriptor.stencilAttachment.texture( m_renderer.depthStencilTexture() );
    renderPassDescriptor.colorAttachments[0].loadAction( MTL::LoadActionLoad );
    renderPassDescriptor.depthAttachment.loadAction( MTL::LoadActionLoad );
    renderPassDescriptor.stencilAttachment.loadAction( MTL::LoadActionLoad );

    {
        MTL::RenderCommandEncoder renderEncoder =
            commandBuffer.renderCommandEncoderWithDescriptor( renderPassDescriptor );
        renderEncoder.label( "Stenciled light volumes" );

        // Set simple pipeline which just draws a single color
        renderEncoder.setRenderPipelineState( m_lightVolumeVisualizationPipelineState );
        renderEncoder.setVertexBuffer( m_renderer.frameDataBuffer( m_renderer.frameDataBufferIndex() ), 0, BufferIndexFrameData );
        renderEncoder.setVertexBuffer( m_renderer.lightsData(), 0, BufferIndexLightsData );
        renderEncoder.setVertexBuffer( m_renderer.lightPositions( m_renderer.frameDataBufferIndex() ), 0, BufferIndexLightsPosition );

        const std::vector<MeshBuffer> & icoshedronVertexBuffers = m_renderer.icosahedronMesh().vertexBuffers();
        renderEncoder.setVertexBuffer( icoshedronVertexBuffers[0].buffer(), icoshedronVertexBuffers[0].offset(), BufferIndexMeshPositions );

        const Mesh & icosahedronMesh = m_renderer.icosahedronMesh();
        const std::vector<Submesh> & icosahedronSubmesh = icosahedronMesh.submeshes();

        if(fullVolumes || !LIGHT_STENCIL_CULLING)
        {
            // Set depth stencil state that uses stencil test to cull fragments
            renderEncoder.setDepthStencilState( m_depthTestOnlyDepthStencilState );

            // Set red color to output in fragment function
            simd::float4 redColor = { 1, 0, 0, 1 };
            renderEncoder.setFragmentBytes( &redColor, sizeof(redColor), BufferIndexFlatColor );

            renderEncoder.drawIndexedPrimitives( icosahedronSubmesh[0].primitiveType(),
                                                 icosahedronSubmesh[0].indexCount(),
                                                 icosahedronSubmesh[0].indexType(),
                                                 icosahedronSubmesh[0].indexBuffer().buffer(),
                                                 icosahedronSubmesh[0].indexBuffer().offset(),
                                                 NumLights );
        }

#if LIGHT_STENCIL_CULLING

        // Set green color to output in fragment function
        simd::float4 greenColor = { 0, 1, 0, 1 };
        renderEncoder.setFragmentBytes( &greenColor, sizeof(greenColor), BufferIndexFlatColor );

        // Set depth stencil state that uses stencil test to cull fragments
        renderEncoder.setDepthStencilState( m_renderer.pointLightDepthStencilState() );

        renderEncoder.setCullMode( MTL::CullModeBack );

        renderEncoder.setStencilReferenceValue( 128 );

        renderEncoder.drawIndexedPrimitives( icosahedronSubmesh[0].primitiveType(),
                                             icosahedronSubmesh[0].indexCount(),
                                             icosahedronSubmesh[0].indexType(),
                                             icosahedronSubmesh[0].indexBuffer().buffer(),
                                             icosahedronSubmesh[0].indexBuffer().offset(),
                                             NumLights );
#endif // END LIGHT_STENCIL_CULLING

        renderEncoder.endEncoding();
    }


}

void BufferExaminationManager::drawAlbedoGBufferWithCommandBuffer(MTL::CommandBuffer & commandBuffer)
{
    MTL::RenderPassDescriptor *currentRenderPassDescriptor = m_albedoGBufferView.currentRenderPassDescriptor();

    MTL::RenderCommandEncoder renderEncoder =
        commandBuffer.renderCommandEncoderWithDescriptor( *currentRenderPassDescriptor );

    MTL::RenderPassColorAttachmentDescriptor & attachmentDesc =
        m_albedoGBufferView.currentRenderPassDescriptor()->colorAttachments[0];

    MTL::Texture & texture = attachmentDesc.texture();

    texture.label("m_albedoGBufferViewDrawable");
    renderEncoder.label( "drawAlbedoGBufferWithCommandBuffer" );
    renderEncoder.setRenderPipelineState( m_textureRGBPipelineState );
    renderEncoder.setVertexBuffer( m_renderer.quadVertexBuffer(), 0, BufferIndexMeshPositions );
    renderEncoder.setFragmentTexture( m_renderer.albedo_specular_GBuffer(), TextureIndexBaseColor );
    renderEncoder.drawPrimitives( MTL::PrimitiveTypeTriangle, 0, 6 );
    renderEncoder.endEncoding();
}

void BufferExaminationManager::drawNormalsGBufferWithCommandBuffer(MTL::CommandBuffer & commandBuffer)
{
    MTL::RenderCommandEncoder renderEncoder =
        commandBuffer.renderCommandEncoderWithDescriptor( *m_normalsGBufferView.currentRenderPassDescriptor() );
    renderEncoder.label( "drawNormalsGBufferWithCommandBuffer" );
    renderEncoder.setRenderPipelineState( m_textureRGBPipelineState );
    renderEncoder.setVertexBuffer( m_renderer.quadVertexBuffer(), 0, BufferIndexMeshPositions );
    renderEncoder.setFragmentTexture( m_renderer.normal_shadow_GBuffer(), TextureIndexBaseColor );
    renderEncoder.drawPrimitives( MTL::PrimitiveTypeTriangle, 0, 6 );
    renderEncoder.endEncoding();
}

void BufferExaminationManager::drawDepthGBufferWithCommandBuffer(MTL::CommandBuffer & commandBuffer)
{
    MTL::RenderCommandEncoder renderEncoder =
        commandBuffer.renderCommandEncoderWithDescriptor( *m_depthGBufferView.currentRenderPassDescriptor() );
    renderEncoder.label( "drawDepthGBufferWithCommandBuffer" );
    renderEncoder.setRenderPipelineState( m_textureDepthPipelineState );
    renderEncoder.setVertexBuffer( m_renderer.quadVertexBuffer(), 0, BufferIndexMeshPositions );
    renderEncoder.setFragmentTexture( m_renderer.depth_GBuffer(), TextureIndexBaseColor );
#if USE_EYE_DEPTH
    float depthRange = FarPlane - NearPlane;
#else
    float depthRange = 1.0;
#endif
    renderEncoder.setFragmentBytes( &depthRange, sizeof(depthRange), BufferIndexDepthRange );
    renderEncoder.drawPrimitives( MTL::PrimitiveTypeTriangle, 0, 6 );
    renderEncoder.endEncoding();
}

void BufferExaminationManager::drawShadowGBufferWithCommandBuffer(MTL::CommandBuffer & commandBuffer)
{
    MTL::RenderCommandEncoder renderEncoder =
        commandBuffer.renderCommandEncoderWithDescriptor( *m_shadowGBufferView.currentRenderPassDescriptor() );
    renderEncoder.label( "drawShadowGBufferWithCommandBuffer" );
    renderEncoder.setRenderPipelineState( m_textureAlphaPipelineState );
    renderEncoder.setVertexBuffer( m_renderer.quadVertexBuffer(), 0, BufferIndexMeshPositions );
    renderEncoder.setFragmentTexture( m_renderer.normal_shadow_GBuffer(), TextureIndexBaseColor );
    renderEncoder.drawPrimitives( MTL::PrimitiveTypeTriangle, 0, 6 );
    renderEncoder.endEncoding();
}

void BufferExaminationManager::drawFinalRenderWithCommandBuffer(MTL::CommandBuffer & commandBuffer)
{
    MTL::RenderCommandEncoder renderEncoder =
        commandBuffer.renderCommandEncoderWithDescriptor( *m_finalFrameView.currentRenderPassDescriptor() );
    renderEncoder.label( "drawFinalRenderWithCommandBuffer" );
    renderEncoder.setRenderPipelineState( m_textureRGBPipelineState );
    renderEncoder.setVertexBuffer( m_renderer.quadVertexBuffer(), 0, BufferIndexMeshPositions );
    renderEncoder.setFragmentTexture( *m_offscreenDrawable, TextureIndexBaseColor );
    renderEncoder.drawPrimitives( MTL::PrimitiveTypeTriangle, 0, 6 );
    renderEncoder.endEncoding();
}


void BufferExaminationManager::drawSpecularGBufferWithCommandBuffer(MTL::CommandBuffer & commandBuffer)
{
    MTL::RenderCommandEncoder renderEncoder =
        commandBuffer.renderCommandEncoderWithDescriptor( *m_specularGBufferView.currentRenderPassDescriptor() );
    renderEncoder.label( "drawSpecularGBufferWithCommandBuffer" );
    renderEncoder.setRenderPipelineState( m_textureAlphaPipelineState );
    renderEncoder.setVertexBuffer( m_renderer.quadVertexBuffer(), 0, BufferIndexMeshPositions );
    renderEncoder.setFragmentTexture( m_renderer.albedo_specular_GBuffer(), TextureIndexBaseColor );
    renderEncoder.drawPrimitives( MTL::PrimitiveTypeTriangle, 0, 6 );
    renderEncoder.endEncoding();
}

void BufferExaminationManager::drawShadowMapWithCommandBuffer(MTL::CommandBuffer & commandBuffer)
{
    MTL::RenderCommandEncoder renderEncoder =
        commandBuffer.renderCommandEncoderWithDescriptor( *m_shadowMapView.currentRenderPassDescriptor() );
    renderEncoder.label( "drawShadowMapWithCommandBuffer" );
    float depthRange = 1.0;
    renderEncoder.setFragmentBytes( &depthRange, sizeof(depthRange), BufferIndexDepthRange );
    renderEncoder.setRenderPipelineState( m_textureDepthPipelineState );
    renderEncoder.setVertexBuffer( m_renderer.quadVertexBuffer(), 0, BufferIndexMeshPositions );
    renderEncoder.setFragmentTexture( m_renderer.shadowMap(), TextureIndexBaseColor );
    renderEncoder.drawPrimitives( MTL::PrimitiveTypeTriangle, 0, 6 );
    renderEncoder.endEncoding();
}

void BufferExaminationManager::drawLightMaskWithCommandBuffer(MTL::CommandBuffer & commandBuffer)
{
    renderLightVolumesExaminationWithCommandBuffer(commandBuffer, false);

    MTL::RenderCommandEncoder renderEncoder =
        commandBuffer.renderCommandEncoderWithDescriptor( *m_lightMaskView.currentRenderPassDescriptor() );
    renderEncoder.label( "drawLightMaskWithCommandBuffer" );
    renderEncoder.setRenderPipelineState( m_textureRGBPipelineState );
    renderEncoder.setVertexBuffer( m_renderer.quadVertexBuffer(), 0, BufferIndexMeshPositions );
    renderEncoder.setFragmentTexture( *m_lightVolumeTarget, TextureIndexBaseColor );
    renderEncoder.drawPrimitives( MTL::PrimitiveTypeTriangle, 0, 6 );
    renderEncoder.endEncoding();
}

void BufferExaminationManager::drawLightVolumesWithCommandBuffer(MTL::CommandBuffer & commandBuffer)
{
    renderLightVolumesExaminationWithCommandBuffer(commandBuffer, true);

    MTL::RenderCommandEncoder renderEncoder =
        commandBuffer.renderCommandEncoderWithDescriptor( *m_lightCoverageView.currentRenderPassDescriptor() );
    renderEncoder.label( "drawLightVolumesWithCommandBuffer" );
    renderEncoder.setRenderPipelineState( m_textureRGBPipelineState );
    renderEncoder.setVertexBuffer( m_renderer.quadVertexBuffer(), 0, BufferIndexMeshPositions );
    renderEncoder.setFragmentTexture( *m_lightVolumeTarget, TextureIndexBaseColor );
    renderEncoder.drawPrimitives( MTL::PrimitiveTypeTriangle, 0, 6 );
    renderEncoder.endEncoding();
}

void BufferExaminationManager::mode(ExaminationMode mode)
{
    m_mode = mode;

    m_finalFrameView.hidden     ( !(m_mode == ExaminationModeAll) );
    m_albedoGBufferView.hidden  ( !(m_mode & ExaminationModeAlbedo) );
    m_normalsGBufferView.hidden ( !(m_mode & ExaminationModeNormals) );
    m_depthGBufferView.hidden   ( !(m_mode & ExaminationModeDepth) );
    m_shadowGBufferView.hidden  ( !(m_mode & ExaminationModeShadowGBuffer) );
    m_specularGBufferView.hidden( !(m_mode & ExaminationModeSpecular) );
    m_shadowMapView.hidden      ( !(m_mode & ExaminationModeShadowMap) );
    m_lightMaskView.hidden      ( !(m_mode & ExaminationModeMaskedLightVolumes) );
    m_lightCoverageView.hidden  ( !(m_mode & ExaminationModeFullLightVolumes) );

    updateDrawableSize( m_renderer.view().drawableSize() );
}

void BufferExaminationManager::drawAndPresentBuffersWithCommandBuffer(MTL::CommandBuffer & commandBuffer)
{

    struct ExaminationDrawablePresentHandler : MTL::CommandBufferHandler
    {
        std::forward_list<MTL::Drawable> drawablesToPresent;

        void operator()(const MTL::CommandBuffer &)
        {
            for(MTL::Drawable & drawable : drawablesToPresent )
            {
                drawable.present();
            }
            delete this;
        }
    } *examinationDrawablePresentHandler = new ExaminationDrawablePresentHandler;

    if(m_mode == ExaminationModeAll && m_finalFrameView.currentDrawable())
    {
        drawFinalRenderWithCommandBuffer( commandBuffer );
        examinationDrawablePresentHandler->drawablesToPresent.emplace_front( *(m_finalFrameView.currentDrawable()) );
         m_finalFrameView.draw(); // Resets MTKView currentDrawable for next frame
    }

    if(m_mode & ExaminationModeAlbedo && m_albedoGBufferView.currentDrawable())
    {
        drawAlbedoGBufferWithCommandBuffer( commandBuffer );
        examinationDrawablePresentHandler->drawablesToPresent.emplace_front( *(m_albedoGBufferView.currentDrawable()) );
        m_albedoGBufferView.draw(); // Resets MTKView currentDrawable for next frame
    }

    if(m_mode & ExaminationModeNormals && m_normalsGBufferView.currentDrawable())
    {
        drawNormalsGBufferWithCommandBuffer( commandBuffer );
        examinationDrawablePresentHandler->drawablesToPresent.emplace_front( *(m_normalsGBufferView.currentDrawable()) );
        m_normalsGBufferView.draw(); // Resets MTKView currentDrawable for next frame
    }

    if(m_mode & ExaminationModeDepth && m_depthGBufferView.currentDrawable())
    {
        drawDepthGBufferWithCommandBuffer( commandBuffer );
        examinationDrawablePresentHandler->drawablesToPresent.emplace_front( *(m_depthGBufferView.currentDrawable()) );
        m_depthGBufferView.draw(); // Resets MTKView currentDrawable for next frame
    }

    if(m_mode & ExaminationModeShadowGBuffer && m_shadowGBufferView.currentDrawable())
    {
        drawShadowGBufferWithCommandBuffer( commandBuffer );
        examinationDrawablePresentHandler->drawablesToPresent.emplace_front( *(m_shadowGBufferView.currentDrawable()) );
        m_shadowGBufferView.draw(); // Resets MTKView currentDrawable for next frame
    }

    if(m_mode & ExaminationModeSpecular && m_specularGBufferView.currentDrawable())
    {
        drawSpecularGBufferWithCommandBuffer( commandBuffer );
        examinationDrawablePresentHandler->drawablesToPresent.emplace_front( *(m_specularGBufferView.currentDrawable()) );
        m_specularGBufferView.draw(); // Resets MTKView currentDrawable for next frame
    }

    if(m_mode & ExaminationModeShadowMap && m_shadowMapView.currentDrawable())
    {
        drawShadowMapWithCommandBuffer( commandBuffer );
        examinationDrawablePresentHandler->drawablesToPresent.emplace_front( *(m_shadowMapView.currentDrawable()) );
        m_shadowMapView.draw(); // Resets MTKView currentDrawable for next frame
    }

    if(m_mode & ExaminationModeMaskedLightVolumes && m_lightMaskView.currentDrawable())
    {
        drawLightMaskWithCommandBuffer( commandBuffer );
        examinationDrawablePresentHandler->drawablesToPresent.emplace_front( *(m_lightMaskView.currentDrawable()) );
        m_lightMaskView.draw(); // Resets MTKView currentDrawable for next frame
    }

    if(m_mode & ExaminationModeFullLightVolumes && m_lightCoverageView.currentDrawable())
    {
        drawLightVolumesWithCommandBuffer( commandBuffer );
        examinationDrawablePresentHandler->drawablesToPresent.emplace_front( *(m_lightCoverageView.currentDrawable()) );
        m_lightCoverageView.draw(); // Resets MTKView currentDrawable for next frame
    }

    commandBuffer.addScheduledHandler(*examinationDrawablePresentHandler);

}

#endif // END SUPPORT_BUFFER_EXAMINATION
