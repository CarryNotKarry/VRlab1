#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <vector>
#include <string>
#include "Data.h" // ȷ�� Vertex �ṹ���ڴ˶���
#include <gl/glut.h>

class PointCloud {
public:
    vector<Vertex> points;   // �洢���ƵĶ���
    string filename;         // �����ļ���
    GLfloat pointColor[3];    // Ĭ�Ϻ�ɫ
    PointCloud(const std::string& fname);
    PointCloud();
    void loadXYZ(const std::string& filename);
    void loadXYZ();
    void setFilename(const std::string& name);
    void drawPointCloud(bool highlight, bool wireframeMode);
};

#endif // POINTCLOUD_H
