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

const string OBJ_FILE = "model/spot/spot_triangulated.obj";   // OBJ �ļ�
const string TEXTURE_FILE = "model/spot/spot_texture.png";    // �����ļ�����ת��Ϊ BMP ��ʽ��
const string XYZ_FILE = "model/pointcloud/camel.xyz";        // XYZ �ļ�
const GLfloat PI = acos(-1.0f);
const GLfloat LIGHT_AMBIENT[] = { 0.2f, 0.2f, 0.2f, 1.0f };     // ������
const GLfloat LIGHT_POSITION[] = { 1.0f, 1.0f, 1.0f, 0.0f };    // ��Դλ��
const GLfloat LIGHT_DIFFUSE[] = { 1.0f, 1.0f, 1.0f, 1.0f };     // �������
const GLfloat LIGHT_SPECULAR[] = { 1.0f, 1.0f, 1.0f, 1.0f };    // ���淴���
const GLfloat MATERIAL_AMBIENT[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // ������
const GLfloat MATERIAL_DIFFUSE[] = { 0.8f, 0.8f, 0.8f, 1.0f };  // �������
const GLfloat MATERIAL_SPECULAR[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���淴���
const GLfloat MATERIAL_SHININESS = 128.0f;                      // �߹�ϵ��

int DISPLAY_3D_Mesh = 0;            // 1 for Mesh, 0 for PointCloud
int HIGH_LIGHT_FOR_3D_POINT = 1;    // 1 for HighLight
bool DISPLAY_AXISCIRCLE = 0;		// 1 for display axis and circles
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
const char* WINDOW_TITLE = "Lab1";
float ZOOM = 1.0f;                                        // ��������
float ROTATION_ANGLE[4] = { 0, 0, 0, 0 };                 // ��ת�Ƕ�
bool WIREFRAME_MODE = false;                              // �߿�ģʽ
bool LIGHT_MODE = true;                                   // ����ģʽ
float MODEL_POSITION[3] = { 0.0f, 0.0f, -5.0f };          // ģ��λ��
bool MOUSE_DOWN = false;                                  // ����Ƿ���
int PRE_DX = -1000, PRE_DY = -1000;                       // ��һ�����λ��
int DX = -1000, DY = -1000;                               // ��ǰ���λ��

OBJmodel objmodel;    // OBJ�ļ�ģ��
PointCloud pcmodel;   // PointCloud�ļ�ģ�ͣ�����.xyz

static void initDisplay() {
    // ��ʼ������
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow(WINDOW_TITLE);

    // �����ӿ�
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT, 0.1, 50.0);

    // ���ñ�����ɫ��ģ��λ��
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);  // ���ñ�����ɫ
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);
}

static void init() {
    // ���ò�������
    glMaterialfv(GL_FRONT, GL_AMBIENT, MATERIAL_AMBIENT);    // ������
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MATERIAL_DIFFUSE);    // �������
    glMaterialfv(GL_FRONT, GL_SPECULAR, MATERIAL_SPECULAR);  // ���淴���
    glMaterialf(GL_FRONT, GL_SHININESS, MATERIAL_SHININESS); // �߹�ϵ��

    // ���ù�ԴGL_LIGHT0������
    glLightfv(GL_LIGHT0, GL_AMBIENT, LIGHT_AMBIENT);      // ������
    glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POSITION);    // ��Դλ��
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LIGHT_DIFFUSE);      // �������
    glLightfv(GL_LIGHT0, GL_SPECULAR, LIGHT_SPECULAR);    // ���淴���
}

static void drawAxis() {
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    // X �ᣬ��ɫ
    glColor3f(1.0f, 0.0f, 0.0f); // ��ɫ
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    // Y �ᣬ��ɫ
    glColor3f(0.0f, 1.0f, 0.0f); // ��ɫ
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    // Z �ᣬ��ɫ
    glColor3f(0.0f, 0.0f, 1.0f); // ��ɫ
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
}

