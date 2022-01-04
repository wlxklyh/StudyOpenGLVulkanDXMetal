/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Metal shaders used to render buffer examination mode
*/
#include <metal_stdlib>

using namespace metal;

// Include header shared between this Metal shader code and C code executing Metal API commands
#include "AAPLShaderTypes.h"

#if SUPPORT_BUFFER_EXAMINATION 

// Include header shared between all Metal shader code files
#include "AAPLShaderCommon.h"

struct LightInfoData
{
    float4 position [[position]];
};

vertex LightInfoData
light_volume_visualization_vertex(const device float4        *vertices        [[ buffer(BufferIndexMeshPositions) ]],
                                  const device PointLight    *light_data      [[ buffer(BufferIndexLightsData) ]],
                                  const device vector_float4 *light_positions [[ buffer(BufferIndexLightsPosition) ]],
                                        uint                  iid             [[ instance_id ]],
                                        uint                  vid             [[ vertex_id ]],
                                        constant FrameData   &frameData       [[ buffer(BufferIndexFrameData) ]])
{
    LightInfoData out;

    // Transform light to position relative to the temple
    float4 vertex_view_position = float4(vertices[vid].xyz * light_data[iid].light_radius + light_positions[iid].xyz, 1);

    out.position = frameData.projection_matrix * vertex_view_position;

    return out;
}

fragment float4
light_volume_visualization_fragment(constant float4 & color [[ buffer(BufferIndexFlatColor) ]])
{
    return color;
}

struct RenderTextureData
{
    float4 position [[position]];
    float2 tex_coord;
};

vertex RenderTextureData
texture_values_vertex(const device SimpleVertex *vertices [[ buffer(BufferIndexMeshPositions) ]],
                      uint                       vid      [[ vertex_id ]])
{
    RenderTextureData out;

    out.position = float4(vertices[vid].position, 0, 1);
    out.tex_coord = (out.position.xy + 1) * .5;
    out.tex_coord.y = 1-out.tex_coord.y;

    return out;
}

fragment half4
texture_rgb_fragment(RenderTextureData in      [[ stage_in ]],
                     texture2d<half>   texture [[ texture(TextureIndexBaseColor) ]])
{
    constexpr sampler linearSampler(mip_filter::none,
                                    mag_filter::linear,
                                    min_filter::linear);

    half4 sample = texture.sample(linearSampler, in.tex_coord);

    return sample;
}

fragment half4
texture_alpha_fragment(RenderTextureData in      [[ stage_in ]],
                       texture2d<float>  texture [[ texture(TextureIndexBaseColor) ]])
{
    constexpr sampler linearSampler(mip_filter::none,
                                    mag_filter::linear,
                                    min_filter::linear);

    float4 sample = texture.sample(linearSampler, in.tex_coord);

    return half4(sample.wwww);
}

// Used to visualize the linear depth buffer

fragment half4
texture_depth_fragment(RenderTextureData  in         [[ stage_in ]],
                       texture2d<float>   texture    [[ texture(TextureIndexBaseColor) ]],
                       constant float    &depthRange [[ buffer(BufferIndexDepthRange) ]])
{
    constexpr sampler linearSampler(mip_filter::none,
                                    mag_filter::linear,
                                    min_filter::linear);

    float sample = texture.sample(linearSampler, in.tex_coord).x;

    return sample / depthRange;
}

#endif
