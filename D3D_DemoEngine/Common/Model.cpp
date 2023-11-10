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
    // ���� ��Ʈ���� �ʱ�ȭ
    m_WorldMatrix = DirectX::XMMatrixIdentity();
}

Model::~Model()
{
    m_Importer.FreeScene();

    SAFE_RELEASE(m_CBTransform);
    SAFE_RELEASE(m_CBMaterial);
    SAFE_RELEASE(m_AlphaBlendState);
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
    m_WorldMatrix = ConvertaiMatrixToXMMATRIX(m_Node->m_Nodes[0]->m_NodeWorldTM);
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
    // Mesh Render
    deviceContext->VSSetConstantBuffers(0, 1, &m_CBTransform);
    deviceContext->PSSetConstantBuffers(0, 1, &m_CBTransform);
    deviceContext->PSSetConstantBuffers(2, 1, &m_CBMaterial);

    m_Transform.WorldMatrix = DirectX::XMMatrixTranspose(m_WorldMatrix);
    deviceContext->UpdateSubresource(m_CBTransform, 0, nullptr, &m_Transform, 0, 0);

    // Material Render
    for (size_t i = 0; i < m_Meshes.size(); i++)
    {
        size_t mi = m_Meshes[i].m_MaterialIndex;

        deviceContext->PSSetShaderResources(0, 1, &m_Materials[mi].m_DiffuseRV);
        deviceContext->PSSetShaderResources(1, 1, &m_Materials[mi].m_NormalRV);
        deviceContext->PSSetShaderResources(2, 1, &m_Materials[mi].m_SpecularRV);
        deviceContext->PSSetShaderResources(3, 1, &m_Materials[mi].m_EmissiveRV);
        deviceContext->PSSetShaderResources(4, 1, &m_Materials[mi].m_OpacityRV);

        m_Material.UseDiffuseMap = m_Materials[mi].m_DiffuseRV != nullptr ? true : false;
        m_Material.UseNormalMap = m_Materials[mi].m_NormalRV != nullptr ? true : false;
        m_Material.UseSpecularMap = m_Materials[mi].m_SpecularRV != nullptr ? true : false;
        m_Material.UseEmissiveMap = m_Materials[mi].m_EmissiveRV != nullptr ? true : false;
        m_Material.UseOpacityMap = m_Materials[mi].m_OpacityRV != nullptr ? true : false;

        if (m_Material.UseOpacityMap)
        {
            deviceContext->OMSetBlendState(m_AlphaBlendState, nullptr, 0xffffffff);
        }
        else
        {
            deviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
        }

        deviceContext->UpdateSubresource(m_CBMaterial, 0, nullptr, &m_Material, 0, 0);
        deviceContext->IASetIndexBuffer(m_Meshes[i].m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        deviceContext->IASetVertexBuffers(0, 1, &m_Meshes[i].m_VertexBuffer, &m_Meshes[i].m_VertexBufferStride, &m_Meshes[i].m_VertexBufferOffset);
        deviceContext->DrawIndexed(m_Meshes[i].m_IndexCount, 0, 0);
    }
}