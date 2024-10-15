#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Data.h"
#include "OBJLoader.h"
#include "PointCloud.h"
using namespace std;

const string OBJ_FILE = "model/spot/spot_triangulated.obj";   // OBJ 文件
const string TEXTURE_FILE = "model/spot/spot_texture.png";    // 纹理文件（已转换为 BMP 格式）
const string XYZ_FILE = "model/pointcloud/camel.xyz";        // XYZ 文件
const GLfloat PI = acos(-1.0f);
const GLfloat LIGHT_AMBIENT[] = { 0.2f, 0.2f, 0.2f, 1.0f };     // 环境光
const GLfloat LIGHT_POSITION[] = { 1.0f, 1.0f, 1.0f, 0.0f };    // 光源位置
const GLfloat LIGHT_DIFFUSE[] = { 1.0f, 1.0f, 1.0f, 1.0f };     // 漫反射光
const GLfloat LIGHT_SPECULAR[] = { 1.0f, 1.0f, 1.0f, 1.0f };    // 镜面反射光
const GLfloat MATERIAL_AMBIENT[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // 环境光
const GLfloat MATERIAL_DIFFUSE[] = { 0.8f, 0.8f, 0.8f, 1.0f };  // 漫反射光
const GLfloat MATERIAL_SPECULAR[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 镜面反射光
const GLfloat MATERIAL_SHININESS = 128.0f;                      // 高光系数

int DISPLAY_3D_Mesh = 0;            // 1 for Mesh, 0 for PointCloud
int HIGH_LIGHT_FOR_3D_POINT = 1;    // 1 for HighLight
bool DISPLAY_AXISCIRCLE = 0;		// 1 for display axis and circles
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
const char* WINDOW_TITLE = "Lab1";
float ZOOM = 1.0f;                                        // 缩放因子
float ROTATION_ANGLE[4] = { 0, 0, 0, 0 };                 // 旋转角度
bool WIREFRAME_MODE = false;                              // 线框模式
bool LIGHT_MODE = true;                                   // 光照模式
float MODEL_POSITION[3] = { 0.0f, 0.0f, -5.0f };          // 模型位置
bool MOUSE_DOWN = false;                                  // 鼠标是否按下
int PRE_DX = -1000, PRE_DY = -1000;                       // 上一次鼠标位置
int DX = -1000, DY = -1000;                               // 当前鼠标位置

OBJmodel objmodel;    // OBJ文件模型
PointCloud pcmodel;   // PointCloud文件模型，这里.xyz

static void initDisplay() {
    // 初始化窗口
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow(WINDOW_TITLE);

    // 设置视口
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT, 0.1, 50.0);

    // 设置背景颜色与模型位置
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);  // 设置背景颜色
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);
}

static void init() {
    // 设置材质属性
    glMaterialfv(GL_FRONT, GL_AMBIENT, MATERIAL_AMBIENT);    // 环境光
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MATERIAL_DIFFUSE);    // 漫反射光
    glMaterialfv(GL_FRONT, GL_SPECULAR, MATERIAL_SPECULAR);  // 镜面反射光
    glMaterialf(GL_FRONT, GL_SHININESS, MATERIAL_SHININESS); // 高光系数

    // 设置光源GL_LIGHT0的属性
    glLightfv(GL_LIGHT0, GL_AMBIENT, LIGHT_AMBIENT);      // 环境光
    glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POSITION);    // 光源位置
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LIGHT_DIFFUSE);      // 漫反射光
    glLightfv(GL_LIGHT0, GL_SPECULAR, LIGHT_SPECULAR);    // 镜面反射光
}

static void drawAxis() {
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    // X 轴，红色
    glColor3f(1.0f, 0.0f, 0.0f); // 红色
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    // Y 轴，绿色
    glColor3f(0.0f, 1.0f, 0.0f); // 绿色
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    // Z 轴，蓝色
    glColor3f(0.0f, 0.0f, 1.0f); // 蓝色
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
}

static void drawUnitCircles() {
    glColor3f(1.0f, 1.0f, 1.0f); // 使用单一颜色，例如白色
    int num_segments = 100; // 圆的分段数，越大越平滑
    float angle;
    // 绘制位于 XY 平面的单位圆
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++) {
        angle = 2.0f * PI * i / num_segments;
        float x = cosf(angle);
        float y = sinf(angle);
        glVertex3f(x, y, 0.0f); // Z 坐标为 0
    }
    glEnd();
    // 绘制位于 YZ 平面的单位圆
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++) {
        angle = 2.0f * PI * i / num_segments;
        float y = cosf(angle);
        float z = sinf(angle);
        glVertex3f(0.0f, y, z); // X 坐标为 0
    }
    glEnd();
    // 绘制位于 XZ 平面的单位圆
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++) {
        angle = 2.0f * PI * i / num_segments;
        float x = cosf(angle);
        float z = sinf(angle);
        glVertex3f(x, 0.0f, z); // Y 坐标为 0
    }
    glEnd();
}

