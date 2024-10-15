# 虚拟现实-lab1-202421081048 黄恺瑞

## 一、实验内容

1. 建立一个编程环境，实现读入三维物体，进行线框显示，shading, 纹理映射以及旋转，zoom in/out等功能。
2. 读入的三维模型： 点云，mesh，volume model，NURBS

## 二、实验设计

### 2.1 实现内容

（包括问题描述、核心算法、技术难点等）

本实验实现读入三维模型有：点云（`.xyz`）以及mesh（`.obj`）形式，并且完成了纹理（`.png`）映射。实现了读入三维物体，可以进行线框显示，并且完成了着色（shading）、旋转（rotate）以及摄像机缩放（zoom in/out）等功能。并且实现了鼠标和键盘的交互事件。

​	数据使用的如下：

- mesh文件使用的是GAMES101作业3中的奶牛，将其`.svg`文件转化为`.png`文件进行读入进而纹理贴图。
- pointcloud文件使用的是PUNet数据集中的测试集中50k点的`camel.xyz`文件。

​	环境使用的是OpenGL以及GLUT库，在Visual Studio 2022进行运行。

### 2.2 核心算法

1. 数据结构

首先创建四个结构体`Vertex`、`Normal`、`TexCoord`以及`Face`，分别来存储点、法线、纹理坐标以及面，多个数据的时候使用的是`std::vector`进行存储。

2. 三维模型的读入以及绘制

设计了`OBJLoader`类以及`PointCloud`类。

- `OBJLoader`类

  使用自定义的OBJ模型读取网格模型的顶点、法线、纹理坐标等信息。主要成员变量有：

  | 变量                          | 备注         |
  | ----------------------------- | ------------ |
  | `GLuint textureId;`           | 纹理ID       |
  | `vector<Vertex> vertices;`    | 点坐标       |
  | `vector<TexCoord> texcoords;` | 纹理坐标     |
  | `vector<Face> faces;`         | 面           |
  | `string obj_filename;`        | OBJ文件路径  |
  | `string texture_filename;`    | 纹理文件路径 |

  主要类函数（多个重名函数只写其一）如下：

  | 函数                                          | 备注        |
  | --------------------------------------------- | ----------- |
  | `void loadOBJ(const string& filename);`       | 加载OBJ文件 |
  | `void loadTexture(const string& filename);`   | 加载PNG纹理 |
  | `void setObjFilename(const string& name) `    | 赋值OBJ地址 |
  | `void setTextureFilename(const string& name)` | 赋值PNG地址 |
  | `void drawOBJ(bool WIREFRAME_MODE);`          | 绘制图形    |

  该难点主要是在对文件的读入，因为OBJ文件较为复杂，所以需要对每一行进行判断是否是`v`、`vn`、`vt`还是`f`，并且保存至对应的数据结构中。

  因篇幅有限，这里展示对于OBJ文件逐行读入的处理代码：

  ```cpp
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
      ... // 后续处理面（f）代码未展示
  }
  ```

  

  加载纹理时（WIN环境下）则需要调整一下UV才可以使得纹理方向正确，在加载OBJ文件时，每个面由顶点、法线和纹理坐标组成。在绘制时则需要将每一个面片对应相应的纹理坐标，纹理坐标（`texcoords`）与顶点索引（`vertexIndices`）相匹配，用于在渲染时正确映射纹理图像到模型的表面上。

  ```cpp
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
  		}
  		glVertex3f(vertices[vi].x, vertices[vi].y, vertices[vi].z); // 指出顶点位置
  	}
  }
  glEnd();
  glDisable(GL_TEXTURE_2D); // 禁用2D纹理
  ```

