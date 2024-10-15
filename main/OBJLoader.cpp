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
		cerr << "�޷���OBJ�ļ���" << filename << endl;
		return;
	}
	string inputline;
	while (getline(file, inputline)) {
		if (inputline[0] == 'v' && inputline[1] == ' ') { // ����Ƕ���
			istringstream in(inputline.substr(2));
			Vertex v;
			in >> v;
			vertices.push_back(v);
		}
		else if (inputline[0] == 'v' && inputline[1] == 'n' && inputline[2] == ' ') { // ����Ƿ���
			istringstream in(inputline.substr(3));
			Normal n;
			in >> n;
			normals.push_back(n);
		}
		else if (inputline[0] == 'v' && inputline[1] == 't' && inputline[2] == ' ') { // �������������
			istringstream in(inputline.substr(3));
			TexCoord t;
			in >> t;
			texcoords.push_back(t);
		}
		else if (inputline[0] == 'f' && inputline[1] == ' ') { // �������
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
		cerr << "�ļ�·��Ϊ�գ��޷�����" << endl;
		return;
	}
	loadOBJ(obj_filename);
}

void OBJmodel::loadTexture(const string& filename) {
	stbi_set_flip_vertically_on_load(true); // ����UV
	int width, height, nrChannels;
	auto data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		cerr << "�޷���������" << filename << endl;
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
		std::cerr << "��֧�ֵ�ͨ����: " << nrChannels << std::endl;
		return;
	}
	glGenTextures(1, &textureId); // ����һ��������󣬲�����ID�洢��textureId������
	glBindTexture(GL_TEXTURE_2D, textureId); // �󶨣�������Ϊ��ǰ2D���������������䴦��
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // ������S�ᣨˮƽ������������Ϊ�ظ���GL_REPEAT����ʵ���������ò���
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // ������T�ᣨ��ֱ������������Ϊ�ظ���GL_REPEAT��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // ��С����_MIN_FILTER����ʱ��ʹ�����Թ��ˣ�GL_LINEAR�������в�ֵ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // �Ŵ�����_MAG_FILTER����ʱ��ʹ�����Թ��ˣ�GL_LINEAR�������в�ֵ����
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // ��������
	stbi_image_free(data); // �ͷ�ͼ���ڴ�
}

void OBJmodel::loadTexture() {
	if (obj_filename.empty()) {
		cerr << "�ļ�·��Ϊ�գ��޷���������" << endl;
		return;
	}
	loadTexture(texture_filename);
}

void OBJmodel::drawOBJ(bool WIREFRAME_MODE) {
	glBindTexture(GL_TEXTURE_2D, textureId); // ������
	if (WIREFRAME_MODE) { // �߿�ģʽ
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_LINE
		glColor3f(1.0f, 1.0f, 1.0f); // ��ɫ
	}
	else { // �������ģʽ
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_FILL
	}
	glEnable(GL_TEXTURE_2D); // ����2D����
	glBegin(GL_TRIANGLES);   // ��ʼ����������
	for (const auto& face : faces) { // ����faces�����е��� 
		for (int i = 0; i < face.vertexIndices.size(); i++) { // ��������ÿһ������
			int vi = face.vertexIndices[i];		// ��������
			int ni = face.normalIndices[i];		// ��������
			int ti = face.texcoordIndices[i];	// ������������
			if (ni >= 0 && ni < normals.size()) { // ������������Ϸ�
				glNormal3f(normals[ni].nx, normals[ni].ny, normals[ni].nz); 
			}
			if (ti >= 0 && ti < texcoords.size()) { // ����������������Ϸ�
				glTexCoord2f(texcoords[ti].u, texcoords[ti].v);
				// cout << texcoords[ti].u << " " << texcoords[ti].v << endl;
			}
			glVertex3f(vertices[vi].x, vertices[vi].y, vertices[vi].z); // ָ������λ��
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D); // ����2D����
}
