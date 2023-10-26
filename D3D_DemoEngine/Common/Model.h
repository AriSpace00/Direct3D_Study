#pragma once

#include <assimp/Importer.hpp>

#include "Node.h"
#include "Mesh.h"
#include "Material.h"
#include "Animation.h"

class Model
{
public:
    Model();
    ~Model();

public:
    std::vector<Mesh> m_Meshes;
    std::vector<Material> m_Materials;
    std::vector<Animation> m_Animations;

    const aiScene* m_Scene;
    const aiNode* m_RootNode;
    Assimp::Importer m_Importer;

    std::wstring m_FileName;
    bool IsFileLoad;

public:
    void ReadFile(ID3D11Device* device, const std::string& path);
    void UpdateAnimation();
    void Update(const float& deltaTime);
};

