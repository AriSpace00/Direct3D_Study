#include "pch.h"
#include "Model.h"

#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Node.h"
#include "Helper.h"

Model::Model()
{
}

Model::~Model()
{
    m_Importer.FreeScene();
}

void Model::ReadFile(ID3D11Device* device, const std::string& path)
{
    if (IsFileLoad)
    {
        return;
    }

    // ���� ��ηκ��� FBX ���� �̸� ����
    size_t lastSlash = path.find_last_of('/');
    size_t lastDot = path.find_last_of('.');

    if (lastSlash != std::string::npos && lastDot != std::string::npos)
    {
        std::wstring finalFilePath(path.begin(), path.end());
        m_FileName = finalFilePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
    }

    // FBX ���� ��θ� scene�� ���ε�
    unsigned int importFlags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace |
        aiProcess_ConvertToLeftHanded;

    m_Scene = m_Importer.ReadFile(path, importFlags);

    if (!m_Scene) {
        LOG_ERRORA("Error loading FBX file: %s", m_Importer.GetErrorString());
        return;
    }

    // Node�� �����Ѵٸ�, FBX ���Ͽ� �ش��ϴ� Node ���� Create
    if (m_Scene->mRootNode != nullptr)
    {
        m_Node = new Node();
        m_Node->SetScene(m_Scene);
        m_Node->Create(m_Scene->mRootNode);

        // FBX ���Ͽ� �ش��ϴ� Mesh ���� Create
        //m_Meshes.resize(m_Scene->mNumMeshes);
        for (unsigned int i = 0; i < m_Node->m_Nodes.size(); i++)
        {
            const aiNode* currentNode = m_Node->m_Nodes[i]->m_Node;
            const aiMatrix4x4& worldTransform = m_Node->m_Nodes[i]->m_NodeWorldTM;

            if (currentNode->mNumMeshes > 0)
            {
                for (unsigned int j = 0; j < currentNode->mNumMeshes; j++)
                {
                    aiMesh* mesh = m_Scene->mMeshes[currentNode->mMeshes[j]];

                    // Mesh�� Create �Լ��� ȣ���Ͽ� Mesh�� ������Ʈ�մϴ�.
                    Mesh currentMesh;
                    currentMesh.Create(device, mesh, worldTransform);
                    m_Meshes.push_back(currentMesh);
                }
            }
        }
    }
    else
    {
        // Node�� �������� �ʴ´ٸ�, FBX ���Ͽ� �ش��ϴ� Mesh ���� Create
        m_Meshes.resize(m_Scene->mNumMeshes);
        const aiMatrix4x4& worldTransform = aiMatrix4x4();
        for (unsigned int i = 0; i < m_Scene->mNumMeshes; ++i)
        {
            m_Meshes[i].Create(device, m_Scene->mMeshes[i], worldTransform);
        }
    }

    // Node ���� ���ο� ���þ��� FBX ���Ͽ� �ش��ϴ� Material ���� Create
    m_Materials.resize(m_Scene->mNumMaterials);
    for (unsigned int i = 0; i < m_Scene->mNumMaterials; ++i)
    {
        m_Materials[i].SetFileName(m_FileName);
        m_Materials[i].Create(device, m_Scene->mMaterials[i]);
    }

    IsFileLoad = true;
}

void Model::UpdateAnimation()
{
    // NodeAnimation���� ���� ��ȯ�� NodeWorldTM�� Mesh�� ������Ʈ

}

void Model::Update(const float& deltaTime)
{
    // �ִϸ��̼��� ���� �� ������Ʈ
    m_Node->Update(deltaTime);

    // FBX ���Ͽ� �ش��ϴ� Mesh ���� Update
    for (unsigned int i = 0; i < m_Node->m_Nodes.size(); i++)
    {
        const aiNode* currentNode = m_Node->m_Nodes[i]->m_Node;
        const aiMatrix4x4& worldTransform = m_Node->m_Nodes[i]->m_NodeWorldTM;

        if (currentNode->mNumMeshes > 0)
        {
            for (unsigned int j = 0; j < currentNode->mNumMeshes; j++)
            {
                m_Meshes[j].Update(m_Scene->mMeshes[j], worldTransform);
            }
        }
    }
}

void Model::Render()
{
    // Mesh�� Material Render
}

void Model::SetDC(ID3D11DeviceContext* deviceContext)
{
    m_DeviceContext = deviceContext;
}
