#pragma once

// Node Class
// �� Node�� localTM, WorldTM, NodeAnimation ������ ������Ʈ�Ǿ� ����ȴ�.

#include <d3d11.h>
#include <wrl/client.h>
#include <directxtk/SimpleMath.h>

#include <string>
#include <vector>

#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include "Animation.h"
#include "Mesh.h"

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace std;

class Model;

class Node
{
public:
    Node();
    ~Node();

public:
    string m_NodeName;

    Matrix m_NodeLocalTM;
    Matrix m_NodeWorldTM;

    Animation* m_Animation;

    Node* m_ParentNode;

    vector<Node> m_Childrens;
    vector<Mesh> m_Meshes;

public:
    void Create(ID3D11Device* device, Model* model, const aiScene* scene, const aiNode* node);

    // �ִϸ��̼� ������Ʈ
    void Update(const float& deltaTime, Model* model);
    void Render(ID3D11DeviceContext* deviceContext, Model* model);

private:
    Matrix GetParentWorldTransform(const Node* parentNode);
    void FindNodeAnimation(const aiScene* scene, const aiNode* node);
};

