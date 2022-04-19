# ImprovedMesh-2D
This repository includes the source code for the paper: 
-  **Quanbing Luo**, Improved physically-based high-quality mesh generation method on two-dimensional regions, Submitted to Journal. 
<!-- , [Engineering with Computers](https://doi.org/10.1007/s00366-020-01262-x), 2021 (Published Online) -->

The source code is divided to two parts: (1) Initial Mesh and (2) Mesh Generation. 

## (1) Initial Mesh

The source code of Initial Mesh generated the Initial Mesh used for mesh generation. The initial mesh reflected the mesh coarseness and fineness around boundary edges.   


## (2) Mesh Generation

There are two mesh generation methods in the source code: (a) Old Method, (b) New Method.

### (a) Old Method
Old method (or original method) is the method provided in my previous paper "Automatic Delaunay mesh generation method and physically-based mesh optimization method on two-dimensional regions". The source code is revised in some places.   

### (b) New Method
New method (or improved method) is the method provided in the  paper "Improved physically-based high-quality mesh generation method on two-dimensional regions (Submitted to Journal)". 



**Notice:** The source code does not provide the intermediate data and final mesh data. You should generate these data step by step. There is a Figure.m program in each program directory to draw the figure of generated data.    



