#ifndef DATA_H
#define DATA_H
#include <iostream>
#include <vector>
using namespace std;
// 模型数据结构
struct Vertex {
	float x, y, z;
	friend istream& operator>>(istream& is, Vertex& v) {
		is >> v.x >> v.y >> v.z;
		return is;
	} // 重载输入运算符，方便后续读入
};// 点

struct Normal {
	float nx, ny, nz;
	friend istream& operator>>(istream& is, Normal& n) {
		is >> n.nx >> n.ny >> n.nz;
		return is;
	} // 重载输入运算符，方便后续读入
}; // 法线

struct TexCoord {
	float u, v;
	friend istream& operator>>(istream& is, TexCoord& t) {
		is >> t.u >> t.v;
		return is;
	} // 重载输入运算符，方便后续读入
}; // 纹理坐标

struct Face {
	vector<int> vertexIndices;
	vector<int> normalIndices;
	vector<int> texcoordIndices;
}; // 面

#endif // DATA_H