#include "OBJLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "stb_image.h"
#include <gl/glut.h>

void OBJmodel::loadOBJ(const string& filename) {
	vertices.clear();
	normals.clear();
	texcoords.clear();
	faces.clear();
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "无法打开OBJ文件：" << filename << endl;
		return;
	}
	string inputline;
	while (getline(file, inputline)) {
		if (inputline[0] == 'v' && inputline[1] == ' ') { // 如果是顶点
			istringstream in(inputline.substr(2));
			Vertex v;
			in >> v;
			vertices.push_back(v);
		}
		else if (inputline[0] == 'v' && inputline[1] == 'n' && inputline[2] == ' ') { // 如果是法线
			istringstream in(inputline.substr(3));
			Normal n;
			in >> n;
			normals.push_back(n);
		}
		else if (inputline[0] == 'v' && inputline[1] == 't' && inputline[2] == ' ') { // 如果是纹理坐标
			istringstream in(inputline.substr(3));
			TexCoord t;
			in >> t;
			texcoords.push_back(t);
		}
		else if (inputline[0] == 'f' && inputline[1] == ' ') { // 如果是面
			istringstream in(inputline.substr(2));
			Face f;
			string str;
			while (in >> str) {
				istringstream in(str);
				string v, t, n;
				int vi = -1, ti = -1, ni = -1;
				if (str.find("//") != string::npos) {
					int pos = str.find("//");
					v = str.substr(0, pos);
					n = str.substr(pos + 2);
					vi = stoi(v) - 1;
					ni = stoi(n) - 1;
				}
				else if (str.find('/') != string::npos) {
					int first = str.find('/'), second = str.find('/', first + 1);
					v = str.substr(0, first);
					t = str.substr(first + 1, second - first - 1);
					n = str.substr(second + 1);
					vi = stoi(v) - 1;
					ti = stoi(t) - 1;
					ni = stoi(n) - 1;
				}
				else {
					vi = stoi(str) - 1;
				}
				f.vertexIndices.emplace_back(vi);
				f.texcoordIndices.emplace_back(ti);
				f.normalIndices.emplace_back(ni);
			}
			faces.push_back(f);
		}
	}
}

void OBJmodel::loadOBJ() {
	if (obj_filename.empty()) {
		cerr << "文件路径为空，无法加载" << endl;
		return;
	}
	loadOBJ(obj_filename);
}

void OBJmodel::loadTexture(const string& filename) {
	stbi_set_flip_vertically_on_load(true); // 调整UV
	int width, height, nrChannels;
	auto data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		cerr << "无法加载纹理：" << filename << endl;
		return;
	}
	GLenum format;
	switch (nrChannels) {
	case 1:
		format = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		std::cerr << "不支持的通道数: " << nrChannels << std::endl;
		return;
	}
	glGenTextures(1, &textureId); // 生成一个纹理对象，并将其ID存储到textureId变量中
	glBindTexture(GL_TEXTURE_2D, textureId); // 绑定，将其作为当前2D纹理，后续操作对其处理
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // 纹理在S轴（水平），超出部分为重复（GL_REPEAT），实际上这里用不到
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // 纹理在T轴（垂直），超出部分为重复（GL_REPEAT）
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 缩小纹理（_MIN_FILTER）的时候使用线性过滤（GL_LINEAR），进行插值处理
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 放大纹理（_MAG_FILTER）的时候使用线性过滤（GL_LINEAR），进行插值处理
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // 生成纹理
	stbi_image_free(data); // 释放图像内存
}

void OBJmodel::loadTexture() {
	if (obj_filename.empty()) {
		cerr << "文件路径为空，无法加载纹理" << endl;
		return;
	}
	loadTexture(texture_filename);
}

void OBJmodel::drawOBJ(bool WIREFRAME_MODE) {
	glBindTexture(GL_TEXTURE_2D, textureId); // 绑定纹理
	if (WIREFRAME_MODE) { // 线框模式
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_LINE
		glColor3f(1.0f, 1.0f, 1.0f); // 白色
	}
	else { // 否则填充模式
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_FILL
	}
	glEnable(GL_TEXTURE_2D); // 启用2D纹理
	glBegin(GL_TRIANGLES);   // 开始绘制三角形
	for (const auto& face : faces) { // 遍历faces中所有的面 
		for (int i = 0; i < face.vertexIndices.size(); i++) { // 遍历面中每一个顶点
			int vi = face.vertexIndices[i];		// 顶点索引
			int ni = face.normalIndices[i];		// 法线索引
			int ti = face.texcoordIndices[i];	// 纹理坐标索引
			if (ni >= 0 && ni < normals.size()) { // 如果法线索引合法
				glNormal3f(normals[ni].nx, normals[ni].ny, normals[ni].nz); 
			}
			if (ti >= 0 && ti < texcoords.size()) { // 如果纹理坐标索引合法
				glTexCoord2f(texcoords[ti].u, texcoords[ti].v);
				// cout << texcoords[ti].u << " " << texcoords[ti].v << endl;
			}
			glVertex3f(vertices[vi].x, vertices[vi].y, vertices[vi].z); // 指出顶点位置
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D); // 禁用2D纹理
}