- `PointCloud`类

  而该类相较而言比较简单，主要成员变量如下：

  | 变量                     | 备注           |
  | ------------------------ | -------------- |
  | `vector<Vertex> points;` | 存储点云的顶点 |
  | `string filename;`       | 文件路径       |
  | `GLfloat pointColor[3];` | 绘制点的颜色   |

  函数如下：

  | 函数                                                       | 备注                      |
  | ---------------------------------------------------------- | ------------------------- |
  | `void loadXYZ(const std::string& filename);`               | 加载XYZ文件，读取点云坐标 |
  | `void setFilename(const std::string& name);`               | 赋值文件路径              |
  | `void drawPointCloud(bool highlight, bool wireframeMode);` | 绘制点云                  |

  与mesh文件处理类似，也是读入之后进行绘制，不过这里有一个高亮模式，将 y 坐标从 -1 到 1 的范围映射到 0 到 1 的范围，使得点的颜色在灰度上变化，否则使用函数内的设置的颜色。

  部分重要代码如下：

  ```cpp
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
  ```

3. 主函数重要函数

   在加载模型之前需要对OpenGL需要处理的进行准备，在`main.cpp`中有如下重要函数

   | 主要函数                 | 备注                                     |
   | ------------------------ | ---------------------------------------- |
   | `void initDisplay()`     | 初始化窗口，设置视口、设置背景和模型位置 |
   | `void init()`            | 设置材质和光源的属性                     |
   | `void drawAxis()`        | 对于点云可视化时绘制坐标系               |
   | `void drawUnitCircles()` | 对于点云可视化时绘制三个单位圆           |
   | `void display()`         | 展示                                     |
   | `void keyboard`          | 键盘事件，同时还有鼠标点击和拖动事件     |

   在准备时使用的`glut`库一些函数以及OpenGL里面对于材质环境等的设置函数。在`display()`函数中，主要对于模型位置、缩放和旋转等进行处理，然后绘制相应的图形，处理相应的模式。代码如下：

   ```cpp
   static void display() {
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       glLoadIdentity();
   
       // 应用模型位置、缩放和旋转
       glTranslatef(MODEL_POSITION[0], MODEL_POSITION[1], MODEL_POSITION[2]);
       glScalef(ZOOM, ZOOM, ZOOM); // 有缩放因子ZOOM进行缩放
   
       glRotatef(ROTATION_ANGLE[0], 0, 1, 0);
       glRotatef(ROTATION_ANGLE[1], 0, -1, 0);
       glRotatef(ROTATION_ANGLE[2], 1, 0, 0);
       glRotatef(ROTATION_ANGLE[3], -1, 0, 0); 
       // Rotate旋转
   
       // 绘制 XYZ 三轴和三个单位圆
       if (DISPLAY_AXISCIRCLE) {
           glDisable(GL_LIGHTING); // 禁用光照，确保颜色正确显示
           drawAxis();
           drawUnitCircles();
           glEnable(GL_LIGHTING); // 重新启用光照
       }
       if (LIGHT_MODE) { // 光照模式
           glEnable(GL_LIGHTING);
           glEnable(GL_LIGHT0); // 打开
       }
       else {
           glDisable(GL_LIGHTING);
           glDisable(GL_LIGHT0); // 关闭
       }
       if (DISPLAY_3D_Mesh == 1) {
           objmodel.drawOBJ(WIREFRAME_MODE);
       } // 绘制mesh图形
       else {
           pcmodel.drawPointCloud(HIGH_LIGHT_FOR_3D_POINT, WIREFRAME_MODE);
       } // 进行点云
       glutSwapBuffers(); // 刷新双缓冲区
   }
   ```

   有如下的鼠标键盘事件：

   | 事件          | 备注                                     |
   | ------------- | ---------------------------------------- |
   | 按下`w/a/s/d` | 摄像机分别朝下、左、上、右旋转           |
   | 按下`q`       | 切换线框模式，再按下则返回（针对mesh）   |
   | 按下`x`       | 切换光照模式，再按下则返回（针对点云）   |
   | 按下`=/-`     | 拉近/拉远照相机与图形距离（zoom in/out） |
   | 按下`r/g/b`   | 将颜色转变为红绿蓝（针对点云）           |
   | 按下`z`       | 切换绘制坐标系和单位圆模型，再按下返回   |
   | 按下`ESC`     | 退出                                     |
   | 鼠标按下拖动  | 可以调整上下左右的视角                   |

