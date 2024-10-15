#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <vector>
#include <string>
#include "Data.h" // 确保 Vertex 结构体在此定义
#include <gl/glut.h>

class PointCloud {
public:
    vector<Vertex> points;   // 存储点云的顶点
    string filename;         // 点云文件名
    GLfloat pointColor[3];    // 默认红色
    PointCloud(const std::string& fname);
    PointCloud();
    void loadXYZ(const std::string& filename);
    void loadXYZ();
    void setFilename(const std::string& name);
    void drawPointCloud(bool highlight, bool wireframeMode);
};

#endif // POINTCLOUD_H
