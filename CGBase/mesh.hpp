/**
 * @file mesh.hpp
 * @author Jovan Ivosevic
 * @brief Model mesh, a renderable piece of a model
 * @version 0.1
 * @date 2022-10-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <assimp/scene.h>
#include <vector>
#include <GL/glew.h>
#include <iostream>
#include "texture.hpp"

class Mesh {
public:
    std::vector<unsigned> mIndices;
    std::vector<float> mVertices;

    /**
     * @brief Ctor - buffers mesh data
     *
     * @param mesh - Assimp mesh
     * @param MeshMaterial - Assimp material
     * @param resPath - Resource relative path. For loading textures, etc...
     *
     */
    Mesh(const aiMesh* mesh, const aiMaterial* material, const std::string& resPath);

    /**
     * @brief Renders the current mesh
     *
     */
    void Render() const;

private:
    unsigned mVAO;
    unsigned mVBO;
    unsigned mEBO;
    unsigned mVertexCount;
    unsigned mIndexCount;
    unsigned mDiffuseTexture;
    unsigned mSpecularTexture;
    unsigned loadMeshTexture(const aiMaterial* material, const std::string& resPath, aiTextureType type);
    void processMesh(const aiMesh* mesh, const aiMaterial* material, const std::string& resPath);
};


//#pragma once
//
//#include <GL/glew.h>
//#include <assimp/scene.h>
//#include<vector>
//
//class Mesh {
//public:
//    /**
//     * @brief Ctor - buffers mesh data
//     *
//     * @param mesh - Assimp mesh
//     * @param MeshMaterial - Assimp material
//     * @param resPath - Resource relative path. For loading textures, etc...
//     * 
//     */
//    Mesh(const aiMesh* mesh, aiMaterial* MeshMaterial, const std::string& resPath);
//
//    /**
//     * @brief Renders the mesh
//     *
//     */
//    void Render() const;
//private:
//    unsigned mVAO;
//    unsigned mVBO;
//    unsigned mEBO;
//    unsigned mIndicesCount;
//    unsigned mVerticesCount;
//
//    /**
//     * @brief Buffers mesh data into GL
//     * 
//     * @param mesh Assimp mesh struct
//     * @param MeshMaterial Assimp material struct
//     * @param resPath Resource path, used for loading textures
//     * 
//     */
//    void processMesh(const aiMesh* mesh, aiMaterial* MeshMaterial, const std::string& resPath);
//};