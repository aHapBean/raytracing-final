#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "vec3.h"
#include <vector> 
#include <string>

//有多个贴图的obj导入
void obj_load_with_texture_and_diff_pics(const char *path, std::vector<vec3> &triangle_ls, std::vector<double> &store_u, std::vector<double> &store_v, std::vector<vec3> &normals, std::vector<std::string> &pic_names) {
    /*
        输入路径
        得到模型三角形等信息
    */
    triangle_ls.clear();
    store_u.clear();
    store_v.clear();
    normals.clear();
    pic_names.clear();
    // pic_names的大小与normals一致
    
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);
    std::cerr << "load obj from : " << path << std::endl;
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading OBJ file: " << importer.GetErrorString() << std::endl;
        exit(-1);
    }
    
    vec3 objectCenter(0., 0., 0.);
    int numVertices = 0;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];

        unsigned int materialIndex = mesh->mMaterialIndex;
        aiMaterial* material = scene->mMaterials[materialIndex];

        // 获取材质名称
        aiString materialName;
        material->Get(AI_MATKEY_NAME, materialName);

        aiString diffuseMapFilename;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseMapFilename);
        std::string name = diffuseMapFilename.C_Str();
        

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace face = mesh->mFaces[j];
            for(int i = 0; i < 3; ++i) {    // must be triangle !
                triangle_ls.push_back(convert_to_vec3(mesh->mVertices[face.mIndices[i]]));
                store_u.push_back(mesh->mTextureCoords[0][face.mIndices[i]].x);
                store_v.push_back(mesh->mTextureCoords[0][face.mIndices[i]].y);
                objectCenter += triangle_ls.back();
            }
            pic_names.push_back(name);
            numVertices += 3;
            normals.push_back(vec3(0,0,0));
        }
    }
    /*
    // 法向量
            aiVector3D normal = mesh->mNormals[vertexIndex];
            vec3 normalVec = convert_to_vec3(normal);

            // 纹理坐标 (u, v)
            aiVector3D texCoord = mesh->mTextureCoords[0][vertexIndex];  // Assuming only one set of texture coordinates
            vec2 uv(texCoord.x, texCoord.y);
    */

    
    objectCenter /= numVertices;
    double abs_Z = -1e18, abs_Y = -1e18, abs_X = -1e18;
    
    // 平移物体
    for(int i = 0; i < triangle_ls.size(); ++i) {
        triangle_ls[i] += vec3(-objectCenter[0], -objectCenter[1], -objectCenter[2]);
        abs_Z = ffmax(ffmax(abs(triangle_ls[i].z()), ffmax(abs(triangle_ls[i].z()), abs(triangle_ls[i].z()))), abs_Z);
        abs_Y = ffmax(ffmax(abs(triangle_ls[i].y()), ffmax(abs(triangle_ls[i].y()), abs(triangle_ls[i].y()))), abs_Y);
        abs_X = ffmax(ffmax(abs(triangle_ls[i].x()), ffmax(abs(triangle_ls[i].x()), abs(triangle_ls[i].x()))), abs_X);
    }

    //normalize物体
    vec3 scale(1/abs_X, 1/abs_Y, 1/abs_Z);  //用于sacle
    for(int i = 0; i < triangle_ls.size(); ++i) {
        triangle_ls[i] = triangle_ls[i] * scale;
    }
    for(int i = 0; i < normals.size(); ++i) {   //recalculate normal
        vec3 x = triangle_ls[3 * i], y = triangle_ls[3 * i + 1], z = triangle_ls[3 * i + 2];
        vec3 v1 = x - y, v2 = z - y;
        vec3 normal = cross(v1, v2);
        normal = normalize(normal);
        normals[i] = normal;
    }
}