## 三、结果展示

1. mesh

首先是完整的加载以及绘制，得到正面图如下：

<img src="https://s2.loli.net/2024/10/11/D29ysLoFWERnpHr.png" alt="image-20241011201900865" style="zoom:40%;" /><img src="https://s2.loli.net/2024/10/11/SC3vDkPJwHao7At.png" alt="image-20241011201909304" style="zoom:40%;" />

取消光照得到如下：

<img src="https://s2.loli.net/2024/10/11/B4RIsTYdFcg5Mv2.png" alt="image-20241011201505294" style="zoom:40%;" /><img src="https://s2.loli.net/2024/10/11/M3XqcpY1kFxbaIA.png" alt="image-20241011201518183" style="zoom:40%;" />

若打开线框模式则有：

<img src="https://s2.loli.net/2024/10/11/nBb3FGMkdAYN1Ds.png" alt="image-20241011201658694" style="zoom:50%;" />

打开坐标系以及单位圆如下：

<img src="https://s2.loli.net/2024/10/11/w6LI9DZ32piyPzC.png" alt="image-20241011201822104" style="zoom:50%;" />

同样也可以通过按键以及鼠标进行调整视角：

<img src="https://s2.loli.net/2024/10/11/3KWtEg4GTIJscDV.png" alt="image-20241011202002096" style="zoom:40%;" /><img src="https://s2.loli.net/2024/10/11/vqk1r3JyXlxfbEF.png" alt="image-20241011202029872" style="zoom:40%;" />

<img src="https://s2.loli.net/2024/10/11/u487KobVdpSORkZ.png" alt="image-20241011202045407" style="zoom:40%;" /><img src="https://s2.loli.net/2024/10/11/EZuMXrK6RAIVsBl.png" alt="image-20241011202127649" style="zoom:39%;" />

2. pointcloud

我们将文件路径切换为`XYZ_FILE`，即可得到下面的图片：

<img src="https://s2.loli.net/2024/10/11/AKSJwu9FXZLlWnh.png" alt="image-20241011202300230" style="zoom:40%;" /><img src="https://s2.loli.net/2024/10/11/IQGeXSbxHc3WDfh.png" alt="image-20241011202316930" style="zoom:40%;" />

也可以切换其颜色：

<img src="https://s2.loli.net/2024/10/11/G6t1fXBYw378MqT.png" alt="image-20241011202421207" style="zoom:30%;" /><img src="https://s2.loli.net/2024/10/11/y5YApma91kFgs3N.png" alt="image-20241011202433588" style="zoom:30%;" /><img src="https://s2.loli.net/2024/10/11/Rgda95Jek3fEYhw.png" alt="image-20241011202444416" style="zoom:30%;" />

而加入高亮之后则对y进行灰度颜色赋值，得到：

<img src="https://s2.loli.net/2024/10/11/Ek1PSe6v5W9ZgFl.png" alt="image-20241011202712923" style="zoom:50%;" />

我们按下`=`进行拉近，可以得到：

<img src="https://s2.loli.net/2024/10/11/cxpIlsrLouCUXQg.png" alt="image-20241011202745914" style="zoom:20%;" /><img src="https://s2.loli.net/2024/10/11/Kb1UwtuEy9Svrc3.png" alt="image-20241011202912323" style="zoom:20%;" />

同样可以各种旋转得到：

<img src="https://s2.loli.net/2024/10/11/9BSWOCLJm7dveDh.png" alt="image-20241011202818581" style="zoom:20%;" /><img src="https://s2.loli.net/2024/10/11/JWDXRyP6pViMI12.png" alt="image-20241011202836877" style="zoom:20%;" />

## 四、总结

通过本次实验，我对于模型文件有了更深的了解，对于纹理映射以及OpenGL中如何处理这类问题有了深刻的体会。也熟悉了OpenGL中线框模式、着色（shading）以及纹理映射，也实现了一定的交互功能，也完成了基础的三维物体的旋转、缩放和平移操作。