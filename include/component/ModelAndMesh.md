# New Model and Mesh System

load Model with ASSIMP
- aiProcess_OptimizeMeshes
- aiProcess_SortByPType ignore point and line later
- aiProcess_Triangulate
- aiProcess_CalcTangentSpace
- aiProcess_PreTransformVertices
- aiProcess_RemoveRedundantMaterials
- aiProcess_OptimizeGraph

## Mesh vertex data are independent

## material loading
read aiScene->mNumMaterials to allocate material lib space

materialHeap=malloc(sizeof(Material)*aiScene->mNumMaterials)

std::shared< Material > newMatPtr(new((Material[])material[savedMaterialNum++]) Material(Arg...))