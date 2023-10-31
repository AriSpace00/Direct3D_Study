#include "pch.h"
#include "Model.h"

#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include"Node.h"
#include "Helper.h"

Model::Model()
{
}

Model::~Model()
{
    m_Importer.FreeScene();

    /*SAFE_RELEASE(m_Node);
    SAFE_RELEASE(m_Scene);*/
}

void Model::ReadFile(ID3D11Device* device, const std::string& path)
{
    if(IsFileLoad)
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
    unsigned int importFlags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace |
        aiProcess_ConvertToLeftHanded;

    m_Scene = m_Importer.ReadFile(path, importFlags);

    if (!m_Scene) {
        LOG_ERRORA("Error loading FBX file: %s", m_Importer.GetErrorString());
        return;
    }

    // FBX 파일에 해당하는 Node 정보 Create
    m_Node = new Node();
    m_Node->Create(m_Scene->mRootNode);
    m_Node->SetScene(m_Scene);

    // FBX 파일에 해당하는 Material, Mesh 정보 Create
    m_Materials.resize(m_Scene->mNumMaterials);
    for (unsigned int i = 0; i < m_Scene->mNumMaterials; ++i)
    {
        m_Materials[i].SetFileName(m_FileName);
        m_Materials[i].Create(device, m_Scene->mMaterials[i]);
    }

    m_Meshes.resize(m_Scene->mNumMeshes);
    for (unsigned int i = 0; i < m_Scene->mNumMeshes; ++i)
    {
        m_Meshes[i].Create(device, m_Scene->mMeshes[i]);
    }
    IsFileLoad = true;
}

void Model::UpdateAnimation()
{
}

void Model::Update(const float& deltaTime)
{
}

void Model::Render()
{
    // Mesh와 Material Render
    /*for (unsigned int i = 0; i < m_Scene->mNumMeshes; ++i)
    {
        m_Meshes[i].Render();
    }

    for (unsigned int i = 0; i < m_Scene->mNumMaterials; ++i)
    {
        m_Materials[i].Render();
    }*/

    /*m_Node->Render(TODO);*/
}
