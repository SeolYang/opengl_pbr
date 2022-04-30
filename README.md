# OpenGL Model Viewer
-------------------------------------
## Descriptions
This project was study driven project for OpenGL, Voxelization, VCT, and others. So it is not well organized and optimized.
And also there are many limitation caused by lack of understand about textures format, compute shader and scene voxelization scheme.
I hope, break out these limitations in next project through this project. And also, i learned about importantness of cross validation and solid understanding through this project(also in project_mile which is previous project of this one).

As last, I would like to thanks especially to Cyril Crassin who published one of greatest piece about GI and voxelization schemes.

## Features
* Simple Scene
* Model Loader
* Build hierarchy bounding box cluster
* CPU side Frustum culling with AABB
* Scene Voxelization
* GI based on Voxel Cone Tracing (include AO)
* Physically Based Material
* Simple Compute 3D Texture Mipmap Generation
* PSM for Directional Light Source
* Camera Controller
* Camera Path(Interpolate Camera position and orientation with slerp)
* Debug AABB and Cone directions, etc

## Figures
![VCT_GI_0](Figures/VCT_GI_0.png)
![VCT_GI_1](Figures/VCT_GI_1.png)
![VCT_GI_2](Figures/VCT_with_smooth_surface_material.png)
![VCT_GI_3](Figures/VCT_self_emitted_object.png)
![VCT_AO](Figures/VCT_AO.png)
![VCT_DEBUG](Figures/Debug.png)
![VOXELIZATION](Figures/Voxelization.png)

# License
-------------------------------------
This software is provided under the **MIT License**.
