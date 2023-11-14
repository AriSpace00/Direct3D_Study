#pragma once

// Model Class
// FBX 경로 임포트하면 메쉬, 머테리얼, 애니메이션 정보를 로드하여 저장한다.

#include <assimp/Importer.hpp>

#include "Mesh.h"
#include "Material.h"
#include "Animation.h"

struct CB_Transform
{
    Matrix WorldMatrix;
    Matrix ViewMatrix;
    Matrix ProjectionMatrix;
};

struct CB_Material
{
    Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Emissive = { 1.0f,1.0f,1.0f,1.0f };
    float SpecularPower = 80;
    bool UseDiffuseMap = true;
    bool MT_pad0[3];
    bool UseNormalMap = true;
    bool MT_pad1[3];
    bool UseSpecularMap = true;
    bool MT_pad2[3];
    bool UseEmissiveMap = true;
    bool MT_pad3[3];
    bool UseOpacityMap = true;
    bool MT_pad4[3];
    Vector2 MT_pad5;
};

class Node;

class Model
{
public:
    Model();
    ~Model();

public:
    //Node* m_RootNode;
    std::vector<Node*> m_Nodes;
    std::vector<Mesh> m_Meshes;
    std::vector<Material> m_Materials;
    std::vector<Animation> m_Animations;

    std::wstring m_FileName;
    bool IsFileLoad;

    CB_Transform m_Transform;
    CB_Material m_Material;

    ID3D11Buffer* m_CBTransform = nullptr;                  // 상수 버퍼: 변환행렬
    ID3D11Buffer* m_CBMaterial = nullptr;                   // 상수 버퍼: 변환행렬

    ID3D11BlendState* m_AlphaBlendState = nullptr;          // 샘플러 상태

    Matrix m_Position;
    Matrix m_Rotation;
    Matrix m_Scale;

public:
    void ReadFile(ID3D11Device* device, const std::string& path);
    void UpdateAnimation();

    void Update(const float& deltaTime);
    void Render(ID3D11DeviceContext* deviceContext);

    void SetTransform(Matrix position, Matrix rotation, Matrix scale);
};

