#ifndef DATA_H
#define DATA_H
#include <iostream>
#include <vector>
using namespace std;
// ģ�����ݽṹ
struct Vertex {
	float x, y, z;
	friend istream& operator>>(istream& is, Vertex& v) {
		is >> v.x >> v.y >> v.z;
		return is;
	} // ��������������������������
};// ��

struct Normal {
	float nx, ny, nz;
	friend istream& operator>>(istream& is, Normal& n) {
		is >> n.nx >> n.ny >> n.nz;
		return is;
	} // ��������������������������
}; // ����

struct TexCoord {
	float u, v;
	friend istream& operator>>(istream& is, TexCoord& t) {
		is >> t.u >> t.v;
		return is;
	} // ��������������������������
}; // ��������

struct Face {
	vector<int> vertexIndices;
	vector<int> normalIndices;
	vector<int> texcoordIndices;
}; // ��

#endif // DATA_H