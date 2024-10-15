#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <string>
#include "Data.h"
#include <gl/glut.h>
using namespace std;

class OBJmodel {
public:
	GLuint textureId;
	vector<Vertex> vertices;
	vector<Normal> normals;
	vector<TexCoord> texcoords;
	vector<Face> faces;
	string obj_filename;
	string texture_filename;
	OBJmodel(const string& obj_fname, const string& tex_fname)
		: obj_filename(obj_fname), texture_filename(tex_fname) {}

	OBJmodel() {}
	void loadOBJ(const string& filename);
	void loadOBJ();
	void loadTexture(const string& filename);
	void loadTexture();
	void setObjFilename(const string& name) { obj_filename = name; }
	void setTextureFilename(const string& name) { texture_filename = name; }
	void drawOBJ(bool WIREFRAME_MODE);
};

#endif // OBJLOADER_H