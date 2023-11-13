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
    // 월드 매트릭스 초기화
    m_WorldMatrix = DirectX::XMMatrixIdentity();
}

Model::~Model()
{
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

    // 파일 경로로부터 FBX 파일 이름 추출
    size_t lastSlash = path.find_last_of('/');
    size_t lastDot = path.find_last_of('.');

    if (lastSlash != std::string::npos && lastDot != std::string::npos)
    {
        std::wstring finalFilePath(path.begin(), path.end());
        m_FileName = finalFilePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
    }

    // FBX 파일 경로를 scene에 바인딩
    Assimp::Importer importer;
    unsigned int importFlags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace |
        aiProcess_ConvertToLeftHanded;

    const aiScene* scene = importer.ReadFile(path, importFlags);
    if (!scene) {
        LOG_ERRORA("Error loading FBX file: %s", importer.GetErrorString());
        return;
    }

    // Node 정보 Create
    m_Meshes.resize(scene->mNumMeshes);
    if (scene->mRootNode != nullptr)
    {
        Node* m_RootNode = new Node();
        m_RootNode->SetScene(scene);
        m_RootNode->Create(device, this, scene->mRootNode);
    }

    // Mesh 정보 Create
    /*m_Meshes.resize(scene->mNumMeshes);
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        m_Meshes[i].Create(device, scene->mMeshes[i]);
    }*/

    // Material 정보 Create
    m_Materials.resize(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        m_Materials[i].SetFileName(m_FileName);
        m_Materials[i].Create(device, scene->mMaterials[i]);
    }

    IsFileLoad = true;

    importer.FreeScene();
}

void Model::UpdateAnimation()
{
    // NodeAnimation에서 최종 변환한 NodeWorldTM을 Mesh에 업데이트
}

void Model::Update(const float& deltaTime)
{
    // 애니메이션을 위한 모델 업데이트
    for (int i = 0; i < m_Meshes.size(); i++)
    {
        m_Meshes[i].m_NodeWorldTM = m_Nodes[i]->m_NodeWorldTM;
    }

    m_WorldMatrix = m_Meshes[0].m_NodeWorldTM;
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