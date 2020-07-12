//
//  main.cpp
//  OpenGL绘制甜甜圈
//
//  Created by yz on 2020/3/27.
//  Copyright © 2020 yz. All rights reserved.
//


#include <stdio.h>
#include "GLTools.h"
#include "GLBatch.h"//三角形批次类(传输顶点/光照/纹理/颜色->存储着色器)
#include "GLMatrixStack.h"//矩阵工具(单元矩阵/矩阵/矩阵相乘/压栈/出栈/缩放/平移/旋转)
#include "GLFrustum.h"//矩阵工具(透视/正视投影矩阵 坐标3D->2D映射)
#include "GLFrame.h"//矩阵工具(位置)
#include "GLGeometryTransform.h"//变换管线

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


//着色器
GLShaderManager shaderManager;

//三角形批次
GLTriangleBatch torusBatch;

//变换管线
GLGeometryTransform transformPipeLine;

//模型矩阵
GLMatrixStack modeViewMatrix;
//投影矩阵
GLMatrixStack projectionMatrix;

//透视投影
GLFrustum viewFrustum;

//设置角色帧，作为相机 观察者
GLFrame viewFrame;

//背面剔除/深度测试
int isCull = 0;
int isDepth = 0;

//初始数据设置
void SetupRC(){
    
    /*业务逻辑 1、设置背影颜色 2、初始化着色器 3、设置图形顶点数据 4、用GLBatch三角形批次类将数据传递着色器*/
    
    //设置背景色
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    //初始化着色器
    shaderManager.InitializeStockShaders();
    
    //将相机向后移动7个单元。肉眼到物体之间的距离
    viewFrame.MoveForward(7.0);
//    viewFrame.MoveUp(11.0);
//    viewFrame.MoveRight(3.0);
    
    //创建甜甜圈(系统工具)
    /*gltMakeTorus(<#GLTriangleBatch &torusBatch#>, <#GLfloat majorRadius#>, <#GLfloat minorRadius#>, <#GLint numMajor#>, <#GLint numMinor#>)
     参数1：GLTriangleBatch 容器帮助类
     参数2：外边缘半径
     参数3：内边缘半径
     参数4、5：主半径和从半径的细分单元数量
     参数4、减少(填充物减少，有明显的棱角)
     参数5、减少(变薄，没有厚度)
     */
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 23);
    
    //点的大小。方便填充时肉眼观察
    glPointSize(4.0f);
}

//渲染
void RenderScene(){
    /*业务逻辑1、清理缓冲 2、使用存储着色器 3、绘制图形*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (isCull) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }else{
        glDisable(GL_CULL_FACE);
    }
    
    if (isDepth) {
        glEnable(GL_DEPTH_TEST);
    }else{
        glDisable(GL_DEPTH_TEST);
    }
    
    //把摄像机 观察者 压入矩阵模型
    modeViewMatrix.PushMatrix(viewFrame);
    
    //设置绘制图形颜色
    GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    
    //使用平面着色器(平面着色器，模型投影矩阵，颜色)
//    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeLine.GetModelViewProjectionMatrix(),vRed);
    
    //使用默认光源着色器
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,transformPipeLine.GetModelViewMatrix(),transformPipeLine.GetProjectionMatrix(),vGreen);
    /*使用默认光源着色器 旋转时会产生*/
    
    
    //绘制
    torusBatch.Draw();
    //出栈
    modeViewMatrix.PopMatrix();
    glutSwapBuffers();
}

//视口change
void ChangeSize(int w,int h){
    glViewport(0, 0, w, h);
    
    //创建透视投影矩阵
    viewFrustum.SetPerspective(35.0f,float(w) / float(h),1.0f,100.0f);
    //透视投影矩阵加载到矩阵堆栈中
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //模型矩阵
    //单元矩阵(相当于赋初始值，可省)
    modeViewMatrix.LoadIdentity();
    
    //变换管线处理 模型/透视投影矩阵 数据
    transformPipeLine.SetMatrixStacks(modeViewMatrix, projectionMatrix);
    
}

//键位处理
void SpecailKeys(int key,int x,int y){
    if (key == GLUT_KEY_UP) {
        
        viewFrame.RotateWorld(m3dDegToRad(-5.0f) , 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_DOWN) {
        viewFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        viewFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_LEFT) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    
    //重绘
    glutPostRedisplay();
}

//右击
void processMenu(int value){
    switch (value) {
        case 1:
            isCull = !isCull;
            break;
        case 2:
            isDepth = !isDepth;
            break;
        case 3:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//颜色填充
            break;
        case 4:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//线填充
            break;
        case 5:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);//点填充
            break;
            
        default:
            break;
    }
    glutPostRedisplay();
}


int main(int argc,char * argv[]){
    
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("甜甜圈");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecailKeys);
    glutDisplayFunc(RenderScene);
    
    //添加右击菜单
    glutCreateMenu(processMenu);
    glutAddMenuEntry("背面剔除", 1);
    glutAddMenuEntry("深度测试", 2);
    glutAddMenuEntry("颜色填充", 3);
    glutAddMenuEntry("线填充", 4);
    glutAddMenuEntry("点填充", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    SetupRC();
    glutMainLoop();
      
    return 0;
}
