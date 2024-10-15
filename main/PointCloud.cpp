#include "PointCloud.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <GL/glut.h>

PointCloud::PointCloud(const std::string& fname) : filename(fname) { pointColor[0] = 1.0f; pointColor[1] = 0.0f; pointColor[2] = 0.0f; }
// ���캯��
PointCloud::PointCloud() { pointColor[0] = 1.0f; pointColor[1] = 0.0f; pointColor[2] = 0.0f; }
// Ĭ�Ϲ��캯��
void PointCloud::setFilename(const std::string& name) {
    filename = name;
} // ���ⲿ������ļ�����ֵ���ڲ��� filename   

void PointCloud::loadXYZ(const std::string& filename) {
    points.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "�޷��� XYZ �ļ���" << filename << std::endl;
        return;
    }
    std::string line;
    while (getline(file, line)) {
        std::istringstream in(line);
        Vertex v;
        in >> v;
        if (in.fail()) {
            // ������Ч����
            continue;
        }
        points.push_back(v);
    }
}

void PointCloud::loadXYZ() {
    if (filename.empty()) {
        std::cerr << "�ļ�·��Ϊ�գ��޷�����" << std::endl;
        return;
    }
    loadXYZ(filename);
}

void PointCloud::drawPointCloud(bool highlight, bool wireframeMode) {
    if (highlight) {
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        for (const auto& v : points) {
            float intensity = (v.y + 1.0f) / 2.0f;
            glColor3f(intensity, intensity, intensity); // �� y ����� -1 �� 1 �ķ�Χӳ�䵽 0 �� 1 �ķ�Χ��ʹ�õ����ɫ�ڻҶ��ϱ仯
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
    }
    else {
        glColor3f(pointColor[0], pointColor[1], pointColor[2]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_POINTS);
        for (const auto& v : points) {
			glVertex3f(v.x, v.y, v.z); // ���Ƶ���
        }
        glEnd();
    }
}