static void drawUnitCircles() {
    glColor3f(1.0f, 1.0f, 1.0f); // ʹ�õ�һ��ɫ�������ɫ
    int num_segments = 100; // Բ�ķֶ�����Խ��Խƽ��
    float angle;
    // ����λ�� XY ƽ��ĵ�λԲ
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++) {
        angle = 2.0f * PI * i / num_segments;
        float x = cosf(angle);
        float y = sinf(angle);
        glVertex3f(x, y, 0.0f); // Z ����Ϊ 0
    }
    glEnd();
    // ����λ�� YZ ƽ��ĵ�λԲ
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++) {
        angle = 2.0f * PI * i / num_segments;
        float y = cosf(angle);
        float z = sinf(angle);
        glVertex3f(0.0f, y, z); // X ����Ϊ 0
    }
    glEnd();
    // ����λ�� XZ ƽ��ĵ�λԲ
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++) {
        angle = 2.0f * PI * i / num_segments;
        float x = cosf(angle);
        float z = sinf(angle);
        glVertex3f(x, 0.0f, z); // Y ����Ϊ 0
    }
    glEnd();
}

static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Ӧ��ģ��λ�á����ź���ת
    glTranslatef(MODEL_POSITION[0], MODEL_POSITION[1], MODEL_POSITION[2]);
    glScalef(ZOOM, ZOOM, ZOOM);

    glRotatef(ROTATION_ANGLE[0], 0, 1, 0);
    glRotatef(ROTATION_ANGLE[1], 0, -1, 0);
    glRotatef(ROTATION_ANGLE[2], 1, 0, 0);
    glRotatef(ROTATION_ANGLE[3], -1, 0, 0);

    // ���� XYZ �����������λԲ
    if (DISPLAY_AXISCIRCLE) {
        glDisable(GL_LIGHTING); // ���ù��գ�ȷ����ɫ��ȷ��ʾ
        drawAxis();
        drawUnitCircles();
        glEnable(GL_LIGHTING); // �������ù���
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
    } // ���ж�Ӧ�Ļ���
    glutSwapBuffers(); // ˢ��˫������
}

static void reshape(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 0.1, 50.0);
    glMatrixMode(GL_MODELVIEW);
} // ���ܴ���

static void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
        WIREFRAME_MODE = !WIREFRAME_MODE; // �л��Ƿ����߿�ģʽ
        glutPostRedisplay();
        break;
    case 'x':
        LIGHT_MODE = !LIGHT_MODE; // �л�����ģʽ
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
    case '=': // ���� '=' �������������
        ZOOM += 0.1f;
        glutPostRedisplay();
        break;
    case '-': // ���� '-' ������Զ�����
        ZOOM -= 0.1f;
        glutPostRedisplay();
        break;
    case 'r': // ���� 'r' ����������ɫΪ��ɫ
        pcmodel.pointColor[0] = 1.0f;
        pcmodel.pointColor[1] = 0.0f;
        pcmodel.pointColor[2] = 0.0f;
        glutPostRedisplay();
        break;
    case 'g': // ���� 'g' ����������ɫΪ��ɫ
        pcmodel.pointColor[0] = 0.0f;
        pcmodel.pointColor[1] = 1.0f;
        pcmodel.pointColor[2] = 0.0f;
        glutPostRedisplay();
        break;
    case 'b': // ���� 'b' ����������ɫΪ��ɫ
        pcmodel.pointColor[0] = 0.0f;
        pcmodel.pointColor[1] = 0.0f;
        pcmodel.pointColor[2] = 1.0f;
        glutPostRedisplay();
        break;
    case 'z':
		DISPLAY_AXISCIRCLE = !DISPLAY_AXISCIRCLE; // �л��Ƿ��������ϵ�͵�λԲ
        glutPostRedisplay();
        break;
    case 27: // ESC ��
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
} // ��갴��

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
} // ����϶�

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    initDisplay();
    init();
	string filename = XYZ_FILE;
	if (filename.substr(filename.length() - 3, 3) == "obj") { // �����OBJ�ļ�
		DISPLAY_3D_Mesh = 1; // �л�ģʽ
        objmodel.setObjFilename(OBJ_FILE); // �����ļ���ַ
        objmodel.setTextureFilename(TEXTURE_FILE); // ���������ַ
        objmodel.loadOBJ(); // �����ļ�
        objmodel.loadTexture(); // ��������
	}
	else if (filename.substr(filename.length() - 3, 3) == "xyz") { // ����ǵ���XYZ�ļ�
		DISPLAY_3D_Mesh = 0; // �л�ģʽ
        pcmodel.setFilename(XYZ_FILE); // �����ļ���ַ
        pcmodel.loadXYZ(); // ���ص���
    }
    else {
		cerr << "��ʱ�޷�������ļ����ͣ������" << endl;
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
