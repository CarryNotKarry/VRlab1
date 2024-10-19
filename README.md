# README

## 实验报告
实验报告放在了个人博客[CarryNotKarry-VRlab1](https://carrynotkarry.com/research/vtlab1/)，里面有对于框架、函数、实验结果的相关内容

## 目录结构

在本目录中当前目录一共有三个文件，结构如下：

```bash
.
├── main 						  # 方便查看代码
├── vrlab1_report.md  # 实验报告markdown形式
└── README.md					# readme 本文件
```

其中项目代码以及放在了`main`文件夹中，里面主要的目录结构如下：

```bash
main/.
├── Data.h               # 定义模型数据的结构体，包括顶点、法线、纹理坐标和面
├── OBJLoader.h          # 声明OBJ模型加载器类
├── OBJLoader.cpp        # 实现OBJ模型加载器类，用于读取OBJ文件、加载纹理和渲染模型
├── PointCloud.h         # 声明PointCloud类
├── PointCloud.cpp       # 实现PointCloud类，负责加载和渲染点云数据
├── stb_image.h          # 第三方库，本项目处理png文件
├── main.cpp             # 主程序入口
├── model/                   # 模型文件夹
│   ├── spot/                # mesh
│   │   ├── spot_triangulated.obj   # OBJ三角化模型文件
│   │   ├── spot_texture.svg   		# 纹理图片（svg文件）
│   │   └── spot_texture.png        # 纹理图片（png文件）
│   └── pointcloud/          # pointcloud
└──     └── camel.xyz        # 点云数据
```

## 实验环境

软件环境：

- 操作系统 Windows 11
- 图形库 OpenGL、GLUT、stb_image.h
- 开发工具 Visual Studio 2022