static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 应用模型位置、缩放和旋转
    glTranslatef(MODEL_POSITION[0], MODEL_POSITION[1], MODEL_POSITION[2]);
    glScalef(ZOOM, ZOOM, ZOOM);

    glRotatef(ROTATION_ANGLE[0], 0, 1, 0);
    glRotatef(ROTATION_ANGLE[1], 0, -1, 0);
    glRotatef(ROTATION_ANGLE[2], 1, 0, 0);
    glRotatef(ROTATION_ANGLE[3], -1, 0, 0);

    // 绘制 XYZ 三轴和三个单位圆
    if (DISPLAY_AXISCIRCLE) {
        glDisable(GL_LIGHTING); // 禁用光照，确保颜色正确显示
        drawAxis();
        drawUnitCircles();
        glEnable(GL_LIGHTING); // 重新启用光照
    }
    if (LIGHT_MODE) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }
    else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }
    if (DISPLAY_3D_Mesh == 1) {
        objmodel.drawOBJ(WIREFRAME_MODE);
    }
    else {
        pcmodel.drawPointCloud(HIGH_LIGHT_FOR_3D_POINT, WIREFRAME_MODE);
    } // 进行对应的绘制
    glutSwapBuffers(); // 刷新双缓冲区
}

static void reshape(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 0.1, 50.0);
    glMatrixMode(GL_MODELVIEW);
} // 重塑窗口

static void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
        WIREFRAME_MODE = !WIREFRAME_MODE; // 切换是否是线框模式
        glutPostRedisplay();
        break;
    case 'x':
        LIGHT_MODE = !LIGHT_MODE; // 切换光照模式
        glutPostRedisplay();
        break;
    case 'a': // 
        ROTATION_ANGLE[0] += 5;
        glutPostRedisplay();
        break;
    case 'd':
        ROTATION_ANGLE[1] += 5;
        glutPostRedisplay();
        break;
    case 'w':
        ROTATION_ANGLE[2] += 5;
        glutPostRedisplay();
        break;
    case 's':
        ROTATION_ANGLE[3] += 5;
        glutPostRedisplay();
        break;
    case '=': // 按下 '=' 键，拉近摄像机
        ZOOM += 0.1f;
        glutPostRedisplay();
        break;
    case '-': // 按下 '-' 键，拉远摄像机
        ZOOM -= 0.1f;
        glutPostRedisplay();
        break;
    case 'r': // 按下 'r' 键，设置颜色为红色
        pcmodel.pointColor[0] = 1.0f;
        pcmodel.pointColor[1] = 0.0f;
        pcmodel.pointColor[2] = 0.0f;
        glutPostRedisplay();
        break;
    case 'g': // 按下 'g' 键，设置颜色为绿色
        pcmodel.pointColor[0] = 0.0f;
        pcmodel.pointColor[1] = 1.0f;
        pcmodel.pointColor[2] = 0.0f;
        glutPostRedisplay();
        break;
    case 'b': // 按下 'b' 键，设置颜色为蓝色
        pcmodel.pointColor[0] = 0.0f;
        pcmodel.pointColor[1] = 0.0f;
        pcmodel.pointColor[2] = 1.0f;
        glutPostRedisplay();
        break;
    case 'z':
		DISPLAY_AXISCIRCLE = !DISPLAY_AXISCIRCLE; // 切换是否绘制坐标系和单位圆
        glutPostRedisplay();
        break;
    case 27: // ESC 键
        exit(0);
        break;
    default:
        break;
    }
}


static void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            MOUSE_DOWN = true;
            PRE_DX = x;
            PRE_DY = y;
        }
        else {
            MOUSE_DOWN = false;
        }
    }
} // 鼠标按下

static void motion(int x, int y) {
    if (MOUSE_DOWN) {
        DX = x;
        DY = y;
        MODEL_POSITION[0] -= (PRE_DX - DX) * 0.002f;
        MODEL_POSITION[1] += (PRE_DY - DY) * 0.002f;
        PRE_DX = DX;
        PRE_DY = DY;
        glutPostRedisplay();
    }
} // 鼠标拖动

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    initDisplay();
    init();
	string filename = XYZ_FILE;
	if (filename.substr(filename.length() - 3, 3) == "obj") { // 如果是OBJ文件
		DISPLAY_3D_Mesh = 1; // 切换模式
        objmodel.setObjFilename(OBJ_FILE); // 传入文件地址
        objmodel.setTextureFilename(TEXTURE_FILE); // 传入纹理地址
        objmodel.loadOBJ(); // 加载文件
        objmodel.loadTexture(); // 加载纹理
	}
	else if (filename.substr(filename.length() - 3, 3) == "xyz") { // 如果是点云XYZ文件
		DISPLAY_3D_Mesh = 0; // 切换模式
        pcmodel.setFilename(XYZ_FILE); // 传入文件地址
        pcmodel.loadXYZ(); // 加载点云
    }
    else {
		cerr << "暂时无法处理该文件类型，待添加" << endl;
		return 0;
    }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}
