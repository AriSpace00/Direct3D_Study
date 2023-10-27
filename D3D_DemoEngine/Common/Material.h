#pragma once

// Material Class
// 3d 모델로부터 불러온 머테리얼 정보로 텍스처 생성한다.

#include <d3d11.h>
#include <string>

struct aiMaterial;

class Material
{
public:
    Material();
    ~Material();

private:
    std::wstring FBXFileName;

public:
    ID3D11ShaderResourceView* m_DiffuseRV = nullptr;        // 텍스처 리소스 뷰
    ID3D11ShaderResourceView* m_NormalRV = nullptr;         // 텍스처 노말맵 리소스 뷰
    ID3D11ShaderResourceView* m_SpecularRV = nullptr;       // 텍스처 스펙큘러맵 리소스 뷰
    ID3D11ShaderResourceView* m_EmissiveRV = nullptr;       // 텍스처 이미시브맵 리소스 뷰
    ID3D11ShaderResourceView* m_OpacityRV = nullptr;        // 텍스처 오파시티맵 리소스 뷰

public:
    void Create(ID3D11Device* device, aiMaterial* material);
    void SetFileName(const std::wstring& fileName);
    void Render();
};

