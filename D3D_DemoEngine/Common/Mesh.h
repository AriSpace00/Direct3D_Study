#pragma once

// Mesh Class
// 3d 모델로부터 불러온 버텍스 정보로 버텍스 버퍼를 생성한다.
// 3d 모델로부터 불러온 버텍스 정보로 인덱스 버퍼를 생성한다.

#include <d3d11.h>
#include <memory>
#include <wrl/client.h>
#include <directxtk/SimpleMath.h>

#include "Node.h"

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace std;

struct Vertex
{
    Vector3 Position;
    Vector2 Texcoord;
    Vector3 Normal;
    Vector3 Tangent;
};

struct aiMesh;

class Mesh
{
public:
    Mesh();
    ~Mesh();

public:
    ID3D11Buffer* m_VertexBuffer;     
    ID3D11Buffer* m_IndexBuffer;      
                                       
    UINT m_VertexBufferStride = 0;    // 버텍스 하나의 크기
    UINT m_VertexBufferOffset = 0;    // 버텍스 버퍼의 오프셋
    UINT m_VertexCount = 0;           // 버텍스 개수 
    UINT m_IndexCount = 0;            // 인덱스 개수
    UINT m_MaterialIndex = 0;         // 머테리얼 인덱스

    unique_ptr<Vertex[]> m_Vertices;

private:
    // aiMesh로부터 불러온 정보로 Create 함수를 진행하기 위한 내부함수
    // 버텍스 버퍼 생성
    void CreateVertexBuffer(ID3D11Device* device, Vertex* vertices, UINT vertexCount);
    // 인덱스 버퍼 생성
    void CreateIndexBuffer(ID3D11Device* device, WORD* indices, UINT indexCount);

public:
    // aiMesh로부터 불러온 정보로 버텍스 버퍼 & 인덱스 버퍼 생성
    void Create(ID3D11Device* device, aiMesh* mesh, const aiMatrix4x4& nodeWorldTransform);
    void Update(const float& deltaTime);
    void Render();
};

