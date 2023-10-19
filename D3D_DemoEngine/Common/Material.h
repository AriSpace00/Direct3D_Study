#pragma once

#include <d3d11.h>

struct aiMaterial;

class Material
{
public:
    Material();
    ~Material();

public:
    ID3D11ShaderResourceView* m_DiffuseRV = nullptr;        // �ؽ�ó ���ҽ� ��
    ID3D11ShaderResourceView* m_NormalRV = nullptr;         // �ؽ�ó �븻�� ���ҽ� ��
    ID3D11ShaderResourceView* m_SpecularRV = nullptr;       // �ؽ�ó ����ŧ���� ���ҽ� ��
    ID3D11ShaderResourceView* m_EmissiveRV = nullptr;       // �ؽ�ó �̹̽ú�� ���ҽ� ��
    ID3D11ShaderResourceView* m_OpacityRV = nullptr;        // �ؽ�ó ���Ľ�Ƽ�� ���ҽ� ��

public:
    void Create(ID3D11Device* device, aiMaterial* material);
};

