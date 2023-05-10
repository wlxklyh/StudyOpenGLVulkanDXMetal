# 图形库

<!-- TOC -->

- [图形库](#图形库)
  - [Opengl(Win)](#openglwin)
    - [Chapter1 getting\_started](#chapter1-getting_started)
      - [1.1.hello\_window](#11hello_window)
      - [1.2.hello\_window\_clear](#12hello_window_clear)
      - [2.1.hello\_triangle](#21hello_triangle)
      - [2.2.hello\_triangle\_indexed](#22hello_triangle_indexed)
      - [2.3.hello\_triangle\_exercise1](#23hello_triangle_exercise1)
      - [2.4.hello\_triangle\_exercise2](#24hello_triangle_exercise2)
      - [2.5.hello\_triangle\_exercise3](#25hello_triangle_exercise3)
      - [3.1.shaders\_uniform](#31shaders_uniform)
      - [3.2.shaders\_interpolation](#32shaders_interpolation)
      - [3.3.shaders\_class](#33shaders_class)
      - [3.4.shaders\_exercise1](#34shaders_exercise1)
      - [3.5.shaders\_exercise2](#35shaders_exercise2)
      - [3.6.shaders\_exercise3](#36shaders_exercise3)
      - [4.1.textures](#41textures)
      - [4.2.textures\_combined](#42textures_combined)
      - [4.3.textures\_exercise1](#43textures_exercise1)
      - [4.4.textures\_exercise2](#44textures_exercise2)
      - [4.5.textures\_exercise3](#45textures_exercise3)
      - [4.6.textures\_exercise4](#46textures_exercise4)
      - [5.1.transformations](#51transformations)
      - [6.1.coordinate\_systems](#61coordinate_systems)
      - [6.2.coordinate\_systems\_depth](#62coordinate_systems_depth)
      - [6.3.coordinate\_systems\_multiple](#63coordinate_systems_multiple)
      - [7.1.camera\_circle](#71camera_circle)
      - [7.2.camera\_keyboard\_dt](#72camera_keyboard_dt)
      - [7.3.camera\_mouse\_zoom](#73camera_mouse_zoom)
      - [7.4.camera\_class](#74camera_class)
    - [Chapter2 lighting](#chapter2-lighting)
      - [1.colors](#1colors)
      - [2.1.basic\_lighting\_diffuse](#21basic_lighting_diffuse)
      - [2.2.basic\_lighting\_specular](#22basic_lighting_specular)
      - [3.1.materials](#31materials)
      - [3.2.materials\_exercise1](#32materials_exercise1)
      - [4.1.lighting\_maps\_diffuse\_map](#41lighting_maps_diffuse_map)
      - [4.2.lighting\_maps\_specular\_map](#42lighting_maps_specular_map)
      - [4.4.lighting\_maps\_exercise4](#44lighting_maps_exercise4)
      - [5.1.light\_casters\_directional](#51light_casters_directional)
      - [5.2.light\_casters\_point](#52light_casters_point)
      - [5.3.light\_casters\_spot](#53light_casters_spot)
      - [5.4.light\_casters\_spot\_soft](#54light_casters_spot_soft)
      - [6.multiple\_lights](#6multiple_lights)
    - [Chapter3.model\_loading](#chapter3model_loading)
      - [1.model\_loading](#1model_loading)
    - [Chapter4.advanced\_opengl](#chapter4advanced_opengl)
      - [1.1.depth\_testing](#11depth_testing)
      - [1.2.depth\_testing\_view](#12depth_testing_view)
      - [2.stencil\_testing](#2stencil_testing)
      - [3.1.blending\_discard](#31blending_discard)
      - [3.2.blending\_sort](#32blending_sort)
      - [5.1.framebuffers](#51framebuffers)
      - [5.2.framebuffers\_exercise1](#52framebuffers_exercise1)
      - [6.1.cubemaps\_skybox](#61cubemaps_skybox)
      - [6.2.cubemaps\_environment\_mapping](#62cubemaps_environment_mapping)
      - [8.advanced\_glsl\_ubo](#8advanced_glsl_ubo)
      - [9.1.geometry\_shader\_houses](#91geometry_shader_houses)
      - [9.2.geometry\_shader\_exploding](#92geometry_shader_exploding)
      - [9.3.geometry\_shader\_normals](#93geometry_shader_normals)
      - [10.1.instancing\_quads](#101instancing_quads)
      - [10.2.asteroids](#102asteroids)
      - [10.3.asteroids\_instanced](#103asteroids_instanced)
      - [11.1.anti\_aliasing\_msaa](#111anti_aliasing_msaa)
      - [11.2.anti\_aliasing\_offscreen](#112anti_aliasing_offscreen)
    - [Chapter5.advanced\_lighting](#chapter5advanced_lighting)
      - [1.advanced\_lighting](#1advanced_lighting)
      - [2.gamma\_correction](#2gamma_correction)
      - [3.1.1.shadow\_mapping\_depth](#311shadow_mapping_depth)
      - [3.1.2.shadow\_mapping\_base](#312shadow_mapping_base)
      - [3.1.3.shadow\_mapping](#313shadow_mapping)
      - [3.2.1.point\_shadows](#321point_shadows)
      - [3.2.2.point\_shadows\_soft](#322point_shadows_soft)
      - [4.normal\_mapping](#4normal_mapping)
      - [5.1.parallax\_mapping](#51parallax_mapping)
      - [5.2.steep\_parallax\_mapping](#52steep_parallax_mapping)
      - [5.3.parallax\_occlusion\_mapping](#53parallax_occlusion_mapping)
      - [6.hdr](#6hdr)
      - [7.bloom](#7bloom)
      - [8.1.deferred\_shading](#81deferred_shading)
      - [8.2.deferred\_shading\_volumes](#82deferred_shading_volumes)
      - [9.ssao](#9ssao)
    - [Chapter6.pbr](#chapter6pbr)
      - [1.1.lighting](#11lighting)
      - [1.2.lighting\_textured](#12lighting_textured)
      - [2.1.1.ibl\_irradiance\_conversion](#211ibl_irradiance_conversion)
      - [2.1.2.ibl\_irradiance](#212ibl_irradiance)
      - [2.2.1.ibl\_specular](#221ibl_specular)
      - [2.2.2.ibl\_specular\_textured](#222ibl_specular_textured)
    - [Chapter7.in\_practice](#chapter7in_practice)
      - [1.debugging](#1debugging)
      - [2.text\_rendering](#2text_rendering)
    - [自己的练习](#自己的练习)

<!-- /TOC -->

## Opengl(Win)

环境：
https://github.com/wlxklyh/LearnOpenglEmpty

工程地址：
https://github.com/wlxklyh/StudyOpenGLVulkanDXMetal/tree/master/Opengl/Win

教程：
https://learnopengl-cn.github.io/


### Chapter1 getting_started

#### 1.1.hello_window

![](Img/73245585712800.png)

#### 1.2.hello_window_clear

![](Img/73087679189900.png)

#### 2.1.hello_triangle

![](Img/80136028977200.png)

#### 2.2.hello_triangle_indexed

![](Img/80514319809600.png)

#### 2.3.hello_triangle_exercise1

![](Img/81700324298100.png)

#### 2.4.hello_triangle_exercise2

![](Img/81679339973300.png)

#### 2.5.hello_triangle_exercise3

![](Img/82838181847400.png)

#### 3.1.shaders_uniform

![](Img/shaderdemo.gif)

#### 3.2.shaders_interpolation

![](Img/86402297262800.png)

#### 3.3.shaders_class

![](Img/89142019129100.png)

#### 3.4.shaders_exercise1

![](Img/90298239035300.png)

#### 3.5.shaders_exercise2

![](Img/show1.3.5.gif)

#### 3.6.shaders_exercise3

左下角是(-0.5,-0.5,0) clamp to (0.0,0.0,0.0) 所以黑色
![](Img/91163892378100.png)

#### 4.1.textures

![](Img/143207961767300.png)

#### 4.2.textures_combined

![](Img/144150398471700.png)
![](Img/146516589965100.png)

#### 4.3.textures_exercise1

![](Img/146870393141500.png)

#### 4.4.textures_exercise2

![](Img/146995996232500.png)

#### 4.5.textures_exercise3

![](Img/147376013630600.png)

#### 4.6.textures_exercise4

![](Img/show1.4.6.gif)

#### 5.1.transformations

![](Img/show1.5.1.gif)

#### 6.1.coordinate_systems

//省略了

#### 6.2.coordinate_systems_depth

//省略了

#### 6.3.coordinate_systems_multiple

用了EBO绘制 左右面 上下面的 UV是相邻的 所以看起来是被拉伸了。
![](Img/show1.6.3.gif)

#### 7.1.camera_circle

//see 7.4.camera_class

#### 7.2.camera_keyboard_dt

//see 7.4.camera_class

#### 7.3.camera_mouse_zoom

//see 7.4.camera_class

#### 7.4.camera_class

![](Img/show1.7.4.gif)

### Chapter2 lighting

#### 1.colors

![](Img/213638602829400.png)

#### 2.1.basic_lighting_diffuse

![](Img/230843365859100.png)

#### 2.2.basic_lighting_specular

![](Img/230793363366600.png)

#### 3.1.materials

![](Img/show2.3.1.gif)

#### 3.2.materials_exercise1


#### 4.1.lighting_maps_diffuse_map

![](Img/250082963569600.png)


#### 4.2.lighting_maps_specular_map

![](Img/293985668245400.png)

#### 4.4.lighting_maps_exercise4


#### 5.1.light_casters_directional

![](Img/show2.5.1.gif)

#### 5.2.light_casters_point

![](Img/show2.5.2.gif)

#### 5.3.light_casters_spot

![](Img/show2.5.3.gif)

#### 5.4.light_casters_spot_soft

![](Img/show2.5.4.gif)

#### 6.multiple_lights

蓝色是点光  红色是聚光灯 还有个方向灯是镜面反射比较明显的。
![](Img/show2.6.gif)

### Chapter3.model_loading

#### 1.model_loading

![](Img/show3.1.gif)

### Chapter4.advanced_opengl

#### 1.1.depth_testing

按着Q键切换
![](Img/show4.1.gif)

#### 1.2.depth_testing_view

#### 2.stencil_testing

![](Img/show4.2.gif)

#### 3.1.blending_discard

没有discard opengl不知道怎么处理alpha 所以就会下面这样
![](Img/show4.3.1.png)

discard 但是环绕模式是GL_REPEAT
![](Img/show4.3.1discardbutrepeat.png)

GL_CLAMP_TO_EDGE
![](Img/show4.3.1discardedge.png)

#### 3.2.blending_sort

![](Img/2022-05-13-00-13-31.png)

#### 5.1.framebuffers

#### 5.2.framebuffers_exercise1

#### 6.1.cubemaps_skybox

#### 6.2.cubemaps_environment_mapping

#### 8.advanced_glsl_ubo

#### 9.1.geometry_shader_houses

#### 9.2.geometry_shader_exploding

#### 9.3.geometry_shader_normals

#### 10.1.instancing_quads

#### 10.2.asteroids

#### 10.3.asteroids_instanced

#### 11.1.anti_aliasing_msaa

#### 11.2.anti_aliasing_offscreen


### Chapter5.advanced_lighting
#### 1.advanced_lighting

#### 2.gamma_correction

#### 3.1.1.shadow_mapping_depth

#### 3.1.2.shadow_mapping_base

#### 3.1.3.shadow_mapping

#### 3.2.1.point_shadows

#### 3.2.2.point_shadows_soft

#### 4.normal_mapping

#### 5.1.parallax_mapping

#### 5.2.steep_parallax_mapping

#### 5.3.parallax_occlusion_mapping

#### 6.hdr

#### 7.bloom

#### 8.1.deferred_shading

#### 8.2.deferred_shading_volumes

#### 9.ssao


### Chapter6.pbr
#### 1.1.lighting

#### 1.2.lighting_textured

#### 2.1.1.ibl_irradiance_conversion

#### 2.1.2.ibl_irradiance

#### 2.2.1.ibl_specular

#### 2.2.2.ibl_specular_textured

### Chapter7.in_practice
#### 1.debugging

#### 2.text_rendering


### 自己的练习

||例1|例2|
|-|-|-|
|CPU:|![](Img/2023-05-10-22-26-42.png)|![](Img/2023-05-10-22-26-42.png)|
|Shader:|![](Img/2023-05-10-22-27-16.png)|![](Img/2023-05-10-22-35-12.png)|
|表现:|![](Img/2023-05-10-22-27-36.png)|![](Img/2023-05-10-22-27-36.png)|



