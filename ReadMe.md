# iOS OpenGL ES 2.0学习

<!-- TOC -->

- [iOS OpenGL ES 2.0学习](#ios-opengl-es-20学习)
  - [OS OpenGL ES 2.0 开发（一）：绘制一个三角形](#os-opengl-es-20-开发一绘制一个三角形)
    - [1、xcode工程遇到的问题](#1xcode工程遇到的问题)
    - [2、结果如下：](#2结果如下)
  - [OS OpenGL ES 2.0 开发（二）：着色器（预告）](#os-opengl-es-20-开发二着色器预告)

<!-- /TOC -->
## OS OpenGL ES 2.0 开发（一）：绘制一个三角形
### 1、xcode工程遇到的问题
1、Storyboard的View的class要设置成GLKview

2、有Warning 要设置一个宏到build setting
![](2020-09-28-02-14-57.png)

### 2、结果如下：

```
glDrawArrays(GL_POINTS, 0, 3);          //画点
glDrawArrays(GL_LINE_LOOP, 3, 3);       //画线
glDrawArrays(GL_TRIANGLES, 6, 3);       //画三角形
glDrawArrays(GL_LINE_STRIP, 9, 4);      //画线
glDrawArrays(GL_TRIANGLE_STRIP, 13, 4); //画三角形
```
![](2020-09-28-02-17-04.png)



## OS OpenGL ES 2.0 开发（二）：着色器（预告）