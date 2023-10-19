#include "pch.h"
#include "Material.h"

#include "Helper.h"
#include <Directxtk/WICTextureLoader.h>
#include <filesystem>
#include <assimp/scene.h>
#include <assimp/types.h>

using namespace DirectX;

Material::Material()
{
} 

Material::~Material()
{
    SAFE_RELEASE(m_DiffuseRV);
    SAFE_RELEASE(m_NormalRV);
    SAFE_RELEASE(m_SpecularRV);
    SAFE_RELEASE(m_EmissiveRV);
    SAFE_RELEASE(m_OpacityRV);
}

void Material::Create(ID3D11Device* device, aiMaterial* material)
{
    // 경로 설정
    aiString texturePath;
    std::filesystem::path path;
    std::wstring folderPath;
    std::string name;
    std::wstring basePath = L"../Resource/FBXLoad_Test/texture";

    for (const auto& entry : std::filesystem::recursive_directory_iterator(basePath))
    {
        if (entry.is_regular_file() && entry.path().extension() == L".png")
        {
            folderPath = entry.path().parent_path().wstring();
            name = material->GetName().C_Str();
        }
    }

    // 텍스처 로드
    if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        HR_T(DirectX::CreateWICTextureFromFile(device, finalPath.c_str(), nullptr, &m_DiffuseRV));
    }

    if (AI_SUCCESS == material->GetTexture(aiTextureType_NORMALS, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        HR_T(DirectX::CreateWICTextureFromFile(device, finalPath.c_str(), nullptr, &m_NormalRV));
    }

    if (AI_SUCCESS == material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        HR_T(DirectX::CreateWICTextureFromFile(device, finalPath.c_str(), nullptr, &m_SpecularRV));
    }

    if (AI_SUCCESS == material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        HR_T(DirectX::CreateWICTextureFromFile(device, finalPath.c_str(), nullptr, &m_EmissiveRV));
    }

    if (AI_SUCCESS == material->GetTexture(aiTextureType_OPACITY, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        HR_T(DirectX::CreateWICTextureFromFile(device, finalPath.c_str(), nullptr, &m_OpacityRV));
    }
}
