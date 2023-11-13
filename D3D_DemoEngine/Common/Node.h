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
    const aiScene* m_Scene;
    const aiNode* m_Node;

    string m_NodeName;
    string m_ParentNodeName;

    Matrix m_NodeLocalTM;
    Matrix m_NodeWorldTM;

    const aiNodeAnim* m_NodeAnimPtr;

    vector<Node> m_Childrens;

public:
    void SetScene(const aiScene* scene);
    void Create(ID3D11Device* device, Model* model, const aiNode* node);

    // �ִϸ��̼� ������Ʈ
    void Update(const float& deltaTime, Model* model, const aiNode* rootNode);
    void Render(ID3D11DeviceContext* deviceContext);

private:
    Matrix GetParentWorldTransform(const aiNode* parentNode);
    void FindNodeAnimation(const aiNode* node);
};