//只有一个贴图的obj导入
void obj_load_with_texture(const char *path, std::vector<vec3> &triangle_ls, std::vector<double> &store_u, std::vector<double> &store_v, std::vector<vec3> &normals) {
    triangle_ls.clear();
    store_u.clear();
    store_v.clear();
    normals.clear();
    
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);
    std::cerr << "load obj from : " << path << std::endl;
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading OBJ file: " << importer.GetErrorString() << std::endl;
        exit(-1);
    }
    
    vec3 objectCenter(0., 0., 0.);
    int numVertices = 0;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];

        unsigned int materialIndex = mesh->mMaterialIndex;
        aiMaterial* material = scene->mMaterials[materialIndex];

        // 获取材质名称
        aiString materialName;
        material->Get(AI_MATKEY_NAME, materialName);

        aiString diffuseMapFilename;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseMapFilename);
        std::cerr << diffuseMapFilename.C_Str() << std::endl;
        

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace face = mesh->mFaces[j];
            for(int i = 0; i < 3; ++i) {    // must be triangle !
                triangle_ls.push_back(convert_to_vec3(mesh->mVertices[face.mIndices[i]]));
                store_u.push_back(mesh->mTextureCoords[0][face.mIndices[i]].x);
                store_v.push_back(mesh->mTextureCoords[0][face.mIndices[i]].y);
                objectCenter += triangle_ls.back();
            }
            numVertices += 3;
            normals.push_back(vec3(0,0,0));
        }
    }
    /*
    // 法向量
            aiVector3D normal = mesh->mNormals[vertexIndex];
            vec3 normalVec = convert_to_vec3(normal);

            // 纹理坐标 (u, v)
            aiVector3D texCoord = mesh->mTextureCoords[0][vertexIndex];  // Assuming only one set of texture coordinates
            vec2 uv(texCoord.x, texCoord.y);
    */

    
    objectCenter /= numVertices;
    double abs_Z = -1e18, abs_Y = -1e18, abs_X = -1e18;
    
    // 平移物体
    for(int i = 0; i < triangle_ls.size(); ++i) {
        triangle_ls[i] += vec3(-objectCenter[0], -objectCenter[1], -objectCenter[2]);
        abs_Z = ffmax(ffmax(abs(triangle_ls[i].z()), ffmax(abs(triangle_ls[i].z()), abs(triangle_ls[i].z()))), abs_Z);
        abs_Y = ffmax(ffmax(abs(triangle_ls[i].y()), ffmax(abs(triangle_ls[i].y()), abs(triangle_ls[i].y()))), abs_Y);
        abs_X = ffmax(ffmax(abs(triangle_ls[i].x()), ffmax(abs(triangle_ls[i].x()), abs(triangle_ls[i].x()))), abs_X);
    }

    //normalize物体
    vec3 scale(1/abs_X, 1/abs_Y, 1/abs_Z);  //用于sacle
    for(int i = 0; i < triangle_ls.size(); ++i) {
        triangle_ls[i] = triangle_ls[i] * scale;
    }
    for(int i = 0; i < normals.size(); ++i) {   //recalculate normal
        vec3 x = triangle_ls[3 * i], y = triangle_ls[3 * i + 1], z = triangle_ls[3 * i + 2];
        vec3 v1 = x - y, v2 = z - y;
        vec3 normal = cross(v1, v2);
        normal = normalize(normal);
        normals[i] = normal;
    }
}

//没有贴图的obj导入
void obj_load_without_texture(const char *path, std::vector<vec3> &triangle_ls, std::vector<vec3> &normals) {
    triangle_ls.clear();
    normals.clear();
    
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);
    std::cerr << "load obj from : " << path << std::endl;
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading OBJ file: " << importer.GetErrorString() << std::endl;
        exit(-1);
    }
    
    vec3 objectCenter(0., 0., 0.);
    int numVertices = 0;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace face = mesh->mFaces[j];
            for(int i = 0; i < 3; ++i) {    // must be triangle !
                triangle_ls.push_back(convert_to_vec3(mesh->mVertices[face.mIndices[i]]));
                objectCenter += triangle_ls.back();
            }
            numVertices += 3;
            normals.push_back(vec3(0,0,0));
        }
    }
    /*
    // 法向量
            aiVector3D normal = mesh->mNormals[vertexIndex];
            vec3 normalVec = convert_to_vec3(normal);

            // 纹理坐标 (u, v)
            aiVector3D texCoord = mesh->mTextureCoords[0][vertexIndex];  // Assuming only one set of texture coordinates
            vec2 uv(texCoord.x, texCoord.y);
    */

    
    objectCenter /= numVertices;
    double abs_Z = -1e18, abs_Y = -1e18, abs_X = -1e18;
    
    // 平移物体
    for(int i = 0; i < triangle_ls.size(); ++i) {
        triangle_ls[i] += vec3(-objectCenter[0], -objectCenter[1], -objectCenter[2]);
        abs_Z = ffmax(ffmax(abs(triangle_ls[i].z()), ffmax(abs(triangle_ls[i].z()), abs(triangle_ls[i].z()))), abs_Z);
        abs_Y = ffmax(ffmax(abs(triangle_ls[i].y()), ffmax(abs(triangle_ls[i].y()), abs(triangle_ls[i].y()))), abs_Y);
        abs_X = ffmax(ffmax(abs(triangle_ls[i].x()), ffmax(abs(triangle_ls[i].x()), abs(triangle_ls[i].x()))), abs_X);
    }

    //normalize物体
    vec3 scale(1/abs_X, 1/abs_Y, 1/abs_Z);  //用于sacle
    for(int i = 0; i < triangle_ls.size(); ++i) {
        triangle_ls[i] = triangle_ls[i] * scale;
    }
    for(int i = 0; i < normals.size(); ++i) {   //recalculate normal
        vec3 x = triangle_ls[3 * i], y = triangle_ls[3 * i + 1], z = triangle_ls[3 * i + 2];
        vec3 v1 = x - y, v2 = z - y;
        vec3 normal = cross(v1, v2);
        normal = normalize(normal);
        normals[i] = normal;
    }
}

#endif 