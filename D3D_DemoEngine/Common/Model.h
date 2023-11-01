#pragma once

// Model Class
// FBX ��� ����Ʈ�ϸ� �޽�, ���׸���, �ִϸ��̼� ������ �ε��Ͽ� �����Ѵ�.

#include <assimp/Importer.hpp>

#include "Mesh.h"
#include "Material.h"
#include "Animation.h"

class Node;

class Model
{
public:
    Model();
    ~Model();

public:
    Node* m_Node;
    std::vector<Mesh> m_Meshes;
    std::vector<Material> m_Materials;
    std::vector<Node> m_Nodes;
    //std::vector<Animation> m_Animations;

    const aiScene* m_Scene;
    Assimp::Importer m_Importer;

    std::wstring m_FileName;
    bool IsFileLoad;

public:
    void ReadFile(ID3D11Device* device, const std::string& path);
    void UpdateAnimation();

    void Update(const float& deltaTime);
    void Render();
};

