//
//  ViewController.m
//  LearnOpengles
//
//  Created by wlxk on 2020/9/28.
//  Copyright © 2020 wlxk. All rights reserved.
//

#import "ViewController.h"

typedef struct{
    GLKVector3 positionCoords;
}SceneVertex;
static const SceneVertex verticesForTriangle[] = {
    {{-0.9f,0.8f,0.0}},
    {{-0.8f,0.8f,0.0}},
    {{-0.9f,0.9f,0.0}}, 
    
    {{-0.9f,0.8f-0.2f,0.0}},
    {{-0.8f,0.8f-0.2f,0.0}},
    {{-0.9f,0.9f-0.2f,0.0}},
    
    {{-0.9f,0.8f-0.4f,0.0}},
    {{-0.8f,0.8f-0.4f,0.0}},
    {{-0.9f,0.9f-0.4f,0.0}},
    
    {{-0.9f,0.8f-0.6f,0.0}},
    {{-0.8f,0.8f-0.6f,0.0}},
    {{-0.9f,0.9f-0.6f,0.0}},
    {{-0.8f,0.9f-0.6f,0.0}},
    
    {{-0.9f,0.8f-0.8f,0.0}},
    {{-0.8f,0.8f-0.8f,0.0}},
    {{-0.9f,0.9f-0.8f,0.0}},
    {{-0.8f,0.9f-0.8f,0.0}}
};

@interface ViewController ()

@property(nonatomic,strong)GLKBaseEffect *baseEffect;
@property(nonatomic,assign)GLuint vertexBufferID;//缓存ID属性

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    //1、上下文准备
   GLKView *view = (GLKView *)self.view;
   view.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
   [EAGLContext setCurrentContext:view.context];

   self.baseEffect = [[GLKBaseEffect alloc] init];
   self.baseEffect.useConstantColor = GL_TRUE;
   self.baseEffect.constantColor = GLKVector4Make(1.0f,0.0f,0.0f,1.0f);
   
    //清除颜色 用白色来清除一遍缓冲区
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
   //2、生成一个缓冲ID三个步骤：生成id 绑定id 传入数据
   glGenBuffers(1, &_vertexBufferID);
   glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verticesForTriangle), verticesForTriangle, GL_STATIC_DRAW);
}

 

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect{
    [self.baseEffect prepareToDraw];
    glClear(GL_COLOR_BUFFER_BIT);

    //3.1 启动顶点缓存渲染操作
    glEnableVertexAttribArray(GLKVertexAttribPosition);

   //3.2 设置顶点缓冲的属性
    glVertexAttribPointer(GLKVertexAttribPosition, 3
                         , GL_FLOAT, GL_FALSE, sizeof(SceneVertex), NULL);
    //3.3 绘制
    glDrawArrays(GL_POINTS, 0, 3);
    glDrawArrays(GL_LINE_LOOP, 3, 3);
    glDrawArrays(GL_TRIANGLES, 6, 3);
    glDrawArrays(GL_LINE_STRIP, 9, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 13, 4);
}

-(void)dealloc{
    //4. 清理
    GLKView *view = (GLKView *)self.view;
    [EAGLContext setCurrentContext:view.context];
    
    if (0!=_vertexBufferID) {
        glDeleteBuffers(1, &_vertexBufferID);
        _vertexBufferID = 0;
    }
    
    ((GLKView *)self.view).context = nil;
    [EAGLContext setCurrentContext:nil];
}

@end
