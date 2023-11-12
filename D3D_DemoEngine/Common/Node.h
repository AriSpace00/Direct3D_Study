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

    string m_NodeName;
    string m_ParentNodeName;

    Matrix m_NodeLocalTM;
    Matrix m_NodeWorldTM;

    const aiNodeAnim* m_NodeAnimPtr;

    vector<Node> m_Childrens;

    ID3D11Buffer* m_WorldTMBuffer;

public:
    void SetScene(const aiScene* scene);
    void Create(const aiNode* node, Model* model);

    // �ִϸ��̼� ������Ʈ
    void Update(ID3D11DeviceContext* deviceContext);
    void Render(ID3D11DeviceContext* device);

private:
    Matrix GetParentWorldTransform(const aiNode* parentNode);
    void FindNodeAnimation(const aiNode* node);
};

