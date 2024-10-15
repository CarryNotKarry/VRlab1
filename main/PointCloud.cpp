#include "PointCloud.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <GL/glut.h>

PointCloud::PointCloud(const std::string& fname) : filename(fname) { pointColor[0] = 1.0f; pointColor[1] = 0.0f; pointColor[2] = 0.0f; }
// 构造函数
PointCloud::PointCloud() { pointColor[0] = 1.0f; pointColor[1] = 0.0f; pointColor[2] = 0.0f; }
// 默认构造函数
void PointCloud::setFilename(const std::string& name) {
    filename = name;
} // 将外部传入的文件名赋值给内部的 filename   

void PointCloud::loadXYZ(const std::string& filename) {
    points.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开 XYZ 文件：" << filename << std::endl;
        return;
    }
    std::string line;
    while (getline(file, line)) {
        std::istringstream in(line);
        Vertex v;
        in >> v;
        if (in.fail()) {
            // 跳过无效的行
            continue;
        }
        points.push_back(v);
    }
}

void PointCloud::loadXYZ() {
    if (filename.empty()) {
        std::cerr << "文件路径为空，无法加载" << std::endl;
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
            glColor3f(intensity, intensity, intensity); // 将 y 坐标从 -1 到 1 的范围映射到 0 到 1 的范围，使得点的颜色在灰度上变化
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
    }
    else {
        glColor3f(pointColor[0], pointColor[1], pointColor[2]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_POINTS);
        for (const auto& v : points) {
			glVertex3f(v.x, v.y, v.z); // 绘制点云
        }
        glEnd();
    }
}
