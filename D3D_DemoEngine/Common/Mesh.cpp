#include "pch.h"
#include "Mesh.h"
#include "Helper.h"

#include <assimp/scene.h>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    /*SAFE_RELEASE(m_VertexBuffer);
    SAFE_RELEASE(m_IndexBuffer);*/
}

void Mesh::CreateVertexBuffer(ID3D11Device* device, const vector<Vertex>& vertices, UINT vertexCount)
{
    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(Vertex) * vertexCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;

    // 버텍스 버퍼 생성
    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();
    HR_T(device->CreateBuffer(&bd, &vbData, &m_VertexBuffer));

    // 버텍스 버퍼 정보
    m_VertexCount = vertexCount;
    m_VertexBufferStride = sizeof(Vertex);
    m_VertexBufferOffset = 0;
}

void Mesh::CreateIndexBuffer(ID3D11Device* device, const vector<WORD>& indices, UINT indexCount)
{
    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(WORD) * indexCount;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;

    // 인덱스 버퍼 생성
    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices.data();
    HR_T(device->CreateBuffer(&bd, &ibData, &m_IndexBuffer));

    // 인덱스 버퍼 정보
    m_IndexCount = indexCount;
}

void Mesh::Create(ID3D11Device* device, aiMesh* mesh, const aiMatrix4x4& nodeWorldTransform)
{
    m_MaterialIndex = mesh->mMaterialIndex;

    // 버텍스 정보 생성
    m_Vertices.resize(mesh->mNumVertices);
    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        if (nodeWorldTransform != aiMatrix4x4())
        {
            aiVector3D transformedPosition = mesh->mVertices[i];
            auto finalTransform = nodeWorldTransform * transformedPosition;

            m_Vertices[i].Position = Vector3(finalTransform.x, finalTransform.y, finalTransform.z);
        }
        else
        {
            m_Vertices[i].Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        }

        // 실제 쓰일 머테리얼은 Material 클래스에서 조정하므로 TexCoords 세트는 1개만 존재함
        m_Vertices[i].Texcoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        m_Vertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        m_Vertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
    }
    CreateVertexBuffer(device, m_Vertices, mesh->mNumVertices);

    // 인덱스 정보 생성
    // 면(삼각형)의 정점은 3개이기 때문에 면(Face)의 갯수에 3을 곱해줌
    m_Indices.resize(mesh->mNumFaces * 3);
    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        m_Indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
        m_Indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
        m_Indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
    }
    CreateIndexBuffer(device, m_Indices, mesh->mNumFaces * 3);
}

void Mesh::Update(const float& deltaTime)
{

}
